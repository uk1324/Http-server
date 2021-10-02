#include "Http/HttpServer.h"

#include "App/HttpApp.h"

#include <cstring>
#include "Json/Json.h"
#include "Json/JsonParser.h"
#include <regex>

#include "Json/JsonPrinter.h"
#include "Utf8/Utf8String.h"

// Make generic scanner and parser classes
// Try to use more asserts
// Namespaces
// Unnamed union variables

// Move all includes to .cpp files

// Improve to string methods of HTTP REQUEST AND RESPNSE
#include <bitset>

//#define RUN_TESTS

#include <variant>

#ifndef RUN_TESTS


int main(int argc, char* argv[])
{

	//try
	//{
	//	if (argc == 2)
	//	{
	//		std::ifstream file(argv[1], std::ios::binary);
	//		//std::streamsize size = file.tellg();

	//		std::string buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

	//		//file.seekg(0, std::ios::beg);

	//		std::cerr << argv[1] << '\n';
	//		//std::cerr << buffer << '\n';
	//		//std::string buffer;
	//		//buffer.reserve(size);
	//		//file.read(buffer.data(), size);
	//		try
	//		{
	//			JsonParser::parse(buffer);
	//			return 0;
	//		}
	//		catch (const JsonParser::ParsingError& e)
	//		{
	//			//std::cerr << e.what() << '\n';
	//			return 1;
	//		}
	//	}
	//	//else
	//	//{
	//	//	return 1;
	//	//}
	//}
	//catch (const std::exception&)
	//{
	//	return 3;
	//}

	//std::string_view view;

	//view.

	//sizeof(a);

	//char a[] = u8"🎋";
	//
	//for (char chr : a)
	//{
	//	std::cout << std::bitset<8>(chr) << '\n';
	//}
	////	auto b = countRequiredOctets(chr);

	////	if (b.has_value())
	////	{
	////		std::cout << b.value() << '\n';
	////	}
	////	else
	////	{
	////		std::cout << "bad" << '\n';
	////	}
	////}

	std::string string = u8"{\"foo\":3.14,\"bar\":[2,4,8,16],\"baz\":{\"beep\": true}, \"bop\": 2.1e+2, \"bep\": \"🎋\"}";
	//std::string string = "[123.456e-789]";
	//std::string string = u8"[123ĺ]";
	////////std::string string = "{\"widget\":{\"debug\":\"on\",\"window\":{\"title\":\"Sample Konfabulator Widget\",\"name\":\"main_window\",\"width\":500,\"height\":500},\"image\":{\"src\":\"Images/Sun.png\",\"name\":\"sun1\",\"hOffset\":250,\"vOffset\":250,\"alignment\":\"center\"},\"text\":{\"data\":\"Click Here\",\"size\":36,\"style\":\"bold\",\"name\":\"text1\",\"hOffset\":250,\"vOffset\":100,\"alignment\":\"center\",\"onMouseUp\":\"sun1.opacity = (sun1.opacity / 100) * 90;\"}}}";
	////////std::string string = "{\"medications\":[{\"aceInhibitors\":[{\"name\":\"lisinopril\",\"strength\":\"10 mg Tab\",\"dose\":\"1 tab\",\"route\":\"PO\",\"sig\":\"daily\",\"pillCount\":\"#90\",\"refills\":\"Refill 3\"}],\"antianginal\":[{\"name\":\"nitroglycerin\",\"strength\":\"0.4 mg Sublingual Tab\",\"dose\":\"1 tab\",\"route\":\"SL\",\"sig\":\"q15min PRN\",\"pillCount\":\"#30\",\"refills\":\"Refill 1\"}],\"anticoagulants\":[{\"name\":\"warfarin sodium\",\"strength\":\"3 mg Tab\",\"dose\":\"1 tab\",\"route\":\"PO\",\"sig\":\"daily\",\"pillCount\":\"#90\",\"refills\":\"Refill 3\"}],\"betaBlocker\":[{\"name\":\"metoprolol tartrate\",\"strength\":\"25 mg Tab\",\"dose\":\"1 tab\",\"route\":\"PO\",\"sig\":\"daily\",\"pillCount\":\"#90\",\"refills\":\"Refill 3\"}],\"diuretic\":[{\"name\":\"furosemide\",\"strength\":\"40 mg Tab\",\"dose\":\"1 tab\",\"route\":\"PO\",\"sig\":\"daily\",\"pillCount\":\"#90\",\"refills\":\"Refill 3\"}],\"mineral\":[{\"name\":\"potassium chloride ER\",\"strength\":\"10 mEq Tab\",\"dose\":\"1 tab\",\"route\":\"PO\",\"sig\":\"daily\",\"pillCount\":\"#90\",\"refills\":\"Refill 3\"}]}],\"labs\":[{\"name\":\"Arterial Blood Gas\",\"time\":\"Today\",\"location\":\"Main Hospital Lab\"},{\"name\":\"BMP\",\"time\":\"Today\",\"location\":\"Primary Care Clinic\"},{\"name\":\"BNP\",\"time\":\"3 Weeks\",\"location\":\"Primary Care Clinic\"},{\"name\":\"BUN\",\"time\":\"1 Year\",\"location\":\"Primary Care Clinic\"},{\"name\":\"Cardiac Enzymes\",\"time\":\"Today\",\"location\":\"Primary Care Clinic\"},{\"name\":\"CBC\",\"time\":\"1 Year\",\"location\":\"Primary Care Clinic\"},{\"name\":\"Creatinine\",\"time\":\"1 Year\",\"location\":\"Main Hospital Lab\"},{\"name\":\"Electrolyte Panel\",\"time\":\"1 Year\",\"location\":\"Primary Care Clinic\"},{\"name\":\"Glucose\",\"time\":\"1 Year\",\"location\":\"Main Hospital Lab\"},{\"name\":\"PT/INR\",\"time\":\"3 Weeks\",\"location\":\"Primary Care Clinic\"},{\"name\":\"PTT\",\"time\":\"3 Weeks\",\"location\":\"Coumadin Clinic\"},{\"name\":\"TSH\",\"time\":\"1 Year\",\"location\":\"Primary Care Clinic\"}],\"imaging\":[{\"name\":\"Chest X-Ray\",\"time\":\"Today\",\"location\":\"Main Hospital Radiology\"},{\"name\":\"Chest X-Ray\",\"time\":\"Today\",\"location\":\"Main Hospital Radiology\"},{\"name\":\"Chest X-Ray\",\"time\":\"Today\",\"location\":\"Main Hospital Radiology\"}]}";
	Json object;

	try
	{
		object = JsonParser::parse(string);
	}
	catch (const JsonParser::ParsingError& error)
	{
		std::cout << error.what() << '\n';
		std::cout << error.charIndex() << '\n';
	}

	//object = "abc";

	////std::ostringstream s;
	JsonPrinter::prettyPrint(std::cout, object);
	//std::cout << object << '\n';


	//s << object;

	//std::ofstream f("test.txt");

	//std::cout << JsonPrinter::escape(s.str());

	//Json json = {
	//	{"pi", 3.141},
	//	{"happy", true},
	//	{"name", "Niels"},
	//	{"nothing", nullptr},
	//	{"answer", {
	//		{"everything", 42}
	//	}},
	//	{"list", {1, 0, 2}},
	//	{"object", {
	//		{"currency", "USD"},
	//		{"value", 42.99}
	//	}}
	//};
	
	//std::cout << json << '\n';




	//Json array = Json::emptyArray();

	//std::cout << "is arr " << array.isArray() << '\n';
	//array.array().emplace_back(3.2);
	//std::cout << array;

	//HttpApp app;

	//app.get("/test", std::static_pointer_cast<RequestHandler>(std::shared_ptr<Test>(new Test)));

	//app.listen(8080);
	//Tests::HttpRequestParserTests a("HttpParsingUtilsTests");
	//a.runAllTests();
	//HttpRequestTarget a;

	//Json a = { 1, 2, 3 };

	//Json b = Json("abc");

	//HttpRequestTarget r("/a/b");

	//std::cout << r.path() << " " << r.query() << '\n';
	//std::cout << r.segments().size() << '\n';
	//for (const auto& a : r.segments())
	//{
	//	std::cout << a << '\n';
	//}
	//while (true)
	//{

	//	Json json = {
	//		{"pi", 3.141},
	//		{"happy", true},
	//		{"name", "Niels"},
	//		{"nothing", nullptr},
	//		{"answer", {
	//			{"everything", 42}
	//		}},
	//		{"list", {1, 0, 2}},
	//		{"object", {
	//			{"currency", "USD"},
	//			{"value", 42.99}
	//		}}
	//	};

	//	Json a = "Hello";
	//	Json b = json;
	//	Json c = { 1, 2, 3 };
	//}

	//std::cout << json << '\n';

	//std::cout << json["f"]["abc"].number() << '\n';

	//Json json = { {"abc", 2}, { "abc", 2 } };
	//HttpServer server;
	//server.listen(8080);
}


#else
	#undef min
	#undef max
    #define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
    #include "Test/catch2/catch.h"
#endif

//unsigned int Factorial(unsigned int number) {
//    return number <= 1 ? number : Factorial(number - 1) * number;
//}
//
//TEST_CASE("Factorials are computed", "[factorial]") {
//    REQUIRE(Factorial(1) == 1);
//    REQUIRE(Factorial(1) == 3);
//    REQUIRE(Factorial(2) == 2);
//    REQUIRE(Factorial(3) == 6);
//    REQUIRE(Factorial(10) == 3628800);
//}








//#include <cassert>
//void copy(char* source, volatile char* destination, int count)
//{
//	for (int i = 0; i < count; i++)
//	{
//		source[i] = destination[i];
//	}
//}
//int main()
//{
//	//Tests::HttpRequestParserTests a("HttpParsingUtilsTests");
//	//a.runAllTests();
//
//	volatile char volatile request1[] = "GET /wp-content/uploads/2010/03/hello-kitty-darth-vader-pink.jpg HTTP/1.1\r\n"
//		"Host: www.kittyhell.com\r\n"
//		"User-Agent: Mozilla/5.0 (Macintosh; U; Intel Mac OS X 10.6; ja-JP-mac; rv:1.9.2.3) Gecko/20100401 Firefox/3.6.3 Pathtraq/0.9\r\n"
//		"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
//		"Accept-Language: ja,en-us;q=0.7,en;q=0.3\r\n"
//		"Accept-Encoding: gzip,deflate\r\n"
//		"Accept-Charset: Shift_JIS,utf-8;q=0.7,*;q=0.7\r\n"
//		"Keep-Alive: 115\r\n"
//		"Connection: keep-alive\r\n"
//		"Cookie: wp_ozh_wsa_visits=2; wp_ozh_wsa_visit_lasttime=xxxxxxxxxx; __utma=xxxxxxxxx.xxxxxxxxxx.xxxxxxxxxx.xxxxxxxxxx.xxxxxxxxxx.x; __utmz=xxxxxxxxx.xxxxxxxxxx.x.x.utmccn=(referral)|utmcsr=reader.livedoor.com|utmcct=/reader/|utmcmd=referral\r\n\r\n";
//
//	volatile char volatile request2 [] = "GET / HTTP/1.1\r\n"
//		"Host: www.reddit.com\r\n"
//		"User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10.8; rv:15.0) Gecko/20100101 Firefox/15.0.1\r\n"
//		"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
//		"Accept-Language: en-us,en;q=0.5\r\n"
//		"Accept-Encoding: gzip, deflate\r\n"
//		"Connection: keep-alive\r\n\r\n";
//
//	char buffer[1000];
//
//
//
//	std::unordered_map<int, std::pair<volatile char*, volatile int>> map = {
//		{0, {request1, sizeof(request1)}},
//		{1, {request2, sizeof(request2)}}
//	};
//
//	//copy(buffer, request, sizeof(request));
//
//	HttpRequestParser parser(buffer);
//
//	long long elapsed = 0;
//
//	//for (int i = 0; i < 10000; i++)
//	int i = 0;
//
//	HttpRequest request;
//
//	int n = 100000;
//
//	for (i = 0; i < n; i++)
//	//while (true)
//	{
//		int r = rand() % map.size();
//		//std::cout << "random: " << (char*)map[r].first << '\n';
//		auto item = map[r];
//		copy(buffer, item.first, item.second);
//		auto start = std::chrono::high_resolution_clock::now();
//
//		HttpRequestParser::Status interrupt = parser.parse(item.second, &request);
//
//		auto end = std::chrono::high_resolution_clock::now();
//
//		if (interrupt != HttpRequestParser::Status::Complete) { std::cout << "bad bad\n"; exit(-1); }
//		if (r == 0 && request.requestTarget.path() != "/wp-content/uploads/2010/03/hello-kitty-darth-vader-pink.jpg") { std::cout << "bad bad\n"; exit(-1); }
//		if (r == 1 && request.requestTarget.path() != "/") {	std::cout << "bad bad\n"; exit(-1);}
//		if (request.headers["Connection"] != "keep-alive") { std::cout << "bad bad\n"; exit(-1); }
//		auto delta = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
//		elapsed += delta;
//
//		if (elapsed >= 1e+9)
//			break;
//
//		//std::cout << parser.result().requestTarget.path() << '\n';;
//		//std::cout << "elapsed: " << delta << '\n';
//		//i++;
//		//std::cout << "elapsed: " << (double)elapsed / double(1e+9) << " i: " << i << '\n';
//	}
//	std::cout << "elapsed: " << elapsed << '\n';
//	std::cout << "avg: " << elapsed / i << '\n';
//	std::cout << "parsed: " << i << '\n';
//	//std::cout << "Elapsed: " << elapsed << '\n';
//	//const char* buffer = "GET / HTTP/1.1\r\nContent-Length: 12\r\n\r\nHello world!";
//
//	//HttpRequestParser parser(buffer);
//
//	//HttpRequestParser::Interrupt interrupt = parser.parse(sizeof(buffer));
//	//std::cout << int(interrupt) << '\n';
//
//	//HttpRequest request = parser.result();
//	//std::cout << static_cast<int>(request.method) << '\n';
//	//std::cout << request.requestTarget.path() << '\n';
//	//std::cout << static_cast<int>(request.version) << '\n';
//
//	//for (auto& [name, value] : request.headers)
//	//{
//	//	std::cout << name << "\t" << value << '\n';
//	//}
//
//	//std::cout << request.body << '\n';
//
//	//HttpServer server;
//	//try
//	//{
//	//	server.listen(8080);
//	//}
//	//catch (std::exception& err)
//	//{
//	//	throw;
//	//	std::cout << err.what() << '\n';
//	//}
//}