/********************************************************************************
** Form generated from reading UI file 'DialogButtonBottom.ui'
**
** Created by: Qt User Interface Compiler version 5.6.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIALOGBUTTONBOTTOM_H
#define UI_DIALOGBUTTONBOTTOM_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Dialog
{
public:
    QWidget *layoutWidget;
    QHBoxLayout *hboxLayout;
    QSpacerItem *spacerItem;
    QPushButton *okButton;
    QPushButton *cancelButton;
    QCheckBox *checkBoxMark;
    QWidget *layoutWidget1;
    QHBoxLayout *horizontalLayout;
    QLabel *label_5;
    QLabel *label_version;
    QCheckBox *checkBox_showBur;
    QRadioButton *radioButton_auto;
    QRadioButton *radioButton_manual;
    QSpinBox *smoothBox;
    QLabel *label;
    QLabel *label_2;
    QSpinBox *OffsetStepBox;
    QDoubleSpinBox *intevalBox;
    QLabel *label_3;
    QLabel *label_4;
    QSpinBox *zangleBox;
    QLabel *label_6;
    QSpinBox *zthresholdBox;
    QWidget *layoutWidget2;
    QGridLayout *gridLayout;
    QDoubleSpinBox *doubleSpinBox_cutlineInterval;
    QSpinBox *spinBox_curveture;
    QLabel *label_11;
    QLabel *label_12;
    QDoubleSpinBox *doubleSpinBox_cutOffset;
    QLabel *label_13;
    QSpinBox *spinBox_cutDegreeInsideBack;
    QSpinBox *spinBox_cutDegreeOut;
    QLabel *label_9;
    QSpinBox *spinBox_fbDegree;
    QLabel *label_7;
    QSpinBox *spinBox_cutDegreeInsideFront;
    QLabel *label_8;
    QLabel *label_10;
    QGroupBox *groupBox;
    QRadioButton *radioButton_Robotic;
    QRadioButton *radioButton_5or4Axis;
    QRadioButton *radioButton_5or4Axis_2;
    QRadioButton *radioButton_5or4Axis_3;
    QRadioButton *radioButton_5or4Axis_4;
    QGroupBox *groupBox_2;
    QRadioButton *radioButton_QR;
    QRadioButton *radioButton_Info;
    QGroupBox *groupBox_3;
    QWidget *layoutWidget3;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_14;
    QComboBox *comboBox_correction;
    QWidget *layoutWidget4;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_15;
    QDoubleSpinBox *doubleSpinBox_qrwidth;
    QSplitter *splitter;
    QRadioButton *withoutButton;
    QRadioButton *withNormalButton;

    void setupUi(QDialog *Dialog)
    {
        if (Dialog->objectName().isEmpty())
            Dialog->setObjectName(QStringLiteral("Dialog"));
        Dialog->resize(400, 581);
        Dialog->setMaximumSize(QSize(800, 1000));
        layoutWidget = new QWidget(Dialog);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        layoutWidget->setGeometry(QRect(20, 530, 221, 33));
        hboxLayout = new QHBoxLayout(layoutWidget);
        hboxLayout->setSpacing(6);
        hboxLayout->setObjectName(QStringLiteral("hboxLayout"));
        hboxLayout->setContentsMargins(0, 0, 0, 0);
        spacerItem = new QSpacerItem(131, 31, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout->addItem(spacerItem);

        okButton = new QPushButton(layoutWidget);
        okButton->setObjectName(QStringLiteral("okButton"));

        hboxLayout->addWidget(okButton);

        cancelButton = new QPushButton(layoutWidget);
        cancelButton->setObjectName(QStringLiteral("cancelButton"));

        hboxLayout->addWidget(cancelButton);

        checkBoxMark = new QCheckBox(Dialog);
        checkBoxMark->setObjectName(QStringLiteral("checkBoxMark"));
        checkBoxMark->setGeometry(QRect(20, 410, 191, 31));
        layoutWidget1 = new QWidget(Dialog);
        layoutWidget1->setObjectName(QStringLiteral("layoutWidget1"));
        layoutWidget1->setGeometry(QRect(280, 10, 116, 23));
        horizontalLayout = new QHBoxLayout(layoutWidget1);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        label_5 = new QLabel(layoutWidget1);
        label_5->setObjectName(QStringLiteral("label_5"));

        horizontalLayout->addWidget(label_5);

        label_version = new QLabel(layoutWidget1);
        label_version->setObjectName(QStringLiteral("label_version"));
        label_version->setMaximumSize(QSize(10000, 10000));
        QFont font;
        font.setPointSize(9);
        label_version->setFont(font);

        horizontalLayout->addWidget(label_version);

        checkBox_showBur = new QCheckBox(Dialog);
        checkBox_showBur->setObjectName(QStringLiteral("checkBox_showBur"));
        checkBox_showBur->setGeometry(QRect(20, 490, 181, 30));
        radioButton_auto = new QRadioButton(Dialog);
        radioButton_auto->setObjectName(QStringLiteral("radioButton_auto"));
        radioButton_auto->setGeometry(QRect(448, 180, 83, 17));
        radioButton_auto->setCheckable(false);
        radioButton_auto->setChecked(false);
        radioButton_manual = new QRadioButton(Dialog);
        radioButton_manual->setObjectName(QStringLiteral("radioButton_manual"));
        radioButton_manual->setGeometry(QRect(448, 160, 71, 17));
        radioButton_manual->setCheckable(true);
        radioButton_manual->setChecked(true);
        smoothBox = new QSpinBox(Dialog);
        smoothBox->setObjectName(QStringLiteral("smoothBox"));
        smoothBox->setGeometry(QRect(552, 76, 40, 20));
        label = new QLabel(Dialog);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(449, 22, 72, 16));
        label_2 = new QLabel(Dialog);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(449, 49, 72, 16));
        OffsetStepBox = new QSpinBox(Dialog);
        OffsetStepBox->setObjectName(QStringLiteral("OffsetStepBox"));
        OffsetStepBox->setGeometry(QRect(552, 22, 40, 20));
        OffsetStepBox->setMinimum(1);
        OffsetStepBox->setMaximum(50);
        intevalBox = new QDoubleSpinBox(Dialog);
        intevalBox->setObjectName(QStringLiteral("intevalBox"));
        intevalBox->setGeometry(QRect(552, 49, 52, 20));
        intevalBox->setMinimum(0.1);
        intevalBox->setMaximum(5);
        intevalBox->setSingleStep(0.2);
        label_3 = new QLabel(Dialog);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(449, 76, 72, 16));
        label_4 = new QLabel(Dialog);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(449, 103, 90, 16));
        zangleBox = new QSpinBox(Dialog);
        zangleBox->setObjectName(QStringLiteral("zangleBox"));
        zangleBox->setGeometry(QRect(552, 103, 40, 20));
        zangleBox->setMaximum(90);
        label_6 = new QLabel(Dialog);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setGeometry(QRect(449, 130, 84, 16));
        zthresholdBox = new QSpinBox(Dialog);
        zthresholdBox->setObjectName(QStringLiteral("zthresholdBox"));
        zthresholdBox->setGeometry(QRect(552, 130, 46, 20));
        zthresholdBox->setMinimum(-20);
        zthresholdBox->setMaximum(40);
        layoutWidget2 = new QWidget(Dialog);
        layoutWidget2->setObjectName(QStringLiteral("layoutWidget2"));
        layoutWidget2->setGeometry(QRect(20, 33, 361, 111));
        gridLayout = new QGridLayout(layoutWidget2);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        doubleSpinBox_cutlineInterval = new QDoubleSpinBox(layoutWidget2);
        doubleSpinBox_cutlineInterval->setObjectName(QStringLiteral("doubleSpinBox_cutlineInterval"));
        doubleSpinBox_cutlineInterval->setEnabled(true);
        doubleSpinBox_cutlineInterval->setMinimum(0);
        doubleSpinBox_cutlineInterval->setMaximum(5);
        doubleSpinBox_cutlineInterval->setSingleStep(0.05);
        doubleSpinBox_cutlineInterval->setValue(0.05);

        gridLayout->addWidget(doubleSpinBox_cutlineInterval, 1, 1, 1, 1);

        spinBox_curveture = new QSpinBox(layoutWidget2);
        spinBox_curveture->setObjectName(QStringLiteral("spinBox_curveture"));
        spinBox_curveture->setEnabled(true);
        spinBox_curveture->setMaximum(4);
        spinBox_curveture->setValue(2);

        gridLayout->addWidget(spinBox_curveture, 2, 1, 1, 1);

        label_11 = new QLabel(layoutWidget2);
        label_11->setObjectName(QStringLiteral("label_11"));

        gridLayout->addWidget(label_11, 0, 0, 1, 1);

        label_12 = new QLabel(layoutWidget2);
        label_12->setObjectName(QStringLiteral("label_12"));
        label_12->setEnabled(true);

        gridLayout->addWidget(label_12, 1, 0, 1, 1);

        doubleSpinBox_cutOffset = new QDoubleSpinBox(layoutWidget2);
        doubleSpinBox_cutOffset->setObjectName(QStringLiteral("doubleSpinBox_cutOffset"));
        doubleSpinBox_cutOffset->setMaximum(2);
        doubleSpinBox_cutOffset->setSingleStep(0.1);
        doubleSpinBox_cutOffset->setValue(0.5);

        gridLayout->addWidget(doubleSpinBox_cutOffset, 0, 1, 1, 1);

        label_13 = new QLabel(layoutWidget2);
        label_13->setObjectName(QStringLiteral("label_13"));

        gridLayout->addWidget(label_13, 2, 0, 1, 1);

        spinBox_cutDegreeInsideBack = new QSpinBox(Dialog);
        spinBox_cutDegreeInsideBack->setObjectName(QStringLiteral("spinBox_cutDegreeInsideBack"));
        spinBox_cutDegreeInsideBack->setGeometry(QRect(588, 300, 81, 20));
        spinBox_cutDegreeInsideBack->setMaximum(90);
        spinBox_cutDegreeInsideBack->setValue(45);
        spinBox_cutDegreeOut = new QSpinBox(Dialog);
        spinBox_cutDegreeOut->setObjectName(QStringLiteral("spinBox_cutDegreeOut"));
        spinBox_cutDegreeOut->setGeometry(QRect(588, 244, 81, 20));
        spinBox_cutDegreeOut->setMaximum(90);
        spinBox_cutDegreeOut->setValue(45);
        label_9 = new QLabel(Dialog);
        label_9->setObjectName(QStringLiteral("label_9"));
        label_9->setGeometry(QRect(450, 272, 132, 20));
        spinBox_fbDegree = new QSpinBox(Dialog);
        spinBox_fbDegree->setObjectName(QStringLiteral("spinBox_fbDegree"));
        spinBox_fbDegree->setGeometry(QRect(588, 216, 81, 20));
        spinBox_fbDegree->setMaximum(180);
        spinBox_fbDegree->setValue(45);
        label_7 = new QLabel(Dialog);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setGeometry(QRect(450, 216, 132, 20));
        spinBox_cutDegreeInsideFront = new QSpinBox(Dialog);
        spinBox_cutDegreeInsideFront->setObjectName(QStringLiteral("spinBox_cutDegreeInsideFront"));
        spinBox_cutDegreeInsideFront->setGeometry(QRect(588, 272, 81, 20));
        spinBox_cutDegreeInsideFront->setMaximum(90);
        spinBox_cutDegreeInsideFront->setValue(45);
        label_8 = new QLabel(Dialog);
        label_8->setObjectName(QStringLiteral("label_8"));
        label_8->setGeometry(QRect(450, 244, 132, 20));
        label_10 = new QLabel(Dialog);
        label_10->setObjectName(QStringLiteral("label_10"));
        label_10->setGeometry(QRect(450, 300, 132, 20));
        groupBox = new QGroupBox(Dialog);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setGeometry(QRect(20, 270, 351, 131));
        groupBox->setMaximumSize(QSize(500, 500));
        radioButton_Robotic = new QRadioButton(groupBox);
        radioButton_Robotic->setObjectName(QStringLiteral("radioButton_Robotic"));
        radioButton_Robotic->setGeometry(QRect(10, 22, 151, 31));
        radioButton_Robotic->setCheckable(true);
        radioButton_Robotic->setChecked(false);
        radioButton_Robotic->setAutoExclusive(true);
        radioButton_5or4Axis = new QRadioButton(groupBox);
        radioButton_5or4Axis->setObjectName(QStringLiteral("radioButton_5or4Axis"));
        radioButton_5or4Axis->setGeometry(QRect(10, 60, 151, 21));
        radioButton_5or4Axis->setCheckable(true);
        radioButton_5or4Axis->setChecked(false);
        radioButton_5or4Axis->setAutoExclusive(true);
        radioButton_5or4Axis_2 = new QRadioButton(groupBox);
        radioButton_5or4Axis_2->setObjectName(QStringLiteral("radioButton_5or4Axis_2"));
        radioButton_5or4Axis_2->setGeometry(QRect(180, 22, 171, 31));
        radioButton_5or4Axis_2->setCheckable(true);
        radioButton_5or4Axis_2->setChecked(false);
        radioButton_5or4Axis_2->setAutoExclusive(true);
        radioButton_5or4Axis_3 = new QRadioButton(groupBox);
        radioButton_5or4Axis_3->setObjectName(QStringLiteral("radioButton_5or4Axis_3"));
        radioButton_5or4Axis_3->setGeometry(QRect(180, 60, 161, 21));
        radioButton_5or4Axis_3->setChecked(false);
        radioButton_5or4Axis_3->setAutoExclusive(true);
        radioButton_5or4Axis_4 = new QRadioButton(groupBox);
        radioButton_5or4Axis_4->setObjectName(QStringLiteral("radioButton_5or4Axis_4"));
        radioButton_5or4Axis_4->setGeometry(QRect(10, 100, 161, 20));
        radioButton_5or4Axis_4->setChecked(true);
        radioButton_5or4Axis_4->setAutoExclusive(true);
        groupBox_2 = new QGroupBox(Dialog);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        groupBox_2->setGeometry(QRect(20, 160, 161, 101));
        radioButton_QR = new QRadioButton(groupBox_2);
        radioButton_QR->setObjectName(QStringLiteral("radioButton_QR"));
        radioButton_QR->setGeometry(QRect(10, 25, 141, 21));
        radioButton_QR->setChecked(true);
        radioButton_Info = new QRadioButton(groupBox_2);
        radioButton_Info->setObjectName(QStringLiteral("radioButton_Info"));
        radioButton_Info->setGeometry(QRect(10, 60, 141, 21));
        radioButton_Info->setChecked(false);
        groupBox_3 = new QGroupBox(Dialog);
        groupBox_3->setObjectName(QStringLiteral("groupBox_3"));
        groupBox_3->setGeometry(QRect(190, 160, 181, 101));
        layoutWidget3 = new QWidget(groupBox_3);
        layoutWidget3->setObjectName(QStringLiteral("layoutWidget3"));
        layoutWidget3->setGeometry(QRect(10, 22, 161, 22));
        horizontalLayout_2 = new QHBoxLayout(layoutWidget3);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        label_14 = new QLabel(layoutWidget3);
        label_14->setObjectName(QStringLiteral("label_14"));

        horizontalLayout_2->addWidget(label_14);

        comboBox_correction = new QComboBox(layoutWidget3);
        comboBox_correction->setObjectName(QStringLiteral("comboBox_correction"));

        horizontalLayout_2->addWidget(comboBox_correction);

        layoutWidget4 = new QWidget(groupBox_3);
        layoutWidget4->setObjectName(QStringLiteral("layoutWidget4"));
        layoutWidget4->setGeometry(QRect(10, 60, 161, 22));
        horizontalLayout_3 = new QHBoxLayout(layoutWidget4);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(0, 0, 0, 0);
        label_15 = new QLabel(layoutWidget4);
        label_15->setObjectName(QStringLiteral("label_15"));

        horizontalLayout_3->addWidget(label_15);

        doubleSpinBox_qrwidth = new QDoubleSpinBox(layoutWidget4);
        doubleSpinBox_qrwidth->setObjectName(QStringLiteral("doubleSpinBox_qrwidth"));
        doubleSpinBox_qrwidth->setMinimum(5);
        doubleSpinBox_qrwidth->setMaximum(30);
        doubleSpinBox_qrwidth->setValue(15);

        horizontalLayout_3->addWidget(doubleSpinBox_qrwidth);

        splitter = new QSplitter(Dialog);
        splitter->setObjectName(QStringLiteral("splitter"));
        splitter->setGeometry(QRect(20, 450, 351, 31));
        splitter->setOrientation(Qt::Horizontal);
        withoutButton = new QRadioButton(splitter);
        withoutButton->setObjectName(QStringLiteral("withoutButton"));
        splitter->addWidget(withoutButton);
        withNormalButton = new QRadioButton(splitter);
        withNormalButton->setObjectName(QStringLiteral("withNormalButton"));
        withNormalButton->setChecked(true);
        splitter->addWidget(withNormalButton);

        retranslateUi(Dialog);
        QObject::connect(okButton, SIGNAL(clicked()), Dialog, SLOT(accept()));
        QObject::connect(cancelButton, SIGNAL(clicked()), Dialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(Dialog);
    } // setupUi

    void retranslateUi(QDialog *Dialog)
    {
        Dialog->setWindowTitle(QApplication::translate("Dialog", "\350\256\276\347\275\256", Q_NULLPTR));
        okButton->setText(QApplication::translate("Dialog", "OK", Q_NULLPTR));
        cancelButton->setText(QApplication::translate("Dialog", "Cancel", Q_NULLPTR));
        checkBoxMark->setText(QApplication::translate("Dialog", "\344\277\235\345\255\230\346\277\200\345\205\211\346\211\223\346\240\207\344\275\215\347\275\256", Q_NULLPTR));
        label_5->setText(QApplication::translate("Dialog", "\347\211\210\346\234\254\357\274\232", Q_NULLPTR));
        label_version->setText(QApplication::translate("Dialog", "V3.1", Q_NULLPTR));
        checkBox_showBur->setText(QApplication::translate("Dialog", "\346\230\276\347\244\272\346\250\241\346\213\237\345\210\200\345\244\264", Q_NULLPTR));
        radioButton_auto->setText(QApplication::translate("Dialog", "\350\207\252\345\212\250\347\224\237\346\210\220\347\202\271", Q_NULLPTR));
        radioButton_manual->setText(QApplication::translate("Dialog", "\346\211\213\345\212\250\347\273\230\347\202\271", Q_NULLPTR));
        label->setText(QApplication::translate("Dialog", "\351\276\210\347\274\230\347\272\277\350\267\235\347\246\273\357\274\232", Q_NULLPTR));
        label_2->setText(QApplication::translate("Dialog", "\351\207\207\346\240\267\347\202\271\351\227\264\350\267\235\357\274\232", Q_NULLPTR));
        label_3->setText(QApplication::translate("Dialog", "\346\233\262\347\272\277\345\271\263\346\273\221\345\272\246\357\274\232", Q_NULLPTR));
        label_4->setText(QApplication::translate("Dialog", "\346\263\225\345\220\221\344\270\216Z\350\275\264\345\244\271\350\247\222\357\274\232", Q_NULLPTR));
        label_6->setText(QApplication::translate("Dialog", "\345\272\225\345\272\247\350\277\207\346\273\244\351\253\230\345\272\246\357\274\232", Q_NULLPTR));
        label_11->setText(QApplication::translate("Dialog", "\346\230\276\347\244\272\345\210\200\345\205\267\345\215\212\345\276\204", Q_NULLPTR));
        label_12->setText(QApplication::translate("Dialog", "\345\210\207\345\211\262\347\202\271\351\227\264\350\267\235", Q_NULLPTR));
        label_13->setText(QApplication::translate("Dialog", "\345\210\207\345\211\262\347\272\277\345\274\247\345\272\246\357\274\210\350\266\212\345\244\247\350\266\212\346\267\261\357\274\211", Q_NULLPTR));
        label_9->setText(QApplication::translate("Dialog", "\350\210\214\344\276\247\344\270\255\351\227\264\345\210\207\345\211\262\350\247\222\345\272\246", Q_NULLPTR));
        label_7->setText(QApplication::translate("Dialog", "\345\211\215\345\220\216\347\211\231\345\214\272\345\210\206\350\247\222\345\272\246", Q_NULLPTR));
        label_8->setText(QApplication::translate("Dialog", "\345\256\214\346\225\264\345\224\207\344\276\247\345\210\207\345\211\262\350\247\222\345\272\246", Q_NULLPTR));
        label_10->setText(QApplication::translate("Dialog", "\350\210\214\344\276\247\345\217\214\344\276\247\345\210\207\345\211\262\350\247\222\345\272\246", Q_NULLPTR));
        groupBox->setTitle(QApplication::translate("Dialog", "\345\272\225\347\233\230\351\200\211\346\213\251", Q_NULLPTR));
        radioButton_Robotic->setText(QApplication::translate("Dialog", "\346\234\272\345\231\250\344\272\272\345\272\225\347\233\230", Q_NULLPTR));
        radioButton_5or4Axis->setText(QApplication::translate("Dialog", "V1\344\270\211\351\224\256\345\256\232\344\275\215\351\224\256 ", Q_NULLPTR));
        radioButton_5or4Axis_2->setText(QApplication::translate("Dialog", "V2\344\270\211\351\224\256\344\275\216\345\256\232\344\275\215\351\224\256", Q_NULLPTR));
        radioButton_5or4Axis_3->setText(QApplication::translate("Dialog", "V3\345\234\206\345\255\224\345\256\232\344\275\215\351\224\256", Q_NULLPTR));
        radioButton_5or4Axis_4->setText(QApplication::translate("Dialog", "V4\345\234\206\345\255\224\345\256\232\344\275\215\351\224\256", Q_NULLPTR));
        groupBox_2->setTitle(QApplication::translate("Dialog", "\344\277\241\346\201\257\346\230\276\347\244\272\346\226\271\345\274\217", Q_NULLPTR));
        radioButton_QR->setText(QApplication::translate("Dialog", "\344\272\214\347\273\264\347\240\201\350\257\206\345\210\253", Q_NULLPTR));
        radioButton_Info->setText(QApplication::translate("Dialog", "OCR\345\255\227\347\254\246\350\257\206\345\210\253", Q_NULLPTR));
        groupBox_3->setTitle(QApplication::translate("Dialog", "\344\272\214\347\273\264\347\240\201\345\217\202\346\225\260", Q_NULLPTR));
        label_14->setText(QApplication::translate("Dialog", "\345\256\271\351\224\231\347\216\207", Q_NULLPTR));
        comboBox_correction->clear();
        comboBox_correction->insertItems(0, QStringList()
         << QApplication::translate("Dialog", "L-7%", Q_NULLPTR)
         << QApplication::translate("Dialog", "M-15%", Q_NULLPTR)
         << QApplication::translate("Dialog", "Q-25%", Q_NULLPTR)
         << QApplication::translate("Dialog", "H-30%", Q_NULLPTR)
        );
        label_15->setText(QApplication::translate("Dialog", "\345\256\275\345\272\246", Q_NULLPTR));
        withoutButton->setText(QApplication::translate("Dialog", "\344\277\235\345\255\230\344\270\211\345\235\220\346\240\207", Q_NULLPTR));
        withNormalButton->setText(QApplication::translate("Dialog", "\344\277\235\345\255\230\345\205\255\345\235\220\346\240\207", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class Dialog: public Ui_Dialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOGBUTTONBOTTOM_H
