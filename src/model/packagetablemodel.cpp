#include "packagetablemodel.hpp"

#include <QElapsedTimer>
#include <QFileInfo>
#include <QString>

#include "config.hpp"

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
	const auto package = packages[packageName].at(0);

	switch (static_cast<ItemRole>(role))
	{
		case ItemRole::PackageName:
			return package.name;

		case ItemRole::PackageVersion:
			return package.version;

		case ItemRole::PackageSource:
			return getPackageSourceIcon(package.type);

		case ItemRole::AssignedTeam:
			return QStringLiteral("(no team)");

		case ItemRole::Status:
			return QStringLiteral("(awaiting)");

		case ItemRole::LastChecked:
			return QStringLiteral("(never)");

		case ItemRole::FilePath:
			return package.filePath;

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


void PackageTableModel::loadItems(const QString &fileName, const QList<DotNet::PackageReference> &dotNetPackages)
{
	QElapsedTimer timer;
	timer.start();

	beginInsertRows({},
		static_cast<int>(packages.size()),
		static_cast<int>(packages.size() + dotNetPackages.length())
	);

	for (const auto &package: dotNetPackages)
	{
		addPackage({
			.name = package.include,
			.version = package.version,
			.type = PackageType::DotNet,
			.assignedTeam = {},
			.status = PackageStatus::Unknown,
			.lastChecked = {},
			.filePath = fileName,
		});
	}

	endInsertRows();

	if (--repositoryFileCount == 0)
	{
		sort(0, Qt::AscendingOrder);
	}

	qInfo() << "Loaded" << packages.size() << "packages in" << timer.elapsed() << "ms";
}

void PackageTableModel::loadItems()
{
	if (!packages.isEmpty())
	{
		return;
	}

	repositoryFileCount = devOpsApi.repositoryFileCount(QStringLiteral(".csproj"));

	for (const auto &repositoryId: devOpsApi.repositoryIds())
	{
		for (const auto &path: devOpsApi.repositoryFiles(repositoryId, QStringLiteral(".csproj")))
		{
			const auto fileName = QFileInfo(path).fileName();

			devOpsApi.getPackageReferences(repositoryId, path,
				[this, fileName](const QList<DotNet::PackageReference> &packages)
				{
					loadItems(fileName, packages);
				});
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

	for (auto &team: teams)
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
