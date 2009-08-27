/********************************************************************************
** Form generated from reading ui file 'KwEdit.ui'
**
** Created: Wed 26. Aug 18:14:54 2009
**      by: Qt User Interface Compiler version 4.5.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_KWEDIT_H
#define UI_KWEDIT_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QTabWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include "ProgTextEdit.h"

QT_BEGIN_NAMESPACE

class Ui_KwEdit
{
public:
    QVBoxLayout *verticalLayout;
    QTabWidget *tabs;
    QWidget *tab;
    QVBoxLayout *verticalLayout_2;
    ProgTextEdit *ed;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *addFaceBot;
    QWidget *tab_2;
    QVBoxLayout *verticalLayout_3;
    ProgTextEdit *vtxEd;
    QWidget *tab_3;
    QVBoxLayout *verticalLayout_4;
    ProgTextEdit *fragEd;
    QWidget *shaderControl;
    QVBoxLayout *verticalLayout_6;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *update_shader;
    QPushButton *reloadBot;
    QLabel *infoLabel;
    QHBoxLayout *horizontalLayout_4;
    QCheckBox *shaderEnable;
    QComboBox *runTypeBox;
    QSpacerItem *horizontalSpacer;
    QPushButton *addParam;

    void setupUi(QDialog *KwEdit)
    {
        if (KwEdit->objectName().isEmpty())
            KwEdit->setObjectName(QString::fromUtf8("KwEdit"));
        KwEdit->resize(364, 511);
        verticalLayout = new QVBoxLayout(KwEdit);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 2);
        tabs = new QTabWidget(KwEdit);
        tabs->setObjectName(QString::fromUtf8("tabs"));
        tabs->setAcceptDrops(true);
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        verticalLayout_2 = new QVBoxLayout(tab);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 3, 0, 0);
        ed = new ProgTextEdit(tab);
        ed->setObjectName(QString::fromUtf8("ed"));
        QFont font;
        font.setFamily(QString::fromUtf8("Courier"));
        font.setPointSize(10);
        ed->setFont(font);
        ed->setLineWrapMode(QTextEdit::NoWrap);
        ed->setTabStopWidth(40);
        ed->setAcceptRichText(false);

        verticalLayout_2->addWidget(ed);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(-1, -1, -1, 0);
        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);

        addFaceBot = new QPushButton(tab);
        addFaceBot->setObjectName(QString::fromUtf8("addFaceBot"));
        addFaceBot->setMaximumSize(QSize(60, 16777215));
        addFaceBot->setCheckable(true);

        horizontalLayout->addWidget(addFaceBot);


        verticalLayout_2->addLayout(horizontalLayout);

        tabs->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QString::fromUtf8("tab_2"));
        verticalLayout_3 = new QVBoxLayout(tab_2);
        verticalLayout_3->setSpacing(3);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        verticalLayout_3->setContentsMargins(0, 3, 0, 0);
        vtxEd = new ProgTextEdit(tab_2);
        vtxEd->setObjectName(QString::fromUtf8("vtxEd"));
        QFont font1;
        font1.setFamily(QString::fromUtf8("Courier"));
        font1.setPointSize(10);
        font1.setBold(false);
        font1.setItalic(false);
        font1.setUnderline(false);
        font1.setWeight(50);
        font1.setStrikeOut(false);
        vtxEd->setFont(font1);
        vtxEd->setLineWrapMode(QTextEdit::NoWrap);
        vtxEd->setTabStopWidth(40);
        vtxEd->setAcceptRichText(false);

        verticalLayout_3->addWidget(vtxEd);

        tabs->addTab(tab_2, QString());
        tab_3 = new QWidget();
        tab_3->setObjectName(QString::fromUtf8("tab_3"));
        verticalLayout_4 = new QVBoxLayout(tab_3);
        verticalLayout_4->setSpacing(3);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        verticalLayout_4->setContentsMargins(0, 3, 0, 0);
        fragEd = new ProgTextEdit(tab_3);
        fragEd->setObjectName(QString::fromUtf8("fragEd"));
        fragEd->setFont(font1);
        fragEd->setLineWrapMode(QTextEdit::NoWrap);
        fragEd->setTabStopWidth(40);
        fragEd->setAcceptRichText(false);

        verticalLayout_4->addWidget(fragEd);

        tabs->addTab(tab_3, QString());

        verticalLayout->addWidget(tabs);

        shaderControl = new QWidget(KwEdit);
        shaderControl->setObjectName(QString::fromUtf8("shaderControl"));
        verticalLayout_6 = new QVBoxLayout(shaderControl);
        verticalLayout_6->setObjectName(QString::fromUtf8("verticalLayout_6"));
        verticalLayout_6->setContentsMargins(3, 0, 3, 0);
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(-1, 0, -1, -1);
        update_shader = new QPushButton(shaderControl);
        update_shader->setObjectName(QString::fromUtf8("update_shader"));
        update_shader->setMaximumSize(QSize(70, 23));

        horizontalLayout_2->addWidget(update_shader);

        reloadBot = new QPushButton(shaderControl);
        reloadBot->setObjectName(QString::fromUtf8("reloadBot"));
        reloadBot->setMaximumSize(QSize(50, 23));

        horizontalLayout_2->addWidget(reloadBot);

        infoLabel = new QLabel(shaderControl);
        infoLabel->setObjectName(QString::fromUtf8("infoLabel"));
        infoLabel->setMinimumSize(QSize(0, 20));
        infoLabel->setFrameShape(QFrame::StyledPanel);

        horizontalLayout_2->addWidget(infoLabel);


        verticalLayout_6->addLayout(horizontalLayout_2);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        horizontalLayout_4->setContentsMargins(-1, -1, -1, 0);
        shaderEnable = new QCheckBox(shaderControl);
        shaderEnable->setObjectName(QString::fromUtf8("shaderEnable"));

        horizontalLayout_4->addWidget(shaderEnable);

        runTypeBox = new QComboBox(shaderControl);
        runTypeBox->setObjectName(QString::fromUtf8("runTypeBox"));

        horizontalLayout_4->addWidget(runTypeBox);

        horizontalSpacer = new QSpacerItem(40, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer);

        addParam = new QPushButton(shaderControl);
        addParam->setObjectName(QString::fromUtf8("addParam"));
        addParam->setMaximumSize(QSize(20, 20));

        horizontalLayout_4->addWidget(addParam);


        verticalLayout_6->addLayout(horizontalLayout_4);


        verticalLayout->addWidget(shaderControl);


        retranslateUi(KwEdit);

        tabs->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(KwEdit);
    } // setupUi

    void retranslateUi(QDialog *KwEdit)
    {
        KwEdit->setWindowTitle(QApplication::translate("KwEdit", "KawaiiGL Control", 0, QApplication::UnicodeUTF8));
        ed->setHtml(QApplication::translate("KwEdit", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Courier'; font-size:10pt; font-weight:400; font-style:normal;\">\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"></p></body></html>", 0, QApplication::UnicodeUTF8));
        addFaceBot->setText(QApplication::translate("KwEdit", "Add", 0, QApplication::UnicodeUTF8));
        tabs->setTabText(tabs->indexOf(tab), QApplication::translate("KwEdit", "Model", 0, QApplication::UnicodeUTF8));
        vtxEd->setHtml(QApplication::translate("KwEdit", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Courier'; font-size:10pt; font-weight:400; font-style:normal;\">\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-family:'MS Shell Dlg 2'; font-size:8pt;\"></p></body></html>", 0, QApplication::UnicodeUTF8));
        tabs->setTabText(tabs->indexOf(tab_2), QApplication::translate("KwEdit", "Vertex Shader", 0, QApplication::UnicodeUTF8));
        tabs->setTabText(tabs->indexOf(tab_3), QApplication::translate("KwEdit", "Fragment Shader", 0, QApplication::UnicodeUTF8));
        update_shader->setText(QApplication::translate("KwEdit", "Compile", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        reloadBot->setToolTip(QApplication::translate("KwEdit", "Reload the last program loaded", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        reloadBot->setText(QApplication::translate("KwEdit", "Reload", 0, QApplication::UnicodeUTF8));
        shaderEnable->setText(QApplication::translate("KwEdit", "Enable Shaders", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        addParam->setStatusTip(QApplication::translate("KwEdit", "Add Parameter", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        addParam->setText(QApplication::translate("KwEdit", "+", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(KwEdit);
    } // retranslateUi

};

namespace Ui {
    class KwEdit: public Ui_KwEdit {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_KWEDIT_H
