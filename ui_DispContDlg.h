/********************************************************************************
** Form generated from reading ui file 'DispContDlg.ui'
**
** Created: Mon Oct 12 16:34:01 2009
**      by: Qt User Interface Compiler version 4.5.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_DISPCONTDLG_H
#define UI_DISPCONTDLG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QFrame>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSlider>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DispContDlg
{
public:
    QVBoxLayout *verticalLayout;
    QWidget *widget;
    QFrame *line_2;
    QCheckBox *SD_points;
    QSpinBox *coordFontSize;
    QLabel *label;
    QCheckBox *SD_litback;
    QCheckBox *SD_polygons;
    QFrame *frame;
    QCheckBox *SD_pass1;
    QCheckBox *SD_p1r;
    QCheckBox *SD_light;
    QFrame *frame_2;
    QCheckBox *SD_pass5;
    QCheckBox *SD_p5r;
    QFrame *line;
    QCheckBox *SD_cull;
    QCheckBox *SD_lines;
    QCheckBox *SD_persp;
    QFrame *frame1;
    QCheckBox *SD_pass4;
    QCheckBox *SD_p4r;
    QCheckBox *coordNum;
    QCheckBox *SD_lightMove;
    QCheckBox *SD_unused;
    QFrame *frame2;
    QCheckBox *SD_pass3;
    QCheckBox *SD_p3r;
    QCheckBox *SD_linesAll;
    QPushButton *resetLight;
    QCheckBox *coordName;
    QFrame *frame3;
    QCheckBox *SD_pass2;
    QCheckBox *SD_p2r;
    QCheckBox *SD_vtxNormals;
    QPushButton *selColBot;
    QPushButton *backColBot;
    QPushButton *mattColBot;
    QLineEdit *shineEdit;
    QPushButton *ambBot;
    QPushButton *diffBot;
    QPushButton *specBot;
    QLabel *label_6;
    QLabel *label_7;
    QPushButton *saveBot;
    QPushButton *lineColBot;
    QCheckBox *SD_pntLight;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout;
    QLabel *label_2;
    QSlider *clipSlider;
    QPushButton *resetView;
    QHBoxLayout *horizontalLayout_5;
    QSpacerItem *horizontalSpacer;
    QComboBox *texSel;
    QWidget *tex0Fake;
    QHBoxLayout *horizontalLayout_6;
    QLabel *label_8;
    QLineEdit *tex0FakeLabek;
    QCheckBox *multiSampQuad;
    QWidget *tex0cont;
    QHBoxLayout *horizontalLayout_7;
    QLabel *label_3;
    QLineEdit *tex1NameLabel;
    QPushButton *tex1FileBot;
    QWidget *tex1Fake;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_9;
    QLineEdit *tex0FakeLabek_2;
    QWidget *tex1cont;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_4;
    QLineEdit *tex2NameLabel;
    QPushButton *tex2FileBot;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_5;
    QLineEdit *tex3NameLabel;
    QPushButton *tex3FileBot;

    void setupUi(QWidget *DispContDlg)
    {
        if (DispContDlg->objectName().isEmpty())
            DispContDlg->setObjectName(QString::fromUtf8("DispContDlg"));
        DispContDlg->resize(208, 517);
        DispContDlg->setMinimumSize(QSize(208, 0));
        verticalLayout = new QVBoxLayout(DispContDlg);
        verticalLayout->setSpacing(3);
        verticalLayout->setMargin(4);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        widget = new QWidget(DispContDlg);
        widget->setObjectName(QString::fromUtf8("widget"));
        widget->setMinimumSize(QSize(0, 335));
        line_2 = new QFrame(widget);
        line_2->setObjectName(QString::fromUtf8("line_2"));
        line_2->setGeometry(QRect(10, 150, 85, 3));
        line_2->setFrameShape(QFrame::HLine);
        line_2->setFrameShadow(QFrame::Sunken);
        SD_points = new QCheckBox(widget);
        SD_points->setObjectName(QString::fromUtf8("SD_points"));
        SD_points->setGeometry(QRect(106, 116, 71, 18));
        coordFontSize = new QSpinBox(widget);
        coordFontSize->setObjectName(QString::fromUtf8("coordFontSize"));
        coordFontSize->setEnabled(false);
        coordFontSize->setGeometry(QRect(38, 213, 44, 22));
        coordFontSize->setMinimum(6);
        coordFontSize->setMaximum(72);
        coordFontSize->setValue(8);
        label = new QLabel(widget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(50, 10, 46, 14));
        label->setAlignment(Qt::AlignCenter);
        SD_litback = new QCheckBox(widget);
        SD_litback->setObjectName(QString::fromUtf8("SD_litback"));
        SD_litback->setGeometry(QRect(105, 65, 81, 18));
        SD_polygons = new QCheckBox(widget);
        SD_polygons->setObjectName(QString::fromUtf8("SD_polygons"));
        SD_polygons->setGeometry(QRect(106, 194, 71, 18));
        frame = new QFrame(widget);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setGeometry(QRect(7, 26, 81, 25));
        frame->setFrameShadow(QFrame::Plain);
        frame->setLineWidth(0);
        SD_pass1 = new QCheckBox(frame);
        SD_pass1->setObjectName(QString::fromUtf8("SD_pass1"));
        SD_pass1->setGeometry(QRect(5, 4, 66, 18));
        SD_p1r = new QCheckBox(frame);
        SD_p1r->setObjectName(QString::fromUtf8("SD_p1r"));
        SD_p1r->setGeometry(QRect(61, 4, 16, 18));
        SD_light = new QCheckBox(widget);
        SD_light->setObjectName(QString::fromUtf8("SD_light"));
        SD_light->setGeometry(QRect(105, 5, 81, 18));
        frame_2 = new QFrame(widget);
        frame_2->setObjectName(QString::fromUtf8("frame_2"));
        frame_2->setGeometry(QRect(7, 119, 81, 25));
        frame_2->setFrameShape(QFrame::NoFrame);
        frame_2->setFrameShadow(QFrame::Raised);
        SD_pass5 = new QCheckBox(frame_2);
        SD_pass5->setObjectName(QString::fromUtf8("SD_pass5"));
        SD_pass5->setGeometry(QRect(5, 4, 66, 18));
        SD_p5r = new QCheckBox(frame_2);
        SD_p5r->setObjectName(QString::fromUtf8("SD_p5r"));
        SD_p5r->setGeometry(QRect(61, 5, 21, 18));
        line = new QFrame(widget);
        line->setObjectName(QString::fromUtf8("line"));
        line->setGeometry(QRect(101, 109, 85, 3));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        SD_cull = new QCheckBox(widget);
        SD_cull->setObjectName(QString::fromUtf8("SD_cull"));
        SD_cull->setGeometry(QRect(105, 84, 81, 18));
        SD_lines = new QCheckBox(widget);
        SD_lines->setObjectName(QString::fromUtf8("SD_lines"));
        SD_lines->setGeometry(QRect(106, 156, 71, 18));
        SD_persp = new QCheckBox(widget);
        SD_persp->setObjectName(QString::fromUtf8("SD_persp"));
        SD_persp->setGeometry(QRect(105, 45, 81, 18));
        frame1 = new QFrame(widget);
        frame1->setObjectName(QString::fromUtf8("frame1"));
        frame1->setGeometry(QRect(7, 96, 81, 25));
        frame1->setFrameShape(QFrame::NoFrame);
        frame1->setFrameShadow(QFrame::Raised);
        SD_pass4 = new QCheckBox(frame1);
        SD_pass4->setObjectName(QString::fromUtf8("SD_pass4"));
        SD_pass4->setGeometry(QRect(5, 4, 66, 18));
        SD_p4r = new QCheckBox(frame1);
        SD_p4r->setObjectName(QString::fromUtf8("SD_p4r"));
        SD_p4r->setGeometry(QRect(61, 5, 21, 18));
        coordNum = new QCheckBox(widget);
        coordNum->setObjectName(QString::fromUtf8("coordNum"));
        coordNum->setGeometry(QRect(107, 216, 81, 20));
        SD_lightMove = new QCheckBox(widget);
        SD_lightMove->setObjectName(QString::fromUtf8("SD_lightMove"));
        SD_lightMove->setGeometry(QRect(125, 25, 71, 18));
        SD_unused = new QCheckBox(widget);
        SD_unused->setObjectName(QString::fromUtf8("SD_unused"));
        SD_unused->setGeometry(QRect(126, 136, 71, 18));
        frame2 = new QFrame(widget);
        frame2->setObjectName(QString::fromUtf8("frame2"));
        frame2->setGeometry(QRect(7, 73, 81, 25));
        frame2->setFrameShape(QFrame::NoFrame);
        frame2->setFrameShadow(QFrame::Raised);
        SD_pass3 = new QCheckBox(frame2);
        SD_pass3->setObjectName(QString::fromUtf8("SD_pass3"));
        SD_pass3->setGeometry(QRect(5, 4, 66, 18));
        SD_p3r = new QCheckBox(frame2);
        SD_p3r->setObjectName(QString::fromUtf8("SD_p3r"));
        SD_p3r->setGeometry(QRect(61, 4, 21, 18));
        SD_linesAll = new QCheckBox(widget);
        SD_linesAll->setObjectName(QString::fromUtf8("SD_linesAll"));
        SD_linesAll->setGeometry(QRect(126, 176, 61, 18));
        resetLight = new QPushButton(widget);
        resetLight->setObjectName(QString::fromUtf8("resetLight"));
        resetLight->setGeometry(QRect(172, 24, 20, 20));
        coordName = new QCheckBox(widget);
        coordName->setObjectName(QString::fromUtf8("coordName"));
        coordName->setEnabled(true);
        coordName->setGeometry(QRect(88, 217, 16, 18));
        frame3 = new QFrame(widget);
        frame3->setObjectName(QString::fromUtf8("frame3"));
        frame3->setGeometry(QRect(7, 49, 81, 25));
        frame3->setFrameShape(QFrame::NoFrame);
        frame3->setFrameShadow(QFrame::Raised);
        SD_pass2 = new QCheckBox(frame3);
        SD_pass2->setObjectName(QString::fromUtf8("SD_pass2"));
        SD_pass2->setGeometry(QRect(5, 4, 66, 18));
        SD_p2r = new QCheckBox(frame3);
        SD_p2r->setObjectName(QString::fromUtf8("SD_p2r"));
        SD_p2r->setGeometry(QRect(61, 5, 21, 18));
        SD_vtxNormals = new QCheckBox(widget);
        SD_vtxNormals->setObjectName(QString::fromUtf8("SD_vtxNormals"));
        SD_vtxNormals->setGeometry(QRect(12, 160, 71, 21));
        selColBot = new QPushButton(widget);
        selColBot->setObjectName(QString::fromUtf8("selColBot"));
        selColBot->setGeometry(QRect(62, 283, 61, 24));
        backColBot = new QPushButton(widget);
        backColBot->setObjectName(QString::fromUtf8("backColBot"));
        backColBot->setGeometry(QRect(124, 259, 71, 24));
        mattColBot = new QPushButton(widget);
        mattColBot->setObjectName(QString::fromUtf8("mattColBot"));
        mattColBot->setGeometry(QRect(62, 259, 61, 24));
        shineEdit = new QLineEdit(widget);
        shineEdit->setObjectName(QString::fromUtf8("shineEdit"));
        shineEdit->setGeometry(QRect(79, 310, 41, 21));
        ambBot = new QPushButton(widget);
        ambBot->setObjectName(QString::fromUtf8("ambBot"));
        ambBot->setGeometry(QRect(9, 259, 51, 24));
        diffBot = new QPushButton(widget);
        diffBot->setObjectName(QString::fromUtf8("diffBot"));
        diffBot->setGeometry(QRect(9, 283, 51, 24));
        specBot = new QPushButton(widget);
        specBot->setObjectName(QString::fromUtf8("specBot"));
        specBot->setGeometry(QRect(9, 307, 51, 24));
        label_6 = new QLabel(widget);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(12, 242, 46, 14));
        label_7 = new QLabel(widget);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setGeometry(QRect(67, 243, 46, 14));
        saveBot = new QPushButton(widget);
        saveBot->setObjectName(QString::fromUtf8("saveBot"));
        saveBot->setGeometry(QRect(10, 184, 41, 23));
        lineColBot = new QPushButton(widget);
        lineColBot->setObjectName(QString::fromUtf8("lineColBot"));
        lineColBot->setGeometry(QRect(144, 283, 51, 24));
        SD_pntLight = new QCheckBox(widget);
        SD_pntLight->setObjectName(QString::fromUtf8("SD_pntLight"));
        SD_pntLight->setGeometry(QRect(158, 5, 72, 18));

        verticalLayout->addWidget(widget);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label_2 = new QLabel(DispContDlg);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        horizontalLayout->addWidget(label_2);

        clipSlider = new QSlider(DispContDlg);
        clipSlider->setObjectName(QString::fromUtf8("clipSlider"));
        clipSlider->setFocusPolicy(Qt::NoFocus);
        clipSlider->setMaximum(100);
        clipSlider->setOrientation(Qt::Horizontal);

        horizontalLayout->addWidget(clipSlider);

        resetView = new QPushButton(DispContDlg);
        resetView->setObjectName(QString::fromUtf8("resetView"));
        resetView->setMaximumSize(QSize(21, 21));

        horizontalLayout->addWidget(resetView);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer);

        texSel = new QComboBox(DispContDlg);
        texSel->setObjectName(QString::fromUtf8("texSel"));

        horizontalLayout_5->addWidget(texSel);


        verticalLayout->addLayout(horizontalLayout_5);

        tex0Fake = new QWidget(DispContDlg);
        tex0Fake->setObjectName(QString::fromUtf8("tex0Fake"));
        horizontalLayout_6 = new QHBoxLayout(tex0Fake);
        horizontalLayout_6->setSpacing(3);
        horizontalLayout_6->setMargin(0);
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        label_8 = new QLabel(tex0Fake);
        label_8->setObjectName(QString::fromUtf8("label_8"));

        horizontalLayout_6->addWidget(label_8);

        tex0FakeLabek = new QLineEdit(tex0Fake);
        tex0FakeLabek->setObjectName(QString::fromUtf8("tex0FakeLabek"));
        tex0FakeLabek->setEnabled(false);
        tex0FakeLabek->setAcceptDrops(false);
        tex0FakeLabek->setReadOnly(true);

        horizontalLayout_6->addWidget(tex0FakeLabek);

        multiSampQuad = new QCheckBox(tex0Fake);
        multiSampQuad->setObjectName(QString::fromUtf8("multiSampQuad"));

        horizontalLayout_6->addWidget(multiSampQuad);


        verticalLayout->addWidget(tex0Fake);

        tex0cont = new QWidget(DispContDlg);
        tex0cont->setObjectName(QString::fromUtf8("tex0cont"));
        horizontalLayout_7 = new QHBoxLayout(tex0cont);
        horizontalLayout_7->setSpacing(3);
        horizontalLayout_7->setMargin(0);
        horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));
        label_3 = new QLabel(tex0cont);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label_3->sizePolicy().hasHeightForWidth());
        label_3->setSizePolicy(sizePolicy);

        horizontalLayout_7->addWidget(label_3);

        tex1NameLabel = new QLineEdit(tex0cont);
        tex1NameLabel->setObjectName(QString::fromUtf8("tex1NameLabel"));

        horizontalLayout_7->addWidget(tex1NameLabel);

        tex1FileBot = new QPushButton(tex0cont);
        tex1FileBot->setObjectName(QString::fromUtf8("tex1FileBot"));
        tex1FileBot->setMaximumSize(QSize(21, 21));

        horizontalLayout_7->addWidget(tex1FileBot);


        verticalLayout->addWidget(tex0cont);

        tex1Fake = new QWidget(DispContDlg);
        tex1Fake->setObjectName(QString::fromUtf8("tex1Fake"));
        horizontalLayout_3 = new QHBoxLayout(tex1Fake);
        horizontalLayout_3->setSpacing(3);
        horizontalLayout_3->setMargin(0);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        label_9 = new QLabel(tex1Fake);
        label_9->setObjectName(QString::fromUtf8("label_9"));

        horizontalLayout_3->addWidget(label_9);

        tex0FakeLabek_2 = new QLineEdit(tex1Fake);
        tex0FakeLabek_2->setObjectName(QString::fromUtf8("tex0FakeLabek_2"));
        tex0FakeLabek_2->setEnabled(false);
        tex0FakeLabek_2->setAcceptDrops(false);
        tex0FakeLabek_2->setReadOnly(true);

        horizontalLayout_3->addWidget(tex0FakeLabek_2);


        verticalLayout->addWidget(tex1Fake);

        tex1cont = new QWidget(DispContDlg);
        tex1cont->setObjectName(QString::fromUtf8("tex1cont"));
        horizontalLayout_4 = new QHBoxLayout(tex1cont);
        horizontalLayout_4->setSpacing(3);
        horizontalLayout_4->setMargin(0);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        label_4 = new QLabel(tex1cont);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        horizontalLayout_4->addWidget(label_4);

        tex2NameLabel = new QLineEdit(tex1cont);
        tex2NameLabel->setObjectName(QString::fromUtf8("tex2NameLabel"));

        horizontalLayout_4->addWidget(tex2NameLabel);

        tex2FileBot = new QPushButton(tex1cont);
        tex2FileBot->setObjectName(QString::fromUtf8("tex2FileBot"));
        tex2FileBot->setMaximumSize(QSize(21, 21));

        horizontalLayout_4->addWidget(tex2FileBot);


        verticalLayout->addWidget(tex1cont);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(3);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        label_5 = new QLabel(DispContDlg);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        horizontalLayout_2->addWidget(label_5);

        tex3NameLabel = new QLineEdit(DispContDlg);
        tex3NameLabel->setObjectName(QString::fromUtf8("tex3NameLabel"));

        horizontalLayout_2->addWidget(tex3NameLabel);

        tex3FileBot = new QPushButton(DispContDlg);
        tex3FileBot->setObjectName(QString::fromUtf8("tex3FileBot"));
        tex3FileBot->setMaximumSize(QSize(21, 21));

        horizontalLayout_2->addWidget(tex3FileBot);


        verticalLayout->addLayout(horizontalLayout_2);


        retranslateUi(DispContDlg);
        QObject::connect(SD_light, SIGNAL(toggled(bool)), SD_lightMove, SLOT(setEnabled(bool)));
        QObject::connect(SD_points, SIGNAL(toggled(bool)), SD_unused, SLOT(setEnabled(bool)));

        QMetaObject::connectSlotsByName(DispContDlg);
    } // setupUi

    void retranslateUi(QWidget *DispContDlg)
    {
        DispContDlg->setWindowTitle(QApplication::translate("DispContDlg", "KawaiiGL Control", 0, QApplication::UnicodeUTF8));
        SD_points->setText(QApplication::translate("DispContDlg", "Points", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("DispContDlg", "Round", 0, QApplication::UnicodeUTF8));
        SD_litback->setText(QApplication::translate("DispContDlg", "Lit backface", 0, QApplication::UnicodeUTF8));
        SD_polygons->setText(QApplication::translate("DispContDlg", "Polygons", 0, QApplication::UnicodeUTF8));
        SD_pass1->setText(QApplication::translate("DispContDlg", "Pass 1", 0, QApplication::UnicodeUTF8));
        SD_light->setText(QApplication::translate("DispContDlg", "Light", 0, QApplication::UnicodeUTF8));
        SD_pass5->setText(QApplication::translate("DispContDlg", "Pass 5", 0, QApplication::UnicodeUTF8));
        SD_cull->setText(QApplication::translate("DispContDlg", "Cull faces", 0, QApplication::UnicodeUTF8));
        SD_lines->setText(QApplication::translate("DispContDlg", "Lines", 0, QApplication::UnicodeUTF8));
        SD_persp->setText(QApplication::translate("DispContDlg", "Perspective", 0, QApplication::UnicodeUTF8));
        SD_pass4->setText(QApplication::translate("DispContDlg", "Pass 4", 0, QApplication::UnicodeUTF8));
        coordNum->setText(QApplication::translate("DispContDlg", "Coordinates", 0, QApplication::UnicodeUTF8));
        SD_lightMove->setText(QApplication::translate("DispContDlg", "Move", 0, QApplication::UnicodeUTF8));
        SD_unused->setText(QApplication::translate("DispContDlg", "Unused", 0, QApplication::UnicodeUTF8));
        SD_pass3->setText(QApplication::translate("DispContDlg", "Pass 3", 0, QApplication::UnicodeUTF8));
        SD_linesAll->setText(QApplication::translate("DispContDlg", "Subdiv", 0, QApplication::UnicodeUTF8));
        resetLight->setText(QApplication::translate("DispContDlg", "0", 0, QApplication::UnicodeUTF8));
        coordName->setText(QString());
        SD_pass2->setText(QApplication::translate("DispContDlg", "Pass 2", 0, QApplication::UnicodeUTF8));
        SD_vtxNormals->setText(QApplication::translate("DispContDlg", "Smooth", 0, QApplication::UnicodeUTF8));
        selColBot->setText(QApplication::translate("DispContDlg", "Selected", 0, QApplication::UnicodeUTF8));
        backColBot->setText(QApplication::translate("DispContDlg", "Background", 0, QApplication::UnicodeUTF8));
        mattColBot->setText(QApplication::translate("DispContDlg", "Default", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        shineEdit->setToolTip(QApplication::translate("DispContDlg", "Shininess", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        ambBot->setText(QApplication::translate("DispContDlg", "Ambient", 0, QApplication::UnicodeUTF8));
        diffBot->setText(QApplication::translate("DispContDlg", "Diffuse", 0, QApplication::UnicodeUTF8));
        specBot->setText(QApplication::translate("DispContDlg", "Specular", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("DispContDlg", "Light 0:", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("DispContDlg", "Material:", 0, QApplication::UnicodeUTF8));
        saveBot->setText(QApplication::translate("DispContDlg", "Save", 0, QApplication::UnicodeUTF8));
        lineColBot->setText(QApplication::translate("DispContDlg", "Lines", 0, QApplication::UnicodeUTF8));
        SD_pntLight->setText(QApplication::translate("DispContDlg", "Pnt", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("DispContDlg", "Clip", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        resetView->setToolTip(QApplication::translate("DispContDlg", "Reset View", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        resetView->setText(QApplication::translate("DispContDlg", "0", 0, QApplication::UnicodeUTF8));
        label_8->setText(QApplication::translate("DispContDlg", "0.", 0, QApplication::UnicodeUTF8));
        tex0FakeLabek->setText(QApplication::translate("DispContDlg", "[SCREEN]", 0, QApplication::UnicodeUTF8));
        multiSampQuad->setText(QApplication::translate("DispContDlg", "Multi-Samp", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("DispContDlg", "0.", 0, QApplication::UnicodeUTF8));
        tex1FileBot->setText(QApplication::translate("DispContDlg", "...", 0, QApplication::UnicodeUTF8));
        label_9->setText(QApplication::translate("DispContDlg", "0.", 0, QApplication::UnicodeUTF8));
        tex0FakeLabek_2->setText(QApplication::translate("DispContDlg", "[SCREEN]", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("DispContDlg", "1.", 0, QApplication::UnicodeUTF8));
        tex2FileBot->setText(QApplication::translate("DispContDlg", "...", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("DispContDlg", "2.", 0, QApplication::UnicodeUTF8));
        tex3FileBot->setText(QApplication::translate("DispContDlg", "...", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(DispContDlg);
    } // retranslateUi

};

namespace Ui {
    class DispContDlg: public Ui_DispContDlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DISPCONTDLG_H
