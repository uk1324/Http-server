#include "../catch2/catch.h"
#include "../../Json/JsonParser.h"

TEST_CASE("integers", "[JsonParser][Json]")
{
	SECTION("positive")
	{
		Json result = JsonParser::parse("213");
		REQUIRE(result.getNumber() == 213);
	}

	SECTION("negative")
	{
		Json result = JsonParser::parse("-61");
		REQUIRE(result.getNumber() == -61);
	}

	SECTION("don't allow leading zeros")
	{
		REQUIRE_THROWS_AS(JsonParser::parse("01"), JsonParser::ParsingError);
	}
}

TEST_CASE("floats", "[JsonParser][Json]")
{
	SECTION("simple float")
	{
		Json result = JsonParser::parse("3.75");
		REQUIRE(result.getNumber() == 3.75);
	}
}

TEST_CASE("exponent", "[JsonParser][Json]")
{
	SECTION("positive exponent")
	{
		Json result = JsonParser::parse("3e3");
		REQUIRE(result.getNumber() == 3000);
	}

	SECTION("negative exponent")
	{
		Json result = JsonParser::parse("2.125e-3");
		REQUIRE(result.getNumber() == 0.002125);
	}

	SECTION("negative number negative exponent")
	{
		Json result = JsonParser::parse("-2.125e-3");
		REQUIRE(result.getNumber() == -0.002125);
	}

	SECTION("large exponent")
	{
		// Number can't fit in double.
		REQUIRE_THROWS_AS(JsonParser::parse("10e1000"), JsonParser::ParsingError);
	}

	SECTION("empty exponent")
	{
		REQUIRE_THROWS_AS(JsonParser::parse("10e"), JsonParser::ParsingError);
	}
}

TEST_CASE("booleans", "[JsonParser][Json]")
{
	SECTION("true")
	{
		Json result = JsonParser::parse("true");
		REQUIRE(result.getBoolean() == true);
	}

	SECTION("false")
	{
		Json result = JsonParser::parse("false");
		REQUIRE(result.getBoolean() == false);
	}
}

TEST_CASE("strings", "[JsonParser][Json]")
{
	SECTION("simple string")
	{
		Json result = JsonParser::parse(R"("hello")");
		REQUIRE(result.getString() == "hello");
	}

}

TEST_CASE("empty object", "[JsonParser][Json]")
{
	Json result = JsonParser::parse("{}");
	REQUIRE(result.type() == Json::Type::Object);
}

TEST_CASE("object single property", "[JsonParser][Json]")
{
	Json result = JsonParser::parse(R"({"x": 123})");
	REQUIRE(result["x"].getNumber() == 123);
}

TEST_CASE("object multiple properties", "[JsonParser][Json]")
{
	Json result = JsonParser::parse(R"({"a": 321, "b": true, "c": "abc"})");
	CHECK(result["a"].getNumber() == 321);
	CHECK(result["b"].getBoolean() == true);
	CHECK(result["c"].getString() == "abc");
}

TEST_CASE("array", "[JsonParser][Json]")
{
	Json result = JsonParser::parse(R"([null, 3, "abc"])");
	CHECK(result.getArray()[0].type() == Json::Type::Null);
	CHECK(result.getArray()[1].getNumber() == 3);
	CHECK(result.getArray()[2].getString() == "abc");
}

TEST_CASE("trailing comma", "[JsonParser][Json]")
{
	SECTION("object")
	{
		REQUIRE_THROWS_AS(JsonParser::parse(R"({"a": 1,})"), JsonParser::ParsingError);
	}

	SECTION("array")
	{
		REQUIRE_THROWS_AS(JsonParser::parse("[2,]"), JsonParser::ParsingError);
	}
}