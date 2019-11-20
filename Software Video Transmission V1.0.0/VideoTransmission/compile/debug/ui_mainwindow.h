/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.13.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QToolBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionStart;
    QAction *actionStop;
    QAction *actionInstructions;
    QAction *actionData_Structure;
    QAction *actionAbout;
    QWidget *centralWidget;
    QGridLayout *gridLayout_2;
    QTabWidget *tabWidget;
    QWidget *tab_device_configuration;
    QGridLayout *gridLayout_4;
    QFormLayout *formLayout_4;
    QLabel *label_12;
    QComboBox *cbAvailableCameras;
    QCheckBox *cbResize;
    QCheckBox *cbCompress;
    QSlider *hsCompression;
    QRadioButton *rbJpg;
    QRadioButton *rbPng;
    QLabel *label_13;
    QLineEdit *leFPS;
    QSlider *hsResize;
    QWidget *tab_communication;
    QVBoxLayout *verticalLayout_2;
    QToolBox *toolBox;
    QWidget *page;
    QFormLayout *formLayout;
    QCheckBox *checkBox_shm_enable;
    QLabel *label_shm_status;
    QLabel *label;
    QLineEdit *lineEdit_shm_baseName;
    QLabel *label_2;
    QSpinBox *spinBox_shm_number;
    QLabel *label_3;
    QLineEdit *lineEdit_shm_inputName;
    QLabel *label_4;
    QLineEdit *lineEdit_shm_outputName;
    QLabel *label_5;
    QLineEdit *lineEdit_shm_configName;
    QLabel *label_14;
    QLineEdit *lineEdit_shm_updateRate;
    QWidget *page_2;
    QVBoxLayout *verticalLayout_4;
    QHBoxLayout *horizontalLayout;
    QCheckBox *checkBox_tcp_Enabled;
    QCheckBox *cbTcpMulticast;
    QCheckBox *cbTcpBroadcast;
    QFormLayout *formLayout_2;
    QLabel *label_6;
    QComboBox *cbTcpLocalAddres;
    QLabel *label_10;
    QLabel *label_7;
    QLineEdit *leTcpRemoteAddres;
    QLineEdit *leTcpPort;
    QLabel *label_15;
    QLineEdit *lineEdit_tcp_updateRate;
    QWidget *page_3;
    QVBoxLayout *verticalLayout_5;
    QHBoxLayout *horizontalLayout_2;
    QCheckBox *checkBox_udp_Enabled;
    QCheckBox *checkBox_6;
    QCheckBox *checkBox_7;
    QFormLayout *formLayout_3;
    QLabel *label_8;
    QComboBox *cbUdpLocalAddres;
    QLabel *label_11;
    QLabel *label_9;
    QLineEdit *leUdpRemoteAddres;
    QLineEdit *leUdpPort;
    QLabel *label_16;
    QLineEdit *lineEdit_udp_updateRate;
    QWidget *tab_device_input;
    QWidget *tab_device_output;
    QGridLayout *gridLayout_3;
    QLabel *lblImageOutput;
    QLabel *lblOriginal;
    QLabel *lblResolution;
    QMenuBar *menuBar;
    QMenu *menuSystem;
    QMenu *menuHelp;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(481, 389);
        MainWindow->setToolButtonStyle(Qt::ToolButtonIconOnly);
        actionStart = new QAction(MainWindow);
        actionStart->setObjectName(QString::fromUtf8("actionStart"));
        actionStop = new QAction(MainWindow);
        actionStop->setObjectName(QString::fromUtf8("actionStop"));
        actionInstructions = new QAction(MainWindow);
        actionInstructions->setObjectName(QString::fromUtf8("actionInstructions"));
        actionData_Structure = new QAction(MainWindow);
        actionData_Structure->setObjectName(QString::fromUtf8("actionData_Structure"));
        actionAbout = new QAction(MainWindow);
        actionAbout->setObjectName(QString::fromUtf8("actionAbout"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        gridLayout_2 = new QGridLayout(centralWidget);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        tabWidget = new QTabWidget(centralWidget);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tabWidget->setTabShape(QTabWidget::Rounded);
        tab_device_configuration = new QWidget();
        tab_device_configuration->setObjectName(QString::fromUtf8("tab_device_configuration"));
        gridLayout_4 = new QGridLayout(tab_device_configuration);
        gridLayout_4->setSpacing(6);
        gridLayout_4->setContentsMargins(11, 11, 11, 11);
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        formLayout_4 = new QFormLayout();
        formLayout_4->setSpacing(6);
        formLayout_4->setObjectName(QString::fromUtf8("formLayout_4"));
        label_12 = new QLabel(tab_device_configuration);
        label_12->setObjectName(QString::fromUtf8("label_12"));

        formLayout_4->setWidget(0, QFormLayout::LabelRole, label_12);

        cbAvailableCameras = new QComboBox(tab_device_configuration);
        cbAvailableCameras->setObjectName(QString::fromUtf8("cbAvailableCameras"));

        formLayout_4->setWidget(0, QFormLayout::FieldRole, cbAvailableCameras);

        cbResize = new QCheckBox(tab_device_configuration);
        cbResize->setObjectName(QString::fromUtf8("cbResize"));
        cbResize->setChecked(true);

        formLayout_4->setWidget(1, QFormLayout::LabelRole, cbResize);

        cbCompress = new QCheckBox(tab_device_configuration);
        cbCompress->setObjectName(QString::fromUtf8("cbCompress"));
        cbCompress->setChecked(false);

        formLayout_4->setWidget(2, QFormLayout::LabelRole, cbCompress);

        hsCompression = new QSlider(tab_device_configuration);
        hsCompression->setObjectName(QString::fromUtf8("hsCompression"));
        hsCompression->setMinimum(10);
        hsCompression->setMaximum(101);
        hsCompression->setValue(50);
        hsCompression->setOrientation(Qt::Horizontal);

        formLayout_4->setWidget(2, QFormLayout::FieldRole, hsCompression);

        rbJpg = new QRadioButton(tab_device_configuration);
        rbJpg->setObjectName(QString::fromUtf8("rbJpg"));
        rbJpg->setChecked(true);

        formLayout_4->setWidget(3, QFormLayout::LabelRole, rbJpg);

        rbPng = new QRadioButton(tab_device_configuration);
        rbPng->setObjectName(QString::fromUtf8("rbPng"));

        formLayout_4->setWidget(3, QFormLayout::FieldRole, rbPng);

        label_13 = new QLabel(tab_device_configuration);
        label_13->setObjectName(QString::fromUtf8("label_13"));
        label_13->setAlignment(Qt::AlignCenter);

        formLayout_4->setWidget(4, QFormLayout::LabelRole, label_13);

        leFPS = new QLineEdit(tab_device_configuration);
        leFPS->setObjectName(QString::fromUtf8("leFPS"));

        formLayout_4->setWidget(4, QFormLayout::FieldRole, leFPS);

        hsResize = new QSlider(tab_device_configuration);
        hsResize->setObjectName(QString::fromUtf8("hsResize"));
        hsResize->setMinimum(10);
        hsResize->setMaximum(100);
        hsResize->setPageStep(10);
        hsResize->setValue(50);
        hsResize->setOrientation(Qt::Horizontal);

        formLayout_4->setWidget(1, QFormLayout::FieldRole, hsResize);


        gridLayout_4->addLayout(formLayout_4, 0, 0, 1, 1);

        tabWidget->addTab(tab_device_configuration, QString());
        tab_communication = new QWidget();
        tab_communication->setObjectName(QString::fromUtf8("tab_communication"));
        verticalLayout_2 = new QVBoxLayout(tab_communication);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        toolBox = new QToolBox(tab_communication);
        toolBox->setObjectName(QString::fromUtf8("toolBox"));
        toolBox->setContextMenuPolicy(Qt::DefaultContextMenu);
        page = new QWidget();
        page->setObjectName(QString::fromUtf8("page"));
        page->setGeometry(QRect(0, 0, 361, 191));
        formLayout = new QFormLayout(page);
        formLayout->setSpacing(6);
        formLayout->setContentsMargins(11, 11, 11, 11);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        checkBox_shm_enable = new QCheckBox(page);
        checkBox_shm_enable->setObjectName(QString::fromUtf8("checkBox_shm_enable"));
        checkBox_shm_enable->setChecked(true);

        formLayout->setWidget(0, QFormLayout::LabelRole, checkBox_shm_enable);

        label_shm_status = new QLabel(page);
        label_shm_status->setObjectName(QString::fromUtf8("label_shm_status"));
        QFont font;
        font.setFamily(QString::fromUtf8("Eras Bold ITC"));
        font.setPointSize(10);
        font.setBold(false);
        font.setWeight(50);
        label_shm_status->setFont(font);

        formLayout->setWidget(0, QFormLayout::FieldRole, label_shm_status);

        label = new QLabel(page);
        label->setObjectName(QString::fromUtf8("label"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label);

        lineEdit_shm_baseName = new QLineEdit(page);
        lineEdit_shm_baseName->setObjectName(QString::fromUtf8("lineEdit_shm_baseName"));

        formLayout->setWidget(1, QFormLayout::FieldRole, lineEdit_shm_baseName);

        label_2 = new QLabel(page);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        formLayout->setWidget(2, QFormLayout::LabelRole, label_2);

        spinBox_shm_number = new QSpinBox(page);
        spinBox_shm_number->setObjectName(QString::fromUtf8("spinBox_shm_number"));

        formLayout->setWidget(2, QFormLayout::FieldRole, spinBox_shm_number);

        label_3 = new QLabel(page);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        formLayout->setWidget(3, QFormLayout::LabelRole, label_3);

        lineEdit_shm_inputName = new QLineEdit(page);
        lineEdit_shm_inputName->setObjectName(QString::fromUtf8("lineEdit_shm_inputName"));

        formLayout->setWidget(3, QFormLayout::FieldRole, lineEdit_shm_inputName);

        label_4 = new QLabel(page);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        formLayout->setWidget(4, QFormLayout::LabelRole, label_4);

        lineEdit_shm_outputName = new QLineEdit(page);
        lineEdit_shm_outputName->setObjectName(QString::fromUtf8("lineEdit_shm_outputName"));

        formLayout->setWidget(4, QFormLayout::FieldRole, lineEdit_shm_outputName);

        label_5 = new QLabel(page);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        formLayout->setWidget(5, QFormLayout::LabelRole, label_5);

        lineEdit_shm_configName = new QLineEdit(page);
        lineEdit_shm_configName->setObjectName(QString::fromUtf8("lineEdit_shm_configName"));

        formLayout->setWidget(5, QFormLayout::FieldRole, lineEdit_shm_configName);

        label_14 = new QLabel(page);
        label_14->setObjectName(QString::fromUtf8("label_14"));

        formLayout->setWidget(6, QFormLayout::LabelRole, label_14);

        lineEdit_shm_updateRate = new QLineEdit(page);
        lineEdit_shm_updateRate->setObjectName(QString::fromUtf8("lineEdit_shm_updateRate"));

        formLayout->setWidget(6, QFormLayout::FieldRole, lineEdit_shm_updateRate);

        toolBox->addItem(page, QString::fromUtf8("Shared Memory"));
        page_2 = new QWidget();
        page_2->setObjectName(QString::fromUtf8("page_2"));
        page_2->setGeometry(QRect(0, 0, 229, 143));
        verticalLayout_4 = new QVBoxLayout(page_2);
        verticalLayout_4->setSpacing(6);
        verticalLayout_4->setContentsMargins(11, 11, 11, 11);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        checkBox_tcp_Enabled = new QCheckBox(page_2);
        checkBox_tcp_Enabled->setObjectName(QString::fromUtf8("checkBox_tcp_Enabled"));
        checkBox_tcp_Enabled->setChecked(true);

        horizontalLayout->addWidget(checkBox_tcp_Enabled);

        cbTcpMulticast = new QCheckBox(page_2);
        cbTcpMulticast->setObjectName(QString::fromUtf8("cbTcpMulticast"));

        horizontalLayout->addWidget(cbTcpMulticast);

        cbTcpBroadcast = new QCheckBox(page_2);
        cbTcpBroadcast->setObjectName(QString::fromUtf8("cbTcpBroadcast"));

        horizontalLayout->addWidget(cbTcpBroadcast);


        verticalLayout_4->addLayout(horizontalLayout);

        formLayout_2 = new QFormLayout();
        formLayout_2->setSpacing(6);
        formLayout_2->setObjectName(QString::fromUtf8("formLayout_2"));
        label_6 = new QLabel(page_2);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        formLayout_2->setWidget(0, QFormLayout::LabelRole, label_6);

        cbTcpLocalAddres = new QComboBox(page_2);
        cbTcpLocalAddres->setObjectName(QString::fromUtf8("cbTcpLocalAddres"));
        cbTcpLocalAddres->setEditable(true);

        formLayout_2->setWidget(0, QFormLayout::FieldRole, cbTcpLocalAddres);

        label_10 = new QLabel(page_2);
        label_10->setObjectName(QString::fromUtf8("label_10"));

        formLayout_2->setWidget(1, QFormLayout::LabelRole, label_10);

        label_7 = new QLabel(page_2);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        formLayout_2->setWidget(2, QFormLayout::LabelRole, label_7);

        leTcpRemoteAddres = new QLineEdit(page_2);
        leTcpRemoteAddres->setObjectName(QString::fromUtf8("leTcpRemoteAddres"));

        formLayout_2->setWidget(1, QFormLayout::FieldRole, leTcpRemoteAddres);

        leTcpPort = new QLineEdit(page_2);
        leTcpPort->setObjectName(QString::fromUtf8("leTcpPort"));

        formLayout_2->setWidget(2, QFormLayout::FieldRole, leTcpPort);

        label_15 = new QLabel(page_2);
        label_15->setObjectName(QString::fromUtf8("label_15"));

        formLayout_2->setWidget(3, QFormLayout::LabelRole, label_15);

        lineEdit_tcp_updateRate = new QLineEdit(page_2);
        lineEdit_tcp_updateRate->setObjectName(QString::fromUtf8("lineEdit_tcp_updateRate"));

        formLayout_2->setWidget(3, QFormLayout::FieldRole, lineEdit_tcp_updateRate);


        verticalLayout_4->addLayout(formLayout_2);

        toolBox->addItem(page_2, QString::fromUtf8("TCP"));
        page_3 = new QWidget();
        page_3->setObjectName(QString::fromUtf8("page_3"));
        page_3->setGeometry(QRect(0, 0, 439, 193));
        verticalLayout_5 = new QVBoxLayout(page_3);
        verticalLayout_5->setSpacing(6);
        verticalLayout_5->setContentsMargins(11, 11, 11, 11);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        checkBox_udp_Enabled = new QCheckBox(page_3);
        checkBox_udp_Enabled->setObjectName(QString::fromUtf8("checkBox_udp_Enabled"));
        checkBox_udp_Enabled->setChecked(true);

        horizontalLayout_2->addWidget(checkBox_udp_Enabled);

        checkBox_6 = new QCheckBox(page_3);
        checkBox_6->setObjectName(QString::fromUtf8("checkBox_6"));

        horizontalLayout_2->addWidget(checkBox_6);

        checkBox_7 = new QCheckBox(page_3);
        checkBox_7->setObjectName(QString::fromUtf8("checkBox_7"));
        checkBox_7->setChecked(true);

        horizontalLayout_2->addWidget(checkBox_7);


        verticalLayout_5->addLayout(horizontalLayout_2);

        formLayout_3 = new QFormLayout();
        formLayout_3->setSpacing(6);
        formLayout_3->setObjectName(QString::fromUtf8("formLayout_3"));
        label_8 = new QLabel(page_3);
        label_8->setObjectName(QString::fromUtf8("label_8"));

        formLayout_3->setWidget(0, QFormLayout::LabelRole, label_8);

        cbUdpLocalAddres = new QComboBox(page_3);
        cbUdpLocalAddres->setObjectName(QString::fromUtf8("cbUdpLocalAddres"));
        cbUdpLocalAddres->setEditable(true);

        formLayout_3->setWidget(0, QFormLayout::FieldRole, cbUdpLocalAddres);

        label_11 = new QLabel(page_3);
        label_11->setObjectName(QString::fromUtf8("label_11"));

        formLayout_3->setWidget(1, QFormLayout::LabelRole, label_11);

        label_9 = new QLabel(page_3);
        label_9->setObjectName(QString::fromUtf8("label_9"));

        formLayout_3->setWidget(2, QFormLayout::LabelRole, label_9);

        leUdpRemoteAddres = new QLineEdit(page_3);
        leUdpRemoteAddres->setObjectName(QString::fromUtf8("leUdpRemoteAddres"));

        formLayout_3->setWidget(1, QFormLayout::FieldRole, leUdpRemoteAddres);

        leUdpPort = new QLineEdit(page_3);
        leUdpPort->setObjectName(QString::fromUtf8("leUdpPort"));

        formLayout_3->setWidget(2, QFormLayout::FieldRole, leUdpPort);

        label_16 = new QLabel(page_3);
        label_16->setObjectName(QString::fromUtf8("label_16"));

        formLayout_3->setWidget(3, QFormLayout::LabelRole, label_16);

        lineEdit_udp_updateRate = new QLineEdit(page_3);
        lineEdit_udp_updateRate->setObjectName(QString::fromUtf8("lineEdit_udp_updateRate"));

        formLayout_3->setWidget(3, QFormLayout::FieldRole, lineEdit_udp_updateRate);


        verticalLayout_5->addLayout(formLayout_3);

        toolBox->addItem(page_3, QString::fromUtf8("UDP"));

        verticalLayout_2->addWidget(toolBox);

        tabWidget->addTab(tab_communication, QString());
        tab_device_input = new QWidget();
        tab_device_input->setObjectName(QString::fromUtf8("tab_device_input"));
        tabWidget->addTab(tab_device_input, QString());
        tab_device_output = new QWidget();
        tab_device_output->setObjectName(QString::fromUtf8("tab_device_output"));
        tab_device_output->setLayoutDirection(Qt::LeftToRight);
        gridLayout_3 = new QGridLayout(tab_device_output);
        gridLayout_3->setSpacing(6);
        gridLayout_3->setContentsMargins(11, 11, 11, 11);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        gridLayout_3->setSizeConstraint(QLayout::SetFixedSize);
        lblImageOutput = new QLabel(tab_device_output);
        lblImageOutput->setObjectName(QString::fromUtf8("lblImageOutput"));
        lblImageOutput->setAutoFillBackground(true);
        lblImageOutput->setFrameShape(QFrame::Box);
        lblImageOutput->setLineWidth(1);
        lblImageOutput->setAlignment(Qt::AlignCenter);

        gridLayout_3->addWidget(lblImageOutput, 0, 0, 1, 1);

        lblOriginal = new QLabel(tab_device_output);
        lblOriginal->setObjectName(QString::fromUtf8("lblOriginal"));

        gridLayout_3->addWidget(lblOriginal, 1, 0, 1, 1);

        lblResolution = new QLabel(tab_device_output);
        lblResolution->setObjectName(QString::fromUtf8("lblResolution"));

        gridLayout_3->addWidget(lblResolution, 2, 0, 1, 1);

        tabWidget->addTab(tab_device_output, QString());

        gridLayout_2->addWidget(tabWidget, 0, 0, 1, 1);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 481, 21));
        menuSystem = new QMenu(menuBar);
        menuSystem->setObjectName(QString::fromUtf8("menuSystem"));
        menuHelp = new QMenu(menuBar);
        menuHelp->setObjectName(QString::fromUtf8("menuHelp"));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);

        menuBar->addAction(menuSystem->menuAction());
        menuBar->addAction(menuHelp->menuAction());
        menuSystem->addAction(actionStart);
        menuSystem->addAction(actionStop);
        menuHelp->addAction(actionInstructions);
        menuHelp->addAction(actionData_Structure);
        menuHelp->addSeparator();
        menuHelp->addAction(actionAbout);

        retranslateUi(MainWindow);

        tabWidget->setCurrentIndex(3);
        toolBox->setCurrentIndex(2);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "SAS Tool Module", nullptr));
        actionStart->setText(QCoreApplication::translate("MainWindow", "Start", nullptr));
        actionStop->setText(QCoreApplication::translate("MainWindow", "Stop", nullptr));
        actionInstructions->setText(QCoreApplication::translate("MainWindow", "Instructions", nullptr));
        actionData_Structure->setText(QCoreApplication::translate("MainWindow", "Data Structure", nullptr));
        actionAbout->setText(QCoreApplication::translate("MainWindow", "About", nullptr));
        label_12->setText(QCoreApplication::translate("MainWindow", "Availables Cameras", nullptr));
        cbResize->setText(QCoreApplication::translate("MainWindow", "Resize Image", nullptr));
        cbCompress->setText(QCoreApplication::translate("MainWindow", "Compress Image", nullptr));
        rbJpg->setText(QCoreApplication::translate("MainWindow", "JPG", nullptr));
        rbPng->setText(QCoreApplication::translate("MainWindow", "PNG", nullptr));
        label_13->setText(QCoreApplication::translate("MainWindow", "FPS (ms)", nullptr));
        leFPS->setText(QCoreApplication::translate("MainWindow", "50", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_device_configuration), QCoreApplication::translate("MainWindow", "Device Configuration", nullptr));
        checkBox_shm_enable->setText(QCoreApplication::translate("MainWindow", "Enable", nullptr));
        label_shm_status->setText(QCoreApplication::translate("MainWindow", "Shared Memory Communication OFF", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "Base Name", nullptr));
        lineEdit_shm_baseName->setText(QCoreApplication::translate("MainWindow", "baseName", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "Number", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "Input Name", nullptr));
        lineEdit_shm_inputName->setText(QCoreApplication::translate("MainWindow", "baseName_input_1", nullptr));
        label_4->setText(QCoreApplication::translate("MainWindow", "Output Name", nullptr));
        lineEdit_shm_outputName->setText(QCoreApplication::translate("MainWindow", "baseName_output_1", nullptr));
        label_5->setText(QCoreApplication::translate("MainWindow", "Configuration Name", nullptr));
        lineEdit_shm_configName->setText(QCoreApplication::translate("MainWindow", "baseName_configuration_1", nullptr));
        label_14->setText(QCoreApplication::translate("MainWindow", "Update Rate [ms]", nullptr));
        lineEdit_shm_updateRate->setText(QCoreApplication::translate("MainWindow", "50", nullptr));
        toolBox->setItemText(toolBox->indexOf(page), QCoreApplication::translate("MainWindow", "Shared Memory", nullptr));
        checkBox_tcp_Enabled->setText(QCoreApplication::translate("MainWindow", "Enabled", nullptr));
        cbTcpMulticast->setText(QCoreApplication::translate("MainWindow", "Multicast", nullptr));
        cbTcpBroadcast->setText(QCoreApplication::translate("MainWindow", "Broadcast", nullptr));
        label_6->setText(QCoreApplication::translate("MainWindow", "Local Addres", nullptr));
        label_10->setText(QCoreApplication::translate("MainWindow", "Remote Addres", nullptr));
        label_7->setText(QCoreApplication::translate("MainWindow", "Port", nullptr));
        leTcpRemoteAddres->setText(QCoreApplication::translate("MainWindow", "0.0.0.0", nullptr));
        leTcpPort->setText(QCoreApplication::translate("MainWindow", "1234", nullptr));
        label_15->setText(QCoreApplication::translate("MainWindow", "Update Rate [ms]", nullptr));
        lineEdit_tcp_updateRate->setText(QCoreApplication::translate("MainWindow", "50", nullptr));
        toolBox->setItemText(toolBox->indexOf(page_2), QCoreApplication::translate("MainWindow", "TCP", nullptr));
        checkBox_udp_Enabled->setText(QCoreApplication::translate("MainWindow", "Enabled", nullptr));
        checkBox_6->setText(QCoreApplication::translate("MainWindow", "Multicast", nullptr));
        checkBox_7->setText(QCoreApplication::translate("MainWindow", "Broadcast", nullptr));
        label_8->setText(QCoreApplication::translate("MainWindow", "Local Addres", nullptr));
        label_11->setText(QCoreApplication::translate("MainWindow", "Remote Addres", nullptr));
        label_9->setText(QCoreApplication::translate("MainWindow", "Port", nullptr));
        leUdpRemoteAddres->setText(QCoreApplication::translate("MainWindow", "0.0.0.0", nullptr));
        leUdpPort->setText(QCoreApplication::translate("MainWindow", "45454", nullptr));
        label_16->setText(QCoreApplication::translate("MainWindow", "Update Rate [ms]", nullptr));
        lineEdit_udp_updateRate->setText(QCoreApplication::translate("MainWindow", "50", nullptr));
        toolBox->setItemText(toolBox->indexOf(page_3), QCoreApplication::translate("MainWindow", "UDP", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_communication), QCoreApplication::translate("MainWindow", "Communication", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_device_input), QCoreApplication::translate("MainWindow", "Device Input", nullptr));
        lblImageOutput->setText(QCoreApplication::translate("MainWindow", "lblOriginalImage", nullptr));
        lblOriginal->setText(QCoreApplication::translate("MainWindow", "TextLabel", nullptr));
        lblResolution->setText(QCoreApplication::translate("MainWindow", "Resolution", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_device_output), QCoreApplication::translate("MainWindow", "Device Output", nullptr));
        menuSystem->setTitle(QCoreApplication::translate("MainWindow", "System", nullptr));
        menuHelp->setTitle(QCoreApplication::translate("MainWindow", "Help", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
