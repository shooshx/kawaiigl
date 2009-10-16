#ifndef SHADERCONFIGDLG_H
#define SHADERCONFIGDLG_H

#include <QWidget>
#include "ui_ShaderConfigDlg.h"

class PassConf;
class DisplayConf;

class ShaderConfigDlg : public QWidget
{
	Q_OBJECT
public:
	ShaderConfigDlg(QWidget *parent, Pass* conf, T2GLWidget* view, Document* doc, DisplayConf& dconf); 
	// dconf needed for enum
	// view needed for params container which needs it for mouse tracking
	~ShaderConfigDlg() {}

	PParamsWidget* pparamsContainer() { return ui.paramContainer; }

private:
	Ui::ShaderConfigDlgClass ui;
};

#endif // SHADERCONFIGDLG_H
