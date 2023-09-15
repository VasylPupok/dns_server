#include "dns_question.h"

#include <stdlib.h> // malloc, calloc, free
#include <string.h> // strlen

dns_question_t* dns_question_new(const uint8_t* bytes) {
    dns_question_t* question = malloc(sizeof(dns_question_t));
    size_t size = strlen(bytes) + 1;
    question->qname = calloc(size, sizeof(char));
    memcpy(question->qname, bytes, size);
    memcpy(&question->qtype, bytes + size, sizeof(question->qtype));
    memcpy(&question->qclass, bytes + size + sizeof(question->qtype), sizeof(question->qclass));
    return question;
}

dns_question_t* dns_question_copy(const dns_question_t* question) {
    dns_question_t* copy = malloc(sizeof(dns_question_t));
    size_t size = strlen(question->qname) + 1;
    copy->qname = malloc(size);
    memcpy(copy->qname, question->qname, size);
    copy->qtype = question->qtype;
    copy->qclass = question->qclass;
    return copy;
}

void dns_question_delete(const dns_question_t* question) {
    if (question) {
        free(question->qname);

        // TODO figure out why does this shit causes segfault for www.youtube.com

        // free(question);
    }
}

uint8_t* dns_question_to_bytearray(const dns_question_t* question) {
    if (!question) {
        return NULL;
    }

    size_t namesize = strlen(question->qname) + 1;

    uint8_t* bytearray = malloc(namesize + sizeof(question->qtype) + sizeof(question->qclass));

    memcpy(bytearray, question->qname, namesize);
    memcpy(bytearray + namesize, &question->qtype, sizeof(question->qtype));
    memcpy(bytearray + namesize + sizeof(question->qtype), &question->qclass, sizeof(question->qclass));

    return bytearray;
}

size_t dns_question_size(const dns_question_t* question) {
    if (!question) {
        return 0;
    }
    return strlen(question->qname) + 1 + sizeof(question->qtype) + sizeof(question->qclass);
}