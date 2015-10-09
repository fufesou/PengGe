
/**
 * @file udp_utility.h
 * @brief Utility functions. The prefix "U_" stands for utility.
 * @author cxl
 * @version 0.1
 * @date 2015-09-20
 */

#ifndef  UDP_UTILITY_H
#define  UDP_UTILITY_H


#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>
#include <winsock2.h>
#include <stdio.h>


/**
 * @brief U_errexit Print error information and exit(1).
 *
 * @param format 
 * @param ...
 */
void U_errexit(const char* format, ...);

/**
 * @brief U_errexit_value Print error information and exit(exit_value).
 *
 * @param exit_value value for exit()
 * @param format
 * @param ...
 */
void U_errexit_value(int exit_value, const char* format, ...);

/**
 * @brief U_init_winsock2 Wrapped function of WSAStartup with error checking
 *
 * @param wsadata
 */
void U_init_winsock2(WSADATA* wsadata);

/**
 * @brief U_close_socket closesocket() wrapper with error checking
 *
 * @param sockfd socket to close
 */
void U_close_socket(SOCKET sockfd);

/**
 * @brief U_cleanup_winsock2 Wrapped function of WSACleanup with error checking
 *     When your application is finished handling the connections, call WSACleanup.
 */
void U_cleanup_winsock2(void);

int U_printf_sockinfo(SOCKET sockfd, char* msgheader);

#ifdef __cplusplus
}
#endif


#endif  // UDP_UTILITY_H
