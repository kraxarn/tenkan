#include "server/apiserver.hpp"

#include <QJsonDocument>
#include <QJsonObject>

ApiServer::ApiServer()
{
	route();
}

auto ApiServer::listen(const int port) -> bool
{
	if (!tcpServer.listen(QHostAddress::LocalHost, port))
	{
		qWarning() << "Failed to listen on port" << port;
		return false;
	}

	if (!httpServer.bind(&tcpServer))
	{
		qWarning() << "Failed to bind to port" << port;
		return false;
	}

	qInfo() << "Listening on port" << port;
	return true;
}

void ApiServer::route()
{
	httpServer.route(QStringLiteral("/version"), []
	{
		QJsonObject json;
		json[QStringLiteral("name")] = QStringLiteral(APP_NAME);
		json[QStringLiteral("version")] = QStringLiteral(APP_VERSION);
		json[QStringLiteral("date")] = QStringLiteral("%1 %2").arg(__DATE__, __TIME__);

		return QHttpServerResponse(json);
	});
}
