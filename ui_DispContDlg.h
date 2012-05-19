/********************************************************************************
** Form generated from reading UI file 'DispContDlg.ui'
**
** Created: Wed Dec 14 12:12:44 2011
**      by: Qt User Interface Compiler version 4.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
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
    QCheckBox *SD_triang;
    QCheckBox *blendBox;
    QCheckBox *SD_allpoly;
    QSpinBox *pointSizeSpin;
    QCheckBox *zbufBox;
    QPushButton *resetBot;
    QPushButton *resetView;
    QSlider *clipSlider;
    QSpinBox *lineWidthSpin;
    QCheckBox *linePoly;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout_5;
    QSpacerItem *horizontalSpacer;
    QComboBox *texSel;
    QWidget *tex0cont;
    QHBoxLayout *horizontalLayout_7;
    QLabel *label_3;
    QLineEdit *tex1NameLabel;
    QPushButton *tex1FileBot;
    QWidget *tex1cont;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_4;
    QLineEdit *tex2NameLabel;
    QPushButton *tex2FileBot;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_5;
    QLineEdit *tex3NameLabel;
    QPushButton *tex3FileBot;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_8;
    QLineEdit *tex4NameLabel;
    QPushButton *tex4FileBot;

    void setupUi(QWidget *DispContDlg)
    {
        if (DispContDlg->objectName().isEmpty())
            DispContDlg->setObjectName(QString::fromUtf8("DispContDlg"));
        DispContDlg->resize(208, 542);
        DispContDlg->setMinimumSize(QSize(208, 0));
        verticalLayout = new QVBoxLayout(DispContDlg);
        verticalLayout->setSpacing(3);
        verticalLayout->setContentsMargins(4, 4, 4, 4);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        widget = new QWidget(DispContDlg);
        widget->setObjectName(QString::fromUtf8("widget"));
        widget->setMinimumSize(QSize(0, 400));
        line_2 = new QFrame(widget);
        line_2->setObjectName(QString::fromUtf8("line_2"));
        line_2->setGeometry(QRect(10, 150, 85, 3));
        line_2->setFrameShape(QFrame::HLine);
        line_2->setFrameShadow(QFrame::Sunken);
        SD_points = new QCheckBox(widget);
        SD_points->setObjectName(QString::fromUtf8("SD_points"));
        SD_points->setGeometry(QRect(99, 137, 71, 18));
        coordFontSize = new QSpinBox(widget);
        coordFontSize->setObjectName(QString::fromUtf8("coordFontSize"));
        coordFontSize->setEnabled(false);
        coordFontSize->setGeometry(QRect(30, 249, 44, 22));
        coordFontSize->setMinimum(6);
        coordFontSize->setMaximum(72);
        coordFontSize->setValue(8);
        label = new QLabel(widget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(50, 10, 46, 14));
        label->setAlignment(Qt::AlignCenter);
        SD_litback = new QCheckBox(widget);
        SD_litback->setObjectName(QString::fromUtf8("SD_litback"));
        SD_litback->setGeometry(QRect(99, 65, 81, 18));
        SD_polygons = new QCheckBox(widget);
        SD_polygons->setObjectName(QString::fromUtf8("SD_polygons"));
        SD_polygons->setGeometry(QRect(99, 233, 71, 18));
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
        SD_light->setGeometry(QRect(99, 5, 81, 18));
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
        line->setGeometry(QRect(102, 130, 85, 3));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        SD_cull = new QCheckBox(widget);
        SD_cull->setObjectName(QString::fromUtf8("SD_cull"));
        SD_cull->setGeometry(QRect(99, 84, 81, 18));
        SD_lines = new QCheckBox(widget);
        SD_lines->setObjectName(QString::fromUtf8("SD_lines"));
        SD_lines->setGeometry(QRect(99, 195, 71, 18));
        SD_persp = new QCheckBox(widget);
        SD_persp->setObjectName(QString::fromUtf8("SD_persp"));
        SD_persp->setGeometry(QRect(99, 45, 81, 18));
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
        coordNum->setGeometry(QRect(99, 252, 81, 20));
        SD_lightMove = new QCheckBox(widget);
        SD_lightMove->setObjectName(QString::fromUtf8("SD_lightMove"));
        SD_lightMove->setGeometry(QRect(119, 25, 71, 18));
        SD_unused = new QCheckBox(widget);
        SD_unused->setObjectName(QString::fromUtf8("SD_unused"));
        SD_unused->setGeometry(QRect(115, 157, 71, 18));
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
        SD_linesAll->setGeometry(QRect(115, 214, 61, 18));
        resetLight = new QPushButton(widget);
        resetLight->setObjectName(QString::fromUtf8("resetLight"));
        resetLight->setGeometry(QRect(166, 24, 20, 20));
        coordName = new QCheckBox(widget);
        coordName->setObjectName(QString::fromUtf8("coordName"));
        coordName->setEnabled(true);
        coordName->setGeometry(QRect(80, 253, 16, 18));
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
        SD_vtxNormals->setGeometry(QRect(12, 154, 71, 21));
        selColBot = new QPushButton(widget);
        selColBot->setObjectName(QString::fromUtf8("selColBot"));
        selColBot->setGeometry(QRect(62, 321, 61, 24));
        backColBot = new QPushButton(widget);
        backColBot->setObjectName(QString::fromUtf8("backColBot"));
        backColBot->setGeometry(QRect(124, 297, 71, 24));
        mattColBot = new QPushButton(widget);
        mattColBot->setObjectName(QString::fromUtf8("mattColBot"));
        mattColBot->setGeometry(QRect(62, 297, 61, 24));
        shineEdit = new QLineEdit(widget);
        shineEdit->setObjectName(QString::fromUtf8("shineEdit"));
        shineEdit->setGeometry(QRect(79, 348, 41, 21));
        ambBot = new QPushButton(widget);
        ambBot->setObjectName(QString::fromUtf8("ambBot"));
        ambBot->setGeometry(QRect(9, 297, 51, 24));
        diffBot = new QPushButton(widget);
        diffBot->setObjectName(QString::fromUtf8("diffBot"));
        diffBot->setGeometry(QRect(9, 321, 51, 24));
        specBot = new QPushButton(widget);
        specBot->setObjectName(QString::fromUtf8("specBot"));
        specBot->setGeometry(QRect(9, 345, 51, 24));
        label_6 = new QLabel(widget);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(12, 280, 46, 14));
        label_7 = new QLabel(widget);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setGeometry(QRect(67, 281, 46, 14));
        saveBot = new QPushButton(widget);
        saveBot->setObjectName(QString::fromUtf8("saveBot"));
        saveBot->setGeometry(QRect(10, 195, 41, 23));
        lineColBot = new QPushButton(widget);
        lineColBot->setObjectName(QString::fromUtf8("lineColBot"));
        lineColBot->setGeometry(QRect(144, 321, 51, 24));
        SD_pntLight = new QCheckBox(widget);
        SD_pntLight->setObjectName(QString::fromUtf8("SD_pntLight"));
        SD_pntLight->setGeometry(QRect(152, 5, 72, 18));
        SD_triang = new QCheckBox(widget);
        SD_triang->setObjectName(QString::fromUtf8("SD_triang"));
        SD_triang->setGeometry(QRect(12, 175, 81, 18));
        blendBox = new QCheckBox(widget);
        blendBox->setObjectName(QString::fromUtf8("blendBox"));
        blendBox->setGeometry(QRect(99, 104, 72, 18));
        SD_allpoly = new QCheckBox(widget);
        SD_allpoly->setObjectName(QString::fromUtf8("SD_allpoly"));
        SD_allpoly->setGeometry(QRect(115, 176, 72, 18));
        pointSizeSpin = new QSpinBox(widget);
        pointSizeSpin->setObjectName(QString::fromUtf8("pointSizeSpin"));
        pointSizeSpin->setGeometry(QRect(155, 135, 42, 21));
        pointSizeSpin->setAccelerated(true);
        pointSizeSpin->setMaximum(200);
        pointSizeSpin->setValue(14);
        zbufBox = new QCheckBox(widget);
        zbufBox->setObjectName(QString::fromUtf8("zbufBox"));
        zbufBox->setGeometry(QRect(149, 104, 72, 18));
        resetBot = new QPushButton(widget);
        resetBot->setObjectName(QString::fromUtf8("resetBot"));
        resetBot->setGeometry(QRect(10, 221, 61, 23));
        resetView = new QPushButton(widget);
        resetView->setObjectName(QString::fromUtf8("resetView"));
        resetView->setGeometry(QRect(72, 221, 21, 23));
        resetView->setMaximumSize(QSize(21, 23));
        clipSlider = new QSlider(widget);
        clipSlider->setObjectName(QString::fromUtf8("clipSlider"));
        clipSlider->setGeometry(QRect(10, 376, 181, 19));
        clipSlider->setMaximum(100);
        clipSlider->setSingleStep(1);
        clipSlider->setOrientation(Qt::Horizontal);
        lineWidthSpin = new QSpinBox(widget);
        lineWidthSpin->setObjectName(QString::fromUtf8("lineWidthSpin"));
        lineWidthSpin->setGeometry(QRect(150, 195, 42, 21));
        lineWidthSpin->setMaximum(255);
        lineWidthSpin->setValue(1);
        linePoly = new QCheckBox(widget);
        linePoly->setObjectName(QString::fromUtf8("linePoly"));
        linePoly->setGeometry(QRect(163, 234, 70, 17));

        verticalLayout->addWidget(widget);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer);

        texSel = new QComboBox(DispContDlg);
        texSel->setObjectName(QString::fromUtf8("texSel"));

        horizontalLayout_5->addWidget(texSel);


        verticalLayout->addLayout(horizontalLayout_5);

        tex0cont = new QWidget(DispContDlg);
        tex0cont->setObjectName(QString::fromUtf8("tex0cont"));
        horizontalLayout_7 = new QHBoxLayout(tex0cont);
        horizontalLayout_7->setSpacing(3);
        horizontalLayout_7->setContentsMargins(0, 0, 0, 0);
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

        label_3->raise();
        tex1FileBot->raise();
        tex1NameLabel->raise();

        verticalLayout->addWidget(tex0cont);

        tex1cont = new QWidget(DispContDlg);
        tex1cont->setObjectName(QString::fromUtf8("tex1cont"));
        horizontalLayout_4 = new QHBoxLayout(tex1cont);
        horizontalLayout_4->setSpacing(3);
        horizontalLayout_4->setContentsMargins(0, 0, 0, 0);
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

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(3);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(-1, -1, -1, 0);
        label_8 = new QLabel(DispContDlg);
        label_8->setObjectName(QString::fromUtf8("label_8"));

        horizontalLayout_3->addWidget(label_8);

        tex4NameLabel = new QLineEdit(DispContDlg);
        tex4NameLabel->setObjectName(QString::fromUtf8("tex4NameLabel"));

        horizontalLayout_3->addWidget(tex4NameLabel);

        tex4FileBot = new QPushButton(DispContDlg);
        tex4FileBot->setObjectName(QString::fromUtf8("tex4FileBot"));
        tex4FileBot->setMaximumSize(QSize(21, 21));

        horizontalLayout_3->addWidget(tex4FileBot);


        verticalLayout->addLayout(horizontalLayout_3);


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
#ifndef QT_NO_TOOLTIP
        saveBot->setToolTip(QApplication::translate("DispContDlg", "Save the subdivided object to a file", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        saveBot->setText(QApplication::translate("DispContDlg", "Save", 0, QApplication::UnicodeUTF8));
        lineColBot->setText(QApplication::translate("DispContDlg", "Lines", 0, QApplication::UnicodeUTF8));
        SD_pntLight->setText(QApplication::translate("DispContDlg", "Pnt", 0, QApplication::UnicodeUTF8));
        SD_triang->setText(QApplication::translate("DispContDlg", "Triangulate", 0, QApplication::UnicodeUTF8));
        blendBox->setText(QApplication::translate("DispContDlg", "Blend", 0, QApplication::UnicodeUTF8));
        SD_allpoly->setText(QApplication::translate("DispContDlg", "All Poly", 0, QApplication::UnicodeUTF8));
        zbufBox->setText(QApplication::translate("DispContDlg", "Z-buf", 0, QApplication::UnicodeUTF8));
        resetBot->setText(QApplication::translate("DispContDlg", "Reset All", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        resetView->setToolTip(QApplication::translate("DispContDlg", "Reset View", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        resetView->setText(QApplication::translate("DispContDlg", "0", 0, QApplication::UnicodeUTF8));
        linePoly->setText(QApplication::translate("DispContDlg", "line", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("DispContDlg", "0.", 0, QApplication::UnicodeUTF8));
        tex1FileBot->setText(QApplication::translate("DispContDlg", "...", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("DispContDlg", "1.", 0, QApplication::UnicodeUTF8));
        tex2FileBot->setText(QApplication::translate("DispContDlg", "...", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("DispContDlg", "2.", 0, QApplication::UnicodeUTF8));
        tex3FileBot->setText(QApplication::translate("DispContDlg", "...", 0, QApplication::UnicodeUTF8));
        label_8->setText(QApplication::translate("DispContDlg", "3.", 0, QApplication::UnicodeUTF8));
        tex4FileBot->setText(QApplication::translate("DispContDlg", "...", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class DispContDlg: public Ui_DispContDlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DISPCONTDLG_H
