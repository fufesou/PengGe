#ifndef DISPLAYITEMINFO_H
#define DISPLAYITEMINFO_H

#include  <QString>

namespace GuiCommon
{
    class CDisplayItemInfo
    {
    public:
        CDisplayItemInfo(const QString& vName, const QString& vPortraitPath, const QString& vSimpleInfo);

        static void setCurMaxItemID(unsigned int vID) { s_id = vID; }

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

