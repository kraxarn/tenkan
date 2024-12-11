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
	const auto dependencies = package[QStringLiteral("dependencies")].toObject();
	const auto devDependencies = package[QStringLiteral("devDependencies")].toObject();

	QList<NodeJs::Package> packages;
	packages.reserve(dependencies.size() + devDependencies.size());

	for (const auto &dependency : dependencies.keys())
	{
		packages.append({
			.name = dependency,
			.version = getPackageVersion(dependency),
			.dev = false,
		});
	}

	for (const auto &dependency : devDependencies.keys())
	{
		packages.append({
			.name = dependency,
			.version = getPackageVersion(dependency),
			.dev = true,
		});
	}

	return packages;
}

auto PackageJsonParser::getPackageVersion(const QString &packageName) const -> QString
{
	if (lock.canConvert<QJsonDocument>())
	{
		const auto json = lock.toJsonDocument();
		return getPackageVersion(json, packageName);
	}

	if (lock.canConvert<QString>())
	{
		const auto content = lock.toString();
		return getPackageVersion(content, packageName);
	}

	qFatal() << "Unknown lock file type:" << lock.typeName();
	return {};
}

auto PackageJsonParser::getPackageVersion(const QJsonDocument &json, const QString &packageName) -> QString
{
	const auto &packages = json[QStringLiteral("packages")].toObject();
	const auto &package = packages[QStringLiteral("node_modules/%1").arg(packageName)].toObject();
	return package[QStringLiteral("version")].toString();
}

auto PackageJsonParser::getPackageVersion(const QString &content, const QString &packageName) -> QString
{
	return {};
}
