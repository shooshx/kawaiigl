/********************************************************************************
** Form generated from reading ui file 'KawaiiGL.ui'
**
** Created: Mon Oct 12 16:34:01 2009
**      by: Qt User Interface Compiler version 4.5.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_KAWAIIGL_H
#define UI_KAWAIIGL_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_KawaiiGLClass
{
public:
    QWidget *centralWidget;
    QMenuBar *menuBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *KawaiiGLClass)
    {
        if (KawaiiGLClass->objectName().isEmpty())
            KawaiiGLClass->setObjectName(QString::fromUtf8("KawaiiGLClass"));
        KawaiiGLClass->resize(779, 514);
        centralWidget = new QWidget(KawaiiGLClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        KawaiiGLClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(KawaiiGLClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 779, 22));
        KawaiiGLClass->setMenuBar(menuBar);
        statusBar = new QStatusBar(KawaiiGLClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        KawaiiGLClass->setStatusBar(statusBar);

        retranslateUi(KawaiiGLClass);

        QMetaObject::connectSlotsByName(KawaiiGLClass);
    } // setupUi

    void retranslateUi(QMainWindow *KawaiiGLClass)
    {
        KawaiiGLClass->setWindowTitle(QApplication::translate("KawaiiGLClass", "KawaiiGL", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(KawaiiGLClass);
    } // retranslateUi

};

namespace Ui {
    class KawaiiGLClass: public Ui_KawaiiGLClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_KAWAIIGL_H
