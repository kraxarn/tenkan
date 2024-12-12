#include "packagetablemodel.hpp"

#include <QDir>
#include <QFileInfo>
#include <QString>

PackageTableModel::PackageTableModel(QObject *parent)
	: QAbstractListModel(parent),
	proxyModel(new PackageTableProxyModel(this)),
	aikidoApi(config, this),
	devOpsApi(config, this),
	npmApi(this),
	nuGetApi(this)
{
	proxyModel->setSourceModel(this);
}

auto PackageTableModel::rowCount([[maybe_unused]] const QModelIndex &parent) const -> int
{
	return static_cast<int>(packageOrder.size());
}

auto PackageTableModel::data(const QModelIndex &index, const int role) const -> QVariant
{
	const auto packageName = packageOrder.at(index.row());
	const auto &items = packages[packageName];

	switch (static_cast<ItemRole>(role))
	{
		case ItemRole::PackageName:
			return packageName;

		case ItemRole::PackageVersion:
			return getVersionRange(items);

		case ItemRole::PackageSource:
			return getPackageSourceIcon(items.at(0).type);

		case ItemRole::AssignedTeam:
			return QStringLiteral("(no team)");

		case ItemRole::Status:
			return static_cast<int>(items.at(0).status);

		case ItemRole::LastChecked:
			return QStringLiteral("(never)");

		case ItemRole::FilePaths:
			return getFilePaths(items);

		case ItemRole::PackageUrl:
			return getPackageUrl(items.at(0));

		default:
			return {};
	}
}

auto PackageTableModel::roleNames() const -> QHash<int, QByteArray>
{
	return {
		{static_cast<int>(ItemRole::PackageName), "packageName"},
		{static_cast<int>(ItemRole::PackageVersion), "packageVersion"},
		{static_cast<int>(ItemRole::PackageSource), "packageSource"},
		{static_cast<int>(ItemRole::AssignedTeam), "assignedTeam"},
		{static_cast<int>(ItemRole::Status), "status"},
		{static_cast<int>(ItemRole::LastChecked), "lastChecked"},
		{static_cast<int>(ItemRole::FilePaths), "filePaths"},
		{static_cast<int>(ItemRole::PackageUrl), "packageUrl"},
	};
}

void PackageTableModel::sort(const int column, const Qt::SortOrder order)
{
	emit layoutAboutToBeChanged();

	std::sort(packageOrder.begin(), packageOrder.end(), [](const QString &str1, const QString &str2)
	{
		return removePrefix(str1).compare(removePrefix(str2), Qt::CaseInsensitive) < 0;
	});

	emit layoutChanged();
}

auto PackageTableModel::removePrefix(const QString &str) -> QStringView
{
	if (str.at(0) != '@')
	{
		return {str};
	}

	return QStringView(str).right(str.length() - 1);
}


void PackageTableModel::addPackage(const Package &package)
{
	if (packages.contains(package.name))
	{
		packages[package.name].append(package);
		return;
	}

	packages.insert(package.name, {package});
	packageOrder.append(package.name);
}


void PackageTableModel::loadItems(const QString &filePath, const QList<DotNet::PackageReference> &dotNetPackages)
{
	beginInsertRows({},
		static_cast<int>(packages.size()),
		static_cast<int>(packages.size() + dotNetPackages.length())
	);

	for (const auto &package : dotNetPackages)
	{
		addPackage({
			.name = package.include,
			.version = parseVersionNumber(package.version),
			.type = PackageType::DotNet,
			.assignedTeam = {},
			.status = PackageStatus::Unknown,
			.lastChecked = {},
			.filePath = filePath,
			.fileName = QFileInfo(filePath).fileName(),
		});
	}

	endInsertRows();

	if (--repositoryFileCount == 0)
	{
		sort(0, Qt::AscendingOrder);
	}
}

void PackageTableModel::loadItems(const QString &filePath, const QList<NodeJs::Package> &nodeJsPackages)
{
	beginInsertRows({},
		static_cast<int>(packages.size()),
		static_cast<int>(packages.size() + nodeJsPackages.length())
	);

	for (const auto &package : nodeJsPackages)
	{
		const auto fileInfo = QFileInfo(filePath);

		addPackage({
			.name = package.name,
			.version = parseVersionNumber(package.version),
			.type = PackageType::NodeJs,
			.assignedTeam = {},
			.status = PackageStatus::Unknown,
			.lastChecked = {},
			.filePath = filePath,
			.fileName = QStringLiteral("%1/%2").arg(fileInfo.dir().dirName(), fileInfo.fileName()),
		});
	}

	endInsertRows();

	if (--repositoryFileCount == 0)
	{
		sort(0, Qt::AscendingOrder);
	}
}

void PackageTableModel::loadItems()
{
	if (!packages.isEmpty())
	{
		return;
	}

	repositoryFileCount = devOpsApi.repositoryFileCount();

	for (const auto &repositoryId : devOpsApi.repositoryIds())
	{
		for (const auto &path : devOpsApi.repositoryFiles(repositoryId))
		{
			if (path.endsWith(QStringLiteral(".csproj")))
			{
				devOpsApi.getPackageReferences(repositoryId, path,
					[this, path](const QList<DotNet::PackageReference> &packages)
					{
						loadItems(path, packages);
					});
				continue;
			}

			if (path.endsWith(QStringLiteral("package.json")))
			{
				devOpsApi.packages(repositoryId, path, [this, path](const QList<NodeJs::Package> &packages)
				{
					loadItems(path, packages);
				});
				continue;
			}

			qFatal() << "Unknown path:" << path;
		}
	}

	devOpsApi.teams([this](const QList<Team> &teams)
	{
		this->teams = teams;
		emit teamsChanged();
	});
}

auto PackageTableModel::getPackageType(const QString &langauge) -> PackageType
{
	if (langauge == QStringLiteral("dotnet"))
	{
		return PackageType::DotNet;
	}

	if (langauge == QStringLiteral("js"))
	{
		return PackageType::NodeJs;
	}

	return PackageType::Unknown;
}


auto PackageTableModel::getPackageSourceIcon(const PackageType type) -> QString
{
	switch (type)
	{
		case PackageType::Unknown:
			return QStringLiteral("alert");

		case PackageType::DotNet:
			return QStringLiteral("dot-net");

		case PackageType::NodeJs:
			return QStringLiteral("nodejs");

		default:
			return {};
	}
}

auto PackageTableModel::getTeams() const -> QList<QVariant>
{
	QList<QVariant> teamDatas;
	teamDatas.reserve(teams.size());

	for (const auto &team : teams)
	{
		if (!team.name.startsWith(QStringLiteral("Team")))
		{
			continue;
		}

		const auto teamName = QString(team.name).replace(QChar('-'), QChar('/'));

		QMap<QString, QVariant> data;
		data[QStringLiteral("id")] = team.id;
		data[QStringLiteral("name")] = teamName;

		teamDatas.append(data);
	}

	return teamDatas;
}

auto PackageTableModel::parseVersionNumber(const QString &version) -> QVersionNumber
{
	const auto sep = QRegularExpression(QStringLiteral("[\\.-]"));
	const auto parts = version.split(sep);

	QList<int> seg;
	seg.reserve(parts.size());

	for (const auto &part : parts)
	{
		bool ok;
		const int result = part.toInt(&ok);

		if (!ok)
		{
			break;
		}

		seg.append(result);
	}

	return QVersionNumber(seg);
}

auto PackageTableModel::getVersion(const QList<Package> &packages) -> QVersionNumber
{
	QVersionNumber version(packages.at(0).version);

	for (auto iter = packages.begin() + 1; iter != packages.end(); ++iter)
	{
		version = qMin(version, iter->version);
	}

	return version;
}


auto PackageTableModel::getVersionRange(const QList<Package> &packages) -> QString
{
	QVersionNumber min(packages.at(0).version);
	QVersionNumber max(min);

	for (auto iter = packages.begin() + 1; iter != packages.end(); ++iter)
	{
		min = qMin(min, iter->version);
		max = qMax(max, iter->version);
	}

	if (min == max)
	{
		return min.toString();
	}

	return QStringLiteral("%1 - %2")
		.arg(min.toString(), max.toString());
}

auto PackageTableModel::getPackageUrl(const Package &package) -> QUrl
{
	switch (package.type)
	{
		case PackageType::DotNet:
			return QUrl(QStringLiteral("https://www.nuget.org/packages/%1")
				.arg(package.name));

		case PackageType::NodeJs:
			return QUrl(QStringLiteral("https://www.npmjs.com/package/%2")
				.arg(package.name));

		default:
			return {};
	}
}

auto PackageTableModel::getFilePaths(const QList<Package> &packages) -> QList<QVariant>
{
	QList<QVariant> paths;
	paths.reserve(packages.size());

	for (const auto &package : packages)
	{
		QMap<QString, QVariant> path;
		path[QStringLiteral("path")] = package.filePath;
		path[QStringLiteral("name")] = package.fileName;

		paths.append(path);
	}

	return paths;
}

auto PackageTableModel::getStatusIcon(const PackageStatus packageStatus) -> QString
{
	switch (packageStatus)
	{
		case PackageStatus::Unknown:
		case PackageStatus::Updating:
			return QStringLiteral("shield-sync-outline");

		case PackageStatus::UpToDate:
			return QStringLiteral("shield-check-outline");

		case PackageStatus::Outdated:
			return QStringLiteral("shield-refresh-outline");

		case PackageStatus::Vulnerable:
			return QStringLiteral("shield-alert-outline");

		case PackageStatus::Unmaintained:
		case PackageStatus::Deprecated:
			return QStringLiteral("shield-remove-outline");

		default:
			return {};
	}
}

auto PackageTableModel::getStatusText(PackageStatus packageStatus) -> QString
{
	switch (packageStatus)
	{
		case PackageStatus::Unknown:
		case PackageStatus::Updating:
			return QStringLiteral("Updating status...");

		case PackageStatus::UpToDate:
			return QStringLiteral("Package is up to date, with no known vulnerabilities!");

		case PackageStatus::Outdated:
			return QStringLiteral("A new version is available");

		case PackageStatus::Vulnerable:
			return QStringLiteral("Package contains a known security vulnerability");

		case PackageStatus::Unmaintained:
			return QStringLiteral("Package is no longer actively maintained");

		case PackageStatus::Deprecated:
			return QStringLiteral("Package is deprecated and will no longer receive any updates");

		default:
			return {};
	}
}

void PackageTableModel::updatePackageStatus(const QString &packageName, const PackageStatus status)
{
	for (auto &package : packages[packageName])
	{
		package.status = status;
	}

	const auto row = packageOrder.indexOf(packageName);
	const auto index = createIndex(row, 0);

	emit dataChanged(index, index, {
		static_cast<int>(ItemRole::Status),
	});
}

void PackageTableModel::updatePackageStatus(const NpmPackageInfo &info)
{
	if (!info.deprecated.isEmpty())
	{
		updatePackageStatus(info.name, PackageStatus::Deprecated);
		return;
	}

	const auto &items = packages[info.name];

	if (getVersion(items) != parseVersionNumber(info.version))
	{
		updatePackageStatus(info.name, PackageStatus::Outdated);
		return;
	}

	if (info.modified.daysTo(QDateTime::currentDateTimeUtc()) >= maxPackageAge)
	{
		updatePackageStatus(info.name, PackageStatus::Unmaintained);
		return;
	}

	updatePackageStatus(info.name, PackageStatus::UpToDate);
}

void PackageTableModel::updatePackageStatus(const NuGetPackageInfo &info)
{
	if (info.title.contains(QStringLiteral("deprecated"), Qt::CaseInsensitive))
	{
		updatePackageStatus(info.id, PackageStatus::Deprecated);
		return;
	}

	const auto &items = packages[info.id];

	if (getVersion(items) != parseVersionNumber(info.version))
	{
		updatePackageStatus(info.id, PackageStatus::Outdated);
		return;
	}

	if (info.published.daysTo(QDateTime::currentDateTimeUtc()) >= maxPackageAge)
	{
		updatePackageStatus(info.id, PackageStatus::Unmaintained);
		return;
	}

	updatePackageStatus(info.id, PackageStatus::UpToDate);
}

void PackageTableModel::updateStatus(const QString &packageName)
{
	if (packageName.isEmpty())
	{
		return;
	}

	const auto &package = packages[packageName].at(0);

	if (package.status > PackageStatus::Unknown)
	{
		return;
	}

	if (repositoryFileCount > 0)
	{
		return;
	}

	updatePackageStatus(packageName, PackageStatus::Updating);

	switch (package.type)
	{
		case PackageType::DotNet:
			nuGetApi.info(package.name, [this, packageName](const NuGetPackageInfo &info)
			{
				if (info.id.isEmpty())
				{
					qWarning() << "Failed to fetch info for package:" << packageName;
					return;
				}

				updatePackageStatus(info);
			});
			break;

		case PackageType::NodeJs:
			npmApi.info(package.name, [this](const NpmPackageInfo &info)
			{
				updatePackageStatus(info);
			});
			break;

		case PackageType::Unknown:
			break;
	}
}
