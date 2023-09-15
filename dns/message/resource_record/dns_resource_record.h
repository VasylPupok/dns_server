#ifndef _DNS_RESPONSE_H
#define _DNS_RESPONSE_H

#include <stdint.h>

typedef struct dns_response_fields dns_response_fields_t;

struct dns_resource_record {
    char* rname;
    uint16_t rtype;
    uint16_t rclass;
    uint32_t ttl;
    uint16_t rdlength;
    uint8_t* rdata;
};

typedef struct dns_resource_record dns_resource_record_t;

dns_resource_record_t* dns_resource_record_new(const uint8_t* bytes);

dns_resource_record_t* dns_resource_record_copy(const dns_resource_record_t* response);

void dns_resource_record_delete(const dns_resource_record_t* response);

uint8_t* dns_resource_record_to_bytearray(const dns_resource_record_t* response);

unsigned long dns_resource_record_size(const dns_resource_record_t* response);

#endif