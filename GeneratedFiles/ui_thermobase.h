/********************************************************************************
** Form generated from reading UI file 'thermobase.ui'
**
** Created by: Qt User Interface Compiler version 5.6.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_THERMOBASE_H
#define UI_THERMOBASE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ThermoBaseDialog
{
public:
    QWidget *contentWidget;
    QWidget *layoutWidget;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_2;
    QDoubleSpinBox *doubleSpinBox_Y;
    QWidget *layoutWidget1;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label;
    QDoubleSpinBox *doubleSpinBox_X;
    QWidget *layoutWidget_2;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label_3;
    QDoubleSpinBox *doubleSpinBox_Z;
    QRadioButton *radioButton_left;
    QRadioButton *radioButton_right;
    QPushButton *pushButton_save;

    void setupUi(QDialog *ThermoBaseDialog)
    {
        if (ThermoBaseDialog->objectName().isEmpty())
            ThermoBaseDialog->setObjectName(QStringLiteral("ThermoBaseDialog"));
        ThermoBaseDialog->resize(838, 648);
        contentWidget = new QWidget(ThermoBaseDialog);
        contentWidget->setObjectName(QStringLiteral("contentWidget"));
        contentWidget->setGeometry(QRect(20, 20, 600, 600));
        contentWidget->setMinimumSize(QSize(600, 600));
        contentWidget->setMaximumSize(QSize(600, 600));
        contentWidget->setStyleSheet(QStringLiteral(""));
        layoutWidget = new QWidget(ThermoBaseDialog);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        layoutWidget->setGeometry(QRect(660, 90, 161, 22));
        horizontalLayout_3 = new QHBoxLayout(layoutWidget);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(0, 0, 0, 0);
        label_2 = new QLabel(layoutWidget);
        label_2->setObjectName(QStringLiteral("label_2"));

        horizontalLayout_3->addWidget(label_2);

        doubleSpinBox_Y = new QDoubleSpinBox(layoutWidget);
        doubleSpinBox_Y->setObjectName(QStringLiteral("doubleSpinBox_Y"));
        doubleSpinBox_Y->setMinimum(-99);

        horizontalLayout_3->addWidget(doubleSpinBox_Y);

        layoutWidget1 = new QWidget(ThermoBaseDialog);
        layoutWidget1->setObjectName(QStringLiteral("layoutWidget1"));
        layoutWidget1->setGeometry(QRect(660, 60, 161, 22));
        horizontalLayout_2 = new QHBoxLayout(layoutWidget1);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(layoutWidget1);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout_2->addWidget(label);

        doubleSpinBox_X = new QDoubleSpinBox(layoutWidget1);
        doubleSpinBox_X->setObjectName(QStringLiteral("doubleSpinBox_X"));
        doubleSpinBox_X->setMinimum(-99.99);

        horizontalLayout_2->addWidget(doubleSpinBox_X);

        layoutWidget_2 = new QWidget(ThermoBaseDialog);
        layoutWidget_2->setObjectName(QStringLiteral("layoutWidget_2"));
        layoutWidget_2->setGeometry(QRect(660, 120, 161, 22));
        horizontalLayout_5 = new QHBoxLayout(layoutWidget_2);
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        horizontalLayout_5->setContentsMargins(0, 0, 0, 0);
        label_3 = new QLabel(layoutWidget_2);
        label_3->setObjectName(QStringLiteral("label_3"));

        horizontalLayout_5->addWidget(label_3);

        doubleSpinBox_Z = new QDoubleSpinBox(layoutWidget_2);
        doubleSpinBox_Z->setObjectName(QStringLiteral("doubleSpinBox_Z"));
        doubleSpinBox_Z->setMinimum(-99.99);

        horizontalLayout_5->addWidget(doubleSpinBox_Z);

        radioButton_left = new QRadioButton(ThermoBaseDialog);
        radioButton_left->setObjectName(QStringLiteral("radioButton_left"));
        radioButton_left->setGeometry(QRect(660, 20, 107, 17));
        radioButton_left->setChecked(true);
        radioButton_right = new QRadioButton(ThermoBaseDialog);
        radioButton_right->setObjectName(QStringLiteral("radioButton_right"));
        radioButton_right->setGeometry(QRect(720, 20, 83, 17));
        pushButton_save = new QPushButton(ThermoBaseDialog);
        pushButton_save->setObjectName(QStringLiteral("pushButton_save"));
        pushButton_save->setGeometry(QRect(660, 160, 75, 23));

        retranslateUi(ThermoBaseDialog);

        QMetaObject::connectSlotsByName(ThermoBaseDialog);
    } // setupUi

    void retranslateUi(QDialog *ThermoBaseDialog)
    {
        ThermoBaseDialog->setWindowTitle(QApplication::translate("ThermoBaseDialog", "\345\217\214\347\211\231\346\250\241\345\216\213\350\206\234\346\221\206\346\224\276", Q_NULLPTR));
        label_2->setText(QApplication::translate("ThermoBaseDialog", "\346\262\277Y\350\275\264\345\271\263\347\247\273", Q_NULLPTR));
        label->setText(QApplication::translate("ThermoBaseDialog", "\346\262\277X\350\275\264\345\271\263\347\247\273", Q_NULLPTR));
        label_3->setText(QApplication::translate("ThermoBaseDialog", "\347\273\225Z\350\275\264\346\227\213\350\275\254", Q_NULLPTR));
        radioButton_left->setText(QApplication::translate("ThermoBaseDialog", "\345\267\246\346\250\241\345\236\213", Q_NULLPTR));
        radioButton_right->setText(QApplication::translate("ThermoBaseDialog", "\345\217\263\346\250\241\345\236\213", Q_NULLPTR));
        pushButton_save->setText(QApplication::translate("ThermoBaseDialog", "\344\277\235\345\255\230\345\217\230\346\215\242", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class ThermoBaseDialog: public Ui_ThermoBaseDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_THERMOBASE_H
