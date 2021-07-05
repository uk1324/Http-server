#include "HttpServer.h"

#include <type_traits>

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
	char messageBuffer[messageBufferSize];

	HttpRequestParser parser(messageBuffer);

	while (true)
	{
		int messageSize;
		try
		{
			socket.receive(messageBuffer, messageBufferSize, &messageSize);
		}
		catch (SocketError& error)
		{
			std::cout << error.what();
		}

		if (messageSize > 0)
		{
			try
			{
				// Parsing can be split between multiple receive calls
				parser.parse(messageSize);
			}
			catch (HttpRequestParser::Interrupt interrupt)
			{
				if (interrupt == HttpRequestParser::Interrupt::finishedParsingFullRequest)
					handleRequest(socket, parser.result());
				// Continute parsing on reachedBufferEnd
				else if (interrupt != HttpRequestParser::Interrupt::reachedBufferEnd)
					handleInvalidRequest(socket, parser.result(), interrupt);
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

void HttpServer::handleRequest(TcpSocket& socket, const HttpRequest& request)
{
	std::cout << static_cast<int>(request.method) << '\n';
	std::cout << request.requestTarget.path() << '\n';
	std::cout << static_cast<int>(request.version) << '\n';

	for (auto& [name, value] : request.headers)
	{
		std::cout << name << "\t" << value << '\n';
	}

	std::cout << request.body << '\n';

	HttpResponse response;
	std::string message = "Test message";

	response.headers.setHeaderInt("Content-Length", message.length());

	std::string responseString = response.toString();

	socket.send(response.toString().data(), responseString.length());
	socket.send(message.data(), message.length());
}

void HttpServer::handleInvalidRequest(TcpSocket& client, const HttpRequest& request, HttpRequestParser::Interrupt error)
{
	char message[] = "HTTP/1.1 200 OK\r\nContent-Length: 15\r\n\r\nInvalid request";
	char buffer[sizeof(message)];
	strcpy(buffer, message);
	client.send(buffer, sizeof(buffer));
	// maybe use a unordered_map of Interrupt and function
}

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