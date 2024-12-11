#pragma once

#include <qqml.h>
#include <QAbstractTableModel>

#include "api/aikidoapi.hpp"
#include "data/package.hpp"
#include "model/packagetableproxymodel.hpp"
#include "api/devopsapi.hpp"

class PackageTableModel : public QAbstractListModel
{
	Q_OBJECT
	QML_ELEMENT

	Q_PROPERTY(QStringList teams READ getTeams NOTIFY teamsChanged)

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

	[[nodiscard]]
	auto getTeams() const -> QStringList;

signals:
	void teamsChanged();

private:
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

	QHash<QString, QList<Package>> packages;
	QStringList packageOrder;
	qsizetype repositoryFileCount = -1;

	QList<Team> teams;

	static auto getPackageType(const QString &langauge) -> PackageType;

	static auto getPackageSourceIcon(PackageType type) -> QString;

	void loadItems(const QString &filePath, const QList<DotNet::PackageReference> &dotNetPackages);

	void loadItems(const QString &filePath, const QList<NodeJs::Package> &nodeJsPackages);

	void addPackage(const Package &package);

	static auto removePrefix(const QString &str) -> QStringView;

	static auto parseVersionNumber(const QString &version) -> QVersionNumber;

	static auto getVersionRange(const QList<Package> &packages) -> QString;

	static auto getPackageUrl(const Package &package) -> QUrl;

	static auto getFilePaths(const QList<Package> &packages) -> QList<QVariant>;
};
