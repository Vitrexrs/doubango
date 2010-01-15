/*
* Copyright (C) 2009 Mamadou Diop.
*
* Contact: Mamadou Diop <diopmamadou@yahoo.fr>
*	
* This file is part of Open Source Doubango Framework.
*
* DOUBANGO is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*	
* DOUBANGO is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*	
* You should have received a copy of the GNU General Public License
* along with DOUBANGO.
*
*/

/**@file tnet_stun_message.h
 * @brief STUN2 (RFC 5389) message parser.
 *
 * @author Mamadou Diop <diopmamadou(at)yahoo.fr>
 *
 * @date Created: Sat Nov 8 16:54:58 2009 mdiop
 */
#ifndef TNET_STUN_MESSAGE_H
#define TNET_STUN_MESSAGE_H

#include "../tinyNET_config.h"
#include "tnet_stun_attribute.h"

#include "tsk_buffer.h"

TNET_BEGIN_DECLS

#define TNET_STUN_MESSAGE_CREATE()						tsk_object_new(tnet_stun_message_def_t)
#define TNET_STUN_MESSAGE_SAFE_FREE(self)				tsk_object_unref(self), self = 0

/**
 * @def	TNET_STUN_TCP_UDP_DEFAULT_PORT
 *
 * @brief	Default port for both TCP and UDP protos as per RFC 5389 subclause 9.
 *
 * @remarks	Mamadou, 1/14/2010. 
**/
#define TNET_STUN_TCP_UDP_DEFAULT_PORT 3478

/**
 * @def	TNET_STUN_TLS_DEFAULT_PORT
 *
 * @brief	Default port for TLS proto as per RFC 5389 subclause 9.
 *
 * @remarks	Mamadou, 1/14/2010. 
**/
#define TNET_STUN_TLS_DEFAULT_PORT 5349

/**
 * @def	TNET_STUN_MAGIC_COOKIE
 *
 * @brief	STUN magic cookie value in network byte order as per RFC 5389 subclause 6.
 *
 * @remarks	Mamadou, 1/14/2010. 
**/
#define TNET_STUN_MAGIC_COOKIE		0x2112A442

#define TNET_STUN_HEADER_SIZE		20

/**
 * @def	TNET_IS_STUN2(PU8)
 *
 * @brief	Check if the pointer to the buffer hold a STUN header by checking that it starts with 0b00 and contain the magic cookie.
 *			As per RFC 5389 subclause 19: Explicitly point out that the most significant 2 bits of STUN are
 *			0b00, allowing easy differentiation with RTP packets when used with ICE.
 *			As per RFC 5389 subclause 6: The magic cookie field MUST contain the fixed value 0x2112A442 in
 *			network byte order.
 *
 * @remarks	Mamadou, 1/14/2010. 
 *
 * @param	PU8	The pointer to the buffer holding the STUN raw data.
**/
#define TNET_IS_STUN2(PU8)	\
	(((PU8)[0] & 0xc0) == 0x00) && \
	((*((uint32_t *)(PU8))+1) == htonl(TNET_STUN_MAGIC_COOKIE))

/**
 * @def	TNET_STUN_TRANSACID_SIZE
 *
 * @brief	STUN trasactionn ID size (96bits = 12bytes).
 *
 * @remarks	Mamadou, 1/14/2010. 
**/
#define TNET_STUN_TRANSACID_SIZE		12

/**
 * @typedef	char tnet_stun_transacid_t[TNET_STUN_TRANSACID_SIZE+1]
 *
 * @brief	Defines an alias representing the stun transaction id type.
**/
typedef uint8_t tnet_stun_transacid_t[TNET_STUN_TRANSACID_SIZE];

/**
 * @enum	tnet_stun_class_type_e
 *
 * @brief	List of STUN classes as per RFC 5389 subcaluse 6.
**/
typedef enum tnet_stun_class_type_e
{
	tsc_request = 0x00,				/**< Request class: 0b00 */
	tsc_indication = 0x01,			/**< Indication class: 0b01 */
	tsc_success_response = 0x02,	/**< Success response class: 0b10 */
	tsc_error_response = 0x03,		/**< Error/failure response class: 0b11 */
}
tnet_stun_class_type_t;

/**
 * @enum	tnet_stun_method_type_e
 *
 * @brief	List of STUN methods. RFC 5389 only define one method(Bining).
 *			As per RFC 5389 subclause 3: The method indicates which of the
 *		    various requests or indications this is; this specification defines
 *		    just one method, Binding, but other methods are expected to be
 *		    defined in other documents.
**/
typedef enum tnet_stun_method_type_e
{
	tsm_binding = 0x01, /**< Binding method: 0b000000000001 */
}
tnet_stun_method_type_t;

typedef enum tnet_stun_message_type_e
{
	/*	RFC 5389 - 6.  STUN Message Structure
		
		The message type defines the message class (request, success
		response, failure response, or indication) and the message method
		(the primary function) of the STUN message.  Although there are four
		message classes, there are only two types of transactions in STUN:
		request/response transactions (which consist of a request message and
		a response message) and indication transactions (which consist of a
		single indication message).  Response classes are split into error
		and success responses to aid in quickly processing the STUN message.

		The message type field is decomposed further into the following
		structure:

		0                 1
        2  3  4 5 6 7 8 9 0 1 2 3 4 5
       +--+--+-+-+-+-+-+-+-+-+-+-+-+-+
       |M |M |M|M|M|C|M|M|M|C|M|M|M|M|
       |11|10|9|8|7|1|6|5|4|0|3|2|1|0|
       +--+--+-+-+-+-+-+-+-+-+-+-+-+-+
	*/
	tsm_binding_request = 0x0001,				/**< 00000[0]000[0]0001 Where Class=[0][0] as per @ref tnet_stun_class_type_t */
	tsm_binding_indication = 0x0011,			/**< 00000[0]000[1]0001 Where Class=[0][1] as per @ref tnet_stun_class_type_t */
	tsm_binding_success_response = 0x0101,		/**< 00000[1]000[0]0001 Where Class=[1][0] as per @ref tnet_stun_class_type_t */
	tsm_binding_error_response = 0x0111,		/**< 00000[1]000[1]0001 Where Class=[1][1] as per @ref tnet_stun_class_type_t */
}
tnet_stun_message_type_t;

/**
 * @struct	tnet_stun_message_s
 *
 * @brief	STUN Message structure as per RFC 5389 subclause 6.
 *			http://tools.ietf.org/html/rfc5389#section-6
 *
 * @author	Mamadou
 * @date	1/14/2010
**/
typedef struct tnet_stun_message_s
{
	TSK_DECLARE_OBJECT;

	/*
	   0                   1                   2                   3
       0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |0 0|     STUN Message Type     |         Message Length        |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |                         Magic Cookie                          |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |                                                               |
      |                     Transaction ID (96 bits)                  |
      |                                                               |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*/

	tnet_stun_message_type_t type;
	uint16_t length;
	uint32_t cookie;
	tnet_stun_transacid_t transaction_id;

	tnet_stun_attributes_L_t *attributes; /**< List of all attributes associated to this message */
}
tnet_stun_message_t;

//TINYNET_API int tnet_stun_message_set_type(tnet_stun_message_t *message, tnet_stun_message_type_t type);
//TINYNET_API int tnet_stun_message_set_random_transacid(tnet_stun_message_t *message);

TINYNET_API tsk_buffer_t* tnet_stun_message_serialize(const tnet_stun_message_t *message);
TINYNET_API tnet_stun_message_t* tnet_stun_message_deserialize(const uint8_t *data, size_t size);


TINYNET_GEXTERN const void *tnet_stun_message_def_t;


TNET_END_DECLS


#endif /* TNET_STUN_MESSAGE_H */