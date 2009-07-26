#ifndef MYMAINCONTROL_H__INCLUDED
#define MYMAINCONTROL_H__INCLUDED

#include <QMainWindow>
#include <Qt>

#include "AppSettings.h"

class ControlWidget;

class RecentFiles : public QObject
{
	Q_OBJECT
public:
	void reload();
	void addActions(QAction *a1, QAction *a2 = NULL, QAction *a3 = NULL, QAction *a4 = NULL);
	void addAction(QAction *act);
	void setProp(TypeProp<QStringList>* prop) { m_prop = prop; }

	void touched(const QString& s);

private:
	QList<QAction*> m_actions;
	TypeProp<QStringList> *m_prop;
private slots:
	void actionTrig();
signals:
	void fileTriggered(QString filename);

};

class MyMainControl;

extern MyMainControl* g_main;

// abstract class for the main control
class MyMainControl : public QMainWindow
{
	Q_OBJECT
public:
	MyMainControl(QWidget* parent, Qt::WFlags flags, const QString settName) 
		: QMainWindow(parent, flags), m_sett(settName)
	{
		g_main = this;
	}
	AppSettings& sett() { return m_sett; }

	virtual void showMessage(const QString& msg) {}
	void setTitleFilename(const QString& filename);

	ControlWidget *m_curctl;

protected:
	virtual void keyPressEvent(QKeyEvent * e);

	AppSettings m_sett;

signals:
	void numKeyPressed(int key, Qt::KeyboardModifiers buttonState);
	void arrowKeyPressed(int key, Qt::KeyboardModifiers buttonState);

};




#endif // MYMAINCONTROL_H__INCLUDED
