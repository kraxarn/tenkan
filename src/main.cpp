#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QFontDatabase>

#include "config.hpp"
#include "packagetablemodel.hpp"
#include "devops.hpp"
#include "qml/windowmanager.hpp"
#include "qml/greeter.hpp"

namespace
{
	void defineTypes(QQmlApplicationEngine &engine)
	{
		qmlRegisterType<PackageTableModel>("PackageTableModel", 1, 0, "PackageTableModel");
		qmlRegisterType<WindowManager>("WindowManager", 1, 0, "WindowManager");
		qmlRegisterType<Greeter>("Greeter", 1, 0, "Greeter");
	}
}

auto main(int argc, char *argv[]) -> int
{
	const QGuiApplication app(argc, argv);

	QFontDatabase::addApplicationFont(":/res/font/RobotoFlex-Regular.ttf");
	QGuiApplication::setFont(QFont("Roboto Flex"));

	QQmlApplicationEngine engine;
	defineTypes(engine);

	QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
		&app, [] { QCoreApplication::exit(-1); },
		Qt::QueuedConnection);

	engine.loadFromModule(QStringLiteral("%1_app").arg(QStringLiteral(APP_NAME)), "Main");

	return QGuiApplication::exec();
}
