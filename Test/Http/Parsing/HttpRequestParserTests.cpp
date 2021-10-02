#include "../../catch2/catch.h"
#include "../../../Http/Parsing/HttpRequestParser.h"

#include <variant>

static std::variant<HttpRequest, HttpRequestParser::Error> parseHttp(std::initializer_list<const char*> buffers)
{
	char buffer[1024];

	HttpRequestParser parser(buffer);
	HttpRequest request;

	for (auto data : buffers)
	{
		size_t length = strlen(data);
		memcpy(buffer, data, length);
		auto status = parser.parse(length, request);

		if (status == HttpRequestParser::Status::Complete)
			return request;
		else if (status == HttpRequestParser::Status::Error)
			return parser.lastError();
	}

	throw nullptr;
}

TEST_CASE("no headers", "[HttpRequestParser][Http]")
{
	auto requestOrError = parseHttp({ "GET / HTTP/1.1\r\n\r\n" });
	HttpRequest request;
	REQUIRE_NOTHROW(request = std::get<HttpRequest>(requestOrError));
	CHECK(request.method == HttpMethod::Get);
	CHECK(request.requestTarget.path() == "/");
	CHECK(request.version == HttpVersion::Http11);
}

TEST_CASE("with headers", "[HttpRequestParser][Http]")
{
	auto requestOrError = parseHttp({
		"GET /test HTTP/1.0\r\n"
		"User-Agent: curl/7.18.0\r\n"
		"Host: localhost:8080\r\n"
		"Accept: */*\r\n\r\n"
	});
	HttpRequest request;
	REQUIRE_NOTHROW(request = std::get<HttpRequest>(requestOrError));
	CHECK(request.method == HttpMethod::Get);
	CHECK(request.requestTarget.path() == "/test");
	CHECK(request.version == HttpVersion::Http10);
	CHECK(request.headers["User-agent"] == "curl/7.18.0");
	CHECK(request.headers["Host"] == "localhost:8080");
	CHECK(request.headers["Accept"] == "*/*");
}

TEST_CASE("with body", "[HttpRequestParser][Http]")
{
	auto requestOrError = parseHttp({
		"POST /test HTTP/1.1\r\n"
		"Connection: keep-alive\r\n"
		"Content-Length: 11\r\n\r\n"
		"Hello there"
	});
	HttpRequest request;
	REQUIRE_NOTHROW(request = std::get<HttpRequest>(requestOrError));
	CHECK(request.method == HttpMethod::Post);
	CHECK(request.requestTarget.path() == "/test");
	CHECK(request.version == HttpVersion::Http11);
	CHECK(request.headers["Connection"] == "keep-alive");
	CHECK(request.headers["Content-Length"] == "11");
	CHECK(request.body == "Hello there");
}

TEST_CASE("split request", "[HttpRequestParser][Http]")
{
	auto requestOrError = parseHttp({
		"PATCH /path HTTP/1.1\r\nHo",
		"st: google.com\r\n",
		"Content-Length: ",
		" 12\r\nOrigin:",
		"test"
		".com\r\n"
		"Accept: text/html\r\n\r\nHello",
		" world!",
	});
	HttpRequest request;
	REQUIRE_NOTHROW(request = std::get<HttpRequest>(requestOrError));
	CHECK(request.method == HttpMethod::Patch);
	CHECK(request.requestTarget.path() == "/path");
	CHECK(request.version == HttpVersion::Http11);
	CHECK(request.headers["Host"] == "google.com");
	CHECK(request.headers["Content-Length"] == "12");
	CHECK(request.headers["Origin"] == "test.com");
	CHECK(request.headers["Accept"] == "text/html");
	CHECK(request.body == "Hello world!");
}

TEST_CASE("invalid content-length value", "[HttpRequestParser][Http]")
{
	auto requestOrError = parseHttp({
		"POST /test HTTP/1.1\r\n"
		"Content-Length: abc\r\n\r\n"
		"Hello there"
	});
	HttpRequestParser::Error error;
	REQUIRE_NOTHROW(error = std::get<HttpRequestParser::Error>(requestOrError));
	REQUIRE(error == HttpRequestParser::Error::InvalidHeaderValue);
}