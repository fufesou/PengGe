/**
 * @file displayiteminfo.cpp
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-12-06
 * @modified  Tue 2015-12-08 00:41:24 (+0800)
 */

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
