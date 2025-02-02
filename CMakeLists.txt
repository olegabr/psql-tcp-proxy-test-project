cmake_minimum_required(VERSION 3.9)
project(tcp_proxy)

set(CMAKE_CXX_STANDARD 17)
include_directories(src)

file(GLOB IO_SOURCES ${PROJECT_SOURCE_DIR}/src/io/*.cpp)
add_library( io STATIC ${IO_SOURCES} )
# target_compile_definitions(io PUBLIC _IO_DEBUG_ENABLED)

set(TCP_PROXY tcp_proxy)
file(GLOB SOURCES ${PROJECT_SOURCE_DIR}/src/tcp_proxy/*.cpp)
add_executable(${TCP_PROXY} ${SOURCES})
target_link_libraries( ${TCP_PROXY} io )
# target_compile_definitions(${TCP_PROXY} PUBLIC _IO_DEBUG_ENABLED)

set(ECHO_EXE echo)
file(GLOB ECHO_SOURCES ${PROJECT_SOURCE_DIR}/src/echo/*.cpp)
add_executable(${ECHO_EXE} ${ECHO_SOURCES})
target_link_libraries( ${ECHO_EXE} io )
# target_compile_definitions(${ECHO_EXE} PUBLIC _IO_DEBUG_ENABLED)

set(PSQL_PROXY_EXE psql_proxy)
file(GLOB PSQL_PROXY_SOURCES ${PROJECT_SOURCE_DIR}/src/psql_proxy/*.cpp ${PROJECT_SOURCE_DIR}/src/psql_proxy/protocol/*.cpp)
add_executable(${PSQL_PROXY_EXE} ${PSQL_PROXY_SOURCES})
find_package(Threads REQUIRED)
target_link_libraries( ${PSQL_PROXY_EXE} io Threads::Threads )
# target_compile_definitions(${PSQL_PROXY_EXE} PUBLIC _IO_DEBUG_ENABLED)

# cmake v3.11 required to use FetchContent
# 
# include(FetchContent)
# FetchContent_Declare(
#   googletest
#   URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
# )
# # For Windows: Prevent overriding the parent project's compiler/linker settings
# set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
# FetchContent_MakeAvailable(googletest)

# enable_testing()
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

set(TEST_EXE io_test)
file(GLOB TEST_SOURCES ${PROJECT_SOURCE_DIR}/tests/*.cpp ${PROJECT_SOURCE_DIR}/tests/mock/*.cpp)
add_executable(${TEST_EXE} ${TEST_SOURCES})
# target_include_directories(${TEST_EXE} ${GTEST_INCLUDE_DIRS})
target_link_libraries(
    ${TEST_EXE}
    # PUBLIC gtest gtest_main
    PRIVATE ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES} pthread io
)
# target_compile_definitions(${TEST_EXE} PUBLIC _IO_DEBUG_ENABLED)

include(GoogleTest)
# cmake 3.10 is required
# gtest_discover_tests(${TEST_EXE})
gtest_add_tests(
    TARGET ${TEST_EXE}
)

# Test coverage report
# @see https://stackoverflow.com/a/16536401/1490653
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMakeModules)
if(CMAKE_COMPILER_IS_GNUCXX)
    include(CodeCoverage)
    append_coverage_compiler_flags_to_target(${TEST_EXE})
    append_coverage_compiler_flags_to_target(io)
    setup_target_for_coverage_lcov(
#     NAME testrunner_coverage                    # New target name
#     EXECUTABLE testrunner -j ${PROCESSOR_COUNT} # Executable in PROJECT_BINARY_DIR
#     DEPENDENCIES testrunner                     # Dependencies to build first
#     BASE_DIRECTORY "../"                        # Base directory for report
#                                                 #  (defaults to PROJECT_SOURCE_DIR)
#     EXCLUDE "src/dir1/*" "src/dir2/*"           # Patterns to exclude (can be relative
#                                                 #  to BASE_DIRECTORY, with CMake 3.4+)
#     NO_DEMANGLE                                 # Don't demangle C++ symbols
#                                                 #  even if c++filt is found
        NAME ${TEST_EXE}_coverage
        EXECUTABLE ${TEST_EXE}
        DEPENDENCIES io
        BASE_DIRECTORY "../"
        EXCLUDE "/usr/*" "7/*" "*tests*"
        #  coverage
         )
endif()


# Doxygen documentation build
# see https://vicrucann.github.io/tutorials/quick-cmake-doxygen/#comment-4720523411

find_package( Doxygen )

if ( DOXYGEN_FOUND )

set( DOXYGEN_OUTPUT_DIRECTORY doxygen )
set( DOXYGEN_COLLABORATION_GRAPH YES )
set( DOXYGEN_EXTRACT_ALL YES )
set( DOXYGEN_CLASS_DIAGRAMS YES )
set( DOXYGEN_HIDE_UNDOC_RELATIONS NO )
set( DOXYGEN_HAVE_DOT YES )
set( DOXYGEN_CLASS_GRAPH YES )
set( DOXYGEN_CALL_GRAPH YES )
set( DOXYGEN_CALLER_GRAPH YES )
set( DOXYGEN_COLLABORATION_GRAPH YES )
set( DOXYGEN_BUILTIN_STL_SUPPORT YES )
set( DOXYGEN_EXTRACT_PRIVATE YES )
set( DOXYGEN_EXTRACT_PACKAGE YES )
set( DOXYGEN_EXTRACT_STATIC YES )
set( DOXYGEN_EXTRACT_LOCALMETHODS YES )
set( DOXYGEN_UML_LOOK YES )
set( DOXYGEN_UML_LIMIT_NUM_FIELDS 50 )
set( DOXYGEN_TEMPLATE_RELATIONS YES )
set( DOXYGEN_DOT_GRAPH_MAX_NODES 100 )
set( DOXYGEN_MAX_DOT_GRAPH_DEPTH 0 )
set( DOXYGEN_DOT_TRANSPARENT YES )

# @see https://stackoverflow.com/a/44743279/1490653
set(TAGFILES "./cppreference-doxygen-web.tag.xml=http://en.cppreference.com/w/")

doxygen_add_docs( doxygen ${PROJECT_SOURCE_DIR}/src ALL )

else()

message( "Doxygen need to be installed to generate the doxygen documentation" )

endif()