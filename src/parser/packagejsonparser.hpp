#pragma once

#include <QJsonDocument>

#include "data/nodejs/package.hpp"
#include "data/nodejspackagemanager.hpp"

class PackageJsonParser final
{
public:
	explicit PackageJsonParser(const QByteArray &packageData, const QByteArray &lockData);

	[[nodiscard]]
	auto packages() const -> QList<NodeJs::Package>;

	static auto packageManager(const QByteArray &data) -> NodeJsPackageManager;

private:
	QJsonDocument package;
	QMap<QString, QString> packageVersions;

	void parseLockFile(const QJsonDocument &json);

	void parseLockFile(const QString &content);
};
