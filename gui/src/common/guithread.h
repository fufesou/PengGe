/**
 * @file guithread.h
 * @brief  This file provide basic thread class for network request.
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-12-20
 * @modified  Sun 2015-12-20 22:10:04 (+0800)
 */

#ifndef _GUITHREAD_H
#define _GUITHREAD_H

#include  <QVector>
#include  <QThread>

struct csclient;
struct sockaddr_in;

namespace GuiCommon
{
    class CGuiThread : public QThread
    {
        Q_OBJECT

    public:
        explicit CGuiThread(QObject* vParent = 0)
            : QThread(vParent)
            , m_pfunc(NULL)
            , m_pclient(NULL)
            , m_pservaddr(NULL)
        {}
        ~CGuiThread() {}

        void setFixedArgs(struct csclient* vClient, struct sockaddr_in* vServAddr)
        {
            m_pclient = vClient;
            m_pservaddr = vServAddr;
        }

        void setArgs(void* vFunc)
        {
            m_pfunc = vFunc;
        }

        template <typename THead, typename... TTail>
        void setArgs(void* vFunc, THead vHead, TTail... vTail)
        {
            m_vecArgs.push_back(vHead);
            setArgs(vFunc, vTail...);
        }

        void run()
        {
            if (m_pfunc == NULL)
            {
                return;
            }

            if (m_vecArgs.size() == 0)
            {
                ((void (*)(struct csclient*, struct sockaddr_in*))m_pfunc)
                        (m_pclient, m_pservaddr);
            }
            if (m_vecArgs.size() == 1)
            {

                ((void (*)(struct csclient*, struct sockaddr_in*, const QString&))m_pfunc)
                        (m_pclient, m_pservaddr, *m_vecArgs[0]);
            }
            if (m_vecArgs.size() == 2)
            {
                ((void (*)(struct csclient*, struct sockaddr_in*, const QString&, const QString&))m_pfunc)
                        (m_pclient, m_pservaddr, *m_vecArgs[0], *m_vecArgs[1]);
            }
            m_pfunc = NULL;
            m_vecArgs.clear();
        }

    private:
        void* m_pfunc;
        struct csclient* m_pclient;
        struct sockaddr_in* m_pservaddr;
        QVector<const QString*> m_vecArgs;
    };
}

#endif //GUITHREAD_H
