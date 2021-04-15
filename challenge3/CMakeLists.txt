cmake_minimum_required(VERSION 3.8)
project(cracker CXX)

set(_CRACKER_SOURCES
	cracker.cpp
	FileWatcher.cpp
)

add_executable(cracker ${_CRACKER_SOURCES})

target_include_directories(cracker PUBLIC ${PROJECT_SOURCE_DIR})

set_property(TARGET cracker PROPERTY CXX_STANDARD 17)
set_property(TARGET cracker PROPERTY CXX_STANDARD_REQUIRED ON)
