#include "qml/windowmanager.hpp"

#include <QQuickItem>
#include <QQuickWindow>

WindowManager::WindowManager(QObject *parent)
	: QObject(parent)
{
}

auto WindowManager::window() const -> QQuickWindow *
{
	const auto *item = qobject_cast<QQuickItem *>(parent());
	return item->window();
}

void WindowManager::startMove() const
{
	window()->startSystemMove();
}

void WindowManager::minimize() const
{
	window()->setWindowState(Qt::WindowMinimized);
}

void WindowManager::maximize() const
{
	const auto isMaximized = (window()->windowState() & Qt::WindowMaximized) > 0;
	window()->setWindowState(isMaximized ? Qt::WindowNoState : Qt::WindowMaximized);
}

void WindowManager::close() const
{
	QCoreApplication::quit();
}
