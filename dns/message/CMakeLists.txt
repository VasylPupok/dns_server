cmake_minimum_required(VERSION 3.5.0)
project(Hello_CMake VERSION 0.1.0 LANGUAGES C)

set (
    SOURCES

    dns_message.h
    dns_message.c
    header/dns_header.h
    header/dns_header.c
    question/dns_question.h
    question/dns_question.c
    resource_record/dns_resource_record.h
    resource_record/dns_resource_record.c
)

add_library(dns_message ${SOURCES})

target_include_directories(
    dns_message
    PRIVATE 
    ${PROJECT_DIR}
)

export(TARGETS dns_message FILE dns_message.cmake)
export(PACKAGE dns_message)