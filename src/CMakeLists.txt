add_subdirectory(api)
add_subdirectory(model)
add_subdirectory(parser)
add_subdirectory(qml)
add_subdirectory(server)

target_sources(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/config.cpp
)