/**
 * @file msgunit.c
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-10-19
 */

#include  <string.h>
#include  <winsock2.h>
#include  <stdint.h>
#include	"udp_types.h"
#include    "msgunit.h"

int merge2unit(const struct unit_header* hdr_unit, const char* data, char* unit, int unitlen)
{
    int len_unitheader = sizeof(struct unit_header);
#ifdef WIN32
    memcpy_s(unit, unitlen, hdr_unit, len_unitheader);
    memcpy_s(unit + len_unitheader, unitlen - len_unitheader, data, hdr_unit->numbytes);
#else
    memcpy(unit, hdr_unit, len_unitheader);
    memcpy(unit + len_unitheader, data, hdr_unit->numbytes);
#endif

    return 0;
}

void extract_msg(const char* unit, const struct unit_header** hdr_unit, const char** data)
{
    *hdr_unit = (const struct unit_header*)unit;
    *data = (const char*)(unit + sizeof(struct unit_header));
}

int extract_copy_msg(const char* unit, struct unit_header* hdr_unit, char* data, int datalen)
{
    int len_unitheader = sizeof(struct unit_header);
#ifdef WIN32
    memcpy_s(hdr_unit, len_unitheader, unit, len_unitheader);
    memcpy_s(data, datalen, unit + len_unitheader, hdr_unit->numbytes);
#else
    memcpy(hdr_unit, unit, len_unitheader);
    memcpy(data, unit + len_unitheader, hdr_unit->numbytes);
#endif

    return 0;
}
