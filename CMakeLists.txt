cmake_minimum_required(VERSION 3.15)
project(StarMap VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Options
option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(BUILD_EXAMPLES "Build example applications" ON)
option(BUILD_TESTS "Build tests" OFF)

# Find dependencies
find_package(CURL REQUIRED)
find_package(ZLIB REQUIRED)
find_package(LibXml2 REQUIRED)
find_package(SQLite3 REQUIRED)
find_package(nlohmann_json 3.2.0)
find_package(OpenMP)
find_package(ioc_gaialib REQUIRED)

if(NOT nlohmann_json_FOUND)
    message(STATUS "nlohmann_json not found, will use bundled version")
    set(JSON_BuildTests OFF CACHE INTERNAL "")
    
    # Include external dependencies CMake
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/CMakeLists.txt")
        add_subdirectory(external)
    else()
        message(WARNING "External dependencies not found. Run download_deps.sh or install nlohmann_json manually.")
    endif()
endif()

# Include directories
include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${CURL_INCLUDE_DIRS}
)

# Source files
set(STARMAP_SOURCES
    src/core/Coordinates.cpp
    src/core/CelestialObject.cpp
    src/catalog/GaiaClient.cpp
    src/catalog/SAOCatalog.cpp
    src/catalog/CatalogManager.cpp
    src/catalog/GaiaSAODatabase.cpp
    src/map/MapConfiguration.cpp
    src/map/Projection.cpp
    src/map/MapRenderer.cpp
    src/map/GridRenderer.cpp
    src/map/ChartGenerator.cpp
    src/map/ConstellationData.cpp
    src/config/ConfigurationLoader.cpp
    src/config/JSONConfigLoader.cpp
    src/utils/HttpClient.cpp
    src/occultation/OccultationData.cpp
    src/occultation/OccultationChartBuilder.cpp
)

# Header files
set(STARMAP_HEADERS
    include/starmap/core/Coordinates.h
    include/starmap/core/CelestialObject.h
    include/starmap/catalog/GaiaClient.h
    include/starmap/catalog/SAOCatalog.h
    include/starmap/catalog/CatalogManager.h
    include/starmap/catalog/GaiaSAODatabase.h
    include/starmap/map/MapConfiguration.h
    include/starmap/map/Projection.h
    include/starmap/map/MapRenderer.h
    include/starmap/map/GridRenderer.h
    include/starmap/map/ChartGenerator.h
    include/starmap/map/ConstellationData.h
    include/starmap/config/ConfigurationLoader.h
    include/starmap/config/JSONConfigLoader.h
    include/starmap/utils/HttpClient.h
    include/starmap/occultation/OccultationData.h
    include/starmap/occultation/OccultationChartBuilder.h
    include/starmap/StarMap.h
)

# Create library
add_library(starmap ${STARMAP_SOURCES} ${STARMAP_HEADERS})

target_include_directories(starmap PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(starmap
    PUBLIC
        nlohmann_json::nlohmann_json
        ioc_gaialib
    PRIVATE
        ${CURL_LIBRARIES}
        ZLIB::ZLIB
        LibXml2::LibXml2
        SQLite::SQLite3
)

if(OpenMP_CXX_FOUND)
    target_link_libraries(starmap PUBLIC OpenMP::OpenMP_CXX)
else()
    # Fallback per macOS con brew libomp
    target_link_libraries(starmap PUBLIC "/opt/homebrew/opt/libomp/lib/libomp.dylib")
    target_include_directories(starmap PUBLIC "/opt/homebrew/opt/libomp/include")
endif()

# Set library properties
set_target_properties(starmap PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    PUBLIC_HEADER "${STARMAP_HEADERS}"
)

# Examples
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Installation
include(GNUInstallDirs)

install(TARGETS starmap
    EXPORT StarMapTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/starmap
)

install(DIRECTORY include/starmap
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Export targets
install(EXPORT StarMapTargets
    FILE StarMapTargets.cmake
    NAMESPACE StarMap::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/StarMap
)

# Create config file
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/StarMapConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/StarMapConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/StarMapConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/StarMap
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/StarMapConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/StarMapConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/StarMap
)
