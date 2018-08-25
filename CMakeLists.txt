cmake_minimum_required (VERSION 3.1)
project (fanmail)

#add_subdirectory(vendor/libtwittercpp)
add_subdirectory(vendor/rawr-ebooks EXCLUDE_FROM_ALL)

find_package(PkgConfig)
pkg_check_modules(yaml-cpp yaml-cpp REQUIRED)
pkg_check_modules(GraphicsMagick GraphicsMagick++ REQUIRED)

include_directories(
  vendor/rawr-ebooks/vendor/libtwittercpp/src
  vendor/rawr-ebooks
  ${yaml-cpp_INCLUDE_DIRS}
  ${GraphicsMagick_INCLUDE_DIRS})

link_directories(${GraphicsMagick_LIBRARY_DIRS})

add_executable(fanmail fanmail.cpp designer.cpp main.cpp)
set_property(TARGET fanmail PROPERTY CXX_STANDARD 14)
set_property(TARGET fanmail PROPERTY CXX_STANDARD_REQUIRED ON)
target_link_libraries(fanmail ${yaml-cpp_LIBRARIES} ${GraphicsMagick_LIBRARIES} twitter++ rawr)