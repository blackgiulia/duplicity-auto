cmake_minimum_required(VERSION 3.10)
project(DuplicityAuto)

set(CMAKE_CXX_STANDARD 14)

find_package(Boost REQUIRED COMPONENTS filesystem date_time system)

if(Boost_FOUND)

    message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
    message(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")
    message(STATUS "Boost_VERSION: ${Boost_VERSION}")

    include_directories(${Boost_INCLUDE_DIRS})

endif()

set(LLVM_DIR "/usr/local/opt/llvm/lib/cmake/llvm")

find_package(LLVM REQUIRED CONFIG)

if(LLVM_FOUND)

    message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
    message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

    include_directories(${LLVM_INCLUDE_DIRS})
    add_definitions(${LLVM_DEFINITIONS})

endif()

add_executable(${PROJECT_NAME} main.cpp handle.cpp handle.hpp)

if(Boost_FOUND AND LLVM_FOUND)

    llvm_map_components_to_libnames(llvm_libs support core irreader)
    target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES} ${llvm_libs})

endif()

set(DO_CLANG_TIDY "/usr/local/opt/llvm/bin/clang-tidy" "-header-filter=.*" "-checks=*,-clang-analyzer-alpha.*,-android-*,-fuchsia-*,-google-*,-llvm-*,-objc-*,-zircon-*,-hicpp-*")
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_CLANG_TIDY  "${DO_CLANG_TIDY}")