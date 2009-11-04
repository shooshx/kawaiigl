#include "PParamsWidget.h"
#include "ParamUI.h"

#include "Document.h"


static QColor ERROR_COL = QColor(255, 128, 128);
static QColor WARNING_COL = QColor(255, 174, 33);


PParamsWidget::PParamsWidget(QWidget *parent)
	: QWidget(parent), m_view(NULL)
{
	ui.setupUi(this);
}

PParamsWidget::~PParamsWidget() 
{
	clearParam(); // need to dispose of all of the params before closing
}



void PParamsWidget::init(T2GLWidget* view, Document* doc, RenderPass* pass)
{
	m_pass = pass;
	m_view = view;
	m_doc = doc;
	addAllParams();
}

// commit the gui change to the document pass
// additions, removal of params, changes in type
// happens in:
// - before compile.. only? TBD
void PParamsWidget::commit()
{
	m_pass->params.clear();

	foreach(ParamUi* pui, m_paramUi)
	{
		ParamInput pi(pui->name->text(), 
			(EParamType)pui->type->itemData(pui->type->currentIndex()).toInt(), 
			pui->value->text(), pui->m_isUniform, m_pass);
		pui->index = m_pass->params.size();
		if (pui->moreCont != NULL)
			pi.prop = pui->moreCont->prop;
		m_pass->params.append(pi);
	}
}

// after compilation, all parameters are parsed, if there are any errors in their
// parsing, this is the time to set the background to red.
void PParamsWidget::postCompile()
{
	foreach(ParamUi* pui, m_paramUi)
	{
		if (m_pass->params.size() <= pui->index)
			continue;

		ParamInput progpi = m_pass->params[pui->index];
		QColor c;
		if (progpi.lastParseOk)
			c = Qt::white;
		else
			c = ERROR_COL;
		pui->setValueColor(c);

	}
}


// parse and update all parameters.
// happens:
// - when there's a new model text
// - just after a compile of the program
void PParamsWidget::doVarsUpdate()
{
	if (!m_doc->isProgEnabled())
		return; // no need to do anything if not enabled.

	for(int i = 0; i < m_paramUi.size(); ++i)
	{
		ParamUi* pui = m_paramUi[i];
		doVarUpdate(pui, false);
	}

}


// re-parse the param of pui into the document and program
// if update is false, don't do an updateGL.
void PParamsWidget::doVarUpdate(ParamUi* pui, bool update)
{
	ParamInput curpi(pui->name->text(), pui->m_dtype, pui->value->text(), pui->m_isUniform, NULL);

	QColor c = Qt::white;

	if (!curpi.name.isEmpty() && !curpi.value.isEmpty())
	{
		if (pui->index < m_pass->params.size())
		{
			ParamInput progpi = m_pass->params[pui->index];

			if (progpi.name == curpi.name && getBaseType(progpi.type) == getBaseType(curpi.type) && progpi.isUniform == curpi.isUniform)
			{
				progpi.value = curpi.value;
				if (!m_doc->parseSingleParam(progpi, update))
					c = ERROR_COL;
			}
			else
				c = WARNING_COL; // no such param in the compiled prog
		}
		else
			c = WARNING_COL; // no such param	
	}


	pui->setValueColor(c);


}




// received when text is edited
// the sender()->userData(0) should be the ParamUI
void PParamsWidget::doVarUpdate()
{
	QObject *sp = sender();
	ParamUi *pui = ((UserData<ParamUi*>*)sp->userData(0))->i;

	doVarUpdate(pui);
}


////////////////

void PParamsWidget::removeParam()
{
	QObject *sp = sender();
	ParamUi *pui = ((UserData<ParamUi*>*)sp->userData(0))->i;
	removeParam(pui);
}

void PParamsWidget::removeParam(ParamUi *pui)
{
	QWidget *w = pui->containter;
	delete pui;
	delete w;

	m_paramUi.remove(m_paramUi.indexOf(pui));
}

void PParamsWidget::clearParam()
{
	while (!m_paramUi.isEmpty())
		removeParam(*m_paramUi.begin());
}



void PParamsWidget::addAllParams() // of this pass
{
	foreach(const ParamInput& ppi, m_pass->params)
	{
		addParam(ppi);
	}
}

// the (+) button was pressed
void PParamsWidget::on_addParam_clicked()
{
	addParam(ParamInput("", EPFloat, "", true, NULL));
}

void PParamsWidget::addParam(const ParamInput& pi) //const QString& name, EParamType type, const QString& value)
{
	ParamUi *pui = new ParamUi(this, m_view);

	pui->containter = new QWidget(this);
	pui->layout = new QHBoxLayout(); // name edit, type combo, value edit, SOMETHING, x bot. 
	pui->layout->setMargin(0);
	pui->layout->setSpacing(2);

	pui->containter->setLayout(pui->layout);
	pui->containter->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);

	pui->uniAttr = new QPushButton();
	pui->uniAttr->setMaximumSize(20, 20);
	pui->uniAttr->setToolTip("Uniform/Attribute");

	pui->name = new QLineEdit();
	pui->name->setToolTip("name");
	pui->type = new QComboBox();
	pui->type->setMaximumWidth(80);
	pui->type->addItem("float", (int)EPFloat);  // ADDTYPE
	pui->type->addItem("float (range)", (int)EPFloatRange);
	pui->type->addItem("float (time)", (int)EPFloatTime);
	pui->type->addItem("int", (int)EPInt);
	pui->type->addItem("vec2", (int)EPVec2);
	pui->type->addItem("vec3", (int)EPVec3);
	pui->type->addItem("vec4", (int)EPVec4);
	pui->type->addItem("vec3 (color)", (int)EPVec3Color);
	pui->type->addItem("vec4 (color)", (int)EPVec4Color);
	pui->type->addItem("texture", (int)EPTexture);
	pui->type->setToolTip("type");
	pui->value = new QLineEdit();
	pui->value->setUserData(0, new UserData<ParamUi*>(pui)); // needed for doVarUpdate()
	pui->value->setToolTip("value");

	pui->name->setText(pi.name);
	pui->type->setCurrentIndex(pui->type->findData(pi.type));
	pui->value->setText(pi.value);
	pui->m_isUniform = pi.isUniform;
	pui->setUniAttrText();
	// 	pui->guiA = pi.guiA;
	// 	pui->guiB = pi.guiB;

	pui->moreContainer = new QWidget();
	QHBoxLayout *moreLayout = new QHBoxLayout();
	pui->moreContainer->setLayout(moreLayout);
	moreLayout->setMargin(0);
	moreLayout->setSpacing(0);
	pui->moreContainer->hide();

	connect(pui->uniAttr, SIGNAL(clicked()), pui, SLOT(uniAttrChange()));
	connect(pui->value, SIGNAL(textEdited(const QString&)), this, SLOT(doVarUpdate())); // update single variable
	connect(pui->type, SIGNAL(currentIndexChanged(int)), pui, SLOT(initMoreWidget()));

	pui->confGui = new QPushButton("?");
	pui->confGui->setToolTip("Properties");
	pui->confGui->setMaximumSize(20, 20);
	connect(pui->confGui, SIGNAL(clicked()), pui, SLOT(configGui()));

	QPushButton *remBot = new QPushButton("X");
	remBot->setToolTip("remove");
	remBot->setUserData(0, new UserData<ParamUi*>(pui)); // needed
	connect(remBot, SIGNAL(clicked()), this, SLOT(removeParam()));
	//pui.name->set
	remBot->setMaximumSize(20, 20);
	pui->layout->addWidget(pui->uniAttr);
	pui->layout->addWidget(pui->name);
	pui->layout->addWidget(pui->type);
	pui->layout->addWidget(pui->value);
	pui->layout->addWidget(pui->moreContainer);
	pui->layout->addWidget(pui->confGui);
	pui->layout->addWidget(remBot);
	((QBoxLayout*)layout())->addWidget(pui->containter);

	pui->index = m_paramUi.size();
	m_paramUi.append(pui);

	pui->initMoreWidget(&pi);


}
