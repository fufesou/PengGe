/**
 * @file main2.c
 * @brief 
 * @author cxl
 * @version 0.1
 * @date 2015-09-20
 */


#include <winsock2.h>
#include <stdio.h>
#include "utility.h"


int main(int argc, char* argv[])
{
    WSADATA wsadata;
    init_winsock2(&wsadata);
    cleanup_winsock2();

    return 0;
}
