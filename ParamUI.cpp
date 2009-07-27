#include "ParamUI.h"

#include "KwEdit.h"
#include "T2GLWidget.h"
#include "Document.h"

#include <QPushButton>
#include <QSlider>


ParamUi::ParamUi(KwEdit *parent) :QObject(NULL), // we don't want QObject to call our d-tor
m_kwEdit(parent), moreCont(NULL), m_isUniform(true)
{}
ParamUi::~ParamUi()
{
	delete moreCont;
}



ColorPui::ColorPui(ParamUi *pui) : PuiCont(pui)
{
	QPushButton *chp = new QPushButton();
	chp->setMaximumSize(20, 20);
	pui->moreContainer->layout()->addWidget(chp);
	pui->moreContainer->show();
	TypeProp<QColor>* cprop = new TypeProp<QColor>(NULL, NULL, "_c", QColor(0,0,0));
	prop = cprop;
	win = new ColorSelIn(cprop, chp, WidgetIn::DoModal);
	win->reload();
	connect(cprop, SIGNAL(changed()), this, SLOT(colorParamChanged())); // change the text
	pui->confGui->hide();

}


SliderPui::SliderPui(ParamUi *pui, float val, FloatGuiConf* conf) : PuiCont(pui)
	,vmin(0.0), vmax(1.0)
{
	QSlider *slp = new QSlider(Qt::Horizontal);
	slp->setMinimumWidth(100);
	pui->moreContainer->layout()->addWidget(slp);
	pui->moreContainer->show();

	if (conf != NULL)
	{
		vmin = conf->vmin;
		vmax = conf->vmax;
	}
	if (vmin == vmax)
		vmax = vmin + 1.0; // so for 0.0 in both this will become [0,1]

	TypeProp<float>* cprop = new TypeProp<float>(NULL, NULL, "_f", init(val, vmin, vmax));
	prop = cprop;
	win = new SliderIn<float>(cprop, slp, NULL, 100.0);
	win->reload();
	connect(cprop, SIGNAL(changed()), this, SLOT(sliderParamChanged()));
	pui->confGui->show(); // TBD
}

void SliderPui::config()
{
	if (!MyInputDlg::getValues("Min value", "Max value", vmin, vmax, "GUI Config", pui->m_kwEdit))
		return;
	((TypeProp<float>*)prop)->setRange(vmin, vmax);
}

TrackPui::TrackPui(ParamUi *pui, Vec2GuiConf* conf) : PuiCont(pui)
	,relative(true), xmin(0.0), xmax(1.0), ymin(0.0), ymax(1.0)
{
	if (conf != NULL)
	{
		relative = conf->relative;
		xmin = conf->xmin; xmax = conf->xmax; ymin = conf->ymin; ymax = conf->ymax;
	}
	QPushButton *chp = new QPushButton();
	chp->setMaximumSize(20, 20);
	QFont f = chp->font(); f.setFamily("Arial Unicode MS"); f.setPixelSize(14); chp->setFont(f);
	chp->setText(QString(QChar(0x2295))); // circle
	pui->moreContainer->layout()->addWidget(chp);
	pui->moreContainer->show();
	TypeProp<bool> *cprop = new TypeProp<bool>(NULL, NULL, "_c", false);
	prop = cprop;
	win = new CheckBoxIn(cprop, chp); // pui->m_kwEdit->conf().trackForParam
	win->reload();
	connect(pui->m_kwEdit->view(), SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(mouseTrack(QMouseEvent*)));
	pui->confGui->show();
}


void TrackPui::config()
{
	ParamTemp dummy("track"); 
	TypeProp<bool> abs(&dummy, "_abs", "Absolute Coordinates", !relative);
	DTypeProp2(float, minv, float, maxv) x(&dummy, "X range", xmin, xmax), y(&dummy, "Y range", ymin, ymax);
	dummy.reset();
	if (!MyInputDlg::getValues(pui->m_kwEdit, abs, x, y, "Vec2 tracking"))
		return;
	relative = !abs.val();
	xmin = x.minv; xmax = x.maxv;
	ymin = y.minv; ymax = y.maxv;
}

TimePui::TimePui(ParamUi *pui) : PuiCont(pui)
	,curmsec(0), interval(25)
{
	playbot = new QPushButton(">");
	resetbot = new QPushButton("<<");
	playbot->setMaximumSize(20, 20);
	resetbot->setMaximumSize(20, 20);
	pui->moreContainer->layout()->addWidget(playbot);
	pui->moreContainer->layout()->addWidget(resetbot);
	pui->moreContainer->show();

	connect(playbot, SIGNAL(clicked()), this, SLOT(play()));
	connect(resetbot, SIGNAL(clicked()), this, SLOT(reset()));
	connect(&timer, SIGNAL(timeout()), this, SLOT(timeout()));
	timer.setSingleShot(false);

	pui->confGui->show();
}


TimePui::~TimePui()
{
	delete playbot;
	delete resetbot;
}


void TimePui::play()
{
	if (!pui->m_kwEdit->isEnabled())
		return;

	if (timer.isActive())
	{
		timer.stop();
		playbot->setText(">");
	}
	else
	{
		timer.start(interval);
		playbot->setText("||");
	}
}

void TimePui::reset()
{
	curmsec = -interval; // going to be added
	timeout();
}


void TimePui::config()
{
	if (!MyInputDlg::getValue("Timer interval (msec)", interval, "GUI Config", pui->m_kwEdit))
		return;
}

void TimePui::timeout()
{
	if (!pui->m_kwEdit->isEnabled())
		return;

	curmsec += interval;
	
	float sec = (float)curmsec / 1000.0;
	pui->value->setText(QString("%1").arg(sec)); // doesn't cause the variable to be sent
	pui->directUpdate(sec);

}

void ColorPui::colorParamChanged() // Prop changed
{
	if (!win->isInCommit())
		return;
	if (pui->m_dtype != EPVec3Color)
		return; // shouldn't happen
	TypeProp<QColor> *cprop = dynamic_cast<TypeProp<QColor> *>(prop);
	QColor c = *cprop;
	Vec vc(c);

	pui->value->setText("{" + vc.toStringNoParen() + "}"); // doesn't cause the variable to be sent

	if (!pui->m_kwEdit->isEnabled())
		return;
	pui->directUpdate(vc);
}

void SliderPui::sliderParamChanged()
{
	if (!win->isInCommit())
		return;
	TypeProp<float> *cprop = dynamic_cast<TypeProp<float> *>(prop);
	float v = cprop->val();

	pui->value->setText(QString("%1").arg(v));
	if (!pui->m_kwEdit->isEnabled())
		return; 
	pui->directUpdate(v);
}


void TrackPui::mouseTrack(QMouseEvent* event)
{
	if (pui->m_dtype != EPVec2)
		return; // shouldn't happen
	if (!((TypeProp<bool>*)prop)->val())
		return; // not enabled

	Vec2 c;
	c.x = (float)event->x() ; // map to texture coordinates space
	c.y = pui->m_kwEdit->view()->height() - (float)event->y() ;

	if (relative) // relative coordinates
	{
		c.x = c.x / pui->m_kwEdit->view()->width() * (xmax - xmin) + xmin; // map to texture coordinates space
		c.y = c.y / pui->m_kwEdit->view()->height() * (ymax - ymin) + ymin;
	}

	pui->value->setText("{" + c.toStringNoParen() + "}");
	if (!pui->m_kwEdit->isEnabled())
		return;
	pui->directUpdate(c);

}





// in the case the user just changed the type of the param its going to be NULL
void ParamUi::initMoreWidget(const ParamInput* pi)
{
	m_dtype = (EParamType)type->itemData(type->currentIndex()).toInt();

	delete moreCont;
	moreCont = NULL;


	if (m_dtype == EPVec3Color)
	{
		moreCont = new ColorPui(this);
	}
	else if (m_dtype == EPFloatRange)
	{
		if (pi != NULL)
			moreCont = new SliderPui(this, pi->value.toFloat(), (FloatGuiConf*)pi->guiconf);
		else
			moreCont = new SliderPui(this, 0.6f, NULL);
	}
	else if (m_dtype == EPFloatTime)
	{
		moreCont = new TimePui(this);
	}
	else if (m_dtype == EPVec2)
	{
		if (pi != NULL)
			moreCont = new TrackPui(this, (Vec2GuiConf*)pi->guiconf);
		else
			moreCont = new TrackPui(this, NULL);
	}
	else
	{
		moreContainer->hide();
		confGui->hide();
	}
}

void ParamUi::configGui()
{
	moreCont->config();
}

void ParamUi::setUniAttrText()
{
	uniAttr->setText(m_isUniform?"U":"A");
}

void ParamUi::uniAttrChange()
{
	m_isUniform = !m_isUniform;
	setUniAttrText();
}