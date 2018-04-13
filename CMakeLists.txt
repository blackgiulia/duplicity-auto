cmake_minimum_required(VERSION 3.9)

project(DuplicityAuto)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_BUILD_TYPE Release)

find_package(Boost REQUIRED COMPONENTS filesystem date_time)

if(Boost_FOUND)

    message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
    message(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")
    message(STATUS "Boost_VERSION: ${Boost_VERSION}")

    include_directories(${Boost_INCLUDE_DIRS})

endif()

add_executable(${PROJECT_NAME} "main.cpp" "autocore.cpp")

if(Boost_FOUND)

    target_link_libraries(DuplicityAuto ${Boost_LIBRARIES})

endif()
