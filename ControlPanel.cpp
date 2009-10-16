#include "ControlPanel.h"

#include "DisplayConf.h"
#include "KawaiiGL.h"
#include "Document.h"

#include <QFileDialog>




ControlPanel::ControlPanel(DisplayConf* _conf, KawaiiGL* parent, Document *doc)
:QWidget(parent), conf(_conf), m_doc(doc)
{
	ui.setupUi(this);

	m_SD_pass[0] = SD_Pass(ui.SD_pass1, ui.SD_p1r);
	m_SD_pass[1] = SD_Pass(ui.SD_pass2, ui.SD_p2r);
	m_SD_pass[2] = SD_Pass(ui.SD_pass3, ui.SD_p3r);
	m_SD_pass[3] = SD_Pass(ui.SD_pass4, ui.SD_p4r);
	m_SD_pass[4] = SD_Pass(ui.SD_pass5, ui.SD_p5r);
	for (int i = 0; i < N_PASS; ++i)
	{
		connect(m_SD_pass[i].p, SIGNAL(toggled(bool)), this, SLOT(updateSDPasses(bool)));
		//connect(m_SD_pass[i].p, SIGNAL(toggled(bool)), m_SD_pass[i].bs, SLOT(setEnabled(bool)));
		connect(m_SD_pass[i].p, SIGNAL(toggled(bool)), m_SD_pass[i].br, SLOT(setEnabled(bool)));

		(new CheckBoxIn(conf->passRound[i], m_SD_pass[i].br))->reload();
		//connect(m_SD_pass[i].bs, SIGNAL(toggled(bool)), this, SIGNAL(changedRend()));
		connect(m_SD_pass[i].br, SIGNAL(toggled(bool)), this, SIGNAL(changedRend()));
	}

	m_W_Tex[0] = W_Tex(ui.tex1NameLabel, ui.tex1FileBot);
	m_W_Tex[1] = W_Tex(ui.tex2NameLabel, ui.tex2FileBot);
	m_W_Tex[2] = W_Tex(ui.tex3NameLabel, ui.tex3FileBot);
	for(int i = 0; i < N_TEX; ++i)
	{
		m_W_Tex[i].bot->setUserData(0, new UserData<int>(i));
		connect(m_W_Tex[i].bot, SIGNAL(clicked()), this, SLOT(selectTexFile()));
		(new WidgetTIn<QString>::LineEditIn(conf->texFile[i], m_W_Tex[i].file))->reload();
		connect(conf->texFile[i], SIGNAL(changed()), this, SLOT(changedTexFile()));
	}



	(new CheckBoxIn(&conf->lighting, ui.SD_light))->reload();
	(new CheckBoxIn(&conf->bVtxNormals, ui.SD_vtxNormals))->reload();
	(new CheckBoxIn(&conf->bLightMove, ui.SD_lightMove))->reload();
	(new CheckBoxIn(&conf->isPointLight, ui.SD_pntLight))->reload();
	(new CheckBoxIn(&conf->perspective, ui.SD_persp))->reload();
	(new CheckBoxIn(&conf->bLitBackFace, ui.SD_litback))->reload();
	(new CheckBoxIn(&conf->bCull, ui.SD_cull))->reload();
	(new CheckBoxIn(&conf->bLines, ui.SD_lines))->reload();
	(new CheckBoxIn(&conf->bLinesAll, ui.SD_linesAll))->reload();
	(new CheckBoxIn(&conf->bPoly, ui.SD_polygons))->reload();
	(new CheckBoxIn(&conf->bPoints, ui.SD_points))->reload();
	(new CheckBoxIn(&conf->bUnusedPoints, ui.SD_unused))->reload();
	(new CheckBoxIn(&conf->bCoordNum, ui.coordNum))->reload();
	(new CheckBoxIn(&conf->bCoordName, ui.coordName))->reload();
	(new ColorSelIn(&conf->lightAmb, ui.ambBot, WidgetIn::DoModal))->reload();
	(new ColorSelIn(&conf->lightDiff, ui.diffBot, WidgetIn::DoModal))->reload();
	(new ColorSelIn(&conf->lightSpec, ui.specBot, WidgetIn::DoModal))->reload();
	(new WidgetTIn<int>::LineEditIn(&conf->materialShine, ui.shineEdit))->reload();
	(new ColorSelIn(&conf->backCol, ui.backColBot, WidgetIn::DoModal))->reload();
	(new ColorSelIn(&conf->materialCol, ui.mattColBot, WidgetIn::DoModal))->reload();
	(new ColorSelIn(&conf->selectedCol, ui.selColBot, WidgetIn::DoModal))->reload();
	(new ColorSelIn(&conf->lineColor, ui.lineColBot, WidgetIn::DoModal))->reload();
	(new CheckBoxIn(&conf->quadMultiSamp, ui.multiSampQuad))->reload();

	connect(&conf->bCoordName, SIGNAL(changed()), this, SLOT(chFontEnable()));
	connect(&conf->bCoordNum, SIGNAL(changed()), this, SLOT(chFontEnable()));
	connect(&conf->runType, SIGNAL(changed()), this, SLOT(runTypeChange()));

	//(new WidgetTIn<int>::LineEditIn(&conf->coordFontSize, ui.coordFontSize))->reload();
	connect(ui.coordFontSize, SIGNAL(valueChanged(int)), this, SLOT(coordFontSizeChanged(int)));

	connect(ui.resetView, SIGNAL(clicked(bool)), this, SIGNAL(resetView()));
	connect(ui.resetLight, SIGNAL(clicked(bool)), this, SIGNAL(resetLight()));
	connect(ui.saveBot, SIGNAL(clicked()), this, SIGNAL(saveMesh()));

	(new ComboBoxIn<DisplayConf::ETexAct>(&conf->texAct, ui.texSel))->reload();

	updateSDlowLevel(*conf);
	ui.tex0Fake->hide();
	ui.tex1Fake->hide();
}


void ControlPanel::coordFontSizeChanged(int v) { conf->coordFontSize = v; emit changedFont(); }

void ControlPanel::chFontEnable() 
{ 
	ui.coordFontSize->setEnabled(conf->bCoordNum || conf->bCoordName);
}


// void T2Control::countPasses()
// {
// 	int nump = 0;
// 	for(int i = 0; i < N_PASS; ++i)
// 	{
// 		if (m_SD_pass[i].p->isChecked())
// 			nump = i + 1;
// 	}
// 	conf->numberOfPasses = nump;
// }

void ControlPanel::updateSDlowLevel(const DisplayConf& dspc)
{

	//DisplayConf copy = dspc; // unaffected copy
	int nump = dspc.numberOfPasses;
	for (int i = 0; i < N_PASS; ++i)
	{
		bool en = nump > i;
		m_SD_pass[i].p->setChecked(en);
		bool vri = *dspc.passRound[i];
//		m_SD_pass[i].bs->setChecked(!vri); no longer there
//		m_SD_pass[i].bs->setEnabled(en);
		m_SD_pass[i].br->setChecked(vri);
		m_SD_pass[i].br->setEnabled(en);
	}

}

void ControlPanel::updateSDPasses(bool justChecked)
{
	int p;
	if (justChecked)
	{
		p = N_PASS - 1;
		while ((p >= 0) && (!m_SD_pass[p].p->isChecked()))	--p;
		p++;
	}
	else
	{
		p = 0;
		while ((p < N_PASS) && (m_SD_pass[p].p->isChecked())) ++p;
	}
	for(int i = 0; i < N_PASS; ++i)
		m_SD_pass[i].p->setChecked(i < p);

	conf->numberOfPasses = p;
	emit changedRend();
}


void ControlPanel::selectTexFile()
{
	int which = ((UserData<int>*)sender()->userData(0))->i;
	QLineEdit *to = m_W_Tex[which].file;

	QString filename = QFileDialog::getOpenFileName(this, "Open Texture", conf->lastDir, "image files (*.png *.jpg *.gif)");
	if (filename.isEmpty())
		return;
	conf->lastDir = QFileInfo(filename).absolutePath();
	filename = QDir::current().relativeFilePath(filename);

	to->setText(filename);
	(*conf->texFile[which]) = filename;

}

void ControlPanel::changedTexFile()
{
	int which = dynamic_cast<Prop*>(sender())->index();
	emit changedTexFile(which);	
}


/*
void ControlPanel::texSelChanged(int i)
{
	if (i == 0)
		conf->texAct = DisplayConf::Tex_None;
	else
		conf->texAct = DisplayConf::Tex_File;
}
*/


void ControlPanel::runTypeChange()
{
	if (conf->runType == DisplayConf::RunQuadProcess)
	{
		ui.tex0Fake->show();
		ui.tex0cont->hide();
		ui.tex1Fake->hide();
		ui.tex1cont->show();
		ui.multiSampQuad->show();
		emit reassertTex(1);
	}
	else if (conf->runType == DisplayConf::RunTex2Tex)
	{
		ui.tex0Fake->show();
		ui.tex0cont->hide();
		ui.multiSampQuad->hide();
		ui.tex1Fake->show();
		ui.tex1cont->hide();
	}
	else
	{
		ui.tex0Fake->hide();
		ui.tex0cont->show();
		ui.tex1Fake->hide();
		ui.tex1cont->show();
		emit reassertTex(0);
		emit reassertTex(1);
	}
	emit doUpdate();
}

