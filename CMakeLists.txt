cmake_minimum_required(VERSION 3.16)

project(tenkan VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Quick QuickControls2 HttpServer)

qt_standard_project_setup(REQUIRES 6.5)

qt_add_executable(${PROJECT_NAME})

add_subdirectory(src)
include(QmlModule.cmake)

target_include_directories(${PROJECT_NAME} PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
        # QML requires headers in root
        "${CMAKE_CURRENT_SOURCE_DIR}/src/model"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/qml"
)

target_compile_definitions(${PROJECT_NAME} PRIVATE APP_NAME="${PROJECT_NAME}")
target_compile_definitions(${PROJECT_NAME} PRIVATE APP_VERSION="v${PROJECT_VERSION}")
target_compile_definitions(${PROJECT_NAME} PRIVATE ORG_NAME="${PROJECT_NAME}")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PRIVATE QT_QML_DEBUG)
endif ()

if (UNIX)
    install(FILES res/icon/package-variant.svg DESTINATION share/icons/hicolor/scalable/apps)
    install(FILES res/app/tenkan.desktop DESTINATION share/applications)
    install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
endif ()

set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE TRUE
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::HttpServer
)
