#include "HttpServer.h"

void HttpServer::listen(uint16_t port)
{
	m_socket.listen(port);

	while (true)
	{
		try
		{
			TcpSocket client = m_socket.accept();
			std::thread clientThread([this, client = std::move(client)]() mutable {
				this->handleClient(std::move(client));
			});

			clientThread.detach();
		}
		catch (const SocketError& error)
		{
			std::cout << error.what();
		}
	}
}

void HttpServer::handleClient(TcpSocket&& socket)
{
	static constexpr int messageBufferSize = 4096;
	//static constexpr int messageBufferSize = 20;
	char messageBuffer[messageBufferSize];

	HttpRequestParser parser(messageBuffer);
	HttpRequest request;

	while (true)
	{
		int messageSize;
		try
		{
			socket.receive(messageBuffer, messageBufferSize, &messageSize);
		}
		catch (const SocketError& error)
		{
			std::cout << error.what();
		}

		if (messageSize > 0)
		{
			auto start = std::chrono::system_clock::now();
			auto end = std::chrono::system_clock::now();

			// Parsing can be split between multiple receive calls
			start = std::chrono::system_clock::now();
			HttpRequestParser::Status status = parser.parse(messageSize, request);
			//std::cout << int(status) << ' ' << int(parser.lastError()) << '\n';

			if (status == HttpRequestParser::Status::Complete)
			{
				end = std::chrono::system_clock::now();
				std::cout << "elapsed: " << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() << '\n';;
				handleRequest(socket, request);
			}
			// Continute parsing on reachedBufferEnd
			// Could just change to if Status::Error
			else if (status != HttpRequestParser::Status::Partial)
			{
				handleInvalidRequest(socket, parser.lastError());
			}
		}
		else
		{
			// End connection on empty message or error
			//socket.close();
			return;
		}
	}
}

//void HttpServer::handleRequest(TcpSocket& socket, const HttpRequest& request)
//{
//	std::cout << static_cast<int>(request.method) << '\n';
//	std::cout << request.requestTarget.path() << '\n';
//	std::cout << static_cast<int>(request.version) << '\n';
//
//	for (auto& [name, value] : request.headers)
//	{
//		std::cout << name << "\t" << value << '\n';
//	}
//
//	std::cout << request.body << '\n';
//
//	HttpResponse response;
//	std::string message = "Test message";
//
//	response.headers.setHeaderInt("Content-Length", message.length());
//
//	std::string responseString = response.toString();
//
//	socket.send(response.toString().data(), responseString.length());
//	socket.send(message.data(), message.length());
//}
//
//void HttpServer::handleInvalidRequest(TcpSocket& socket, HttpRequestParser::Error error)
//{
//	char message[] = "HTTP/1.1 200 OK\r\nContent-Length: 15\r\n\r\nInvalid request";
//	char buffer[sizeof(message)];
//	strcpy(buffer, message);
//	socket.send(buffer, sizeof(buffer));
//	// maybe use a unordered_map of Interrupt and function
//}

//void HttpServer::sendFile(int socket, const char* path, int fileSize)
//{
//	std::ifstream file(path, std::ifstream::binary);
//	constexpr int bufferSize = 10;
//	char buffer[bufferSize];
//	while (fileSize > 0)
//	{
//		int readSize = bufferSize < fileSize ? bufferSize : fileSize;
//		file.read(buffer, readSize);
//		send(socket, buffer, readSize, 0);
//		fileSize -= readSize;
//	}
//}