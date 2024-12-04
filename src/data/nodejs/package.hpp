#pragma once

namespace NodeJs
{
	using Package = struct package_t
	{
		QString name;
		QString version;
		bool dev;
	};
}
