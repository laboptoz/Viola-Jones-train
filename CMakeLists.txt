cmake_minimum_required(VERSION 3.5)
set (Boost_USE_STATIC_LIBS ON)
set (Boost_USE_MULTITHREADED ON)
project(VJ)
set(CMAKE_CXX_STANDARD 11)
find_package(Boost COMPONENTS system filesystem REQUIRED)
find_package(OpenCV REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${OpenCV_INCLUDE_DIRS})
set(SOURCE_FILES main.cpp Image.h structs.h JPG.h PNG.h Base.h Boosting.h FFS.h)
add_executable(VJ ${SOURCE_FILES})
target_link_libraries(VJ
  ${Boost_FILESYSTEM_LIBRARY}
  ${Boost_SYSTEM_LIBRARY}
  ${Boost_REGEX_LIBRARY} ${OpenCV_LIBS}
)