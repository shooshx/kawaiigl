/********************************************************************************
** Form generated from reading ui file 'MyInputDlg.ui'
**
** Created: Wed Jul 15 12:18:59 2009
**      by: Qt User Interface Compiler version 4.5.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_MYINPUTDLG_H
#define UI_MYINPUTDLG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_MyCheckDlgClass
{
public:
    QVBoxLayout *verticalLayout_2;
    QVBoxLayout *optLayout;
    QLabel *captionLabel;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout;
    QPushButton *okBot;
    QPushButton *applyBot;
    QSpacerItem *horizontalSpacer;
    QPushButton *cancelBot;

    void setupUi(QDialog *MyCheckDlgClass)
    {
        if (MyCheckDlgClass->objectName().isEmpty())
            MyCheckDlgClass->setObjectName(QString::fromUtf8("MyCheckDlgClass"));
        MyCheckDlgClass->resize(269, 91);
        MyCheckDlgClass->setModal(true);
        verticalLayout_2 = new QVBoxLayout(MyCheckDlgClass);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setMargin(11);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        optLayout = new QVBoxLayout();
        optLayout->setSpacing(6);
        optLayout->setObjectName(QString::fromUtf8("optLayout"));
        optLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
        captionLabel = new QLabel(MyCheckDlgClass);
        captionLabel->setObjectName(QString::fromUtf8("captionLabel"));

        optLayout->addWidget(captionLabel);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        optLayout->addItem(verticalSpacer);


        verticalLayout_2->addLayout(optLayout);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
        okBot = new QPushButton(MyCheckDlgClass);
        okBot->setObjectName(QString::fromUtf8("okBot"));

        horizontalLayout->addWidget(okBot);

        applyBot = new QPushButton(MyCheckDlgClass);
        applyBot->setObjectName(QString::fromUtf8("applyBot"));

        horizontalLayout->addWidget(applyBot);

        horizontalSpacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        cancelBot = new QPushButton(MyCheckDlgClass);
        cancelBot->setObjectName(QString::fromUtf8("cancelBot"));

        horizontalLayout->addWidget(cancelBot);


        verticalLayout_2->addLayout(horizontalLayout);


        retranslateUi(MyCheckDlgClass);

        QMetaObject::connectSlotsByName(MyCheckDlgClass);
    } // setupUi

    void retranslateUi(QDialog *MyCheckDlgClass)
    {
        MyCheckDlgClass->setWindowTitle(QApplication::translate("MyCheckDlgClass", "MyCheckDlg", 0, QApplication::UnicodeUTF8));
        captionLabel->setText(QApplication::translate("MyCheckDlgClass", "TextLabel", 0, QApplication::UnicodeUTF8));
        okBot->setText(QApplication::translate("MyCheckDlgClass", "OK", 0, QApplication::UnicodeUTF8));
        applyBot->setText(QApplication::translate("MyCheckDlgClass", "Apply", 0, QApplication::UnicodeUTF8));
        cancelBot->setText(QApplication::translate("MyCheckDlgClass", "Cancel", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(MyCheckDlgClass);
    } // retranslateUi

};

namespace Ui {
    class MyCheckDlgClass: public Ui_MyCheckDlgClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MYINPUTDLG_H
