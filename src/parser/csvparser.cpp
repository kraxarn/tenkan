#include "parser/csvparser.hpp"

#include <QtDebug>

CsvParser::CsvParser(const QByteArray &data)
	: lines(QString::fromUtf8(data).split('\n'))
{
}

auto CsvParser::header() const -> QStringList
{
	if (lines.isEmpty())
	{
		return {};
	}

	return lines.at(0).split(',');
}

auto CsvParser::rows() const -> QList<QStringList>
{
	QList<QStringList> result;
	result.reserve(lines.size() - 1);

	for (qsizetype i = 1; i < lines.size() - 1; i++)
	{
		result.append(lines.at(i).split(','));
	}

	return result;
}
