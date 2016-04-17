/**
 * @file main.cpp
 * @brief  
 * @author 298516439@qq.com, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-12-02
 */

#include  <stdio.h>
#include  <stdlib.h>
#include  <QApplication>
#include    "clientcontroller.h"


int main(int argc, char *argv[])
{
    if (argc < 3) {
        printf("usage: client <server ip> <port>.");
        exit(1);
    }

    QApplication app(argc, argv);

    GuiClient::CController clientController(argv[1], atoi(argv[2]));
    (void)clientController;

    return app.exec();
}
