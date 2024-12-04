#include "packagetablemodel.hpp"

#include <QElapsedTimer>

#include "config.hpp"
#include "devops.hpp"

PackageTableModel::PackageTableModel(QObject *parent)
	: QAbstractListModel(parent),
	aikidoApi(config, this)
{
}

auto PackageTableModel::rowCount([[maybe_unused]] const QModelIndex &parent) const -> int
{
	return static_cast<int>(packages.length());
}

auto PackageTableModel::data(const QModelIndex &index, const int role) const -> QVariant
{
	const auto package = packages.at(index.row());

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

		case ItemRole::Actions:
			return QStringLiteral("...");

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
		{static_cast<int>(ItemRole::Actions), "actions"},
	};
}

// auto PackageTableModel::headerData(int section, Qt::Orientation orientation, int role) const -> QVariant
// {
// 	switch (static_cast<Section>(section))
// 	{
// 		case Section::PackageName:
// 			return QStringLiteral("Name");
//
// 		case Section::PackageVersion:
// 			return QStringLiteral("Version");
//
// 		case Section::PackageSource:
// 			return QStringLiteral("Source");
//
// 		case Section::AssignedTeam:
// 			return QStringLiteral("Team");
//
// 		case Section::Status:
// 			return QStringLiteral("Status");
//
// 		case Section::LastChecked:
// 			return QStringLiteral("Last Checked");
//
// 		case Section::Actions:
// 			return QStringLiteral("Actions");
//
// 		default:
// 			return {};
// 	}
// }

void PackageTableModel::loadItems()
{
	if (!packages.isEmpty())
	{
		return;
	}

	aikidoApi.repositories([this](const QList<AikidoCodeRepository> &aikidoRepositories)
	{
		for (const auto &aikidoRepository: aikidoRepositories)
		{
			aikidoApi.packages(aikidoRepository.id, [this](const QList<AikidoPackage> &aikidoPackages)
			{
				QElapsedTimer timer;
				timer.start();

				beginInsertRows({},
					static_cast<int>(packages.length()),
					static_cast<int>(packages.length() + aikidoPackages.length())
				);

				for (const auto &aikidoPackage: aikidoPackages)
				{
					const qsizetype separatorIndex = aikidoPackage.packageName.lastIndexOf('@');

					auto packageName = aikidoPackage.packageName
						.first(separatorIndex);

					auto packageVersion = aikidoPackage.packageName
						.last(aikidoPackage.packageName.length() - separatorIndex - 1);

					packages.append({
						.name = std::move(packageName),
						.version = std::move(packageVersion),
						.type = getPackageType(aikidoPackage.language),
						.assignedTeam = {},
						.status = PackageStatus::Unknown,
						.lastChecked = {},
					});
				}

				endInsertRows();

				qInfo() << "Loaded" << packages.length() << "packages in" << timer.elapsed() << "ms";
			});
		}
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
