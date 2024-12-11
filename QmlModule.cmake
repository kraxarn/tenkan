# QML files are in qml/
qt_policy(SET QTP0004 NEW)

qt_add_qml_module(${PROJECT_NAME}
        URI ${PROJECT_NAME}_app
        VERSION ${PROJECT_VERSION}
        NO_RESOURCE_TARGET_PATH
        QML_FILES
        qml/Main.qml
        qml/AppLogo.qml
        qml/AboutDialog.qml
        qml/SettingsDialog.qml
        qml/PackageList.qml
        RESOURCES
        res/font/RobotoFlex-Regular.ttf
        res/icon/alert.svg
        res/icon/clock-alert.svg
        res/icon/dots-horizontal.svg
        res/icon/dot-net.svg
        res/icon/file-code.svg
        res/icon/filter.svg
        res/icon/filter-outline.svg
        res/icon/nodejs.svg
        res/icon/package-variant.svg
        res/icon/shield-alert-outline.svg
        res/icon/shield-check-outline.svg
        res/icon/shield-refresh-outline.svg
        res/icon/shield-remove-outline.svg
        res/icon/shield-sync-outline.svg
        res/icon/window-close.svg
        res/icon/window-minimize.svg
)