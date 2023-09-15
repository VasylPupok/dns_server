#include "hostname.h"

#include <stdlib.h>
#include <string.h>

void parse_ipv4(const char* addr_str, uint8_t* arr) {
    uint8_t curr_domain = 0;
    char buff[4];
    buff[3] = '\0';
    uint8_t buff_ptr = 0;

    size_t len = strlen(addr_str);

    for (size_t i = 0; i < len; ++i) {
        if (addr_str[i] == '.') {
            buff[buff_ptr] = '\0';
            arr[curr_domain++] = atoi(buff);
            buff_ptr = 0;
        } else {
            buff[buff_ptr++] = addr_str[i];
        }
    }

    buff[buff_ptr] = 0;
    arr[curr_domain] = atoi(buff);
}

void convert_to_dns_format(char* domainName) {
    uint16_t currIndex = 0;
    uint8_t currDomainLength = 0;

    while (domainName[currIndex]) {
        if (domainName[currIndex] == '.') {
            // shift all chars in domain to the right and prefix with number
            for (uint8_t i = 0; i < currDomainLength; ++i) {
                domainName[currIndex - i] = domainName[currIndex - i - 1];
            }
            domainName[currIndex - currDomainLength] = currDomainLength;
            currDomainLength = 0;
        } else {
            currDomainLength++;
        }
        currIndex++;
    }

    // shift all chars in domain to the right and prefix with number once more
    for (uint8_t i = 0; i < currDomainLength; ++i) {
        domainName[currIndex - i] = domainName[currIndex - i - 1];
    }
    domainName[currIndex - currDomainLength] = currDomainLength;
    currDomainLength = 0;
}

void convert_from_dns_format(char* domainName) {
    size_t len = strlen(domainName) - 1;
    size_t curr_domain_len;

    for (size_t i = 0; i < len; ++i) {
        curr_domain_len = domainName[i];
        for (size_t j = 0; j < curr_domain_len; ++j) {
            domainName[i + j] = domainName[i + j + 1];
        }
        domainName[i + curr_domain_len] = '.';
        i += curr_domain_len;
    }
    domainName[len] = '\0';
}
