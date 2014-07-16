
#include <gl/glew.h>
#include <QApplication>
#include <QIcon>

#include <QtGui>
#include "KawaiiGL.h"

#include <iostream>
#include "GradientDlg.h"

extern QGLFormat g_format;



int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    g_format.setSampleBuffers(true); 

   // GradientDlg gdlg;
   // return gdlg.exec();

    KawaiiGL window;

    return app.exec();
}

