#pragma once

#include <QHttpServer>
#include <QTcpServer>

class ApiServer
{
public:
	ApiServer();

	auto listen(int port = 8000) -> bool;

private:
	QHttpServer httpServer;
	QTcpServer tcpServer;

	void route();

	static auto getVersion() -> QHttpServerResponse;
};
