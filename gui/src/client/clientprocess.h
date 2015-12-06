/**
 * @file clientprocess.h
 * @brief  
 * @author cxl, hermes-sys, <xiaolong.chen@hermes-sys.com>
 * @version 0.1
 * @date 2015-12-06
 * @modified  Sun 2015-12-06 13:39:27 (+0800)
 */

#ifndef _CLIENTPROCESS_H
#define _CLIENTPROCESS_H

#include  <QString>

namespace GuiCommon
{
    enum ELoginStatus;
}

namespace GuiClient
{
    class CClientProcess
    {
    public:
        CClientProcess(void);
        
        enum ELoginStatus login(const QString& vUserInfo, const QString& vPasswd, unsigned int vTimeoutSec);

    private:
        void initClient(void);
    }
}

#endif //CLIENTPROCESS_H
