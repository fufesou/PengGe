/**
 * @file main.cpp
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-12-02
 */

#include  <QApplication>
#include    "clientcontroller.h"
#include    "chatwidget.h"


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    /*
    GuiClient::CController clientController;
    clientController.showLogin();
    */
    GuiCommon::CChatWidget chatWidget;
    chatWidget.show();

    return app.exec();
}
