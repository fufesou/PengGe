/**
 * @file displayiteminfo.h
 * @brief  
 * @author 298516439@qq.com, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-12-06
 * @modified  Sun 2015-12-20 16:17:16 (+0800)
 */

#ifndef DISPLAYITEMINFO_H
#define DISPLAYITEMINFO_H

#include  <QString>

QT_FORWARD_DECLARE_CLASS(QTextStream)

namespace GuiCommon
{
    class CDisplayItemInfo
    {
    public:
        static void setCurMaxItemID(unsigned int vID) { s_id = vID; }

        explicit CDisplayItemInfo(QTextStream& vInStream);
        explicit CDisplayItemInfo(const QString& vName, const QString& vPortraitPath = ":/img/aa9.jpg", const QString& vSimpleInfo = QString(""));

        QTextStream& writeStream(QTextStream& vOutStream) const;
        QTextStream& readStream(QTextStream& vInStream);

        const QString& getName() const { return m_name; }
        const QString& getPortraitPath() const { return m_portraitPath; }
        const QString& getSimpleInfo() const { return m_simpleInfo; }

        QString& fetchName() { return m_name; }
        QString& fetchPortraitPath() { return m_portraitPath; }
        QString& fetchSimpleInfo() { return m_simpleInfo; }

    private:
        unsigned int m_id;
        QString      m_name;
        QString      m_portraitPath;
        QString      m_simpleInfo;

        static unsigned int s_id;
    };
}

#endif // DISPLAYITEMINFO_H

