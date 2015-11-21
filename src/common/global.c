/**
 * @file global.c
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-11-10
 * @modified  Mon 2015-11-16 23:45:01 (+0800)
 */

#include  <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

const char* g_loginmsg_header = "login: ";
const char* g_logoutmsg_header = "logout";
const char* g_loginmsg_SUCCESS = "login_success";
const char* g_loginmsg_FAIL = "login_fail";
const char g_login_delimiter = ' ';

const char* g_exit = "exit";
const char* g_close = "close";

char g_succeed = '0';
char g_fail = '1';

uint32_t g_curmaxid = 0;
uint32_t g_timeout_verification = 120;

uint32_t g_len_randomcode = 4;

#ifdef __cplusplus
}
#endif
