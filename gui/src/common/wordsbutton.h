/**
 * @file wordsbutton.h
 * @brief  
 * @author 298516439@qq.com, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-12-02
 * @modified  周六 2015-12-05 17:21:21 中国标准时间
 */

#ifndef _WORDSBUTTON_H
#define _WORDSBUTTON_H

#include  <QPushButton>

namespace GuiCommon
{
    class CWordsButton : public QPushButton
	{
		Q_OBJECT

	public:
        explicit CWordsButton(const QString& vWords, QWidget* vParent = 0);
	};
}


#endif //WORDSBUTTON_H
