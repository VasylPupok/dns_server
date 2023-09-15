#ifndef _DNS_HEADER_H
#define _DNS_HEADER_H

#include <stdint.h>

struct dns_header {
    uint16_t id;
    uint16_t flags;
    uint16_t qdcount;
    uint16_t anscount;
    uint16_t nscount;
    uint16_t arcount;
};

typedef struct dns_header dns_header_t;

/**
 * @brief Function creates dns_header structure from given byte array
 * @param bytes given array of bytes
 * @return pointer to dns_header structure
 */
dns_header_t* dns_header_new(const uint8_t* bytes);

/**
 * @brief Function creates copy of given dns_header
 * @param header DNS header to be copied
 * @return pointer on copy of given DNS header
 */
dns_header_t* dns_header_copy(const dns_header_t* header);

/**
 * @brief Function deletes given dns_header structure 
 * @param header given structure
 */
void dns_header_delete(const dns_header_t* header);

/**
 * @brief Converts DNS header to bytearray. 
 * Allocates memory for copy of this object as array of bytes. 
 * Size of allocated memory equal to size of given object.
 * @param header header to be converted
 * @return array representation of given DNS header
 */
uint8_t* dns_header_to_bytearray(const dns_header_t* header);

#define dns_header_size(header) 12

#define dns_header_flag_qr(flags) flags >> 15

#define dns_header_flag_opcode(flags) (flags >> 11) & 0b1111

#define dns_header_flag_aa(flags) (flags >> 10) & 1

#define dns_header_flag_tc(flags) (flags >> 9) & 1;

#define dns_header_flag_rd(flags) (flags >> 8) & 1

#define dns_header_flag_ra(flags) (flags >> 7) & 1

#define dns_header_flag_z(flags) (flags >> 4) & 0b111

#define dns_header_flag_rcode(flags) flags & 0b1111

#endif