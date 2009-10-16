/********************************************************************************
** Form generated from reading ui file 'KwEdit.ui'
**
** Created: Fri Oct 16 16:47:09 2009
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
#include <QtGui/QDialog>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QTabWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_KwEdit
{
public:
    QVBoxLayout *verticalLayout;
    QTabWidget *tabs;
    QWidget *tab;
    QVBoxLayout *verticalLayout_2;
    QWidget *modelControl;
    QHBoxLayout *horizontalLayout_3;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *addFaceBot;
    QPushButton *saveBot1;
    QWidget *shaderControl;
    QVBoxLayout *verticalLayout_6;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *update_shader;
    QLabel *infoLabel;
    QPushButton *saveBot2;
    QHBoxLayout *horizontalLayout_4;
    QCheckBox *shaderEnable;
    QSpacerItem *horizontalSpacer;

    void setupUi(QDialog *KwEdit)
    {
        if (KwEdit->objectName().isEmpty())
            KwEdit->setObjectName(QString::fromUtf8("KwEdit"));
        KwEdit->resize(394, 511);
        verticalLayout = new QVBoxLayout(KwEdit);
        verticalLayout->setSpacing(5);
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
        tabs->addTab(tab, QString());

        verticalLayout->addWidget(tabs);

        modelControl = new QWidget(KwEdit);
        modelControl->setObjectName(QString::fromUtf8("modelControl"));
        horizontalLayout_3 = new QHBoxLayout(modelControl);
        horizontalLayout_3->setMargin(0);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        horizontalSpacer_2 = new QSpacerItem(23, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_2);

        addFaceBot = new QPushButton(modelControl);
        addFaceBot->setObjectName(QString::fromUtf8("addFaceBot"));
        addFaceBot->setMaximumSize(QSize(60, 23));
        addFaceBot->setCheckable(true);

        horizontalLayout_3->addWidget(addFaceBot);

        saveBot1 = new QPushButton(modelControl);
        saveBot1->setObjectName(QString::fromUtf8("saveBot1"));
        saveBot1->setMaximumSize(QSize(28, 23));
        QIcon icon;
        icon.addPixmap(QPixmap(QString::fromUtf8("://images/save.png")), QIcon::Normal, QIcon::Off);
        saveBot1->setIcon(icon);

        horizontalLayout_3->addWidget(saveBot1);


        verticalLayout->addWidget(modelControl);

        shaderControl = new QWidget(KwEdit);
        shaderControl->setObjectName(QString::fromUtf8("shaderControl"));
        verticalLayout_6 = new QVBoxLayout(shaderControl);
        verticalLayout_6->setMargin(0);
        verticalLayout_6->setObjectName(QString::fromUtf8("verticalLayout_6"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(-1, 0, -1, -1);
        update_shader = new QPushButton(shaderControl);
        update_shader->setObjectName(QString::fromUtf8("update_shader"));
        update_shader->setMaximumSize(QSize(70, 23));

        horizontalLayout_2->addWidget(update_shader);

        infoLabel = new QLabel(shaderControl);
        infoLabel->setObjectName(QString::fromUtf8("infoLabel"));
        infoLabel->setMinimumSize(QSize(0, 20));
        infoLabel->setFrameShape(QFrame::StyledPanel);

        horizontalLayout_2->addWidget(infoLabel);

        saveBot2 = new QPushButton(shaderControl);
        saveBot2->setObjectName(QString::fromUtf8("saveBot2"));
        saveBot2->setMaximumSize(QSize(28, 23));
        saveBot2->setIcon(icon);

        horizontalLayout_2->addWidget(saveBot2);


        verticalLayout_6->addLayout(horizontalLayout_2);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        horizontalLayout_4->setContentsMargins(-1, -1, -1, 0);
        shaderEnable = new QCheckBox(shaderControl);
        shaderEnable->setObjectName(QString::fromUtf8("shaderEnable"));

        horizontalLayout_4->addWidget(shaderEnable);

        horizontalSpacer = new QSpacerItem(40, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer);


        verticalLayout_6->addLayout(horizontalLayout_4);


        verticalLayout->addWidget(shaderControl);


        retranslateUi(KwEdit);

        tabs->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(KwEdit);
    } // setupUi

    void retranslateUi(QDialog *KwEdit)
    {
        KwEdit->setWindowTitle(QApplication::translate("KwEdit", "KawaiiGL Control", 0, QApplication::UnicodeUTF8));
        tabs->setTabText(tabs->indexOf(tab), QApplication::translate("KwEdit", "Model", 0, QApplication::UnicodeUTF8));
        addFaceBot->setText(QApplication::translate("KwEdit", "Add", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        saveBot1->setStatusTip(QApplication::translate("KwEdit", "Save", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        update_shader->setText(QApplication::translate("KwEdit", "Compile", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        saveBot2->setStatusTip(QApplication::translate("KwEdit", "Save", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        shaderEnable->setText(QApplication::translate("KwEdit", "Enable Shaders", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(KwEdit);
    } // retranslateUi

};

namespace Ui {
    class KwEdit: public Ui_KwEdit {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_KWEDIT_H
