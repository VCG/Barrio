/********************************************************************************
** Form generated from reading UI file 'inputform.ui'
**
** Created by: Qt User Interface Compiler version 5.14.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_INPUTFORM_H
#define UI_INPUTFORM_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_InputForm
{
public:
    QVBoxLayout *verticalLayout_4;
    QGroupBox *groupBox_5;
    QGridLayout *gridLayout_5;
    QCheckBox *checkBox_astrocyte_extrafiles;
    QCheckBox *checkBox_neurites_extrafiles;
    QFrame *frame_10;
    QGridLayout *gridLayout_2;
    QRadioButton *radioButton_load_astrocyte;
    QRadioButton *radioButton_load_neurites;
    QRadioButton *radioButton_load_all;
    QFrame *frame_11;
    QGridLayout *gridLayout_3;
    QLabel *label_2;
    QSpinBox *spinBox_limit;
    QPushButton *pushButton_options_help;
    QFrame *frame_9;
    QGridLayout *gridLayout;
    QGroupBox *groupBox_4;
    QVBoxLayout *verticalLayout_2;
    QFrame *frame_7;
    QHBoxLayout *horizontalLayout_7;
    QLineEdit *lineEdit_extra_files_path;
    QPushButton *pushButton_extra_files_path;
    QPushButton *pushButton_extra_files_path_help;
    QGroupBox *groupBox_3;
    QHBoxLayout *horizontalLayout_10;
    QFrame *frame;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QLineEdit *lineEdit_metadata;
    QPushButton *pushButton_metadata;
    QPushButton *pushButton_metadata_help;
    QGroupBox *groupBox_2;
    QVBoxLayout *verticalLayout_3;
    QFrame *frame_5;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label_5;
    QLineEdit *lineEdit_xml_astrocyte;
    QPushButton *pushButton_xml_astrocyte;
    QFrame *frame_6;
    QHBoxLayout *horizontalLayout_6;
    QLabel *label_6;
    QLineEdit *lineEdit_xml_neurites;
    QPushButton *pushButton_xml_neurites;
    QCheckBox *checkBox_lightXML;
    QPushButton *pushButton_xml_help;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_4;
    QFrame *frame_8;
    QHBoxLayout *horizontalLayout_8;
    QLabel *label_proximity_astrocyte;
    QLineEdit *lineEdit_proximity_astrocyte;
    QPushButton *pushButton_proximity_astrocyte;
    QPushButton *pushButton_proximity_astrocyte_help;
    QFrame *frame_4;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_proximity_astro_mito;
    QLineEdit *lineEdit_proximity_astro_mito;
    QPushButton *pushButton_proximity_astro_mito;
    QPushButton *pushButton_proximity_astro_mito_help;
    QFrame *frame_3;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_proximity_neu_mito;
    QLineEdit *lineEdit_proximity_neu_mito;
    QPushButton *pushButton_proximity_neu_mito;
    QPushButton *pushButton_proximity_neu_mito_help;
    QFrame *frame_2;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_proximity_gly;
    QLineEdit *lineEdit_proximity_gly;
    QPushButton *pushButton_proximity_gly;
    QPushButton *pushButton_proximity_gly_help;
    QPushButton *pushButton_submit;

    void setupUi(QWidget *InputForm)
    {
        if (InputForm->objectName().isEmpty())
            InputForm->setObjectName(QString::fromUtf8("InputForm"));
        InputForm->resize(525, 834);
        verticalLayout_4 = new QVBoxLayout(InputForm);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        groupBox_5 = new QGroupBox(InputForm);
        groupBox_5->setObjectName(QString::fromUtf8("groupBox_5"));
        gridLayout_5 = new QGridLayout(groupBox_5);
        gridLayout_5->setObjectName(QString::fromUtf8("gridLayout_5"));
        checkBox_astrocyte_extrafiles = new QCheckBox(groupBox_5);
        checkBox_astrocyte_extrafiles->setObjectName(QString::fromUtf8("checkBox_astrocyte_extrafiles"));

        gridLayout_5->addWidget(checkBox_astrocyte_extrafiles, 0, 0, 1, 1);

        checkBox_neurites_extrafiles = new QCheckBox(groupBox_5);
        checkBox_neurites_extrafiles->setObjectName(QString::fromUtf8("checkBox_neurites_extrafiles"));

        gridLayout_5->addWidget(checkBox_neurites_extrafiles, 1, 0, 1, 1);

        frame_10 = new QFrame(groupBox_5);
        frame_10->setObjectName(QString::fromUtf8("frame_10"));
        frame_10->setFrameShape(QFrame::StyledPanel);
        frame_10->setFrameShadow(QFrame::Raised);
        gridLayout_2 = new QGridLayout(frame_10);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        radioButton_load_astrocyte = new QRadioButton(frame_10);
        radioButton_load_astrocyte->setObjectName(QString::fromUtf8("radioButton_load_astrocyte"));

        gridLayout_2->addWidget(radioButton_load_astrocyte, 0, 2, 1, 1);

        radioButton_load_neurites = new QRadioButton(frame_10);
        radioButton_load_neurites->setObjectName(QString::fromUtf8("radioButton_load_neurites"));

        gridLayout_2->addWidget(radioButton_load_neurites, 0, 1, 1, 1);

        radioButton_load_all = new QRadioButton(frame_10);
        radioButton_load_all->setObjectName(QString::fromUtf8("radioButton_load_all"));
        radioButton_load_all->setChecked(true);

        gridLayout_2->addWidget(radioButton_load_all, 0, 0, 1, 1);


        gridLayout_5->addWidget(frame_10, 2, 0, 1, 1);

        frame_11 = new QFrame(groupBox_5);
        frame_11->setObjectName(QString::fromUtf8("frame_11"));
        frame_11->setFrameShape(QFrame::StyledPanel);
        frame_11->setFrameShadow(QFrame::Raised);
        gridLayout_3 = new QGridLayout(frame_11);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        label_2 = new QLabel(frame_11);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout_3->addWidget(label_2, 0, 0, 1, 1);

        spinBox_limit = new QSpinBox(frame_11);
        spinBox_limit->setObjectName(QString::fromUtf8("spinBox_limit"));
        spinBox_limit->setWrapping(true);
        spinBox_limit->setMinimum(0);
        spinBox_limit->setMaximum(1000000000);

        gridLayout_3->addWidget(spinBox_limit, 0, 1, 1, 1);


        gridLayout_5->addWidget(frame_11, 3, 0, 1, 1);

        pushButton_options_help = new QPushButton(groupBox_5);
        pushButton_options_help->setObjectName(QString::fromUtf8("pushButton_options_help"));

        gridLayout_5->addWidget(pushButton_options_help, 4, 0, 1, 1);


        verticalLayout_4->addWidget(groupBox_5);

        frame_9 = new QFrame(InputForm);
        frame_9->setObjectName(QString::fromUtf8("frame_9"));
        frame_9->setFrameShape(QFrame::StyledPanel);
        frame_9->setFrameShadow(QFrame::Raised);
        gridLayout = new QGridLayout(frame_9);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        groupBox_4 = new QGroupBox(frame_9);
        groupBox_4->setObjectName(QString::fromUtf8("groupBox_4"));
        verticalLayout_2 = new QVBoxLayout(groupBox_4);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        frame_7 = new QFrame(groupBox_4);
        frame_7->setObjectName(QString::fromUtf8("frame_7"));
        frame_7->setFrameShape(QFrame::StyledPanel);
        frame_7->setFrameShadow(QFrame::Raised);
        horizontalLayout_7 = new QHBoxLayout(frame_7);
        horizontalLayout_7->setSpacing(6);
        horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));
        horizontalLayout_7->setContentsMargins(9, 9, 9, 9);
        lineEdit_extra_files_path = new QLineEdit(frame_7);
        lineEdit_extra_files_path->setObjectName(QString::fromUtf8("lineEdit_extra_files_path"));

        horizontalLayout_7->addWidget(lineEdit_extra_files_path);

        pushButton_extra_files_path = new QPushButton(frame_7);
        pushButton_extra_files_path->setObjectName(QString::fromUtf8("pushButton_extra_files_path"));

        horizontalLayout_7->addWidget(pushButton_extra_files_path);

        pushButton_extra_files_path_help = new QPushButton(frame_7);
        pushButton_extra_files_path_help->setObjectName(QString::fromUtf8("pushButton_extra_files_path_help"));

        horizontalLayout_7->addWidget(pushButton_extra_files_path_help);


        verticalLayout_2->addWidget(frame_7);


        gridLayout->addWidget(groupBox_4, 1, 0, 1, 1);

        groupBox_3 = new QGroupBox(frame_9);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        horizontalLayout_10 = new QHBoxLayout(groupBox_3);
        horizontalLayout_10->setObjectName(QString::fromUtf8("horizontalLayout_10"));
        frame = new QFrame(groupBox_3);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        horizontalLayout = new QHBoxLayout(frame);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label = new QLabel(frame);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout->addWidget(label);

        lineEdit_metadata = new QLineEdit(frame);
        lineEdit_metadata->setObjectName(QString::fromUtf8("lineEdit_metadata"));

        horizontalLayout->addWidget(lineEdit_metadata);

        pushButton_metadata = new QPushButton(frame);
        pushButton_metadata->setObjectName(QString::fromUtf8("pushButton_metadata"));

        horizontalLayout->addWidget(pushButton_metadata);

        pushButton_metadata_help = new QPushButton(frame);
        pushButton_metadata_help->setObjectName(QString::fromUtf8("pushButton_metadata_help"));

        horizontalLayout->addWidget(pushButton_metadata_help);


        horizontalLayout_10->addWidget(frame);


        gridLayout->addWidget(groupBox_3, 2, 0, 1, 1);


        verticalLayout_4->addWidget(frame_9);

        groupBox_2 = new QGroupBox(InputForm);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        verticalLayout_3 = new QVBoxLayout(groupBox_2);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        frame_5 = new QFrame(groupBox_2);
        frame_5->setObjectName(QString::fromUtf8("frame_5"));
        frame_5->setFrameShape(QFrame::StyledPanel);
        frame_5->setFrameShadow(QFrame::Raised);
        horizontalLayout_5 = new QHBoxLayout(frame_5);
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        label_5 = new QLabel(frame_5);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        horizontalLayout_5->addWidget(label_5);

        lineEdit_xml_astrocyte = new QLineEdit(frame_5);
        lineEdit_xml_astrocyte->setObjectName(QString::fromUtf8("lineEdit_xml_astrocyte"));

        horizontalLayout_5->addWidget(lineEdit_xml_astrocyte);

        pushButton_xml_astrocyte = new QPushButton(frame_5);
        pushButton_xml_astrocyte->setObjectName(QString::fromUtf8("pushButton_xml_astrocyte"));

        horizontalLayout_5->addWidget(pushButton_xml_astrocyte);


        verticalLayout_3->addWidget(frame_5);

        frame_6 = new QFrame(groupBox_2);
        frame_6->setObjectName(QString::fromUtf8("frame_6"));
        frame_6->setFrameShape(QFrame::StyledPanel);
        frame_6->setFrameShadow(QFrame::Raised);
        horizontalLayout_6 = new QHBoxLayout(frame_6);
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        label_6 = new QLabel(frame_6);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        horizontalLayout_6->addWidget(label_6);

        lineEdit_xml_neurites = new QLineEdit(frame_6);
        lineEdit_xml_neurites->setObjectName(QString::fromUtf8("lineEdit_xml_neurites"));

        horizontalLayout_6->addWidget(lineEdit_xml_neurites);

        pushButton_xml_neurites = new QPushButton(frame_6);
        pushButton_xml_neurites->setObjectName(QString::fromUtf8("pushButton_xml_neurites"));

        horizontalLayout_6->addWidget(pushButton_xml_neurites);


        verticalLayout_3->addWidget(frame_6);

        checkBox_lightXML = new QCheckBox(groupBox_2);
        checkBox_lightXML->setObjectName(QString::fromUtf8("checkBox_lightXML"));
        checkBox_lightXML->setChecked(false);

        verticalLayout_3->addWidget(checkBox_lightXML);

        pushButton_xml_help = new QPushButton(groupBox_2);
        pushButton_xml_help->setObjectName(QString::fromUtf8("pushButton_xml_help"));

        verticalLayout_3->addWidget(pushButton_xml_help);


        verticalLayout_4->addWidget(groupBox_2);

        groupBox = new QGroupBox(InputForm);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        gridLayout_4 = new QGridLayout(groupBox);
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        frame_8 = new QFrame(groupBox);
        frame_8->setObjectName(QString::fromUtf8("frame_8"));
        frame_8->setFrameShape(QFrame::StyledPanel);
        frame_8->setFrameShadow(QFrame::Raised);
        horizontalLayout_8 = new QHBoxLayout(frame_8);
        horizontalLayout_8->setObjectName(QString::fromUtf8("horizontalLayout_8"));
        label_proximity_astrocyte = new QLabel(frame_8);
        label_proximity_astrocyte->setObjectName(QString::fromUtf8("label_proximity_astrocyte"));

        horizontalLayout_8->addWidget(label_proximity_astrocyte);

        lineEdit_proximity_astrocyte = new QLineEdit(frame_8);
        lineEdit_proximity_astrocyte->setObjectName(QString::fromUtf8("lineEdit_proximity_astrocyte"));

        horizontalLayout_8->addWidget(lineEdit_proximity_astrocyte);

        pushButton_proximity_astrocyte = new QPushButton(frame_8);
        pushButton_proximity_astrocyte->setObjectName(QString::fromUtf8("pushButton_proximity_astrocyte"));

        horizontalLayout_8->addWidget(pushButton_proximity_astrocyte);

        pushButton_proximity_astrocyte_help = new QPushButton(frame_8);
        pushButton_proximity_astrocyte_help->setObjectName(QString::fromUtf8("pushButton_proximity_astrocyte_help"));

        horizontalLayout_8->addWidget(pushButton_proximity_astrocyte_help);


        gridLayout_4->addWidget(frame_8, 0, 0, 1, 1);

        frame_4 = new QFrame(groupBox);
        frame_4->setObjectName(QString::fromUtf8("frame_4"));
        frame_4->setFrameShape(QFrame::StyledPanel);
        frame_4->setFrameShadow(QFrame::Raised);
        horizontalLayout_4 = new QHBoxLayout(frame_4);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        label_proximity_astro_mito = new QLabel(frame_4);
        label_proximity_astro_mito->setObjectName(QString::fromUtf8("label_proximity_astro_mito"));

        horizontalLayout_4->addWidget(label_proximity_astro_mito);

        lineEdit_proximity_astro_mito = new QLineEdit(frame_4);
        lineEdit_proximity_astro_mito->setObjectName(QString::fromUtf8("lineEdit_proximity_astro_mito"));

        horizontalLayout_4->addWidget(lineEdit_proximity_astro_mito);

        pushButton_proximity_astro_mito = new QPushButton(frame_4);
        pushButton_proximity_astro_mito->setObjectName(QString::fromUtf8("pushButton_proximity_astro_mito"));

        horizontalLayout_4->addWidget(pushButton_proximity_astro_mito);

        pushButton_proximity_astro_mito_help = new QPushButton(frame_4);
        pushButton_proximity_astro_mito_help->setObjectName(QString::fromUtf8("pushButton_proximity_astro_mito_help"));

        horizontalLayout_4->addWidget(pushButton_proximity_astro_mito_help);


        gridLayout_4->addWidget(frame_4, 1, 0, 1, 1);

        frame_3 = new QFrame(groupBox);
        frame_3->setObjectName(QString::fromUtf8("frame_3"));
        frame_3->setFrameShape(QFrame::StyledPanel);
        frame_3->setFrameShadow(QFrame::Raised);
        horizontalLayout_3 = new QHBoxLayout(frame_3);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        label_proximity_neu_mito = new QLabel(frame_3);
        label_proximity_neu_mito->setObjectName(QString::fromUtf8("label_proximity_neu_mito"));

        horizontalLayout_3->addWidget(label_proximity_neu_mito);

        lineEdit_proximity_neu_mito = new QLineEdit(frame_3);
        lineEdit_proximity_neu_mito->setObjectName(QString::fromUtf8("lineEdit_proximity_neu_mito"));

        horizontalLayout_3->addWidget(lineEdit_proximity_neu_mito);

        pushButton_proximity_neu_mito = new QPushButton(frame_3);
        pushButton_proximity_neu_mito->setObjectName(QString::fromUtf8("pushButton_proximity_neu_mito"));

        horizontalLayout_3->addWidget(pushButton_proximity_neu_mito);

        pushButton_proximity_neu_mito_help = new QPushButton(frame_3);
        pushButton_proximity_neu_mito_help->setObjectName(QString::fromUtf8("pushButton_proximity_neu_mito_help"));

        horizontalLayout_3->addWidget(pushButton_proximity_neu_mito_help);


        gridLayout_4->addWidget(frame_3, 2, 0, 1, 1);

        frame_2 = new QFrame(groupBox);
        frame_2->setObjectName(QString::fromUtf8("frame_2"));
        frame_2->setFrameShape(QFrame::StyledPanel);
        frame_2->setFrameShadow(QFrame::Raised);
        horizontalLayout_2 = new QHBoxLayout(frame_2);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        label_proximity_gly = new QLabel(frame_2);
        label_proximity_gly->setObjectName(QString::fromUtf8("label_proximity_gly"));

        horizontalLayout_2->addWidget(label_proximity_gly);

        lineEdit_proximity_gly = new QLineEdit(frame_2);
        lineEdit_proximity_gly->setObjectName(QString::fromUtf8("lineEdit_proximity_gly"));

        horizontalLayout_2->addWidget(lineEdit_proximity_gly);

        pushButton_proximity_gly = new QPushButton(frame_2);
        pushButton_proximity_gly->setObjectName(QString::fromUtf8("pushButton_proximity_gly"));

        horizontalLayout_2->addWidget(pushButton_proximity_gly);

        pushButton_proximity_gly_help = new QPushButton(frame_2);
        pushButton_proximity_gly_help->setObjectName(QString::fromUtf8("pushButton_proximity_gly_help"));

        horizontalLayout_2->addWidget(pushButton_proximity_gly_help);


        gridLayout_4->addWidget(frame_2, 3, 0, 1, 1);

        frame_4->raise();
        frame_3->raise();
        frame_2->raise();
        frame_8->raise();

        verticalLayout_4->addWidget(groupBox);

        pushButton_submit = new QPushButton(InputForm);
        pushButton_submit->setObjectName(QString::fromUtf8("pushButton_submit"));

        verticalLayout_4->addWidget(pushButton_submit);


        retranslateUi(InputForm);

        QMetaObject::connectSlotsByName(InputForm);
    } // setupUi

    void retranslateUi(QWidget *InputForm)
    {
        InputForm->setWindowTitle(QCoreApplication::translate("InputForm", "Form", nullptr));
        groupBox_5->setTitle(QCoreApplication::translate("InputForm", "Options", nullptr));
        checkBox_astrocyte_extrafiles->setText(QCoreApplication::translate("InputForm", "Compute Astrocyte Normals + Compressed Binary ", nullptr));
        checkBox_neurites_extrafiles->setText(QCoreApplication::translate("InputForm", "Compute Neurites Normals +  Compressed Binary ", nullptr));
        radioButton_load_astrocyte->setText(QCoreApplication::translate("InputForm", "Load Astrocyte Only", nullptr));
        radioButton_load_neurites->setText(QCoreApplication::translate("InputForm", "Load Neurites  Only", nullptr));
        radioButton_load_all->setText(QCoreApplication::translate("InputForm", "Load  All", nullptr));
        label_2->setText(QCoreApplication::translate("InputForm", "Objects Limit", nullptr));
        pushButton_options_help->setText(QCoreApplication::translate("InputForm", "Help", nullptr));
        groupBox_4->setTitle(QCoreApplication::translate("InputForm", "Extra Files Path", nullptr));
        pushButton_extra_files_path->setText(QCoreApplication::translate("InputForm", "Browse...", nullptr));
        pushButton_extra_files_path_help->setText(QCoreApplication::translate("InputForm", "Help", nullptr));
        groupBox_3->setTitle(QCoreApplication::translate("InputForm", "Metadata (HVGX) ", nullptr));
        label->setText(QString());
        pushButton_metadata->setText(QCoreApplication::translate("InputForm", "Browse...", nullptr));
        pushButton_metadata_help->setText(QCoreApplication::translate("InputForm", "Help", nullptr));
        groupBox_2->setTitle(QCoreApplication::translate("InputForm", "XML Data", nullptr));
        label_5->setText(QCoreApplication::translate("InputForm", "Astrocyte", nullptr));
        pushButton_xml_astrocyte->setText(QCoreApplication::translate("InputForm", "Browse...", nullptr));
        label_6->setText(QCoreApplication::translate("InputForm", "Neurites  ", nullptr));
        pushButton_xml_neurites->setText(QCoreApplication::translate("InputForm", "Browse...", nullptr));
        checkBox_lightXML->setText(QCoreApplication::translate("InputForm", "Light XML with extra files (vertices and faces and normals as binary files) in extra files path", nullptr));
        pushButton_xml_help->setText(QCoreApplication::translate("InputForm", "Help", nullptr));
        groupBox->setTitle(QCoreApplication::translate("InputForm", "Proximity Analysis", nullptr));
        label_proximity_astrocyte->setText(QCoreApplication::translate("InputForm", "Astrocyt Seg.Stack          ", nullptr));
        pushButton_proximity_astrocyte->setText(QCoreApplication::translate("InputForm", "Browse...", nullptr));
        pushButton_proximity_astrocyte_help->setText(QCoreApplication::translate("InputForm", "Help", nullptr));
        label_proximity_astro_mito->setText(QCoreApplication::translate("InputForm", "Astrocytic Mito Seg.Stack", nullptr));
        pushButton_proximity_astro_mito->setText(QCoreApplication::translate("InputForm", "Browse...", nullptr));
        pushButton_proximity_astro_mito_help->setText(QCoreApplication::translate("InputForm", "Help", nullptr));
        label_proximity_neu_mito->setText(QCoreApplication::translate("InputForm", "Neuronal Mito Seg.Stack  ", nullptr));
        pushButton_proximity_neu_mito->setText(QCoreApplication::translate("InputForm", "Browse...", nullptr));
        pushButton_proximity_neu_mito_help->setText(QCoreApplication::translate("InputForm", "Help", nullptr));
        label_proximity_gly->setText(QCoreApplication::translate("InputForm", "Glycogen Seg.Stack         ", nullptr));
        pushButton_proximity_gly->setText(QCoreApplication::translate("InputForm", "Browse...", nullptr));
        pushButton_proximity_gly_help->setText(QCoreApplication::translate("InputForm", "Help", nullptr));
        pushButton_submit->setText(QCoreApplication::translate("InputForm", "Submit", nullptr));
    } // retranslateUi

};

namespace Ui {
    class InputForm: public Ui_InputForm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_INPUTFORM_H
