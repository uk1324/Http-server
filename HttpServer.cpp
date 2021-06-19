#include "HttpServer.h"

void HttpServer::listen(uint16_t port)
{
	try
	{
		m_socket.listen(port);
	}
	catch (std::runtime_error error)
	{
		std::cout << error.what();
		std::exit(-1);
	}

	try
	{
		while (true)
		{
			TcpSocket client = m_socket.accept();

			if (client.isValid())
			{
				std::thread clientThread([this, &client] { this->handleClient(client); });
				clientThread.detach();
			}
		}
	}
	catch (std::runtime_error error)
	{
		std::cout << error.what();
	}

	m_socket.close();
}

void HttpServer::handleClient(TcpSocket& client)
{
	static constexpr int messageBufferSize = 4096;
	char messageBuffer[messageBufferSize];

	HttpRequestParser parser(messageBuffer);

	while (true)
	{
		int messageSize;
		client.receive(messageBuffer, messageBufferSize, &messageSize);

		if (messageSize > 0)
		{
			try
			{
				// Parsing can be split between multiple receive calls
				parser.parse(messageSize);
			}
			catch (HttpRequestParser::Interrupt interrupt)
			{
				std::cout << "interrupt " << int(interrupt) << '\n';
				if (interrupt == HttpRequestParser::Interrupt::finishedParsingFullRequest)
					handleRequest(client, parser.result());
				// Continute parsing on reachedBufferEnd
				else if (interrupt != HttpRequestParser::Interrupt::reachedBufferEnd)
					handleInvalidRequest(client, parser.result(), interrupt);
			}
		}
		else
		{
			// End connection on empty message or error
			std::cout << "err " << messageSize << " " << WSAGetLastError() << '\n';

			client.close();
			return;
		}
	}
}

void HttpServer::handleRequest(TcpSocket& client, const HttpRequest& request)
{
	std::cout << static_cast<int>(request.method) << '\n';
	std::cout << request.requestTarget << '\n';
	std::cout << static_cast<int>(request.version) << '\n';

	for (auto& [name, value] : request.headers)
	{
		std::cout << name << "\t" << value << '\n';
	}

	std::cout << request.body << '\n';

	char message[] = "HTTP/1.1 200 OK\r\nContent-Length: 12\r\n\r\nTest message";
	char buffer[sizeof(message)];
	strcpy(buffer, message);
	client.send(buffer, sizeof(buffer));

}

void HttpServer::handleInvalidRequest(TcpSocket& client, const HttpRequest& request, HttpRequestParser::Interrupt error)
{
	char message[] = "HTTP/1.1 200 OK\r\nContent-Length: 15\r\n\r\nInvalid request";
	char buffer[sizeof(message)];
	strcpy(buffer, message);
	client.send(buffer, sizeof(buffer));
	// maybe use a unordered_map of Interrupt and function
}

void HttpServer::sendFile(int socket, const char* path, int fileSize)
{
	std::ifstream file(path, std::ifstream::binary);
	constexpr int bufferSize = 10;
	char buffer[bufferSize];
	while (fileSize > 0)
	{
		int readSize = bufferSize < fileSize ? bufferSize : fileSize;
		file.read(buffer, readSize);
		send(socket, buffer, readSize, 0);
		fileSize -= readSize;
	}
}