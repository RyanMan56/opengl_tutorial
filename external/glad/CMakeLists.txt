# Specify the source files
set(GLAD_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/glad.c
)

# Create a static library for glad
add_library(glad STATIC ${GLAD_SOURCES})

# Specify include directories for glad
target_include_directories(glad PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/../KHR)
