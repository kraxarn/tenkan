#pragma once

#include <qqml.h>
#include <QAbstractTableModel>

#include "devops.hpp"
#include "data/package.hpp"

class PackageTableModel : public QAbstractListModel
{
	Q_OBJECT
	QML_ELEMENT

public:
	explicit PackageTableModel(QObject *parent = nullptr);

	[[nodiscard]]
	auto rowCount(const QModelIndex &parent) const -> int override;

	// [[nodiscard]]
	// auto columnCount(const QModelIndex &parent) const -> int override;

	[[nodiscard]]
	auto data(const QModelIndex &index, int role) const -> QVariant override;

	[[nodiscard]]
	auto roleNames() const -> QHash<int, QByteArray> override;

	// [[nodiscard]]
	// auto headerData(int section, Qt::Orientation orientation, int role) const -> QVariant override;

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
	const DevOps devOps;

	QList<Package> packages;

	static auto getPackageSourceIcon(PackageType type) -> QString;
};
