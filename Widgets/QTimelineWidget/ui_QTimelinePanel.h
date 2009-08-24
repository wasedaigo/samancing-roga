/********************************************************************************
** Form generated from reading ui file 'QTimelinePanel.ui'
**
** Created: Wed Aug 19 11:50:55 2009
**      by: Qt User Interface Compiler version 4.5.2
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_QTIMELINEPANEL_H
#define UI_QTIMELINEPANEL_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_QTimelinePanel
{
public:
    QVBoxLayout *verticalLayout_2;
    QVBoxLayout *timelineWidgetContainer;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QLabel *currentFrameNo;
    QLabel *label_4;
    QLabel *maxFrameNo;
    QSpacerItem *horizontalSpacer_2;
    QSpacerItem *verticalSpacer;

    void setupUi(QWidget *QTimelinePanel)
    {
        if (QTimelinePanel->objectName().isEmpty())
            QTimelinePanel->setObjectName(QString::fromUtf8("QTimelinePanel"));
        QTimelinePanel->resize(317, 141);
        verticalLayout_2 = new QVBoxLayout(QTimelinePanel);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        timelineWidgetContainer = new QVBoxLayout();
        timelineWidgetContainer->setObjectName(QString::fromUtf8("timelineWidgetContainer"));

        verticalLayout_2->addLayout(timelineWidgetContainer);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label = new QLabel(QTimelinePanel);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout->addWidget(label);

        currentFrameNo = new QLabel(QTimelinePanel);
        currentFrameNo->setObjectName(QString::fromUtf8("currentFrameNo"));

        horizontalLayout->addWidget(currentFrameNo);

        label_4 = new QLabel(QTimelinePanel);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        horizontalLayout->addWidget(label_4);

        maxFrameNo = new QLabel(QTimelinePanel);
        maxFrameNo->setObjectName(QString::fromUtf8("maxFrameNo"));

        horizontalLayout->addWidget(maxFrameNo);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);


        verticalLayout_2->addLayout(horizontalLayout);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_2->addItem(verticalSpacer);


        retranslateUi(QTimelinePanel);

        QMetaObject::connectSlotsByName(QTimelinePanel);
    } // setupUi

    void retranslateUi(QWidget *QTimelinePanel)
    {
        QTimelinePanel->setWindowTitle(QApplication::translate("QTimelinePanel", "Form", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("QTimelinePanel", "FrameNo : ", 0, QApplication::UnicodeUTF8));
        currentFrameNo->setText(QApplication::translate("QTimelinePanel", "0", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("QTimelinePanel", "/", 0, QApplication::UnicodeUTF8));
        maxFrameNo->setText(QApplication::translate("QTimelinePanel", "100", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(QTimelinePanel);
    } // retranslateUi

};

namespace Ui {
    class QTimelinePanel: public Ui_QTimelinePanel {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QTIMELINEPANEL_H
