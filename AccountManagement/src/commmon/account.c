/**
 * @file account.c
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-11-10
 * @modified  周二 2015-11-10 19:45:17 中国标准时间
 */

#include    "account_macros.h"
#include    "account.h"

#ifdef __cplusplus
extern "C"
{
#endif

REGISTER_ACCOUNT_PROCESS_BEGIN
    REGISTER_ACCOUNT_PROCESS(account_create)
    REGISTER_ACCOUNT_PROCESS(account_create_verify)
    REGISTER_ACCOUNT_PROCESS(account_login)
    REGISTER_ACCOUNT_PROCESS(account_inquire)
    REGISTER_ACCOUNT_PROCESS(account_changeusername)
    REGISTER_ACCOUNT_PROCESS(account_changepasswd)
    REGISTER_ACCOUNT_PROCESS(account_changegrade)
REGISTER_ACCOUNT_PROCESS_END

static int s_method_sorted = 0;
static int s_sizeof_method = SIZEOF_ARR(s_methodarr);

static inline int s_comp_method(const void* lhs, const void* rhs)
{
    return strcmp(((const struct account_method_t*)lhs)->methodname, (const struct account_method_t*)rhs.methodname);
}

static uint32_t s_findmethod_sorted(const char* methodname);
static uint32_t s_findmethod_unsorted(const char* methodname);

#ifdef __cplusplus
}
#endif

void am_method_sort()
{
    qsort(s_methodarr, s_sizeof_method, sizeof(s_methodarr[0]), s_comp_str);
    s_method_sorted = 1;
}

const struct account_method_t* am_method_getname(uint32_t methodid)
{
    if (methodid < s_sizeof_method) {
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

    while (low < high) {
        while ((strcmp(methodname, s_methodarr[mid].methodname) < 0) && (low < high)) {
            high = mid - 1;
            mid = (low + high) >> 1;
        }

        if (strcmp(methodname, s_methodarr[mid].methodname) == 0) {
            return mid;
        }

        while ((strcmp(method, s_methodarr[mid].methodname) > 0) && (low < high)) {
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
    for (int i=0; i<s_sizeof_method; ++i) {
        if (strcmp(methodname, s_methodarr[i].methodname) == 0) {
            return (uint32_t)i;
        }
    }
    return INVALID_METHOD_ID;
}
