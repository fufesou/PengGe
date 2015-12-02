/**
 * @file wordsbutton.h
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-12-02
 * @modified  Wed 2015-12-02 21:24:44 (+0800)
 */

#ifndef _WORDSBUTTON_H
#define _WORDSBUTTON_H

#include  <QPushButton>

namespace Common
{
    class CWordsButton : public QPushButton
	{
		Q_OBJECT

	public:
        explicit CWordsButton(const QString& vWords, QWidget* vParent = 0);
	};
}


#endif //WORDSBUTTON_H
