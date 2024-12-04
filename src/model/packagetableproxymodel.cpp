#include "model/packagetableproxymodel.hpp"
#include "model/packagetablemodel.hpp"

PackageTableProxyModel::PackageTableProxyModel(QObject *parent)
	: QSortFilterProxyModel(parent)
{
}

bool PackageTableProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
	return true;
}
