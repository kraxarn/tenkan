#pragma once

#include <QXmlStreamReader>

#include "data/dotnet/packagereference.hpp"

class CsProjParser
{
public:
	explicit CsProjParser(const QByteArray &data);

	[[nodiscard]]
	auto getPackageReferences() -> QList<DotNet::PackageReference>;

private:
	QXmlStreamReader reader;
};
