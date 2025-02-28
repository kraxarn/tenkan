#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QFontDatabase>
#include <QQmlContext>

#include "config.hpp"
#include "packagetablemodel.hpp"
#include "qml/windowmanager.hpp"
#include "qml/greeter.hpp"
#include "qml/settings.hpp"
#include "server/apiserver.hpp"

namespace
{
	void defineTypes(const QQmlApplicationEngine &engine)
	{
		engine.rootContext()->setContextProperty(QStringLiteral("AppName"),
			QCoreApplication::applicationName());

		engine.rootContext()->setContextProperty(QStringLiteral("AppVersion"),
			QCoreApplication::applicationVersion());

		engine.rootContext()->setContextProperty(QStringLiteral("QtVersion"),
			QStringLiteral(QT_VERSION_STR));

		engine.rootContext()->setContextProperty(QStringLiteral("BuildDate"),
			QStringLiteral(__DATE__));

		qmlRegisterType<PackageTableModel>("PackageTableModel", 1, 0, "PackageTableModel");
		qmlRegisterType<WindowManager>("WindowManager", 1, 0, "WindowManager");
		qmlRegisterType<Greeter>("Greeter", 1, 0, "Greeter");

		qmlRegisterSingletonInstance<Settings>("Settings", 1, 0,
			"Settings", new Settings());
	}
}

auto main(int argc, char *argv[]) -> int
{
	QCoreApplication::setApplicationName(QStringLiteral(APP_NAME));
	QCoreApplication::setApplicationVersion(QStringLiteral(APP_VERSION));
	QCoreApplication::setOrganizationName(QStringLiteral(ORG_NAME));

	const QGuiApplication app(argc, argv);

	ApiServer api;
	api.listen();

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
