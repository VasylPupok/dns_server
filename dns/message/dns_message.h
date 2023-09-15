#ifndef _MESSAGE_H
#define _MESSAGE_H

#include <stdbool.h>
#include <stddef.h>
#include "header/dns_header.h"
#include "question/dns_question.h"
#include "resource_record/dns_resource_record.h"

struct dns_message {
    dns_header_t* header;
    dns_question_t* question;

    dns_resource_record_t** answers;
    dns_resource_record_t** authorities;
    dns_resource_record_t** additionals;

    size_t answers_number;
    size_t authorities_number;
    size_t additionals_number;

    size_t answers_size;
    size_t authorities_size;
    size_t additionals_size;
};

typedef struct dns_message dns_message_t;


/**
 * @brief Function creates dns_message structure from given byte array
 * @param bytes given array of bytes
 * @param msg_type tells if given message is being sent as DNS question, or it's a response
 * @return pointer to dns_message structure
 */
dns_message_t* dns_message_new(const uint8_t* bytes);

/**
 * @brief Function creates copy of given DNS message
 * @param dns_msg DNS message to be copied
 * @return pointer on copy of given DNS message
*/
dns_message_t* dns_message_copy(const dns_message_t* dns_msg);

/**
 * @brief Function deletes given dns_message structure 
 * @param dns_msg given structure
 */
void dns_message_delete(const dns_message_t* dns_msg);

uint8_t* dns_message_to_bytearray(const dns_message_t* dns_msg);

size_t dns_message_size(const dns_message_t* message);

#endif