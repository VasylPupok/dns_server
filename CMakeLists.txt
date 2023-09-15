cmake_minimum_required(VERSION 3.5.0)
project(dns_server VERSION 0.1.0 LANGUAGES C)

include(CTest)
enable_testing()

set (
    SOURCES

    main.c

    dns/server/server.h
    dns/server/server.c

    dns/utils/hostname.h
    dns/utils/hostname.c

    # third-party stuff

    third-party/avsej/hashset/hashset.h
    third-party/avsej/hashset/hashset.c
)

add_subdirectory(${PROJECT_SOURCE_DIR}/dns/message)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE dns_message)

target_include_directories(
    ${PROJECT_NAME} PRIVATE 
    ${PROJECT_SOURCE_DIR}
    ../../third-party
)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
