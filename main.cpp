
#include <QApplication>
#include <QIcon>

#include <QtGui>
#include "KawaiiGL.h"


extern QGLFormat g_format;




int main(int argc, char *argv[])
{

	QApplication app(argc, argv);

	g_format.setSampleBuffers(true); 
	g_format.setDoubleBuffer(false); // eH?

	KawaiiGL window;
	//window.show();

	float v[10];
	glGetFloatv(GL_SAMPLES_ARB, v);

	return app.exec();
}

