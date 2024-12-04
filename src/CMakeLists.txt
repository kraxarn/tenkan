add_subdirectory(api)
add_subdirectory(model)
add_subdirectory(parser)
add_subdirectory(simplecrypt)
add_subdirectory(qml)

target_sources(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/devops.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/config.cpp
)