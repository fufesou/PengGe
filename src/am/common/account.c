/**
 * @file account.c
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-11-10
 * @modified  Tue 2015-12-08 21:22:55 (+0800)
 */

#include  <stdlib.h>
#include  <string.h>
#include    "common/cstypes.h"
#include    "common/jxiot.h"
#include    "common/config_macros.h"
#include    "am/account.h"

#define REGISTER_ACCOUNT_PROCESS_BEGIN \
    static struct account_method_t s_methodarr[] = { 

#ifdef _MSC_VER
#define REGISTER_ACCOUNT_PROCESS(method) \
        { \
            #method, \
            am_##method##_reply, \
            am_##method##_react \
        }, 
#else
#define REGISTER_ACCOUNT_PROCESS(method) \
        { \
            .methodname = #method, \
            .reply = am_##method##_reply, \
            .react = am_##method##_react \
        }, 
#endif

#define REGISTER_ACCOUNT_PROCESS_END \
    };


#ifdef __cplusplus
extern "C"
{
#endif

REGISTER_ACCOUNT_PROCESS_BEGIN
    REGISTER_ACCOUNT_PROCESS(account_create)
    REGISTER_ACCOUNT_PROCESS(account_verify)
    REGISTER_ACCOUNT_PROCESS(account_login)
    REGISTER_ACCOUNT_PROCESS(account_logout)
    REGISTER_ACCOUNT_PROCESS(account_changeusername)
    REGISTER_ACCOUNT_PROCESS(account_changepasswd)
    REGISTER_ACCOUNT_PROCESS(account_changegrade)
REGISTER_ACCOUNT_PROCESS_END

static int s_method_sorted = 0;
static int s_sizeof_method = SIZEOF_ARR(s_methodarr);

static int s_comp_method(const void* lhs, const void* rhs);
static uint32_t s_findmethod_sorted(const char* methodname);
static uint32_t s_findmethod_unsorted(const char* methodname);

#ifdef __cplusplus
}
#endif

int s_comp_method(const void* lhs, const void* rhs)
{
    return strcmp(((const struct account_method_t*)lhs)->methodname, ((const struct account_method_t*)rhs)->methodname);
}

void am_method_sort(void)
{
    qsort(s_methodarr, s_sizeof_method, sizeof(s_methodarr[0]), s_comp_method);
    s_method_sorted = 1;
}

const struct account_method_t* am_method_get(uint32_t methodid)
{
    if ((int)methodid < s_sizeof_method) {
        return &s_methodarr[methodid];
    } else {
        return NULL;
    }
}

uint32_t am_method_getid(const char* methodname)
{
    if (s_method_sorted) {
        return s_findmethod_sorted(methodname);
    } else {
        return s_findmethod_unsorted(methodname);
    }
}

uint32_t s_findmethod_sorted(const char* methodname)
{
    int low = 0;
    int mid = s_sizeof_method >> 1;
    int high = s_sizeof_method - 1;

    if (!s_method_sorted) {
        am_method_sort();
    }

    while (low < high) {
        while ((strcmp(methodname, s_methodarr[mid].methodname) < 0) && (low < high)) {
            high = mid - 1;
            mid = (low + high) >> 1;
        }

        if (strcmp(methodname, s_methodarr[mid].methodname) == 0) {
            return mid;
        }

        while ((strcmp(methodname, s_methodarr[mid].methodname) > 0) && (low < high)) {
            low = mid + 1;
            mid = (low + high) >> 1;
        }

        if (strcmp(methodname, s_methodarr[mid].methodname) == 0) {
            return mid;
        }
    }

    return INVALID_METHOD_ID;
}

uint32_t s_findmethod_unsorted(const char* methodname)
{
    int i = 0;
    for (; i<s_sizeof_method; ++i) {
        if (strcmp(methodname, s_methodarr[i].methodname) == 0) {
            return (uint32_t)i;
        }
    }
    return INVALID_METHOD_ID;
}

int am_set_react(const char* methodname, pfunc_react_t func_react)
{
    uint32_t method_index = am_method_getid(methodname);
    if (method_index == INVALID_METHOD_ID) {
        return 0;
    }
    s_methodarr[method_index].react = func_react;

    return 1;
}

int am_set_reply(const char* methodname, pfunc_reply_t func_reply)
{
    uint32_t method_index = am_method_getid(methodname);
    if (method_index == INVALID_METHOD_ID) {
        return 0;
    }
    s_methodarr[method_index].reply = func_reply;

    return 1;
}
