#pragma once

#include <qqml.h>
#include <QQuickWindow>

class WindowManager : public QObject
{
	Q_OBJECT
	QML_ELEMENT

public:
	explicit WindowManager(QObject *parent = nullptr);

	Q_INVOKABLE void startMove() const;

	Q_INVOKABLE void minimize() const;

	Q_INVOKABLE void maximize() const;

	Q_INVOKABLE void close() const;

private:
	auto window() const -> QQuickWindow *;
};
