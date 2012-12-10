/********************************************************************************
** Form generated from reading UI file 'splash.ui'
**
** Created: Sun Dec 9 21:31:18 2012
**      by: Qt User Interface Compiler version 4.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SPLASH_H
#define UI_SPLASH_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QFormLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_splash
{
public:
	QWidget *warningLayoutWidget;
	QVBoxLayout *warningLayout;
	QLabel *alpha_warning;
    QWidget *horizontalLayoutWidget;
    QHBoxLayout *horizontalLayout;
    QPushButton *btn_open;
    QPushButton *btn_cancel;
    QPushButton *btn_new;
    QComboBox *combo_project;
    QWidget *formLayoutWidget;
    QFormLayout *formLayout;
    QLineEdit *project_title;
    QLabel *label;
    QLabel *label_2;
    QLineEdit *main_file;

    void setupUi(QDialog *splash)
    {
        if (splash->objectName().isEmpty())
            splash->setObjectName(QString::fromUtf8("splash"));
        splash->setWindowModality(Qt::WindowModal);
        splash->resize(372, 218);

		warningLayoutWidget = new QWidget(splash);
		warningLayoutWidget->setObjectName(QString::fromUtf8("warningLayoutWidget"));
		warningLayoutWidget->setGeometry(QRect(10, 10, 351, 31));
		warningLayout = new QVBoxLayout(warningLayoutWidget);
		warningLayout->setObjectName(QString::fromUtf8("warningLayout"));
		warningLayout->setContentsMargins(0, 0, 0, 0);

		alpha_warning = new QLabel(warningLayoutWidget);
		alpha_warning->setObjectName(QString::fromUtf8("alpha_warning"));
		alpha_warning->setStyleSheet("QLabel { font-weight: bold; color: #FF0000; }");

		warningLayout->addWidget(alpha_warning);

        horizontalLayoutWidget = new QWidget(splash);
        horizontalLayoutWidget->setObjectName(QString::fromUtf8("horizontalLayoutWidget"));
        horizontalLayoutWidget->setGeometry(QRect(10, 170, 351, 31));
        horizontalLayout = new QHBoxLayout(horizontalLayoutWidget);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        btn_open = new QPushButton(horizontalLayoutWidget);
        btn_open->setObjectName(QString::fromUtf8("btn_open"));

        horizontalLayout->addWidget(btn_open);

        btn_cancel = new QPushButton(horizontalLayoutWidget);
        btn_cancel->setObjectName(QString::fromUtf8("btn_cancel"));

        horizontalLayout->addWidget(btn_cancel);

        btn_new = new QPushButton(horizontalLayoutWidget);
        btn_new->setObjectName(QString::fromUtf8("btn_new"));

        horizontalLayout->addWidget(btn_new);

        combo_project = new QComboBox(splash);
        combo_project->setObjectName(QString::fromUtf8("combo_project"));
        combo_project->setGeometry(QRect(10, 50, 351, 27));
        formLayoutWidget = new QWidget(splash);
        formLayoutWidget->setObjectName(QString::fromUtf8("formLayoutWidget"));
        formLayoutWidget->setGeometry(QRect(10, 90, 351, 71));
        formLayout = new QFormLayout(formLayoutWidget);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        formLayout->setContentsMargins(0, 0, 0, 0);
        project_title = new QLineEdit(formLayoutWidget);
        project_title->setObjectName(QString::fromUtf8("project_title"));

        formLayout->setWidget(0, QFormLayout::FieldRole, project_title);

        label = new QLabel(formLayoutWidget);
        label->setObjectName(QString::fromUtf8("label"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label);

        label_2 = new QLabel(formLayoutWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label_2);

        main_file = new QLineEdit(formLayoutWidget);
        main_file->setObjectName(QString::fromUtf8("main_file"));

        formLayout->setWidget(1, QFormLayout::FieldRole, main_file);


        retranslateUi(splash);

        QMetaObject::connectSlotsByName(splash);
    } // setupUi

    void retranslateUi(QDialog *splash)
    {
        splash->setWindowTitle(QApplication::translate("splash", "DTIDE", 0, QApplication::UnicodeUTF8));
        btn_open->setText(QApplication::translate("splash", "Open...", 0, QApplication::UnicodeUTF8));
        btn_cancel->setText(QApplication::translate("splash", "Cancel", 0, QApplication::UnicodeUTF8));
        btn_new->setText(QApplication::translate("splash", "New", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("splash", "Project title", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("splash", "Main file name", 0, QApplication::UnicodeUTF8));
		alpha_warning->setText(QApplication::translate("splash", "Warning: This software is in alpha. Only use for testing.", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class splash: public Ui_splash {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SPLASH_H
