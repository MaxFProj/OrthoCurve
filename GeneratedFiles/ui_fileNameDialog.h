/********************************************************************************
** Form generated from reading UI file 'fileNameDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.6.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FILENAMEDIALOG_H
#define UI_FILENAMEDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_FileNameDialog
{
public:
    QDialogButtonBox *buttonBox;
    QTableWidget *tableWidget;
    QWidget *layoutWidget;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QLineEdit *lineEdit_patientID;
    QWidget *layoutWidget1;
    QHBoxLayout *horizontalLayout_2;
    QRadioButton *radioButton_max;
    QRadioButton *radioButton_man;

    void setupUi(QDialog *FileNameDialog)
    {
        if (FileNameDialog->objectName().isEmpty())
            FileNameDialog->setObjectName(QStringLiteral("FileNameDialog"));
        FileNameDialog->resize(531, 510);
        buttonBox = new QDialogButtonBox(FileNameDialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setGeometry(QRect(-160, 470, 341, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        tableWidget = new QTableWidget(FileNameDialog);
        if (tableWidget->columnCount() < 3)
            tableWidget->setColumnCount(3);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        tableWidget->setObjectName(QStringLiteral("tableWidget"));
        tableWidget->setGeometry(QRect(20, 60, 471, 400));
        tableWidget->horizontalHeader()->setDefaultSectionSize(150);
        layoutWidget = new QWidget(FileNameDialog);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        layoutWidget->setGeometry(QRect(30, 30, 251, 22));
        horizontalLayout = new QHBoxLayout(layoutWidget);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(layoutWidget);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout->addWidget(label);

        lineEdit_patientID = new QLineEdit(layoutWidget);
        lineEdit_patientID->setObjectName(QStringLiteral("lineEdit_patientID"));

        horizontalLayout->addWidget(lineEdit_patientID);

        layoutWidget1 = new QWidget(FileNameDialog);
        layoutWidget1->setObjectName(QStringLiteral("layoutWidget1"));
        layoutWidget1->setGeometry(QRect(290, 30, 186, 19));
        horizontalLayout_2 = new QHBoxLayout(layoutWidget1);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        radioButton_max = new QRadioButton(layoutWidget1);
        radioButton_max->setObjectName(QStringLiteral("radioButton_max"));
        radioButton_max->setChecked(true);

        horizontalLayout_2->addWidget(radioButton_max);

        radioButton_man = new QRadioButton(layoutWidget1);
        radioButton_man->setObjectName(QStringLiteral("radioButton_man"));

        horizontalLayout_2->addWidget(radioButton_man);


        retranslateUi(FileNameDialog);
        QObject::connect(buttonBox, SIGNAL(rejected()), FileNameDialog, SLOT(reject()));
        QObject::connect(buttonBox, SIGNAL(accepted()), FileNameDialog, SLOT(accept()));

        QMetaObject::connectSlotsByName(FileNameDialog);
    } // setupUi

    void retranslateUi(QDialog *FileNameDialog)
    {
        FileNameDialog->setWindowTitle(QApplication::translate("FileNameDialog", "Change Model Name", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem = tableWidget->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QApplication::translate("FileNameDialog", "Initial File Name", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem1 = tableWidget->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QApplication::translate("FileNameDialog", "Jaw", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem2 = tableWidget->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QApplication::translate("FileNameDialog", "Step", Q_NULLPTR));
        label->setText(QApplication::translate("FileNameDialog", "ID", Q_NULLPTR));
        radioButton_max->setText(QApplication::translate("FileNameDialog", "Maxillary ", Q_NULLPTR));
        radioButton_man->setText(QApplication::translate("FileNameDialog", "Mandibular", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class FileNameDialog: public Ui_FileNameDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FILENAMEDIALOG_H
