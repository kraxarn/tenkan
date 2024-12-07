#include "packagetablemodel.hpp"

#include <QElapsedTimer>

#include "config.hpp"

PackageTableModel::PackageTableModel(QObject *parent)
	: QAbstractListModel(parent),
	proxyModel(new PackageTableProxyModel(this)),
	aikidoApi(config, this)
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


void PackageTableModel::loadItems(const QList<AikidoPackage> &aikidoPackages)
{
	QElapsedTimer timer;
	timer.start();

	beginInsertRows({},
		static_cast<int>(packages.size()),
		static_cast<int>(packages.size() + aikidoPackages.length())
	);

	for (const auto &aikidoPackage: aikidoPackages)
	{
		const qsizetype separatorIndex = aikidoPackage.packageName.lastIndexOf('@');

		auto packageName = aikidoPackage.packageName
			.first(separatorIndex);

		auto packageVersion = aikidoPackage.packageName
			.last(aikidoPackage.packageName.length() - separatorIndex - 1);

		addPackage({
			.name = std::move(packageName),
			.version = std::move(packageVersion),
			.type = getPackageType(aikidoPackage.language),
			.assignedTeam = {},
			.status = PackageStatus::Unknown,
			.lastChecked = {},
		});
	}

	endInsertRows();

	if (--repositoryCount == 0)
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

	aikidoApi.repositories([this](const QList<AikidoCodeRepository> &aikidoRepositories)
	{
		repositoryCount = aikidoRepositories.size();

		for (const auto &aikidoRepository: aikidoRepositories)
		{
			aikidoApi.packages(aikidoRepository.id, [this](const QList<AikidoPackage> &aikidoPackages)
			{
				loadItems(aikidoPackages);
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
