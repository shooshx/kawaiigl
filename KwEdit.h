#pragma once
#ifndef KWEDIT_H
#define KWEDIT_H

#include <QDialog>
#include <QTextEdit>

#include "ui_KwEdit.h"
#include "MyLib/MyDialog.h"
#include "ProgInput.h"

class QLineEdit;
class QComboBox;

class OffsetData;
class MySyntaxHighlighter;
class DisplayConf;
class Document;
class Prop;
class WidgetIn;
class KwEdit;
class T2GLWidget;
class ParamUi;


class KwEdit : public MyDialog
{
	Q_OBJECT
public:
	KwEdit(QWidget *parent, DisplayConf& conf, Document* doc, T2GLWidget *view);
	void addError(int start, int count);
	void clearErrors();
	void finishErrors();
	void loadMenus(QWidget*, const QString& filename);
	void loadProgMenu(QMenu* menu, const QString& filename);

	DisplayConf& conf() { return m_conf; }
	T2GLWidget* view() { return m_view; }
	bool isEnabled() { return m_isEnabled; }

public slots:
	void edChanged();
	void setToPre();
	void readProg();
	void doShadersUpdate();
	void doVarsUpdate();
	void setText(const QString& text);
	void activateAction(const QString& name);

private slots:
	void on_reloadBot_clicked();
	void on_addParam_clicked();
	void on_tabs_currentChanged(int);
	void updateCursorPosLine();
	void zoomEdits(int delta);
	void removeParam();

	void doVarUpdate(); // sent from the LineEdit

signals:
	void changed(const QString& txt);
	void updateShaders(const ProgInput& in);
	void updateVars(const ProgInput& in);

public:
	Ui::KwEdit ui;

private:
	QAction* confAddModel(const QString& display, const QString& filename, bool isMesh);
	QAction* confAddProg(const QString& display, ProgKeep* prog); // taking over this.


	void addParam(const ParamInput& pi);
	void removeParam(ParamUi *pui);
	void clearParam();
	void doVarUpdate(const ParamUi* pui, bool update = true);
	void initMoreWidget(ParamUi *pui);

	QVector<ParamUi*> m_paramUi;

	QTextEdit* m_curEd;
	QMenu *m_progmenu;
	QString m_lastLoadedProg;

	Document *m_doc;
	T2GLWidget *m_view;

	QList<ProgKeep*> m_progrep;

	DisplayConf &m_conf;

	bool m_isEnabled;

	ProgInput m_in;

	typedef QMap<QString, QAction*> TActionMap;
	TActionMap m_userActions;

	QString generateFromFile();

	QString curText;
	MySyntaxHighlighter *m_high;
	QList<int> errorBlockNumbers;

	friend class ConfXmls;
	friend class ParamUi;
};



#endif // KWEDIT_H
