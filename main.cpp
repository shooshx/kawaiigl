
#include <gl/glew.h>
#include <QApplication>
#include <QIcon>

#include <QtGui>
#include "KawaiiGL.h"

#include <iostream>

extern QGLFormat g_format;


int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	g_format.setSampleBuffers(true); 

	KawaiiGL window;

	return app.exec();
}

