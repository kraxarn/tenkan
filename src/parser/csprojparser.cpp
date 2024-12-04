#include "parser/csprojparser.hpp"

#include <QXmlStreamReader>
#include <QDebug>

CsProjParser::CsProjParser(const QByteArray &data)
	: reader(data)
{
}

auto CsProjParser::getPackageReferences() -> QList<DotNet::PackageReference>
{
	QList<DotNet::PackageReference> packages;

	while (!reader.atEnd())
	{
		reader.readNextStartElement();

		if (reader.isEndElement() || reader.name() != "PackageReference")
		{
			continue;
		}

		DotNet::PackageReference package;

		for (const auto &attribute: reader.attributes())
		{
			if (attribute.name() == "Include")
			{
				package.include = attribute.value().toString();
				continue;
			}

			if (attribute.name() == "Version")
			{
				package.version = attribute.value().toString();
				continue;
			}

			qWarning() << "Unknown attribute:" << attribute.name();
		}

		if (package.version.isNull())
		{
			reader.readNextStartElement();
			if (reader.name() == "Version")
			{
				package.version = reader.readElementText();
			}
		}

		if (package.include.isEmpty() || package.version.isEmpty())
		{
			qWarning() << "Package parse failed";
		}

		packages.append(package);
	}

	return packages;
}
