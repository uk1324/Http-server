#include "HttpServer.h"

void HttpServer::listen(uint16_t port)
{
	try
	{
		m_socket.start(port);
		m_socket.listen();
	}
	catch (std::runtime_error error)
	{
		std::cout << error.what();
		std::exit(-1);
	}

	while (true)
	{
		TcpSocket client = m_socket.accept();

		if (client.isValid())
		{
			std::thread clientThread([this, client] { this->handleClient(client); });
			clientThread.detach();
		}
	}

	m_socket.close();
}

void HttpServer::handleClient(TcpSocket client)
{
	static constexpr int dataBufferSize = 8192;
	char dataBuffer[dataBufferSize];

	HttpRequestParser parser(dataBuffer, dataBufferSize);

	while (true)
	{
		int messageSize;
		client.receive(dataBuffer, dataBufferSize, &messageSize);

		if (messageSize == 0 || messageSize == -1)
		{
			break;
		}
		else
		{
			try
			{
				parser.setBufferSize(dataBufferSize);
				parser.parse();
			}
			catch (HttpRequestParser::Interrupt interrupt)
			{
				std::cout << "interrupt " << int(interrupt) << '\n';

				if (interrupt == HttpRequestParser::Interrupt::reachedBufferEnd)
				{
				}
				else if (interrupt == HttpRequestParser::Interrupt::finishedParsingFullRequest)
				{
					handleRequest(client, parser.result());

					break;
				}
				else
				{
					break;
				}
			}


			// recheck if every bytesParsed is added and removed correclty


			// should it be able to be reset? it would introducing more ifs because of the need to save state	
		}
	}
	client.close();
}

void HttpServer::handleRequest(TcpSocket client, const HttpRequest& request)
{
	std::cout << static_cast<int>(request.method) << '\n';
	std::cout << request.requestTarget << '\n';
	std::cout << static_cast<int>(request.version) << '\n';

	for (auto& [name, value] : request.headers)
	{
		std::cout << name << "\t" << value << '\n';
	}

	std::cout << request.body << '\n';
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