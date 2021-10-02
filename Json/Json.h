#pragma once

#include <iostream>
#include <unordered_map>
#include <new>

class Json
{
public:
	class InvalidTypeAccess : public std::exception {};
	class OutOfRangeAccess : public std::exception {};

	Json();
	static Json emptyObject();
	static Json emptyArray();

	Json(const std::string& string);
	Json(std::string&& string) noexcept;
	Json(const char* string);
	Json(double number);
	Json(int number);
	Json(bool boolean);
	Json(std::nullptr_t);
	Json(std::initializer_list<std::pair<const std::string, Json>> object);

	Json(std::initializer_list<double> array);
	Json(std::initializer_list<int> array);
	Json(std::initializer_list<bool> array);

	Json(const Json& json);
	Json(Json&& json) noexcept;

	~Json();

	Json& operator= (const std::string& string);
	Json& operator= (std::string&& string);
	Json& operator= (const char* string);
	Json& operator= (double number);
	Json& operator= (int number);
	Json& operator= (bool boolean);
	Json& operator= (std::nullptr_t);
	Json& operator= (const Json& object);
	Json& operator= (Json&& object) noexcept;

	// Exibits undefined behaviour if this->m_type is not an object.
	// If key does not exists creates it.
	Json& operator[] (const std::string& key);
	Json& operator[] (std::string&& key) noexcept;

	// Throws InvalidTypeAccess when not this->type != Type::Object.
	// Throws OutOfRangeAccess when key doesn't exist.
	Json& at(const std::string& key);
	const Json& at(const std::string& key) const;

	bool contains(const std::string& key) const;

	bool isNumber() const;
	bool isString() const;
	bool isBoolean() const;
	bool isNull() const;
	bool isObject() const;
	bool isArray() const;

	// Exibits undefined behaviour if the type is not correct
	std::string& string();
	double& number();
	bool& boolean();
	std::nullptr_t& null();
	std::unordered_map<std::string, Json>& object();
	std::vector<Json>& array();

	const std::string& string() const;
	double number() const;
	bool boolean() const;
	std::nullptr_t null() const;
	const std::unordered_map<std::string, Json>& object() const;
	const std::vector<Json>& array() const;

	// Type checked access
	std::string& getString();
	double& getNumber();
	bool& getBoolean();
	std::nullptr_t& getNull();
	std::unordered_map<std::string, Json>& getObject();
	std::vector<Json>& getArray();

	const std::string& getString() const;
	double getNumber() const;
	bool getBoolean() const;
	std::nullptr_t getNull() const;
	const std::unordered_map<std::string, Json>& getObject() const;
	const std::vector<Json>& getArray() const;

private:

	template <typename T>
	void setArray(std::initializer_list<T> array);

	void destructValue();

	union Value
	{
		Value();
		~Value();

		double number;
		std::string string;
		bool boolean;
		std::nullptr_t null;
		std::unordered_map<std::string, Json> object;
		std::vector<Json> array;
	};

public:

	enum class Type
	{
		String,
		Number,
		Boolean,
		Null,
		Object,
		Array,
	};

	Type type() const;

private:

	Type m_type;
	Value m_value;
};

template<typename T>
void Json::setArray(std::initializer_list<T> array)
{
	new(&m_value.array) std::vector<Json>();
	m_value.array.reserve(array.size());
	for (T item : array)
		m_value.array.push_back(Json(item));
}
