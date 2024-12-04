#pragma once

#include <qqml.h>
#include <QAbstractTableModel>

#include "api/aikidoapi.hpp"
#include "data/package.hpp"

class PackageTableModel : public QAbstractListModel
{
	Q_OBJECT
	QML_ELEMENT

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

private:
	enum class ItemRole: int
	{
		PackageName = Qt::UserRole + 1,
		PackageVersion,
		PackageSource,
		AssignedTeam,
		Status,
		LastChecked,
		Actions,
	};

	const Config config;
	AikidoApi aikidoApi;

	QHash<QString, QList<Package>> packages;
	QStringList packageOrder;
	qsizetype repositoryCount = -1;

	static auto getPackageType(const QString &langauge) -> PackageType;

	static auto getPackageSourceIcon(PackageType type) -> QString;

	void loadItems(const QList<AikidoPackage> &aikidoPackages);

	void addPackage(const Package &package);

	static auto removePrefix(const QString &str) -> QStringView;
};
