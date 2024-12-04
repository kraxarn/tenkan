#pragma once

#include <QStringList>

class CsvParser
{
public:
	explicit CsvParser(const QByteArray &data);

	[[nodiscard]]
	auto header() const -> QStringList;

	[[nodiscard]]
	auto rows() const -> QList<QStringList>;

private:
	QStringList lines;
};
