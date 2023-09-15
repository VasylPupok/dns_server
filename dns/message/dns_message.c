#include "dns_message.h"

#include <arpa/inet.h>
#include <stdlib.h>  // malloc, calloc, free
#include <string.h>  // strlen

dns_message_t* dns_message_new(const uint8_t* bytes) {
    dns_message_t* dns_msg = malloc(sizeof(dns_message_t));
    memset(dns_msg, 0, sizeof(dns_message_t));

    const uint8_t* offset = bytes;
    dns_msg->header = dns_header_new(offset);
    offset += dns_header_size(dns_header_t);

    dns_msg->question = dns_question_new(offset);
    offset += dns_question_size(dns_msg->question);

    dns_msg->answers_number = ntohs(dns_msg->header->anscount);
    dns_msg->authorities_number = ntohs(dns_msg->header->nscount);
    dns_msg->additionals_number = ntohs(dns_msg->header->arcount);

    dns_msg->answers = calloc(dns_msg->answers_number, sizeof(dns_resource_record_t*));
    dns_msg->authorities = calloc(dns_msg->authorities_number, sizeof(dns_resource_record_t*));
    dns_msg->additionals = calloc(dns_msg->additionals_number, sizeof(dns_resource_record_t*));

    for (size_t i = 0; i < dns_msg->answers_number; ++i) {
        dns_msg->answers[i] = dns_resource_record_new(offset);
        dns_msg->answers_size += dns_resource_record_size(dns_msg->answers[i]);
    }
    offset += dns_msg->answers_size;

    for (size_t i = 0; i < dns_msg->authorities_number; ++i) {
        dns_msg->authorities[i] = dns_resource_record_new(offset);
        dns_msg->authorities_size += dns_resource_record_size(dns_msg->authorities[i]);
    }
    offset += dns_msg->authorities_size;

    for (size_t i = 0; i < dns_msg->additionals_number; ++i) {
        dns_msg->additionals[i] = dns_resource_record_new(offset);
        dns_msg->additionals_size += dns_resource_record_size(dns_msg->additionals[i]);
    }
    offset += dns_msg->additionals_size;

    return dns_msg;
}

dns_message_t* dns_message_copy(const dns_message_t* dns_msg) {
    dns_message_t* copy = malloc(sizeof(dns_message_t));

    copy->header = dns_header_copy(dns_msg->header);
    copy->question = dns_question_copy(dns_msg->question);

    copy->answers_number = dns_msg->answers_number;
    copy->authorities_number = dns_msg->authorities_number;
    copy->additionals_number = dns_msg->additionals_number;

    copy->answers_size = dns_msg->answers_size;
    copy->authorities_size = dns_msg->authorities_size;
    copy->additionals_size = dns_msg->additionals_size;

    copy->answers = calloc(dns_msg->answers_number, sizeof(dns_resource_record_t*));
    copy->authorities = calloc(dns_msg->authorities_number, sizeof(dns_resource_record_t*));
    copy->additionals = calloc(dns_msg->additionals_number, sizeof(dns_resource_record_t*));

    for (size_t i = 0; i < copy->answers_number; ++i) {
        copy->answers[i] = dns_resource_record_copy(dns_msg->answers[i]);
    }

    for (size_t i = 0; i < dns_msg->authorities_number; ++i) {
        copy->authorities[i] = dns_resource_record_new(dns_msg->authorities[i]);
    }

    for (size_t i = 0; i < dns_msg->additionals_number; ++i) {
        dns_msg->additionals[i] = dns_resource_record_new(dns_msg->additionals[i]);
    }

    return copy;
}

void dns_message_delete(const dns_message_t* dns_msg) {
    if (dns_msg) {
        dns_header_delete(dns_msg->header);
        dns_question_delete(dns_msg->question);

        // delete resource records

        if (dns_msg->answers) {
            for (size_t i = 0; i < dns_msg->answers_number; ++i) {
                dns_resource_record_delete(dns_msg->answers[i]);
            }
            free(dns_msg->answers);
        }

        if (dns_msg->authorities) {
            for (size_t i = 0; i < dns_msg->authorities_number; ++i) {
                dns_resource_record_delete(dns_msg->authorities[i]);
            }
            free(dns_msg->authorities);
        }

        if (dns_msg->additionals) {
            for (size_t i = 0; i < dns_msg->additionals_number; ++i) {
                dns_resource_record_delete(dns_msg->additionals[i]);
            }
            free(dns_msg->additionals);
        }

        free(dns_msg);
    }
}

uint8_t* dns_message_to_bytearray(const dns_message_t* dns_msg) {
    if (!dns_msg) {
        return NULL;
    }
    size_t size = dns_message_size(dns_msg);
    uint8_t* arr = malloc(size);
    uint8_t* offset = arr;

    // header
    memcpy(offset, dns_msg->header, dns_header_size(dns_msg->header));
    offset += dns_header_size(dns_msg->header);

    // question
    uint8_t* question = dns_question_to_bytearray(dns_msg->question);
    memcpy(
        offset,
        question,
        dns_question_size(dns_msg->question));
    offset += dns_question_size(dns_msg->question);
    free(question);

    // records
    uint8_t* bytearray = NULL;
    for (size_t i = 0; i < dns_msg->answers_number; ++i) {
        bytearray = dns_resource_record_to_bytearray(dns_msg->answers[i]);
        memcpy(offset, bytearray, dns_resource_record_size(dns_msg->answers[i]));
        offset += dns_resource_record_size(dns_msg->answers[i]);
        free(bytearray);
    }

    for (size_t i = 0; i < dns_msg->authorities_number; ++i) {
        bytearray = dns_resource_record_to_bytearray(dns_msg->authorities[i]);
        memcpy(offset, bytearray, dns_resource_record_size(dns_msg->authorities[i]));
        offset += dns_resource_record_size(dns_msg->authorities[i]);
        free(bytearray);
    }

    for (size_t i = 0; i < dns_msg->additionals_number; ++i) {
        bytearray = dns_resource_record_to_bytearray(dns_msg->additionals[i]);
        memcpy(offset, bytearray, dns_resource_record_size(dns_msg->additionals[i]));
        offset += dns_resource_record_size(dns_msg->additionals[i]);
        free(bytearray);
    }

    return arr;
}

unsigned long dns_message_size(const dns_message_t* message) {
    if (!message) {
        return 0;
    }
    return dns_header_size(message->header) +
           dns_question_size(message->question) +
           message->answers_size +
           message->authorities_size +
           message->additionals_size;
}
