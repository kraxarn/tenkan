#include "parser/packagejsonparser.hpp"

#include <QJsonArray>
#include <QJsonObject>

PackageJsonParser::PackageJsonParser(const QByteArray &packageData, const QByteArray &lockData)
	: package(QJsonDocument::fromJson(packageData))
{
	if (const auto lockJson = QJsonDocument::fromJson(lockData); !lockJson.isNull())
	{
		parseLockFile(lockJson);
	}
	else
	{
		parseLockFile(QString(lockData));
	}
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
			.version = packageVersions.value(dependency),
			.dev = false,
		});
	}

	for (const auto &dependency : devDependencies.keys())
	{
		packages.append({
			.name = dependency,
			.version = packageVersions.value(dependency),
			.dev = true,
		});
	}

	return packages;
}

auto PackageJsonParser::parseLockFile(const QJsonDocument &json) -> void
{
	const auto &packages = json[QStringLiteral("packages")].toObject();

	for (const auto &key : packages.keys())
	{
		const auto prefix = QStringLiteral("node_modules/");
		if (!key.startsWith(prefix))
		{
			continue;
		}

		const auto name = key.right(key.size() - prefix.size());
		const auto version = packages[key][QStringLiteral("version")].toString();

		packageVersions.insert(name, version);
	}
}

auto PackageJsonParser::parseLockFile(const QString &content) -> void
{
	const QRegularExpression exp(
		QStringLiteral("(\"|)(?<name>[a-zA-Z0-9\\/@\\-\\.]+)@.*\\s+version\\s*\"(?<version>[0-9\\.]+)\"")
	);

	if (!exp.isValid())
	{
		qFatal() << "Invalid regex:" << exp.errorString();
	}

	qsizetype pos = 0;
	while (pos < content.size() && pos >= 0)
	{
		const auto match = exp.match(content, pos);

		const auto name = match.captured("name");
		const auto version = match.captured("version");

		packageVersions.insert(name, version);

		pos = match.capturedEnd();
	}
}
