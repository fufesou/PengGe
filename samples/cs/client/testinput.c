/**
 * @file testinput.c
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-11-19
 * @modified  周四 2015-11-19 18:16:30 中国标准时间
 */

#include  <stdio.h>
#include  <stdlib.h>
#include  <ctype.h>
#include  <stdint.h>

static int s_conv_seg(char** from, char** to, uint32_t* size_to);
static int s_conv_segN(char** from, char** to, uint32_t* size_to, int vN);

/**
 * @brief  conv_cmd_msg This function converts the cmd(user input) to msg(net deliver message).
 *
 * @param cmd
 * @param msg
 * @param size_msg
 *
 * @return   The return value indicates whether the output message should be delivered.
 *
 * 1. 0, deliver message, cmd is successfully converted to msg.
 * 2. 1, donot deliver message, cmd is just a comment.
 * 3. 2, donot deliver message, the message is invalid.
 * 4. 3, donot deliver message, cmd failed to convert to msg 
 * with errors(msg is NULL, size_msg is not long enough).
 */
int conv_cmd_msg(char* cmd, char* msg, uint32_t size_msg);

int s_conv_seg(char** from, char** to, uint32_t* size_to)
{
    uint32_t len = 0;

    while (isspace(**from)) {
        ++(*from);
    }

    ++len;
    while ((len < *size_to) && !isspace(*(*to)++ = *(*from)++)) {
        ++len;
    }

    if (len >= *size_to) {
        return 3;
    }

    *((*to) - 1) = '\0';
    *size_to -= (len + 1);

    return 0;
}

int s_conv_segN(char** from, char** to, uint32_t* size_to, int vN)
{
    int i = 0;
    while (i < vN) {
        if (s_conv_seg(from, to, size_to) != 0) {
            return 3;
        }
    }
    return 0;
}

int conv_cmd_msg(char* cmd, char* msg, uint32_t size_msg)
{
    char msgtype;

    if (size_msg < 2) {
        return 3;
    }

    if (cmd == NULL || msg == NULL) {
        return 3;
    }

    while (isspace(*cmd)) {
        ++cmd;
    }
    if (*cmd == '#') {
        return 1;
    }

    msgtype = *msg++ = *cmd++;
    *msg++ = ' ';
    size_msg -= 2;

    switch (msgtype) {
        case '0':
            return s_conv_seg(&cmd, &msg, &size_msg);

        case '1':
        case '2':
            return s_conv_segN(&cmd, &msg, &size_msg, 2);

        case '3':
        case '4':
            return s_conv_segN(&cmd, &msg, &size_msg, 3);

        default:
            return 2;
    }

    return 0;
}
