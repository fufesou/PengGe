
/**
 * @file utility.h
 * @brief utility functions
 * @author cxl
 * @version 0.1
 * @date 2015-09-20
 */

#ifndef  UTILITY_H
#define  UTILITY_H


#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>
#include <winsock2.h>


/**
 * @brief errexit Print error information and exit(1).
 *
 * @param format 
 * @param ...
 */
void errexit(const char* format, ...);

/**
 * @brief init_winsock2 Wrapped function of WSAStartup with error checking
 *
 * @param wsadata
 */
void init_winsock2(WSADATA* wsadata);

/**
 * @brief cleanup_winsock2 Wrapped function of WSACleanup with error checking
 *     When your application is finished handling the connections, call WSACleanup.
 */
void cleanup_winsock2(void);


#ifdef __cplusplus
}
#endif


#endif  // UTILITY_H
