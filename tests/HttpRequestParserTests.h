#pragma once

#include "TestingUtils.h"
#include "../Parsing/HttpRequestParser.h"

namespace Tests
{
	class HttpRequestParserTests : TestCase
	{
		using TestCase::TestCase;

		void methodParsingTest()
		{
			char buffer[] = "GET / HTTP/1.1\r\n\r\n";

			HttpRequestParser::Interrupt status = HttpRequestParser::Interrupt::reachedBufferEnd;
			HttpRequestParser parser(buffer);
			try
			{
				parser.parse(sizeof(buffer) - 1);
			}
			catch (HttpRequestParser::Interrupt interrupt)
			{
				status = interrupt;
			}
			assertTrue(
				status == HttpRequestParser::Interrupt::finishedParsingFullRequest,
				"Parsing failed got interrupt " + std::to_string(static_cast<int>(status))
			);
			assertTrue(
				parser.result().method == HttpMethod::Get,
				"Method doesn't match got " + std::to_string(static_cast<int>(parser.result().method))
			);
		}

		void curlGetTest()
		{
			char buffer[] = "GET /test HTTP/1.1\r\n"
				"User-Agent: curl/7.18.0 (i486-pc-linux-gnu) libcurl/7.18.0 OpenSSL/0.9.8g zlib/1.2.3.3 libidn/1.1\r\n"
				"Host: localhost:8080\r\n"
				"Accept: */*\r\n\r\n";

			HttpRequestParser::Interrupt status = HttpRequestParser::Interrupt::reachedBufferEnd;
			HttpRequestParser parser(buffer);
			try
			{
				parser.parse(sizeof(buffer) - 1);
			}
			catch (HttpRequestParser::Interrupt interrupt)
			{
				status = interrupt;
			}
			assertTrue(
				status == HttpRequestParser::Interrupt::finishedParsingFullRequest,
				"Parsing failed got interrupt " + std::to_string(static_cast<int>(status))
			);
			assertTrue(
				parser.result().method == HttpMethod::Get,
				"Method doesn't match got " + std::to_string(static_cast<int>(parser.result().method))
			);
			assertEquals<std::string>(parser.result().requestTarget.path(), "/test");
			assertEquals<std::string>(parser.result().headers["User-Agent"], "curl/7.18.0 (i486-pc-linux-gnu) libcurl/7.18.0 OpenSSL/0.9.8g zlib/1.2.3.3 libidn/1.1");
			assertEquals<std::string>(parser.result().headers["Host"], "localhost:8080");
			assertEquals<std::string>(parser.result().headers["Accept"], "*/*");
		}

		void firefoxPostTest()
		{
			char buffer[] = "POST / HTTP/1.1\r\n"
				"Host: github.com\r\n"
				"User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:89.0) Gecko/20100101 Firefox/89.0\r\n"
				"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\n"
				"Accept-Language: pl,en-US;q=0.7,en;q=0.3\r\n"
				"Accept-Encoding: gzip, deflate, br\r\n"
				"Referer: https://github.com/login\r\n"
				"Content-Type: application/x-www-form-urlencoded\r\n"
				"Content-Length: 7\r\n"
				"Origin: https://github.com\r\n"
				"Connection: keep-alive\r\n"
				"Upgrade-Insecure-Requests: 1\r\n"
				"TE: Trailers\r\n\r\n"
				"abcdefg";

			HttpRequestParser::Interrupt status = HttpRequestParser::Interrupt::reachedBufferEnd;
			HttpRequestParser parser(buffer);
			try
			{
				parser.parse(sizeof(buffer) - 1);
			}
			catch (HttpRequestParser::Interrupt interrupt)
			{
				status = interrupt;
			}
			assertTrue(
				status == HttpRequestParser::Interrupt::finishedParsingFullRequest,
				"Parsing failed got interrupt " + std::to_string(static_cast<int>(status))
			);
			assertTrue(
				parser.result().method == HttpMethod::Post,
				"Method doesn't match got " + std::to_string(static_cast<int>(parser.result().method))
			);
			assertEquals<std::string>(parser.result().requestTarget.path(), "/");
			assertEquals<std::string>(parser.result().headers["User-Agent"], "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:89.0) Gecko/20100101 Firefox/89.0");
			assertEquals<std::string>(parser.result().headers["Host"], "github.com");
			assertEquals<std::string>(parser.result().headers["Accept"], "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8");
			assertEquals<std::string>(parser.result().headers["Accept-Language"], "pl,en-US;q=0.7,en;q=0.3");
			assertEquals<std::string>(parser.result().headers["Accept-Encoding"], "gzip, deflate, br");
			assertEquals<std::string>(parser.result().headers["Referer"], "https://github.com/login");
			assertEquals<std::string>(parser.result().headers["Referer"], "https://github.com/login");
			assertEquals<std::string>(parser.result().headers["Content-Type"], "application/x-www-form-urlencoded");
			assertEquals<std::string>(parser.result().headers["Content-Length"], "7");
			assertEquals<std::string>(parser.result().headers["Origin"], "https://github.com");
			assertEquals<std::string>(parser.result().headers["Connection"], "keep-alive");
			assertEquals<std::string>(parser.result().headers["TE"], "Trailers");
			assertEquals<std::string>(parser.result().body, "abcdefg");
		}

		void splitRequestTest()
		{
			char buffer[200] = "PATCH /path HTTP/1.1\r\nHo";

			HttpRequestParser::Interrupt status = HttpRequestParser::Interrupt::reachedBufferEnd;
			HttpRequestParser parser(buffer);
			try { parser.parse(strlen(buffer)); }
			catch (HttpRequestParser::Interrupt interrupt) { status = interrupt; }
			assertTrue(status == HttpRequestParser::Interrupt::reachedBufferEnd, "Parsing failed got interrupt " + std::to_string(static_cast<int>(status)));


			strcpy(buffer, "st: google.com");
			try { parser.parse(strlen(buffer)); }
			catch (HttpRequestParser::Interrupt interrupt) { status = interrupt; }
			assertTrue(status == HttpRequestParser::Interrupt::reachedBufferEnd, "Parsing failed got interrupt " + std::to_string(static_cast<int>(status)));

			strcpy(buffer, "\r\nContent-Length: ");
			try { parser.parse(strlen(buffer)); }
			catch (HttpRequestParser::Interrupt interrupt) { status = interrupt; }
			assertTrue(status == HttpRequestParser::Interrupt::reachedBufferEnd, "Parsing failed got interrupt " + std::to_string(static_cast<int>(status)));

			strcpy(buffer, "12\r\nAccept: text/html\r\n\r\nHello");
			try { parser.parse(strlen(buffer)); }
			catch (HttpRequestParser::Interrupt interrupt) { status = interrupt; }
			assertTrue(status == HttpRequestParser::Interrupt::reachedBufferEnd, "Parsing failed got interrupt " + std::to_string(static_cast<int>(status)));

			strcpy(buffer, " world!");
			try { parser.parse(strlen(buffer)); }
			catch (HttpRequestParser::Interrupt interrupt) { status = interrupt; }
			assertTrue(status == HttpRequestParser::Interrupt::finishedParsingFullRequest, "Parsing failed got interrupt " + std::to_string(static_cast<int>(status)));

			assertTrue(
				parser.result().method == HttpMethod::Patch,
				"Method doesn't match got " + std::to_string(static_cast<int>(parser.result().method))
			);
			
			assertEquals<std::string>(parser.result().requestTarget.path(), "/path");
			assertEquals<std::string>(parser.result().headers["Host"], "google.com");
			assertEquals<std::string>(parser.result().headers["Content-Length"], "12");
			assertEquals<std::string>(parser.result().headers["Accept"], "text/html");
			assertEquals<std::string>(parser.result().body, "Hello world!");
		}

	public:
		void runAllTests()
		{
			displayRunningMessage();
			// doesn't break when it method starts parsing when reachBufferEnd
			methodParsingTest();
			curlGetTest();
			firefoxPostTest();
			splitRequestTest();

			displayTestsPassed();
		}
	};
}