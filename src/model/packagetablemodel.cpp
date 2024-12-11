#include "packagetablemodel.hpp"

#include <QDir>
#include <QFileInfo>
#include <QString>

PackageTableModel::PackageTableModel(QObject *parent)
	: QAbstractListModel(parent),
	proxyModel(new PackageTableProxyModel(this)),
	aikidoApi(config, this),
	devOpsApi(config, this)
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
			return QStringLiteral("(awaiting)");

		case ItemRole::LastChecked:
			return QStringLiteral("(never)");

		case ItemRole::FilePath:
			return items.at(0).filePath;

		case ItemRole::FileName:
			return items.at(0).fileName;

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
		{static_cast<int>(ItemRole::FilePath), "filePath"},
		{static_cast<int>(ItemRole::FileName), "fileName"},
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

auto PackageTableModel::getTeams() const -> QStringList
{
	QStringList teamNames;
	teamNames.reserve(teams.size());

	for (const auto &team : teams)
	{
		if (!team.name.startsWith(QStringLiteral("Team")))
		{
			continue;
		}

		const auto teamName = QString(team.name).replace(QChar('-'), QChar('/'));
		teamNames.append(teamName);
	}

	return teamNames;
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

	qInfo() << version << parts << seg;

	return QVersionNumber(seg);
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
