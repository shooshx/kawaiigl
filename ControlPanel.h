#pragma once
#ifndef CONTROLPANEL_H_INCLUDED
#define CONTROLPANEL_H_INCLUDED

#include <QObject>
#include "ui_DispContDlg.h"
#include "MyLib/MyDialog.h"
#include "GLWidget.h"
#include "DisplayConf.h"
#include "ProgInput.h"

class Document;
class KawaiiGL;

class ControlPanel : public MyDialog
{
	Q_OBJECT
public:
	ControlPanel(DisplayConf* _conf, KawaiiGL* parent, Document* doc);
	//void setLightSliders(float amb, float diff, float spec);

private slots:
	void updateSDPasses(bool justChecked);
	void updateSDlowLevel(const DisplayConf& dspc);
	void chFontEnable();
	void coordFontSizeChanged(int v);
	void selectTexFile();
	void changedTexFile();
	//void texSelChanged(int i);

public slots:
	void runTypeChange();

signals:
	void changedRend(); // changed rendering stuff
	void changedPaint(); // changed paint stuff
	void changedFont();
	void resetView();
	void resetLight();
	void changedTexFile(int which);
	void reassertTex(int which);
	void saveMesh();
	void doUpdate();
	
public:
	Ui::DispContDlg ui;
	
private:
	struct SD_Pass
	{
		SD_Pass() :p(NULL), br(NULL) {}
		SD_Pass(QCheckBox *_p, QCheckBox *_br) :p(_p), br(_br) {}
		QCheckBox *p;
		QCheckBox *br;

	} m_SD_pass[N_PASS];

	struct W_Tex
	{
		W_Tex(QLineEdit* ed = NULL, QPushButton* b = NULL) :file(ed), bot(b) {}
		QLineEdit *file;
		QPushButton *bot;
	} m_W_Tex[N_TEX];


	DisplayConf *conf;
	Document *m_doc;
	//bool m_bLastCullCheck;
};



#endif // CONTROLPANEL_H_INCLUDED
