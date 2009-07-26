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


SliderPui::SliderPui(ParamUi *pui, float val, float _vmin, float _vmax) : PuiCont(pui)
	,vmin(_vmin), vmax(_vmax)
{
	QSlider *slp = new QSlider(Qt::Horizontal);
	slp->setMinimumWidth(100);
	pui->moreContainer->layout()->addWidget(slp);
	pui->moreContainer->show();

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

TrackPui::TrackPui(ParamUi *pui, bool _relative) : PuiCont(pui)
	,relative(_relative)
{
	QPushButton *chp = new QPushButton();
	chp->setMaximumSize(20, 20);
	QFont f = chp->font(); f.setFamily("Arial Unicode MS"); f.setPixelSize(14); chp->setFont(f);
	chp->setText(QString(QChar(0x2295))); // circle
	pui->moreContainer->layout()->addWidget(chp);
	pui->moreContainer->show();
	win = new CheckBoxIn(&pui->m_kwEdit->conf().trackForParam, chp);
	win->reload();
	connect(pui->m_kwEdit->view(), SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(mouseTrack(QMouseEvent*)));
	pui->confGui->show();
}

void TrackPui::config()
{
	if (!MyInputDlg::getValue("Relative Coordinates [0,1]x[0,1]", relative, "GUI Config", pui->m_kwEdit))
		return;
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

	Vec2 c;
	c.x = (float)event->x() ; // map to texture coordinates space
	c.y = pui->m_kwEdit->view()->height() - (float)event->y() ;

	if (relative) // relative coordinates
	{
		c.x /= pui->m_kwEdit->view()->width(); // map to texture coordinates space
		c.y /= pui->m_kwEdit->view()->height();
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
			moreCont = new SliderPui(this, pi->value.toFloat(), pi->guiA, pi->guiB);
		else
			moreCont = new SliderPui(this, 0.6f, 0.0f, 1.0f);
	}
	else if (m_dtype == EPFloatTime)
	{
		moreCont = new TimePui(this);
	}
	else if (m_dtype == EPVec2)
	{
		if (pi != NULL)
			moreCont = new TrackPui(this, pi->guiA == 0.0);
		else
			moreCont = new TrackPui(this, false);
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