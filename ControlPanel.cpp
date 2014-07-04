#include "ControlPanel.h"

#include "DisplayConf.h"
#include "KawaiiGL.h"
#include "Document.h"
#include "T2GLWidget.h"
#include "GradientDlg.h"

#include <QFileDialog>




ControlPanel::ControlPanel(DisplayConf* _conf, KawaiiGL* parent, Document *doc, T2GLWidget* view)
:QWidget(parent), conf(_conf), m_doc(doc), m_kview(view)
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

    m_W_Tex[0] = W_Tex(ui.tex1NameLabel, ui.tex1FileBot, ui.tex1Pix);
    m_W_Tex[1] = W_Tex(ui.tex2NameLabel, ui.tex2FileBot, ui.tex2Pix);
    m_W_Tex[2] = W_Tex(ui.tex3NameLabel, ui.tex3FileBot, ui.tex3Pix);
    m_W_Tex[3] = W_Tex(ui.tex4NameLabel, ui.tex4FileBot, ui.tex4Pix);

    for(int i = 0; i < N_TEX; ++i)
    {
        QAction *texFileAct = new QAction("File...", this);
        texFileAct->setUserData(0, new UserData<int>(i));
        QAction *texGradAct = new QAction("Gradient...", this);
        texGradAct->setUserData(0, new UserData<int>(i));
        QMenu *texBotMenu = new QMenu(this);
        texBotMenu->addAction(texFileAct);
        texBotMenu->addAction(texGradAct);
        m_W_Tex[i].bot->setUserData(0, new UserData<int>(i));
        m_W_Tex[i].bot->setMenu(texBotMenu);
        m_W_Tex[i].label->hide();

        connect(texFileAct, SIGNAL(triggered()), this, SLOT(selectTexFile()));
        connect(texGradAct, SIGNAL(triggered()), this, SLOT(selectTexGradient()));
        (new WidgetTIn<QString>::LineEditIn(conf->texFile[i], m_W_Tex[i].file))->reload();
        connect(conf->texFile[i], SIGNAL(changed()), this, SLOT(changedTexFile()));
        *conf->texImage[i] = QImage();
        (new ImageLabelIn(conf->texImage[i], m_W_Tex[i].label))->reload();
        connect(conf->texImage[i], SIGNAL(changed()), this, SLOT(changedTexFile()));
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
    (new CheckBoxIn(&conf->bTriangulate, ui.SD_triang))->reload();
    (new CheckBoxIn(&conf->bBlend, ui.blendBox))->reload();
    (new CheckBoxIn(&conf->bDepthTest, ui.zbufBox))->reload();
    (new CheckBoxIn(&conf->bPoly, ui.SD_polygons))->reload();
    (new CheckBoxIn(&conf->linePoly, ui.linePoly))->reload();
    (new CheckBoxIn(&conf->bPoints, ui.SD_points))->reload();
    (new CheckBoxIn(&conf->bUnusedPoints, ui.SD_unused))->reload();
    (new CheckBoxIn(&conf->bAllPolyPoints, ui.SD_allpoly))->reload();
    (new SpinBoxIn<int>(&conf->pointSize, ui.pointSizeSpin))->reload();
    (new SpinBoxIn<int>(&conf->lineWidth, ui.lineWidthSpin))->reload();
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
    //(new CheckBoxIn(&conf->quadMultiSamp, ui.multiSampQuad))->reload();

    connect(&conf->bCoordName, SIGNAL(changed()), this, SLOT(chFontEnable()));
    connect(&conf->bCoordNum, SIGNAL(changed()), this, SLOT(chFontEnable()));

    (new SpinBoxIn<int>(&conf->coordFontSize, ui.coordFontSize))->reload();
    connect(&conf->coordFontSize, SIGNAL(changed()), this, SIGNAL(changedFont()));
    //connect(ui.coordFontSize, SIGNAL(valueChanged(int)), this, SLOT(coordFontSizeChanged(int)));

    connect(ui.resetView, SIGNAL(clicked(bool)), this, SIGNAL(resetView()));
    connect(ui.resetLight, SIGNAL(clicked(bool)), this, SIGNAL(resetLight()));
    connect(ui.saveBot, SIGNAL(clicked()), this, SIGNAL(saveMesh()));

    (new ComboBoxIn<DisplayConf::ETexAct>(&conf->texAct, ui.texSel))->reload();
// not implemented    (new ComboBoxIn<DisplayConf::ETexEdge>(&conf->texEdge, ui.texEdgeBox))->reload();
//    (new CheckBoxIn(&conf->texSigned, ui.texSignedBox))->reload();

    updateSDlowLevel(*conf);
}


//void ControlPanel::coordFontSizeChanged(int v) { conf->coordFontSize = v; emit changedFont(); }

void ControlPanel::chFontEnable() { 
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
    W_Tex& wt = m_W_Tex[which];
    QLineEdit *to = wt.file;

    QString filename = QFileDialog::getOpenFileName(this, "Open Texture", conf->lastDir, "image files (*.png *.jpg *.gif)");
    if (filename.isEmpty())
        return;
    conf->lastDir = QFileInfo(filename).absolutePath();
    filename = QDir::current().relativeFilePath(filename);

    to->setText(filename);

    if (wt.picker != NULL) {
        wt.picker->hide();
    }
    wt.label->hide();
    wt.file->show();

    (*conf->texFile[which]) = filename;
    (*conf->texImage[which]) = QImage();
}

void ControlPanel::selectTexGradient()
{
    int which = ((UserData<int>*)sender()->userData(0))->i;
    W_Tex& wt = m_W_Tex[which];
    if (wt.picker == NULL) {
        wt.picker = new GradientDlg(this);
        wt.picker->setUserData(0, new UserData<int>(which));
        connect(wt.picker, SIGNAL(changed(QImage)), this, SLOT(gradientChanged(QImage)));
    }
    ((GradientDlg*)wt.picker)->updateImage();
    wt.picker->show();
    wt.label->show();
    wt.file->hide();
}


void ControlPanel::gradientChanged(QImage img)
{
    int which = ((UserData<int>*)sender()->userData(0))->i;
    (*conf->texImage[which]) = img;
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


void ControlPanel::updateTexEdits()
{
    for (int i = 0; i < N_TEX; ++i)
    {
        const T2GLWidget::TexUnit& tu = m_kview->m_texUnits[i];
        PassPtr pass;
        W_Tex &wtex = m_W_Tex[i];

        wtex.file->blockSignals(true); // text changes don't send out fake updates
        if (pass = tu.outputOf.lock())
        {
            wtex.file->setText(QString("Output of %1").arg(pass->name()));
            wtex.file->setReadOnly(true);
            wtex.bot->setEnabled(false);
            wtex.isPassOutput = true;
        }
        else
        {
            wtex.bot->setEnabled(true);
            wtex.file->setText(conf->texFile[i]->val());
            wtex.file->setReadOnly(false);
        }
        wtex.file->blockSignals(false);
    }

    //emit doUpdate();
}



void ControlPanel::on_resetBot_clicked()
{
    conf->reset();

}