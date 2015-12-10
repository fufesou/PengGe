/**
 * @file main.cpp
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
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

    return app.exec();
}
