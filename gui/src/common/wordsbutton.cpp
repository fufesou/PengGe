/**
 * @file wordsbutton.cpp
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-12-02
 * @modified  Thu 2015-12-03 00:07:46 (+0800)
 */

#include  <QString>
#include  <QTextStream>
#include  <QFile>

#include    "wordsbutton.h"

namespace Common
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


