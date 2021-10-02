#include "Json.h"

Json::Json()
	:m_type(Type::Null)
{
	m_value.null = nullptr;
}

Json Json::emptyObject()
{
	return Json(std::initializer_list<std::pair<const std::string, Json>>());
}

Json Json::emptyArray()
{
	return Json(std::initializer_list<int>());
}

Json::Json(const std::string& string)
	: m_type(Type::String)
{
	new(&m_value.string) std::string(string);
}

Json::Json(std::string&& string) noexcept
	: m_type(Type::String)
{
	new(&m_value.string) std::string(std::move(string));
}

// const char* implicitly gets converted to bool
Json::Json(const char* string)
	: m_type(Type::String)
{
	new(&m_value.string) std::string(string);
}

Json::Json(double number)
	: m_type(Type::Number)
{
	m_value.number = number;
}

Json::Json(int number)
	: m_type(Type::Number)
{
	m_value.number = static_cast<double>(number);
}

Json::Json(bool boolean)
	: m_type(Type::Boolean)
{
	m_value.boolean = boolean;
}

Json::Json(std::nullptr_t)
	: m_type(Type::Null)
{
	m_value.null = nullptr;
}

Json::Json(std::initializer_list<std::pair<const std::string, Json>> object)
	: m_type(Type::Object)
{
	new(&m_value.object) std::unordered_map<std::string, Json>(object);
}

Json::Json(std::initializer_list<double> array)
	: m_type(Type::Array)
{
	setArray(array);
}

Json::Json(std::initializer_list<int> array)
	: m_type(Type::Array)
{
	setArray(array);
}

Json::Json(std::initializer_list<bool> array)
	: m_type(Type::Array)
{
	setArray(array);
}

Json::Json(const Json& json)
	: m_type(json.m_type)
{
	switch (m_type)
	{
	case Json::Type::String:
		new(&m_value.string) std::string(json.m_value.string);
		break;
	case Json::Type::Number:
		m_value.number = json.m_value.number;
		break;
	case Json::Type::Boolean:
		m_value.boolean = json.m_value.boolean;
		break;
	case Json::Type::Null:
		m_value.null = json.m_value.null;
		break;
	case Json::Type::Object:
		new(&m_value.object) std::unordered_map<std::string, Json>(json.m_value.object);
		break;
	case Json::Type::Array:
		new(&m_value.array) std::vector<Json>(json.m_value.array);
		break;
	}
}

Json::Json(Json&& json) noexcept
	: m_type(json.m_type)
{
	switch (m_type)
	{
	case Json::Type::String:
		new(&m_value.string) std::string(std::move(json.m_value.string));
		break;
	case Json::Type::Number:
		m_value.number = json.m_value.number;
		break;
	case Json::Type::Boolean:
		m_value.boolean = json.m_value.boolean;
		break;
	case Json::Type::Null:
		m_value.null = json.m_value.null;
		break;
	case Json::Type::Object:
		new(&m_value.object) std::unordered_map<std::string, Json>(std::move(json.m_value.object));
		break;
	case Json::Type::Array:
		new(&m_value.array) std::vector<Json>(std::move(json.m_value.array));
		break;
	}

	json.m_type = Type::Null;
	json.m_value.null = nullptr;
}

Json::~Json()
{
	destructValue();
}

Json& Json::operator=(const std::string& string)
{
	destructValue();
	m_type = Type::String;
	new(&m_value.string) std::string(string);
	return *this;
}

Json& Json::operator=(std::string&& string)
{
	destructValue();
	m_type = Type::String;
	new(&m_value.string) std::string(std::move(string));
	return *this;
}

Json& Json::operator=(const char* string)
{
	destructValue();
	m_type = Type::String;
	new(&m_value.string) std::string(string);
	return *this;
}

Json& Json::operator=(double number)
{
	destructValue();
	m_type = Type::Number;
	m_value.number = number;
	return *this;
}

Json& Json::operator=(int number)
{
	destructValue();
	m_type = Type::Number;
	m_value.number = number;
	return *this;
}

Json& Json::operator=(bool boolean)
{
	destructValue();
	m_type = Type::Boolean;
	m_value.boolean = boolean;
	return *this;
}

Json& Json::operator=(std::nullptr_t)
{
	destructValue();
	m_type = Type::Null;
	m_value.null = nullptr;
	return *this;
}

Json& Json::operator=(const Json& object)
{
	if (this == &object)
		return *this;

	destructValue();

	m_type = object.m_type;

	switch (m_type)
	{
	case Json::Type::String:
		new(&m_value.string) std::string(object.m_value.string);
		break;
	case Json::Type::Number:
		m_value.number = object.m_value.number;
		break;
	case Json::Type::Boolean:
		m_value.boolean = object.m_value.boolean;
		break;
	case Json::Type::Null:
		m_value.null = object.m_value.null;
		break;
	case Json::Type::Object:
		new(&m_value.object) std::unordered_map<std::string, Json>(object.m_value.object);
		break;
	case Json::Type::Array:
		new(&m_value.array) std::vector<Json>(object.m_value.array);
		break;
	}

	return *this;
}

Json& Json::operator=(Json&& object) noexcept
{
	if (this == &object)
		return *this;

	destructValue();
	m_type = object.m_type;

	switch (m_type)
	{
	case Json::Type::String:
		new(&m_value.string) std::string(std::move(object.m_value.string));
		break;
	case Json::Type::Number:
		m_value.number = object.m_value.number;
		break;
	case Json::Type::Boolean:
		m_value.boolean = object.m_value.boolean;
		break;
	case Json::Type::Null:
		m_value.null = object.m_value.null;
		break;
	case Json::Type::Object:
		new(&m_value.object) std::unordered_map<std::string, Json>(std::move(object.m_value.object));
		break;
	case Json::Type::Array:
		new(&m_value.array) std::vector<Json>(std::move(object.m_value.array));
		break;
	}

	object.m_type = Type::Null;
	object.m_value.null = nullptr;

	return *this;
}

Json& Json::operator[](const std::string& key)
{
	return m_value.object[key];
}

Json& Json::operator[](std::string&& key) noexcept
{
	return m_value.object[std::move(key)];
}

Json& Json::at(const std::string& key)
{
	if (m_type != Type::Object)
		throw InvalidTypeAccess();

	if (contains(key) == false)
		throw OutOfRangeAccess();

	return m_value.object.at(key);
}

const Json& Json::at(const std::string& key) const
{
	if (m_type != Type::Object)
		throw InvalidTypeAccess();

	if (contains(key) == false)
		throw OutOfRangeAccess();

	return m_value.object.at(key);
}

bool Json::contains(const std::string& key) const
{
	return m_value.object.count(key) > 0;
}

std::string& Json::string()
{
	return m_value.string;
}

double& Json::number()
{
	return m_value.number;
}

bool& Json::boolean()
{
	return m_value.boolean;
}

std::nullptr_t& Json::null()
{
	return m_value.null;
}

std::unordered_map<std::string, Json>& Json::object()
{
	return m_value.object;
}

std::vector<Json>& Json::array()
{
	return m_value.array;
}

const std::string& Json::string() const
{
	return m_value.string;
}

double Json::number() const
{
	return m_value.number;
}

bool Json::boolean() const
{
	return m_value.boolean;
}

std::nullptr_t Json::null() const
{
	return m_value.null;
}

const std::unordered_map<std::string, Json>& Json::object() const
{
	return m_value.object;
}

const std::vector<Json>& Json::array() const
{
	return m_value.array;
}

std::string& Json::getString()
{
	if (isString())
		return m_value.string;
	throw InvalidTypeAccess();
}

double& Json::getNumber()
{
	if (isNumber())
		return m_value.number;
	throw InvalidTypeAccess();
}

bool& Json::getBoolean()
{
	if (isBoolean())
		return m_value.boolean;
	throw InvalidTypeAccess();
}

std::nullptr_t& Json::getNull()
{
	if (isNull())
		return m_value.null;
	throw InvalidTypeAccess();
}

std::unordered_map<std::string, Json>& Json::getObject()
{
	if (isObject())
		return m_value.object;
	throw InvalidTypeAccess();
}

std::vector<Json>& Json::getArray()
{
	if (isArray())
		return m_value.array;
	throw InvalidTypeAccess();
}

const std::string& Json::getString() const
{
	if (isString())
		return m_value.string;
	throw InvalidTypeAccess();
}

double Json::getNumber() const
{
	if (isNumber())
		return m_value.number;
	throw InvalidTypeAccess();
}

bool Json::getBoolean() const
{
	if (isBoolean())
		return m_value.boolean;
	throw InvalidTypeAccess();
}

std::nullptr_t Json::getNull() const
{
	if (isNull())
		return m_value.null;
	throw InvalidTypeAccess();
}

const std::unordered_map<std::string, Json>& Json::getObject() const
{
	if (isObject())
		return m_value.object;
	throw InvalidTypeAccess();
}

const std::vector<Json>& Json::getArray() const
{
	if (isArray())
		return m_value.array;
	throw InvalidTypeAccess();
}

void Json::destructValue()
{
	switch (m_type)
	{
	case Json::Type::String:
		m_value.string.~basic_string<char>();
		break;
	case Json::Type::Object:
		m_value.object.~unordered_map<std::string, Json>();
		break;
	case Json::Type::Array:
		m_value.array.~vector();
		break;
	}	
}

Json::Type Json::type() const
{
	return m_type;
}

// Union destructors and constructors are implicitly deleted
Json::Value::Value()
// This is useless I just don't want the warnings about uninitialized member variables
// and it would be more trouble to make a cross platform warning disable with #pragma.
	: null(nullptr)
{}
 
Json::Value::~Value()
{}

bool Json::isString() const
{
	return m_type == Type::String;
}

bool Json::isNumber() const
{
	return m_type == Type::Number;
}

bool Json::isBoolean() const
{
	return m_type == Type::Boolean;
}

bool Json::isNull() const
{
	return m_type == Type::Null;
}

bool Json::isObject() const
{
	return m_type == Type::Object;
}

bool Json::isArray() const
{
	return m_type == Type::Array;
}