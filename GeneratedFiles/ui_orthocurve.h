/********************************************************************************
** Form generated from reading UI file 'orthocurve.ui'
**
** Created by: Qt User Interface Compiler version 5.6.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ORTHOCURVE_H
#define UI_ORTHOCURVE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_OrthoCurveClass
{
public:
    QAction *actionImportSTL;
    QAction *actionComputeCurve;
    QAction *actionSaveCurve;
    QAction *actionSetting;
    QAction *actionMarkIcon;
    QAction *actionDeleteCurrentCurve;
    QAction *actionappICON;
    QAction *action_3shape;
    QAction *action_pickAlignPoints;
    QAction *action_alignModel;
    QWidget *centralWidget;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *OrthoCurveClass)
    {
        if (OrthoCurveClass->objectName().isEmpty())
            OrthoCurveClass->setObjectName(QStringLiteral("OrthoCurveClass"));
        OrthoCurveClass->resize(617, 411);
        actionImportSTL = new QAction(OrthoCurveClass);
        actionImportSTL->setObjectName(QStringLiteral("actionImportSTL"));
        QIcon icon;
        icon.addFile(QStringLiteral("Resources/open.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon.addFile(QStringLiteral(":/OrthoCurve/Resources/open.png"), QSize(), QIcon::Normal, QIcon::On);
        actionImportSTL->setIcon(icon);
        actionComputeCurve = new QAction(OrthoCurveClass);
        actionComputeCurve->setObjectName(QStringLiteral("actionComputeCurve"));
        QIcon icon1;
        icon1.addFile(QStringLiteral("Resources/curve.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon1.addFile(QStringLiteral(":/OrthoCurve/Resources/curve.png"), QSize(), QIcon::Normal, QIcon::On);
        actionComputeCurve->setIcon(icon1);
        actionSaveCurve = new QAction(OrthoCurveClass);
        actionSaveCurve->setObjectName(QStringLiteral("actionSaveCurve"));
        QIcon icon2;
        icon2.addFile(QStringLiteral("Resources/save.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon2.addFile(QStringLiteral(":/OrthoCurve/Resources/save.png"), QSize(), QIcon::Normal, QIcon::On);
        actionSaveCurve->setIcon(icon2);
        actionSetting = new QAction(OrthoCurveClass);
        actionSetting->setObjectName(QStringLiteral("actionSetting"));
        QIcon icon3;
        icon3.addFile(QStringLiteral("Resources/setting.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon3.addFile(QStringLiteral(":/OrthoCurve/Resources/setting.png"), QSize(), QIcon::Normal, QIcon::On);
        actionSetting->setIcon(icon3);
        actionMarkIcon = new QAction(OrthoCurveClass);
        actionMarkIcon->setObjectName(QStringLiteral("actionMarkIcon"));
        QIcon icon4;
        icon4.addFile(QStringLiteral("Resources/mark.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon4.addFile(QStringLiteral(":/OrthoCurve/Resources/edit.png"), QSize(), QIcon::Normal, QIcon::On);
        actionMarkIcon->setIcon(icon4);
        actionDeleteCurrentCurve = new QAction(OrthoCurveClass);
        actionDeleteCurrentCurve->setObjectName(QStringLiteral("actionDeleteCurrentCurve"));
        QIcon icon5;
        icon5.addFile(QStringLiteral("Resources/remove.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon5.addFile(QStringLiteral(":/OrthoCurve/Resources/remove.png"), QSize(), QIcon::Normal, QIcon::On);
        actionDeleteCurrentCurve->setIcon(icon5);
        actionappICON = new QAction(OrthoCurveClass);
        actionappICON->setObjectName(QStringLiteral("actionappICON"));
        QIcon icon6;
        icon6.addFile(QStringLiteral(":/OrthoCurve/Resources/OrthoCurve.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionappICON->setIcon(icon6);
        action_3shape = new QAction(OrthoCurveClass);
        action_3shape->setObjectName(QStringLiteral("action_3shape"));
        QIcon icon7;
        icon7.addFile(QStringLiteral(":/OrthoCurve/Resources/3shape.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_3shape->setIcon(icon7);
        action_pickAlignPoints = new QAction(OrthoCurveClass);
        action_pickAlignPoints->setObjectName(QStringLiteral("action_pickAlignPoints"));
        QIcon icon8;
        icon8.addFile(QStringLiteral(":/OrthoCurve/Resources/pick align.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_pickAlignPoints->setIcon(icon8);
        action_alignModel = new QAction(OrthoCurveClass);
        action_alignModel->setObjectName(QStringLiteral("action_alignModel"));
        QIcon icon9;
        icon9.addFile(QStringLiteral(":/OrthoCurve/Resources/align model.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_alignModel->setIcon(icon9);
        centralWidget = new QWidget(OrthoCurveClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        verticalLayoutWidget = new QWidget(centralWidget);
        verticalLayoutWidget->setObjectName(QStringLiteral("verticalLayoutWidget"));
        verticalLayoutWidget->setGeometry(QRect(0, 0, 2, 2));
        verticalLayout = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        OrthoCurveClass->setCentralWidget(centralWidget);
        mainToolBar = new QToolBar(OrthoCurveClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        OrthoCurveClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(OrthoCurveClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        statusBar->setEnabled(true);
        OrthoCurveClass->setStatusBar(statusBar);

        mainToolBar->addAction(actionImportSTL);
        mainToolBar->addAction(action_3shape);
        mainToolBar->addSeparator();
        mainToolBar->addAction(actionSetting);
        mainToolBar->addAction(action_pickAlignPoints);
        mainToolBar->addAction(action_alignModel);
        mainToolBar->addAction(actionComputeCurve);
        mainToolBar->addSeparator();
        mainToolBar->addAction(actionDeleteCurrentCurve);
        mainToolBar->addAction(actionMarkIcon);
        mainToolBar->addSeparator();
        mainToolBar->addAction(actionSaveCurve);

        retranslateUi(OrthoCurveClass);

        QMetaObject::connectSlotsByName(OrthoCurveClass);
    } // setupUi

    void retranslateUi(QMainWindow *OrthoCurveClass)
    {
        OrthoCurveClass->setWindowTitle(QApplication::translate("OrthoCurveClass", "OrthoCurve", Q_NULLPTR));
        actionImportSTL->setText(QApplication::translate("OrthoCurveClass", "\346\211\223\345\274\200STL\346\250\241\345\236\213", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        actionImportSTL->setToolTip(QApplication::translate("OrthoCurveClass", "\346\211\223\345\274\200STL\346\250\241\345\236\213", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        actionComputeCurve->setText(QApplication::translate("OrthoCurveClass", "\350\256\241\347\256\227\350\275\250\350\277\271\347\272\277", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        actionComputeCurve->setToolTip(QApplication::translate("OrthoCurveClass", "\347\202\271\351\200\211\350\275\250\350\277\271\347\272\277", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        actionSaveCurve->setText(QApplication::translate("OrthoCurveClass", "\344\277\235\345\255\230\350\275\250\350\277\271\347\272\277", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        actionSaveCurve->setToolTip(QApplication::translate("OrthoCurveClass", "\344\277\235\345\255\230\350\275\250\350\277\271\347\272\277", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        actionSetting->setText(QApplication::translate("OrthoCurveClass", "\350\256\276\347\275\256", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        actionSetting->setToolTip(QApplication::translate("OrthoCurveClass", "\347\263\273\347\273\237\350\256\276\347\275\256", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        actionMarkIcon->setText(QApplication::translate("OrthoCurveClass", "\351\200\211\346\213\251\346\211\223\346\240\207\344\275\215\347\275\256", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        actionMarkIcon->setToolTip(QApplication::translate("OrthoCurveClass", "\351\200\211\346\213\251\346\211\223\346\240\207\344\275\215\347\275\256", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        actionDeleteCurrentCurve->setText(QApplication::translate("OrthoCurveClass", "\345\210\240\351\231\244\345\275\223\345\211\215\346\255\245\351\252\244\345\210\207\345\211\262\347\272\277", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        actionDeleteCurrentCurve->setToolTip(QApplication::translate("OrthoCurveClass", "\345\210\240\351\231\244\345\275\223\345\211\215\346\255\245\351\252\244\345\210\207\345\211\262\347\272\277", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        actionappICON->setText(QApplication::translate("OrthoCurveClass", "app icon", Q_NULLPTR));
        action_3shape->setText(QApplication::translate("OrthoCurveClass", "\346\211\223\345\274\2003S\346\225\260\346\215\256", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        action_3shape->setToolTip(QApplication::translate("OrthoCurveClass", "\346\211\223\345\274\2003S\346\225\260\346\215\256\357\274\214\345\205\210\345\257\274\345\205\245\344\270\212\351\242\214\357\274\214\347\204\266\345\220\216\350\207\252\345\212\250\345\257\274\345\205\245\344\270\213\351\242\214", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        action_pickAlignPoints->setText(QApplication::translate("OrthoCurveClass", "\347\202\271\351\200\211\346\221\206\346\255\243\347\202\271", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        action_pickAlignPoints->setToolTip(QApplication::translate("OrthoCurveClass", "\347\202\271\351\200\211\344\270\211\344\270\252\347\202\271\347\224\250\344\272\216\346\221\206\346\255\243", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        action_alignModel->setText(QApplication::translate("OrthoCurveClass", "\346\221\206\346\255\243\346\250\241\345\236\213", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        action_alignModel->setToolTip(QApplication::translate("OrthoCurveClass", "\346\221\206\346\255\243\346\250\241\345\236\213", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
    } // retranslateUi

};

namespace Ui {
    class OrthoCurveClass: public Ui_OrthoCurveClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ORTHOCURVE_H
