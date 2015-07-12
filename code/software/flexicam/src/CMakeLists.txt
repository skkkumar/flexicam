cmake_minimum_required(VERSION 2.8)
set(PROJECTNAME flexicam)
project(${PROJECTNAME})

find_package(Boost COMPONENTS system filesystem REQUIRED)
find_package(YARP)
list(APPEND CMAKE_MODULE_PATH ${YARP_MODULE_PATH})
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH})
file(GLOB source *.cpp)
file(GLOB header *.h)

include_directories(${YARP_INCLUDE_DIRS})
add_executable(${PROJECTNAME}  ${source} ${header})
target_link_libraries(${PROJECTNAME} ${YARP_LIBRARIES}  ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY})
install(TARGETS ${PROJECTNAME} DESTINATION bin)

