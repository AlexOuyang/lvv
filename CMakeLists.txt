###############################################################################
# CSE 168 - Rendering
###############################################################################

CMAKE_MINIMUM_REQUIRED (VERSION 2.8)
CMAKE_POLICY (SET CMP0015 NEW)

PROJECT ("CSE168_Rendering")

###############################################################################
# CMake config
###############################################################################

SET (
    CMAKE_MODULE_PATH
    ${CMAKE_SOURCE_DIR}/CMakeModules
    ${CMAKE_MODULE_PATH}
)

###############################################################################
# Compiler
###############################################################################

SET (CMAKE_CXX_FLAGS "-std=c++0x -Wall")

###############################################################################
# Dependencies
###############################################################################

# Find and configure Qt
set(CMAKE_AUTOMOC ON)

# Set path to Qt5 CMake modules
SET (CMAKE_PREFIX_PATH ~/Qt/5.2.0/clang_64/lib/cmake/)

# Qt
FIND_PACKAGE (Qt5Widgets)

# GLM
FIND_PACKAGE (GLM)
INCLUDE_DIRECTORIES (${GLM_INCLUDE_DIR})

# Rapid JSON
FIND_PACKAGE (rapidjson)
INCLUDE_DIRECTORIES (${RAPIDJSON_INCLUDE_DIRS})

# Assimp
FIND_LIBRARY (
    ASSIMP_LIBRARIES
    assimp
    PATHS
    /usr/local/lib
)

# FBX SDK
FIND_PACKAGE (FBX)
INCLUDE_DIRECTORIES (${FBX_INCLUDE_DIR})
if (APPLE)
    SET (CMAKE_EXE_LINKER_FLAGS "-framework Foundation -w")
ENDIF()

# Add build directory to include path (for qt ui geneated files)
INCLUDE_DIRECTORIES (${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_SOURCE_DIR}/src)

FILE (
    GLOB_RECURSE
    SOURCES
    ${CMAKE_SOURCE_DIR}/src/*
)

FILE (
    GLOB_RECURSE
    UI
    ${CMAKE_SOURCE_DIR}/ui/*
)

QT5_WRAP_UI (
    UI_HEADERS
    ${UI}
)

FILE (
    GLOB_RECURSE
    HEADERS
    ${CMAKE_SOURCE_DIR}/src/*.h
)

ADD_EXECUTABLE (
    ${PROJECT_NAME}
    MACOSX_BUNDLE
    ${SOURCES}
    ${UI_HEADERS}
)

TARGET_LINK_LIBRARIES (
    ${PROJECT_NAME}
    ${ASSIMP_LIBRARIES}
    ${FBX_LIBRARY}
)

QT5_USE_MODULES (${PROJECT_NAME} Widgets)