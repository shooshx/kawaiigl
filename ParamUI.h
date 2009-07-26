#ifndef PARAMUI_H_INCLUDED
#define PARAMUI_H_INCLUDED

#include <QWidget>
#include <QTimer>

#include "DisplayConf.h"
#include "ProgInput.h"

class QComboBox;
class QLayout;
class QLineEdit;

class KwEdit;
class ParamUi;

// ParamUI special controls
class PuiCont : public QObject
{
	Q_OBJECT;
public:
	PuiCont(ParamUi *_pui) :QObject(NULL), pui(_pui), prop(NULL), win(NULL) {}
	virtual ~PuiCont()
	{
		delete win;
		delete prop;
	}

	virtual void config() {}

	ParamUi *pui;

	Prop *prop;
	WidgetIn* win;

};

class ColorPui : public PuiCont
{
	Q_OBJECT;
public:
	ColorPui(ParamUi *pui);
public slots:
	void colorParamChanged();
};

class SliderPui : public PuiCont
{
	Q_OBJECT;
public:
	SliderPui(ParamUi *pui, float val, float _vmin, float _vmax);
	virtual void config();
	float vmin, vmax;
public slots:
	void sliderParamChanged();
};


class TrackPui : public PuiCont
{
	Q_OBJECT;
public:
	TrackPui(ParamUi *pui, bool _relative);
	virtual void config();
	bool relative;
public slots:
	void mouseTrack(QMouseEvent* event);
};

class TimePui : public PuiCont
{
	Q_OBJECT;
public:
	TimePui(ParamUi *pui);
	virtual ~TimePui();
	virtual void config();

	QPushButton *playbot;
	QPushButton *resetbot;
	QTimer timer;
	int interval;
	int curmsec;
public slots:
	void timeout();
	void play();
	void reset();
};



class ParamUi : public QObject
{
	Q_OBJECT;
public:
	ParamUi(KwEdit *parent);
	virtual ~ParamUi(); // needs to get rid of all the widgets created somehow.

	QWidget* containter; // contains all the widgets in a line
	QLayout* layout;
	QPushButton* uniAttr; // uniform or attribute
	QLineEdit* name;
	QComboBox* type;
	QLineEdit* value;
	int index; // the index of this ui element in the gui itself	

	QWidget* moreContainer;
	QPushButton *confGui;
	PuiCont *moreCont;

	//int guiA, guiB;

	EParamType m_dtype;
	bool m_isUniform;

	KwEdit *m_kwEdit;

	template<typename T>
	void directUpdate(const T& val)
	{
		if (index >= m_kwEdit->m_in.params.size())
			return;
		m_kwEdit->m_doc->directUpdate(m_kwEdit->m_in.params[index], val);
	}

	void setUniAttrText();

public slots:

	
	void configGui();
	void uniAttrChange();


	void initMoreWidget(const ParamInput* pi = NULL);

};

#endif;