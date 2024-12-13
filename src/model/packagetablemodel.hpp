#pragma once

#include <qqml.h>

#include "api/aikidoapi.hpp"
#include "data/package.hpp"
#include "model/packagetableproxymodel.hpp"
#include "api/devopsapi.hpp"
#include "api/npmapi.hpp"
#include "api/nugetapi.hpp"
#include "api/cosmosdbapi.hpp"
#include "data/packageinfo.hpp"
#include "data/verification.hpp"

class PackageTableModel : public QAbstractListModel
{
	Q_OBJECT
	QML_ELEMENT

	Q_PROPERTY(QList<QVariant> teams READ getTeams NOTIFY teamsChanged)

public:
	explicit PackageTableModel(QObject *parent = nullptr);

	[[nodiscard]]
	auto rowCount(const QModelIndex &parent) const -> int override;

	[[nodiscard]]
	auto data(const QModelIndex &index, int role) const -> QVariant override;

	[[nodiscard]]
	auto roleNames() const -> QHash<int, QByteArray> override;

	Q_INVOKABLE void sort(int column, Qt::SortOrder order) override;

	Q_INVOKABLE void loadItems();

	Q_INVOKABLE static QString getStatusIcon(PackageStatus packageStatus);

	Q_INVOKABLE static QString getStatusText(PackageStatus packageStatus);

	Q_INVOKABLE void updateStatus(const QString &packageName);

	Q_INVOKABLE QString getPackageStatusInfo(const QString &packageName, PackageStatus packageStatus);

	[[nodiscard]]
	auto getTeams() const -> QList<QVariant>;

signals:
	void teamsChanged();

private:
	static constexpr quint64 maxPackageAge = 365 * 2;

	enum class ItemRole: int
	{
		PackageName = Qt::UserRole + 1,
		PackageVersion,
		PackageSource,
		AssignedTeam,
		Status,
		LastChecked,
		FilePaths,
		PackageUrl,
	};

	PackageTableProxyModel *proxyModel;

	const Config config;
	AikidoApi aikidoApi;
	DevOpsApi devOpsApi;
	NpmApi npmApi;
	NuGetApi nuGetApi;
	CosmosDbApi cosmosDbApi;

	QHash<QString, QList<Package>> packages;
	QStringList packageOrder;
	qsizetype repositoryFileCount = -1;

	QList<Team> teams;

	QHash<QString, PackageInfo> packageInfos;
	QHash<QString, Verification> verifications;

	static auto getPackageType(const QString &langauge) -> PackageType;

	static auto getPackageSourceIcon(PackageType type) -> QString;

	void loadItems(const QString &filePath, const QList<DotNet::PackageReference> &dotNetPackages);

	void loadItems(const QString &filePath, const QList<NodeJs::Package> &nodeJsPackages);

	void addPackage(const Package &package);

	void updatePackageStatus(const QString &packageName, PackageStatus status);

	void updatePackageStatus(const NpmPackageInfo &info);

	void updatePackageStatus(const QString &packageName, const NuGetPackageInfo &info);

	void updateVerifications(const QList<QJsonObject> &documents);

	static auto removePrefix(const QString &str) -> QStringView;

	static auto parseVersionNumber(const QString &version) -> QVersionNumber;

	static auto getVersion(const QList<Package> &packages) -> QVersionNumber;

	static auto getVersionRange(const QList<Package> &packages) -> QString;

	static auto getPackageUrl(const Package &package) -> QUrl;

	static auto getFilePaths(const QList<Package> &packages) -> QList<QVariant>;
};
