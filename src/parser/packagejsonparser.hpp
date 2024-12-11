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
	QVariant lock;

	[[nodiscard]]
	auto getPackageVersion(const QString &packageName) const -> QString;

	[[nodiscard]]
	static auto getPackageVersion(const QJsonDocument &json, const QString &packageName) -> QString;

	[[nodiscard]]
	static auto getPackageVersion(const QString &content, const QString &packageName) -> QString;
};
