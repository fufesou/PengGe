/**
 * @file main.cpp
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-12-02
 */

#include <QApplication>
#include "loginwindow.h"


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

	Server::CLoginWindow loginWin;
	loginWin.show();

    return app.exec();
}
