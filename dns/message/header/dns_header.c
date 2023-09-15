#include "dns_header.h"

#include <stdlib.h> // malloc, calloc, free

dns_header_t* dns_header_new(const uint8_t* bytes) {
    dns_header_t* header = malloc(sizeof(dns_header_t));
    memcpy(header, bytes, sizeof(dns_header_t));
    return header;
}

dns_header_t* dns_header_copy(const dns_header_t* header) {
    dns_header_t* copy = malloc(sizeof(dns_header_t));
    memcpy(copy, header, sizeof(dns_header_t));
    return copy;
}

void dns_header_delete(const dns_header_t* header) {
    free(header);
}

uint8_t* dns_header_to_bytearray(const dns_header_t* header) {
    uint8_t* bytearray = malloc(dns_header_size(header));
    uint8_t* ptr = bytearray;

    memcpy(bytearray, ptr, sizeof(uint16_t));
    ptr += sizeof(uint16_t);

    memcpy(bytearray, ptr, sizeof(uint16_t));
    ptr += sizeof(uint16_t);
    
    memcpy(bytearray, ptr, sizeof(uint16_t));
    ptr += sizeof(uint16_t);

    memcpy(bytearray, ptr, sizeof(uint16_t));
    ptr += sizeof(uint16_t);

    memcpy(bytearray, ptr, sizeof(uint16_t));
    ptr += sizeof(uint16_t);

    memcpy(bytearray, ptr, sizeof(uint16_t));
    ptr += sizeof(uint16_t);

    return bytearray;
}