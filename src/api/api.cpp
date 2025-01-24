#include "api/api.hpp"

#include <QNetworkReply>

Api::Api(QObject *parent)
	: QObject(parent),
	manager(new QNetworkAccessManager(this))
{
}

auto Api::prepareRequest(const QString &url) const -> QNetworkRequest
{
	QNetworkRequest request;
	request.setUrl(baseUrl() + url);
	request.setHeaders(headers());
	return request;
}

void Api::await(QNetworkReply *reply, const std::function<void(QByteArray)> &callback) const
{
	connect(reply, &QNetworkReply::finished, this, [reply, callback]
	{
		if (reply->error() != QNetworkReply::NoError)
		{
			const auto statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
			qWarning().noquote().nospace() << reply->errorString() << " (" << statusCode << ")";
			qWarning().noquote() << QString::fromUtf8(reply->readAll());
			return;
		}

		callback(reply->readAll());
		reply->deleteLater();
	});
}

auto Api::http() const -> QNetworkAccessManager *
{
	return manager;
}
