/**
 * @file processlist.h
 * @brief  This file defines the utility functions for message process list.
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2016-01-05
 * @modified  Wed 2016-01-06 22:18:15 (+0800)
 */

#ifndef _PROCESSLIST_H
#define _PROCESSLIST_H

#ifdef __cplusplus
extern "C" {
#endif

typedef int (JXIOT_CALLBACK *pfunc_msgprocess_t)(char* inmsg, char* outmsg, uint32_t* outmsglen);
typedef int (JXIOT_CALLBACK *pfunc_msgprocess_af_t)(char* userdata, char* msg);


struct list_process_t {
    struct list_head        listnode;
    uint8_t                 mflag;
    pfunc_msgprocess_t      pprocess;
    pfunc_msgprocess_af_t   pprocess_af;
};

/**
 * @brief  jxprocesslist_set    this function will set process functions for mflag, if mflag does not exist before,
 * new list node will be created.
 *
 * @param processlist_head      the process list head pointer.
 * @param mflag                 target message flag.
 * @param pfunc_process         if NULL and force is not set, nothing will be done with 'pprocess'.
 * @param pfunc_process_af      if NULL and force is not set, nothing will be done with 'pprocess_af'.
 * @param force                 if set, pfunc_process and pfunc_process_af will replace 'pprocess' and 'pprocess_af' anyhow.
 *
 * @return   
 */
JXIOT_API void jxprocesslist_set(struct list_head* processlist_head, uint8_t mflag, pfunc_msgprocess_t pfunc_process, pfunc_msgprocess_af_t pfunc_process_af, int force);

JXIOT_API void JXIOT_CALLBACK jxprocesslist_clear(void* processlist_head);

/**
 * @brief  jxprocesslist_process_valid this funtion checks whether a valid 'pprocess' is set in any list node.
 *
 * @param processlist_head
 *
 * @return   1 if valid, 0 if invalid.
 */
int jxprocesslist_process_valid(const struct list_head* processlist_head);

/**
 * @brief  jxprocesslist_process_af_valid this funtion checks whether a valid 'pprocess' is set in any list node.
 *
 * @param processlist_head
 *
 * @return   1 if valid, 0 if invalid.
 */
int jxprocesslist_process_af_valid(const struct list_head* processlist_head);

pfunc_msgprocess_t jxprocesslist_process_get(const struct list_head* processlist_head, uint8_t mflag);

pfunc_msgprocess_af_t jxprocesslist_process_af_get(const struct list_head* processlist_head, uint8_t mflag);


#ifdef __cplusplus
}
#endif

#endif //PROCESSLIST_H
