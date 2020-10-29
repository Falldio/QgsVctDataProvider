/********************************************************************************
** Form generated from reading UI file 'qgsvctsourceselectbase.ui'
**
** Created by: Qt User Interface Compiler version 5.11.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QGSVCTSOURCESELECTBASE_H
#define UI_QGSVCTSOURCESELECTBASE_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_QgsVctSourceSelectBase
{
public:
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *labelFileName;
    QLineEdit *lineEditFilePath;
    QToolButton *toolButtonFilePath;
    QHBoxLayout *horizontalLayout_4;
    QLabel *labelLayerName;
    QLineEdit *lineEditLayerName;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *QgsVctSourceSelectBase)
    {
        if (QgsVctSourceSelectBase->objectName().isEmpty())
            QgsVctSourceSelectBase->setObjectName(QStringLiteral("QgsVctSourceSelectBase"));
        QgsVctSourceSelectBase->resize(730, 643);
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(QgsVctSourceSelectBase->sizePolicy().hasHeightForWidth());
        QgsVctSourceSelectBase->setSizePolicy(sizePolicy);
        verticalLayoutWidget = new QWidget(QgsVctSourceSelectBase);
        verticalLayoutWidget->setObjectName(QStringLiteral("verticalLayoutWidget"));
        verticalLayoutWidget->setGeometry(QRect(10, 10, 711, 141));
        verticalLayout = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        labelFileName = new QLabel(verticalLayoutWidget);
        labelFileName->setObjectName(QStringLiteral("labelFileName"));

        horizontalLayout->addWidget(labelFileName);

        lineEditFilePath = new QLineEdit(verticalLayoutWidget);
        lineEditFilePath->setObjectName(QStringLiteral("lineEditFilePath"));

        horizontalLayout->addWidget(lineEditFilePath);

        toolButtonFilePath = new QToolButton(verticalLayoutWidget);
        toolButtonFilePath->setObjectName(QStringLiteral("toolButtonFilePath"));

        horizontalLayout->addWidget(toolButtonFilePath);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        labelLayerName = new QLabel(verticalLayoutWidget);
        labelLayerName->setObjectName(QStringLiteral("labelLayerName"));

        horizontalLayout_4->addWidget(labelLayerName);

        lineEditLayerName = new QLineEdit(verticalLayoutWidget);
        lineEditLayerName->setObjectName(QStringLiteral("lineEditLayerName"));

        horizontalLayout_4->addWidget(lineEditLayerName);


        verticalLayout->addLayout(horizontalLayout_4);

        buttonBox = new QDialogButtonBox(QgsVctSourceSelectBase);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setGeometry(QRect(12, 600, 711, 41));
        buttonBox->setStandardButtons(QDialogButtonBox::NoButton);
        buttonBox->setCenterButtons(false);

        retranslateUi(QgsVctSourceSelectBase);

        QMetaObject::connectSlotsByName(QgsVctSourceSelectBase);
    } // setupUi

    void retranslateUi(QDialog *QgsVctSourceSelectBase)
    {
        QgsVctSourceSelectBase->setWindowTitle(QApplication::translate("QgsVctSourceSelectBase", "Create a Layer from a VCT File", nullptr));
        labelFileName->setText(QApplication::translate("QgsVctSourceSelectBase", "File name", nullptr));
        toolButtonFilePath->setText(QApplication::translate("QgsVctSourceSelectBase", "...", nullptr));
        labelLayerName->setText(QApplication::translate("QgsVctSourceSelectBase", "Layer name", nullptr));
    } // retranslateUi

};

namespace Ui {
    class QgsVctSourceSelectBase: public Ui_QgsVctSourceSelectBase {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QGSVCTSOURCESELECTBASE_H
