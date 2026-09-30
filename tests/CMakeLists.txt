set(ZNC_TEST_FRAMEWORK
    ${CMAKE_CURRENT_SOURCE_DIR}/src/base.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/znctest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
)

# Add your modules here:
set(MODULE_TESTS
    ${CMAKE_CURRENT_SOURCE_DIR}/example_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/lowercasechan_test.cpp
)

add_executable(znc-external-tests
    ${ZNC_TEST_FRAMEWORK}
    ${MODULE_TESTS}
    ${GTEST_SOURCES}          # gtest-all.cc + gmock-all.cc, from parent scope
)

target_include_directories(znc-external-tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${ZNC_INCLUDE_DIR}
    ${GTEST_ROOT}/include
    ${GTEST_ROOT}
    ${GMOCK_ROOT}/include
    ${GMOCK_ROOT}
)

target_compile_features(znc-external-tests PRIVATE cxx_std_17)

target_compile_definitions(znc-external-tests PRIVATE
    ZNC_BIN_DIR="${ZNC_BIN_DIR}"
    EXTERNAL_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
)

target_link_libraries(znc-external-tests PRIVATE
    Qt${ZNC_QT_VER}::Core
    Qt${ZNC_QT_VER}::Network
    Threads::Threads
)

if(CYGWIN)
    add_library(znc-external-cygwin SHARED src/cygwin.cpp)
    target_link_libraries(znc-external-cygwin PRIVATE Qt${ZNC_QT_VER}::NetworkPrivate)
    target_link_libraries(znc-external-tests PRIVATE znc-external-cygwin)
endif()

add_custom_target(znc-external-tests-run
    COMMAND znc-external-tests
    DEPENDS znc-external-tests
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    USES_TERMINAL
)