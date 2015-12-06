#include    "displayiteminfo.h"

namespace GuiCommon
{
    unsigned int CDisplayItemInfo::s_id = 0;

    CDisplayItemInfo::CDisplayItemInfo(const QString& vName, const QString& vPortraitPath, const QString& vSimpleInfo)
        : m_name(vName)
        , m_portraitPath(vPortraitPath)
        , m_simpleInfo(vSimpleInfo)
    {
        m_id = s_id++;
    }
}
