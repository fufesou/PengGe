/**
 * @file msgunit.h
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-10-19
 */

#ifndef  MSGUNIT_H
#define  MSGUNIT_H

#ifdef __cplusplus
extern "C" {
#endif

struct unit_header {
    struct hdr header;
    SOCKADDR_IN addr;
    int addrlen;
    ssize_t numbytes;
};


int merge2unit(const struct unit_header* hdr_unit, const char* data, char* unit, int unitlen);
void extract_msg(const char* unit, const struct unit_header** hdr_unit, const char** data);
int extract_copy_msg(const char* unit, struct unit_header* hdr_unit, char* data, int datalen);


#ifdef __cplusplus
}
#endif


#endif  // MSGUNIT_H
