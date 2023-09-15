#ifndef _DNS_SERVER_H
#define _DNS_SERVER_H

#include <stdbool.h>

#include <netinet/in.h>
#include <netinet/ip.h>
#include <netdb.h>

#include <dns/message/dns_message.h>
#include <third-party/avsej/hashset/hashset.h>
#include <third-party/eteran/c-vector/cvector.h>

typedef struct sockaddr_in sockaddr_in;


typedef struct dns_server {
    sockaddr_in server_addr;
    int server_socket;

    cvector_vector_type(const char*) blacklist;

    char* default_blacklist_msg;
} dns_server_t;

dns_server_t* dns_server_new(const char* path);
void dns_server_delete(const dns_server_t* server);

bool dns_server_hostname_banned(const dns_server_t* server, const char* hostname);

dns_message_t* dns_server_resolve(const dns_server_t* server, const dns_message_t* request);

/**
 * @brief This function resolves given DNS request and retrieves resulting DNS response as array of bytes
 * @param server server, which will resolve given request
 * @param request DNS message which contains user's request
 * @param response_buffer buffer, where response will be written
 * @param buffer_size size of response_buffer in bytes
 * @return size of DNS response in bytes
*/
size_t dns_server_resolve_as_bytearray(
    const dns_server_t* server, 
    const dns_message_t* request, 
    uint8_t* response_buffer, 
    size_t buffer_size
    );

#endif