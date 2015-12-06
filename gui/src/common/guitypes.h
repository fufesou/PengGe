/**
 * @file guitypes.h
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
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

    /** enum ELoginStatus is used to describe the status of login. */
    enum ELoginStatus
    {
        eLoging,        /**< the status is set while login is in process */
        eTimeout,       /**< login timeout */
        eFail,         /**< the 'user info' or 'passwd' is incorrect. */
        eSucceed,      /**< succeed login. */
    };
}

#endif //GUITYPES_H
