/**
 * @file wordsbutton.cpp
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-12-02
 * @modified  周六 2015-12-05 17:21:33 中国标准时间
 */

#include  <QString>
#include  <QTextStream>
#include  <QFile>

#include    "wordsbutton.h"

namespace GuiCommon
{
    CWordsButton::CWordsButton(const QString& vWords, QWidget* vParent)
		: QPushButton(vWords, vParent)
	{
		QFile fqss(":/qss/wordsbutton.qss");
		fqss.open(QFile::ReadOnly | QFile::Text);
		QTextStream ts(&fqss);
		this->setStyleSheet(ts.readAll());
	}
}


