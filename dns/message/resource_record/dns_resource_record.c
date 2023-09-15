#include "dns_resource_record.h"

#include <stdlib.h> // malloc, calloc, free
#include <string.h> // strlen
#include <arpa/inet.h> // ntohs

dns_resource_record_t* dns_resource_record_new(const uint8_t* bytes) {
    dns_resource_record_t* response = malloc(sizeof(dns_resource_record_t));

    const uint8_t* ptr = bytes;
    size_t rname_len;

    if (bytes[0] & 0b11000000) {
        rname_len = 2;
    } else {
        rname_len = strlen(bytes) + 1;
    }

    response->rname = malloc(rname_len * sizeof(char));
    memcpy(response->rname, ptr, rname_len);
    ptr += rname_len;

    memcpy(&response->rtype, ptr, sizeof(uint16_t));
    ptr += sizeof(uint16_t);

    memcpy(&response->rclass, ptr, sizeof(uint16_t));
    ptr += sizeof(uint16_t);

    memcpy(&response->ttl, ptr, sizeof(uint32_t));
    ptr += sizeof(uint32_t);

    memcpy(&response->rdlength, ptr, sizeof(uint16_t));
    ptr += sizeof(uint16_t);

    uint16_t big_endian_length = ntohs(response->rdlength);

    response->rdata = calloc(big_endian_length, sizeof(response->rdata));
    memcpy(response->rdata, ptr, big_endian_length * sizeof(response->rdata));

    return response;
}

dns_resource_record_t* dns_resource_record_copy(const dns_resource_record_t* response) {
    dns_resource_record_t* copy = malloc(sizeof(dns_resource_record_t));
    
    copy->rtype = response->rtype;
    copy->rclass = response->rclass;
    copy->ttl = response->ttl;
    copy->rdlength = response->rdlength;

    size_t rname_len = strlen(response->rname) + 1;
    uint16_t rdata_len = htons(response->rdlength);

    copy->rname = calloc(rname_len, sizeof(char));
    copy->rdata = calloc(rdata_len, sizeof(uint8_t));

    memcpy(copy->rname, response->rname, rname_len * sizeof(char));
    memcpy(copy->rdata, response->rdata, rdata_len * sizeof(uint8_t));

    return copy;
}

void dns_resource_record_delete(const dns_resource_record_t* response) {
    if (response) {
        free(response->rname);
        free(response->rdata);
        free(response);
    }
}

uint8_t* dns_resource_record_to_bytearray(const dns_resource_record_t* response) {
    if (!response) {
        return NULL;
    }

    size_t namesize;

    if (response->rname[0] & 0b11000000) {
        namesize = 2;
    } else {
        namesize = strlen(response->rname) + 1;
    }

    uint8_t* bytearray = malloc(dns_resource_record_size(response));

    const uint8_t* ptr = bytearray;

    memcpy(ptr, response->rname, namesize);
    ptr += namesize;

    memcpy(ptr, &response->rtype, sizeof(uint16_t));
    ptr += sizeof(uint16_t);

    memcpy(ptr, &response->rclass, sizeof(uint16_t));
    ptr += sizeof(uint16_t);

    memcpy(ptr, &response->ttl, sizeof(uint32_t));
    ptr += sizeof(uint32_t);

    memcpy(ptr, &response->rdlength, sizeof(uint16_t));
    ptr += sizeof(uint16_t);

    memcpy(ptr, response->rdata, ntohs(response->rdlength + 1));
    
    return bytearray;
}

unsigned long dns_resource_record_size(const dns_resource_record_t* response) {
    if (!response) {
        return 0;
    }

    size_t namesize;
    if (response->rname[0] & 0b11000000) {
        namesize = 2;
    } else {
        namesize = strlen(response->rname) + 1;
    }

    return 
        namesize +
        10 +
        ntohs(response->rdlength) + 2;
}
