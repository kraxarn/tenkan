#pragma once

#include <QSortFilterProxyModel>

class PackageTableProxyModel : public QSortFilterProxyModel
{
	Q_OBJECT

public:
	explicit PackageTableProxyModel(QObject *parent);

protected:
	bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
};
