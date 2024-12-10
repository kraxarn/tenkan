#include "parser/packagejsonparser.hpp"

#include <QJsonArray>
#include <QJsonObject>

PackageJsonParser::PackageJsonParser(const QByteArray &packageData, const QByteArray &lockData)
	: package(QJsonDocument::fromJson(packageData))
{
	const auto lockJson = QJsonDocument::fromJson(lockData);

	lock = lockJson.isNull()
		? QVariant(QString(lockData))
		: QVariant(lockJson);
}


auto PackageJsonParser::packageManager(const QByteArray &data) -> NodeJsPackageManager
{
	const auto json = QJsonDocument::fromJson(data).object();

	if (!json.contains(QStringLiteral("packageManager")))
	{
		return NodeJsPackageManager::Npm;
	}

	const auto packageManager = json[QStringLiteral("packageManager")].toString();
	if (packageManager.startsWith(QStringLiteral("yarn")))
	{
		return NodeJsPackageManager::Yarn;
	}

	return NodeJsPackageManager::Unknown;
}

auto PackageJsonParser::packages() const -> QList<NodeJs::Package>
{
	const auto dependencies = package[QStringLiteral("dependencies")].toArray();
	const auto devDependencies = package[QStringLiteral("devDependencies")].toArray();

	QList<NodeJs::Package> packages;
	packages.reserve(dependencies.size() + devDependencies.size());

	return packages;
}
