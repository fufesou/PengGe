/**
 * @file displayiteminfo.cpp
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-12-06
 * @modified  Sun 2015-12-20 16:16:49 (+0800)
 */

#include  <QTextStream>

#include    "displayiteminfo.h"

namespace GuiCommon
{
    unsigned int CDisplayItemInfo::s_id = 0;

    CDisplayItemInfo::CDisplayItemInfo(QTextStream& vInStream)
    {
        readStream(vInStream);
    }

    CDisplayItemInfo::CDisplayItemInfo(const QString& vName, const QString& vPortraitPath, const QString& vSimpleInfo)
        : m_name(vName)
        , m_portraitPath(vPortraitPath)
        , m_simpleInfo(vSimpleInfo)
    {
        m_id = s_id++;
    }

    QTextStream& CDisplayItemInfo::writeStream(QTextStream& vOutStream) const
    {
        vOutStream << m_name << m_portraitPath << m_simpleInfo;
    }

    QTextStream& CDisplayItemInfo::readStream(QTextStream& vInStream)
    {
        vInStream >> m_name >> m_portraitPath >> m_simpleInfo;
    }
}
