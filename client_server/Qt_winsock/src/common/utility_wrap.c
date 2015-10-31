/**
 * @file utility_wrap.c
 * @brief  
 * @author cxl, hermes-sys, <xiaolong.chen@hermes-sys.com>
 * @version 0.1
 * @date 2015-10-29
 * @modified  ���� 2015-10-29 18:35:15 �й���׼ʱ��
 */

#include  <memory.h>
#include    "macros.h"
#include    "utility_wrap.h"

int cs_memcpy(void* dst, size_t dstsize, const void* src, size_t count)
{
    if (dstsize < count) {
        return -1;
    }

#ifdef _CHECK_ARGS
    if (dst == NULL || src == NULL) {
        return 1;
    }
#endif

#ifdef WIN32
    memcpy_s(dst, dstsize, src, count);
#else
    memcpy(dst, src, count);
#endif

    return 0;
}
