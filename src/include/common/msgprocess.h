/**
 * @file msgprocess.h
 * @brief  This file contains various kinds of message handler functions.
 * @author 298516439@qq.com, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2016-01-06
 * @modified  Tue 2016-01-12 23:12:07 (+0800)
 */

#ifndef _MSGPROCESS_H
#define _MSGPROCESS_H

#ifdef __cplusplus
extern "C" {
#endif

JXIOT_API void jxprocesslist_init(void);

JXIOT_API struct list_head* jxprocesslist_client_default_get(void);
JXIOT_API struct list_head* jxprocesslist_server_default_get(void);


/** @defgroup client process functions
 *
 * @{
 */

/**
 * @brief  jxprocess_client_am The income message from server
 * will be handle by the coressponding 'react' method in this function.
 *
 * The actual process function will not process data until one byte after 'process id'. Thus
 * the message data passed into the process function is 'inmsg + sizeof(struct jxmsg_header) + sizeof(uint32_t)'.
 *
 * @param inmsg The format of inmsg is: \n
 * ---------------------------------------------------------------------------------------------- \n
 * | struct jxmsg_header | process id(uint32_t) | process data(char*) | ... |                     \n
 * ---------------------------------------------------------------------------------------------- \n
 * or \n
 * ------------------------------------------------------------------------------------------------------------------------ \n
 * | struct jxmsg_header | process id(uint32_t) | user id(uint32_t) | process data(char*) | ...                             \n
 * ------------------------------------------------------------------------------------------------------------------------ \n
 * @param outmsg
 * @param outmsglen
 */
int JXIOT_CALLBACK jxprocess_client_am(char* inmsg, char*outmsg, __jxinout uint32_t* outmsglen);

/** @} */


/** @defgroup server process functions
 *
 * @{
 */

/**
 * @brief  jxprocess_server_am message will be dispatched to account operations.
 *
 * @param inmsg The format of inmsg is \n
 * -----------------------------------------------------------------------------------------\n
 * | struct jxmsg_header | process id(uint32_t) | data(char*) | ... |                       \n
 * -----------------------------------------------------------------------------------------\n
 *
 * @outmsg
 * @outmsglen
 *
 * @return   
 */
int JXIOT_CALLBACK jxprocess_server_am(char* inmsg, char* outmsg, __jxinout uint32_t* outmsglen);
int JXIOT_CALLBACK jxprocess_af_server_am(char* userdata, char* msg);

/**
 * @brief  jxprocess_server_port handle the process of registering client receiving socket's port.
 *
 * @param inmsg        The format of inmsg is \n
 * -----------------------------------------------------------------------------------------\n
 * | struct jxmsg_header | ... |                                                            \n
 * -----------------------------------------------------------------------------------------\n
 * @param outmsg        unused
 * @param outmsglen     unused
 *
 * @return   
 */
int JXIOT_CALLBACK jxprocess_server_port(char* inmsg, char* outmsg, __jxinout uint32_t* outmsglen);

/** @} */

#ifdef __cplusplus
}
#endif


#endif //MSGPROCESS_H
