#ifndef _DNS_QUESTION_H
#define _DNS_QUESTION_H

#include <stdint.h>

struct dns_question {
    uint16_t qtype;
    uint16_t qclass;
    char* qname;
};

typedef struct dns_question dns_question_t;

dns_question_t* dns_question_new(const uint8_t* bytes);

dns_question_t* dns_question_copy(const dns_question_t* question);

void dns_question_delete(const dns_question_t* question);

uint8_t* dns_question_to_bytearray(const dns_question_t* question);

unsigned long dns_question_size(const dns_question_t* question);

#endif