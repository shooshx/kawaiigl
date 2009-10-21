#ifndef PPARAMSWIDGET_H
#define PPARAMSWIDGET_H

#include <QWidget>
#include "ui_PParamsWidget.h"
#include "Pass.h"


class T2GLWidget;
class ParamUi;
class Document;


// Pass Params Widget
class PParamsWidget : public QWidget
{
	Q_OBJECT

public:
	PParamsWidget(QWidget *parent);
	~PParamsWidget() {}

	void init(T2GLWidget* view, Document* doc, RenderPass* pass);

	void commit();
	void postCompile();

	void addAllParams();

	template<typename T>
	void fastVarUpdate(ParamUi* pui, const T& val)
	{
		// check enabled?
		if (pui->index >= m_pass->params.size())
			return;
		m_doc->directUpdate(m_pass->params[pui->index], val);
	}

private slots:
	void on_addParam_clicked();
	void doVarUpdate();  // sent from the LineEdit
	void removeParam();


public slots:
	void doVarsUpdate(); 

private:
	void addParam(const ParamInput& pi);
	void doVarUpdate(ParamUi* pui, bool update = true);

	void removeParam(ParamUi *pui);
	void clearParam();

private:
	Ui::PParamsWidgetClass ui;
	QVector<ParamUi*> m_paramUi;

	RenderPass *m_pass;

	T2GLWidget *m_view; // need to pass it to ParamUIs
	Document *m_doc;
};

#endif // PPARAMSWIDGET_H
