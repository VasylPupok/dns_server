#include "server.h"

#include <dns/utils/hostname.h>
#include <stdio.h>   // FILE*
#include <stdlib.h>  // malloc, free
#include <string.h>

// shame that C does not have lambdas
static int str_comparator(const void* a, const void* b) {
    return strcmp(*(const char**)a, *(const char**)b);
}

dns_server_t* dns_server_new(const char* conf) {
    FILE* conf_file = fopen(conf, "r");

    if (!conf_file) {
        // error
        exit(EXIT_FAILURE);
    }

    dns_server_t* server = malloc(sizeof(dns_server_t));

    char buff[256];

    // read DNS server address
    do {
        fgets(buff, 256, conf_file);
    } while (buff[0] == '\n');

    size_t len = strlen(buff);  // we will replace \n by \0

    buff[len - 1] = '\0';

    server->server_addr.sin_family = AF_INET;
    server->server_addr.sin_port = htons(53);
    server->server_addr.sin_addr.s_addr = inet_addr(buff);

    // creating server socket
    if ((server->server_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        printf(stderr, "Creation of server socket failed", buff);
        exit(EXIT_FAILURE);
    }

    // read default message

    do {
        fgets(buff, 256, conf_file);
    } while (buff[0] == '\n');

    len = strlen(buff);
    server->default_blacklist_msg = malloc(len);
    memcpy(server->default_blacklist_msg, buff, len);
    server->default_blacklist_msg[len - 1] = '\0';

    // read banlist

    server->blacklist = NULL;
    char* curr_name = NULL;

    while (fgets(buff, 256, conf_file)) {
        if (buff[0] == '\n') {
            continue;
        }

        len = strlen(buff) - 1;

        curr_name = malloc(len + 1);
        for (size_t i = 0; i < len; ++i) {
            curr_name[i] = buff[len - 1 - i];
        }
        curr_name[len] = '\0';

        cvector_push_back(server->blacklist, curr_name);
    }

    qsort(server->blacklist, cvector_size(server->blacklist), sizeof(char*), str_comparator);

    return server;
}

void dns_server_delete(const dns_server_t* server) {
    cvector_free(server->blacklist);
    free(server->default_blacklist_msg);

    close(server->server_socket);

    free(server);
}

bool dns_server_hostname_banned(const dns_server_t* server, const char* hostname) {
    size_t hostname_len = strlen(hostname);
    char* hostname_cpy = malloc(hostname_len + 1);
    char* reversed_hostname = malloc(hostname_len);
    memcpy(hostname_cpy, hostname, hostname_len + 1);
    convert_from_dns_format(hostname_cpy);

    for (size_t i = 0; i < hostname_len - 1; ++i) {
        reversed_hostname[i] = hostname_cpy[hostname_len - 2 - i];
    }
    reversed_hostname[hostname_len] = '\0';

    int left = 0;
    int right = cvector_size(server->blacklist) - 1;
    int mid = left + (right - left) / 2;

    uint8_t compare_domain_name; 

    while (left <= right) {
        compare_domain_name = strncmp(server->blacklist[mid], reversed_hostname, strlen(server->blacklist[mid]));

        if (compare_domain_name == 0) {
            return true;
        } else if (compare_domain_name < 0) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }

        mid = left + (right - left) / 2;
    }

    return false;
}

size_t dns_server_resolve_as_bytearray(
    const dns_server_t* server,
    const dns_message_t* request,
    uint8_t* response_buffer,
    size_t buffer_size) {
    if (dns_server_hostname_banned(server, request->question->qname)) {
        dns_message_t* response = dns_message_copy(request);

        // we are just sending 1 additional record to user
        response->additionals_number += 1;

        // change RCODE flag to 5 - Refused
        response->header->flags = 0x8585;
        response->header->arcount = htons(1);


        if (response->additionals_size > 0) { 
            (response->additionals);
        }
        response->additionals = malloc(sizeof(dns_resource_record_t*));
        response->additionals[0] = malloc(sizeof(dns_resource_record_t));
        response->additionals_number = 1;
        dns_resource_record_t* last_record = response->additionals[0];

        size_t hostname_size = strlen(request->question->qname) + 1;
        size_t rdata_size = strlen(server->default_blacklist_msg) + 1;

        // compressed pointer on initial hostname
        last_record->rname = calloc(2, sizeof(char));
        last_record->rname[0] = 0xC0;
        last_record->rname[1] = 0x0C;

        last_record->rtype = htons(16); // TXT
        last_record->rclass = htons(1); // Internet        
        last_record->ttl = htonl(60); // 60 seconds
        last_record->rdlength = htons(rdata_size + 1);

        last_record->rdata = malloc(rdata_size + 1);
        memcpy(last_record->rdata + 1, server->default_blacklist_msg, rdata_size);
        last_record->rdata[0] = rdata_size;

        // updating data about records size
        response->additionals_size = dns_resource_record_size(last_record);

        // writing message to buffer
        uint8_t* bytes = dns_message_to_bytearray(response);
        size_t response_size = dns_message_size(response);
        memcpy(response_buffer, bytes, response_size);

        // freeing all allocated resources
        free(bytes);
        dns_message_delete(response);

        return response_size;
    }
  
    // Connect to the server
    if (connect(server->server_socket, (struct sockaddr*)&server->server_addr, sizeof(server->server_addr)) == -1) {
        printf(stderr, "Connection to server %d failed", server->server_addr.sin_addr.s_addr);
    }

    // Prepare data to send
    unsigned char* data_to_send = dns_message_to_bytearray(request);
    size_t data_size = dns_message_size(request);

   

    // Send data to the server
    int send_status = send(server->server_socket, data_to_send, data_size, 0);
    while (send_status == -1) {
        perror("Send failed");
    }

    free(data_to_send);

    // Receive response from the server
    ssize_t bytes_received = recv(server->server_socket, response_buffer, buffer_size, 0);

    

    return bytes_received;
}

dns_message_t* dns_server_resolve(const dns_server_t* server, const dns_message_t* request) {
    uint8_t response[512];
    dns_server_resolve_as_bytearray(server, request, response, 512);
    dns_message_t* parsed = dns_message_new(response);
    return parsed;
}
