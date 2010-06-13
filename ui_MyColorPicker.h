/********************************************************************************
** Form generated from reading UI file 'MyColorPicker.ui'
**
** Created: Sun Jun 6 22:54:44 2010
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MYCOLORPICKER_H
#define UI_MYCOLORPICKER_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>
#include "QtAdd/qtcolortriangle.h"

QT_BEGIN_NAMESPACE

class Ui_MyColorPickerClass
{
public:
    QVBoxLayout *verticalLayout;
    QtColorTriangle *colortriangle;
    QGridLayout *gridLayout;
    QLineEdit *redEdit;
    QLineEdit *greenEdit;
    QLineEdit *blueEdit;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *viewCol;
    QLineEdit *htmlColEdit;
    QSpacerItem *horizontalSpacer;
    QHBoxLayout *horizontalLayout;
    QPushButton *oldBut;
    QSpacerItem *horizontalSpacer_2;
    QCheckBox *proofCheck;
    QLineEdit *alphaEdit;
    QLabel *label_4;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *invalidBot;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *MyColorPickerClass)
    {
        if (MyColorPickerClass->objectName().isEmpty())
            MyColorPickerClass->setObjectName(QString::fromUtf8("MyColorPickerClass"));
        MyColorPickerClass->resize(339, 415);
        verticalLayout = new QVBoxLayout(MyColorPickerClass);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        colortriangle = new QtColorTriangle(MyColorPickerClass);
        colortriangle->setObjectName(QString::fromUtf8("colortriangle"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(colortriangle->sizePolicy().hasHeightForWidth());
        colortriangle->setSizePolicy(sizePolicy);

        verticalLayout->addWidget(colortriangle);

        gridLayout = new QGridLayout();
        gridLayout->setSpacing(6);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        redEdit = new QLineEdit(MyColorPickerClass);
        redEdit->setObjectName(QString::fromUtf8("redEdit"));
        redEdit->setMaximumSize(QSize(50, 16777215));

        gridLayout->addWidget(redEdit, 0, 7, 1, 1);

        greenEdit = new QLineEdit(MyColorPickerClass);
        greenEdit->setObjectName(QString::fromUtf8("greenEdit"));
        greenEdit->setMaximumSize(QSize(50, 16777215));

        gridLayout->addWidget(greenEdit, 1, 7, 1, 1);

        blueEdit = new QLineEdit(MyColorPickerClass);
        blueEdit->setObjectName(QString::fromUtf8("blueEdit"));
        blueEdit->setMaximumSize(QSize(50, 16777215));

        gridLayout->addWidget(blueEdit, 2, 7, 1, 1);

        label = new QLabel(MyColorPickerClass);
        label->setObjectName(QString::fromUtf8("label"));
        label->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(label, 0, 6, 1, 1);

        label_2 = new QLabel(MyColorPickerClass);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(label_2, 1, 6, 1, 1);

        label_3 = new QLabel(MyColorPickerClass);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(label_3, 2, 6, 1, 1);

        viewCol = new QLabel(MyColorPickerClass);
        viewCol->setObjectName(QString::fromUtf8("viewCol"));
        viewCol->setMinimumSize(QSize(50, 0));
        viewCol->setFrameShape(QFrame::Box);
        viewCol->setFrameShadow(QFrame::Sunken);

        gridLayout->addWidget(viewCol, 0, 8, 3, 1);

        htmlColEdit = new QLineEdit(MyColorPickerClass);
        htmlColEdit->setObjectName(QString::fromUtf8("htmlColEdit"));

        gridLayout->addWidget(htmlColEdit, 1, 1, 1, 1);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer, 2, 5, 1, 1);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        oldBut = new QPushButton(MyColorPickerClass);
        oldBut->setObjectName(QString::fromUtf8("oldBut"));

        horizontalLayout->addWidget(oldBut);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);


        gridLayout->addLayout(horizontalLayout, 2, 1, 2, 1);

        proofCheck = new QCheckBox(MyColorPickerClass);
        proofCheck->setObjectName(QString::fromUtf8("proofCheck"));
        proofCheck->setChecked(true);

        gridLayout->addWidget(proofCheck, 0, 1, 1, 1);

        alphaEdit = new QLineEdit(MyColorPickerClass);
        alphaEdit->setObjectName(QString::fromUtf8("alphaEdit"));
        alphaEdit->setMaximumSize(QSize(50, 16777215));

        gridLayout->addWidget(alphaEdit, 3, 7, 1, 1);

        label_4 = new QLabel(MyColorPickerClass);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setAlignment(Qt::AlignCenter|Qt::AlignHCenter|Qt::AlignLeading|Qt::AlignLeft|Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(label_4, 3, 6, 1, 1);


        verticalLayout->addLayout(gridLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        invalidBot = new QPushButton(MyColorPickerClass);
        invalidBot->setObjectName(QString::fromUtf8("invalidBot"));

        horizontalLayout_2->addWidget(invalidBot);

        buttonBox = new QDialogButtonBox(MyColorPickerClass);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        horizontalLayout_2->addWidget(buttonBox);


        verticalLayout->addLayout(horizontalLayout_2);


        retranslateUi(MyColorPickerClass);

        QMetaObject::connectSlotsByName(MyColorPickerClass);
    } // setupUi

    void retranslateUi(QDialog *MyColorPickerClass)
    {
        MyColorPickerClass->setWindowTitle(QApplication::translate("MyColorPickerClass", "MyColorPicker", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("MyColorPickerClass", "Red:", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("MyColorPickerClass", "Green:", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("MyColorPickerClass", "Blue:", 0, QApplication::UnicodeUTF8));
        oldBut->setText(QApplication::translate("MyColorPickerClass", "Old Picker", 0, QApplication::UnicodeUTF8));
        proofCheck->setText(QApplication::translate("MyColorPickerClass", "Proof", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("MyColorPickerClass", "Alpha:", 0, QApplication::UnicodeUTF8));
        invalidBot->setText(QApplication::translate("MyColorPickerClass", "Set Invalid", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MyColorPickerClass: public Ui_MyColorPickerClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MYCOLORPICKER_H
