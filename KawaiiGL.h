#pragma once
#ifndef KAWAIIGL_H
#define KAWAIIGL_H

#include <QMainWindow>
#include "ui_KawaiiGL.h"
#include "T2GLWidget.h"

class KwEdit;
class ControlPanel;
class ProjBrowser;

class KawaiiGL : public QMainWindow
{
	Q_OBJECT

public:
	KawaiiGL(QWidget *parent = 0);
	~KawaiiGL() {}

	KwSettings& sett() { return m_sett; }
	T2GLWidget* view() { return m_kView; }
	void processCmdArgs();
protected:
	virtual void closeEvent(QCloseEvent *event);

public slots:
	void message(const QString& s);

private:
	Ui::KawaiiGLClass ui;

	KwSettings m_sett;

	T2GLWidget *m_kView;
	KwEdit *m_edDlg;
	ControlPanel *m_contDlg; // this is actually render configuration
	ProjBrowser *m_browse;
	MyDialog *m_control;
	Document *m_doc;

};

#endif // KAWAIIGL_H
