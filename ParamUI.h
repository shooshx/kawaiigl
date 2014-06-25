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
class Document;
class T2GLWidget;

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
	// init might be NULL
	ColorPui(ParamUi *pui, const QColor& init, bool _withAlpha);
public slots:
	void colorParamChanged();
private:
	bool withAlpha;
};

class SliderPui : public PuiCont
{
	Q_OBJECT;
public:
	SliderPui(ParamUi *pui, float val, FloatGuiConf* conf);
	virtual void config();
	float vmin, vmax;
public slots:
	void sliderParamChanged();
};


class TrackPui : public PuiCont
{
	Q_OBJECT;
public:
	TrackPui(ParamUi *pui, Vec2GuiConf* conf);
	virtual void config();
	bool relative;
	float xmin, xmax, ymin, ymax;
public slots:
	void mouseTrack(QMouseEvent* event);
	//void trackEnable();
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


class PParamsWidget;

class ParamUi : public QObject
{
	Q_OBJECT;
public:
	ParamUi(PParamsWidget* parent, T2GLWidget* view);
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
	EParamDest m_dest; // uniform/attribute/model

	PParamsWidget *m_parent;
	T2GLWidget *m_view;
	Document *m_doc;

	template<typename T>
	void directUpdate(const T& val);

	void setUniAttrText();
	void setValueColor(const QColor& c);

public slots:

	
	void configGui();
	void uniAttrChange();


	void initMoreWidget(const ParamInput* pi = NULL);

};

#endif;