cmake_minimum_required(VERSION 3.9)
project(tcp_proxy)

set(CMAKE_CXX_STANDARD 17)
include_directories(src)

set(IO_SOURCES
    src/io/acceptor_base.cpp
    src/io/bus.cpp
    src/io/endianness.cpp
    src/io/event_reciever.cpp
    src/io/session_manager.cpp
    src/io/v4.cpp
    src/io/acceptor.cpp
    src/io/channel.cpp
    src/io/epoll.cpp
    src/io/flags.cpp
    src/io/session_base.cpp
    src/io/addrinfo.cpp
    src/io/context.cpp
    src/io/error.cpp
    src/io/object.cpp
    src/io/socket.cpp
)
add_library( io STATIC ${IO_SOURCES} )
# target_compile_definitions(io PUBLIC _IO_DEBUG_ENABLED)

set(TCP_PROXY tcp_proxy)
set(TCP_PROXY_SOURCES
    src/echo/main.cpp
    src/echo/server.cpp
    src/echo/session.cpp
)
add_executable(${TCP_PROXY} ${TCP_PROXY_SOURCES})
target_link_libraries( ${TCP_PROXY} io )
# target_compile_definitions(${TCP_PROXY} PUBLIC _IO_DEBUG_ENABLED)

set(ECHO_EXE echo)
set(ECHO_SOURCES
    src/echo/main.cpp
    src/echo/server.cpp
    src/echo/session.cpp
)
add_executable(${ECHO_EXE} ${ECHO_SOURCES})
target_link_libraries( ${ECHO_EXE} io )
# target_compile_definitions(${ECHO_EXE} PUBLIC _IO_DEBUG_ENABLED)

set(PSQL_PROXY_EXE psql_proxy)
set(PSQL_PROXY_SOURCES
    src/psql_proxy/endianness.cpp
    src/psql_proxy/main.cpp
    src/psql_proxy/query_processor.cpp
    src/psql_proxy/session.cpp
    src/psql_proxy/handler.cpp
    src/psql_proxy/message.cpp
    src/psql_proxy/message_logger.cpp
    src/psql_proxy/file_writer.cpp
    src/psql_proxy/data_processor.cpp
    src/psql_proxy/server.cpp
    src/psql_proxy/protocol/parameter_status.cpp
    src/psql_proxy/protocol/query.cpp
    src/psql_proxy/protocol/startup_message.cpp
    src/psql_proxy/protocol/terminate.cpp
)
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
set(TEST_SOURCES
    tests/acceptor_base_test.cpp
    tests/channel_test.cpp
    tests/error_test.cpp
    tests/input_object_test.cpp
    tests/session_manager_test.cpp
    tests/acceptor_test.cpp
    tests/context_test.cpp
    tests/flags_bitwise_and_test.cpp
    tests/io_object_test.cpp
    tests/session_base_test.cpp
    tests/addrinfo_test.cpp
    tests/endianness_test.cpp
    tests/flags_bitwise_or_test.cpp
    tests/object_base_test.cpp
    tests/socket_test.cpp
    tests/bus_test.cpp
    tests/epoll_test.cpp
    tests/flags_ostream_test.cpp
    tests/output_object_test.cpp
    tests/v4_test.cpp
    tests/mock/acceptor_base_mock.cpp
    tests/mock/bus_mock.cpp
    tests/mock/object_mock.cpp
    tests/mock/session_manager_mock.cpp
    tests/mock/session_base_mock.cpp
)
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