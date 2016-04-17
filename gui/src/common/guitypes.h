/**
 * @file guitypes.h
 * @brief  
 * @author 298516439@qq.com, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-12-05
 * @modified  Sun 2015-12-06 14:14:11 (+0800)
 */

#ifndef _GUITYPES_H
#define _GUITYPES_H

namespace GuiCommon
{
    /** enum EMesgSendRecv is used to describe the message orientation.
     *
     * With different orientations, message log widget will display message in different ways.
     */
    enum EMsgSendRecv { eSend, eRecv };

    /** enum ERequestStatus is used to describe the status of requests. */
    enum ERequestStatus
    {
        eRequesting,    /**< the status is set while request is in process */
        eTimeout,       /**< request timeout */
        eFail,         	/**< the information is incorrect */
        eSucceed,      	/**< succeed . */
    };
}

#endif //GUITYPES_H
