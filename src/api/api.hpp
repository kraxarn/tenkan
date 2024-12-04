#pragma once

#include <QNetworkAccessManager>

class Api : public QObject
{
	Q_OBJECT

protected:
	explicit Api(QObject *parent);

	[[nodiscard]]
	virtual auto baseUrl() const -> QString = 0;

	[[nodiscard]]
	virtual auto headers() const -> QHttpHeaders = 0;

	[[nodiscard]]
	auto prepareRequest(const QString &url) const -> QNetworkRequest;

	void await(QNetworkReply *reply, const std::function<void(QByteArray)> &callback) const;

	[[nodiscard]]
	auto http() const -> QNetworkAccessManager *;

private:
	QNetworkAccessManager *manager;
};
