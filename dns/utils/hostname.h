#ifndef _HOSTNAME_H
#define _HOSTNAME_H

#include <stdint.h>

/**
 * @brief Converts human readable IPv4 string into array of 4 bytes of IPv4 address. 
 * Example: "192.168.0.1" will be represented as an array {192, 168, 0, 1};
 * @param address_str IPv4 string
 * @param arr array where those bytes will be written
 */
void parse_ipv4(const char* address_str, uint8_t* arr);

/**
 * @brief Converts human readable domain names into dns format. 
 * Example: www.google.com ---> \3www\6google\3com
 * @param domainName domain name string to be converted
*/
void convert_to_dns_format(char* domainName);

/**
 * @brief Converts dns format names into human readable ones. 
 * Example: \3www\6google\3com -> www.google.com
 * @param domainName domain name string to be converted
*/
void convert_from_dns_format(char* domainName);

#endif