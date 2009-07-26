#include <QMenu>
#include <QFileInfo>
#include <QCoreApplication>
#include <float.h>
#include <QKeyEvent>

#include "MyMainControl.h"
#include "Vec.h"

// no where else to put it
const Vec MAX_VEC(FLT_MAX, FLT_MAX, FLT_MAX), MIN_VEC(-FLT_MAX, -FLT_MAX, -FLT_MAX);
const FuncVecToRgb FuncVecToRgb::f;
const CastToColor CastToColor::f;


MyMainControl* g_main = NULL;

#ifdef _DEBUG
static QString g_configName = "DEBUG";
#else
static QString g_configName = "RELEASE";
#endif

void MyMainControl::setTitleFilename(const QString& filename)
{
	if (!filename.isEmpty())
		setWindowTitle(QString("%1:%2 - %3").arg(g_configName).arg(qApp->applicationName()).arg(filename));
	else
		setWindowTitle(QString("%1:%2").arg(g_configName).arg(qApp->applicationName()));
}


QString strippedName(const QString &fullFileName)
{
	return QFileInfo(fullFileName).fileName();
}

void RecentFiles::reload()
{
	const QStringList &files = *m_prop;

	int numRecentFiles = qMin(files.size(), m_actions.size());

	for (int i = 0; i < numRecentFiles; ++i) 
	{
	//	QString text = tr("&%1 %2").arg(i + 1).arg(strippedName(files[i]));
		m_actions[i]->setText(files[i]);
		m_actions[i]->setData(files[i]);
		m_actions[i]->setVisible(true);
	}
	for (int j = numRecentFiles; j < m_actions.size(); ++j)
		m_actions[j]->setVisible(false);

}

void RecentFiles::addActions(QAction *a1, QAction *a2, QAction *a3, QAction *a4)
{
	addAction(a1);
	addAction(a2);
	addAction(a3);
	addAction(a4);
}

void RecentFiles::addAction(QAction *act)
{
	if (act == NULL)
		return;
	act->setData(m_actions.size()); // its index.
	m_actions.append(act);
	connect(act, SIGNAL(triggered(bool)), this, SLOT(actionTrig()));
}


void RecentFiles::touched(const QString& s)
{
	QStringList files = *m_prop;
	files.removeAll(s);
	files.prepend(s);
	while (files.size() > m_actions.size())
		files.removeLast();
	*m_prop = files;

	reload();
}

void RecentFiles::actionTrig()
{
	QAction *snd = dynamic_cast<QAction*>(sender());
	const QString& name = snd->data().toString();
	emit fileTriggered(name);
}


void MyMainControl::keyPressEvent(QKeyEvent* ke)
{
	if (ke->key() >= Qt::Key_0 && ke->key() < Qt::Key_9)
	{
		emit numKeyPressed(ke->key() - Qt::Key_0, ke->modifiers()); 
		ke->accept();
	}
	switch (ke->key())
	{
	case Qt::Key_Left: case Qt::Key_Up: case Qt::Key_Right: case Qt::Key_Down:
		emit arrowKeyPressed(ke->key(), ke->modifiers());
//		ke->accept();
		break;

	}

	QMainWindow::keyPressEvent(ke);
}