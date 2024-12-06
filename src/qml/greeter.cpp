#include "greeter.hpp"

#include <QDateTime>

auto Greeter::getMessage() -> QString
{
	const auto now = QDateTime::currentDateTime();
	const auto hour = now.time().hour();

	constexpr auto morningStart = 5;
	constexpr auto afternoonStart = 12;
	constexpr auto eveningStart = 17;

	if (hour >= morningStart && hour < afternoonStart)
	{
		return QStringLiteral("Good morning!");
	}

	if (hour < eveningStart)
	{
		return QStringLiteral("Good afternoon!");
	}

	return QStringLiteral("Good evening!");
}
