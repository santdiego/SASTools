#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent, int PID) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    shmDataIn = new QSharedMemory(QString::number(PID)+"shmDataIn");
    shmDataOut= new QSharedMemory(QString::number(PID)+"shmDataOut");
    shmConfig = new QSharedMemory(QString::number(PID)+"shmConfig");
    shmTimer = new QTimer;
    tcpTimer = new QTimer;
    udpTimer = new QTimer;
    shmDataIn_native  = new QSharedMemory;
    shmDataOut_native = new QSharedMemory;
    shmConfig_native  = new QSharedMemory;
    udpSocket = new QUdpSocket(this);

    if (shmDataIn->isAttached())  shmDataIn->detach();
    if (shmDataOut->isAttached()) shmDataOut->detach();
    if (shmConfig->isAttached())  shmConfig ->detach();

    if (!shmDataIn->create(HardwareInfo::maxInputDataSize))
        qDebug()<<"Unable to create shared memory segment shmDataIn MainWindow.";
    if (!shmDataOut->create(HardwareInfo::maxOutputDataSize))
        qDebug()<<"Unable to create shared memory segment shmDataOut MainWindow.";
    if (!shmConfig->create(HardwareInfo::configSize))
        qDebug()<<"Unable to create shared memory segment shmConfig MainWindow.";

    device = new HardwareDevice(0,PID);

    //Conectar la señalizacion de nuevo dato del Dispositivo con los Slots del resto de los objetos.
    connect(device,SIGNAL(newDeviceDataOutput()),this,SLOT(paintData()));

    //Comunicacion en red
    connect(&tcpServer,SIGNAL(newConnection()),this,SLOT(newTcpClient()));

    //Funcionalidades del UI
    connect(ui->actionStart,SIGNAL(triggered()),this,SLOT(startWorking()));
    connect(ui->actionStop,SIGNAL(triggered()),this,SLOT(stopWorking()));
    connect(ui->hsResize,SIGNAL(valueChanged(int)),this,SLOT(updateConfiguration()));
    connect(ui->hsCompression,SIGNAL(valueChanged(int)),this,SLOT(updateConfiguration()));
    connect(ui->cbResize,SIGNAL(stateChanged(int)),this,SLOT(updateConfiguration()));
    connect(ui->cbCompress,SIGNAL(stateChanged(int)),this,SLOT(updateConfiguration()));
    connect(shmTimer,SIGNAL(timeout()),this,SLOT(updateExternalSharedMemory()));
    //Leer Las Camaras Disponibles
    QList<QCameraInfo> cameras = QCameraInfo::availableCameras();
    for (int dev=0;dev<cameras.size();dev++){
        ui->cbAvailableCameras->addItem(cameras[dev].description());
    }
    ui->lineEdit_shm_updateRate->setText(ui->leFPS->text());
    qDebug()<<"Configuration Memory Size:"+QString::number(sizeof(configs));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_lineEdit_shm_baseName_textChanged(const QString &shm_baseName)
{
    refresh_SHM_names(shm_baseName, ui->spinBox_shm_number->value());
}

void MainWindow::refresh_SHM_names(const QString &base_name, int number){
    QString shm_input_name=base_name+"_input_"+QString::number(number);
    QString shm_output_name=base_name+"_output_"+QString::number(number);
    QString shm_configuration_name=base_name+"_configuration_"+QString::number(number);
    ui->lineEdit_shm_inputName->setText(shm_input_name);
    ui->lineEdit_shm_outputName->setText(shm_output_name);
    ui->lineEdit_shm_configName->setText(shm_configuration_name);
}

void MainWindow::on_spinBox_shm_number_valueChanged(int value)
{
    refresh_SHM_names(ui->lineEdit_shm_baseName->text(), value);
}

void MainWindow::updateDataInput(){
}

void MainWindow::updateDataOutput(){
}

void MainWindow::startWorking(){
    updateConfiguration();
    device->start();
    if(ui->checkBox_shm_enable->isChecked())
        startExternalSharedMemory();
    if(ui->checkBox_tcp_Enabled->isChecked()){
        if(!tcpServer.isListening())
            if(!tcpServer.listen(QHostAddress::Any,ui->leTcpPort->text().toInt()))
                qDebug()<<"Error listening TCPserver";
    }
    if(ui->checkBox_udp_Enabled->isChecked()){
        initUdpSocket();
    }
    ui->tab_communication->setEnabled(false);
}
void MainWindow::newTcpClient(){

    //if(!client.setSocketDescriptor(server.socketDescriptor()))
     //   qDebug()<<"Error";
    tcpClientConnection=tcpServer.nextPendingConnection();
    //connect(tcpClientConnection, &QAbstractSocket::disconnected,tcpClientConnection, &QObject::deleteLater);

    connect(tcpClientConnection,SIGNAL(readyRead()),this,SLOT(tcpIncomingData()));
    connect(tcpTimer,SIGNAL(timeout()),this,SLOT(writeTcpData()));
    tcpTimer->start(ui->lineEdit_tcp_updateRate->text().toInt());
    qDebug()<<"Nuevo Cliente!";
}

void MainWindow::initUdpSocket()
{

    //udpSocket->bind(QHostAddress(ui->leUdpRemoteAddres->text()), ui->leUdpPort->text().toFloat());
    //connect(udpSocket, SIGNAL(readyRead()),this, SLOT(readPendingUdpDatagrams()));
    connect(udpTimer, SIGNAL(timeout()), this, SLOT(writeUdpData()));
    udpTimer->start(ui->lineEdit_udp_updateRate->text().toInt());
}
void MainWindow::readPendingUdpDatagrams()
{
    while (udpSocket->hasPendingDatagrams()) {
        QNetworkDatagram datagram = udpSocket->receiveDatagram();
        //processTheDatagram(datagram);
    }
}

void MainWindow::tcpIncomingData(){
    //qDebug()<<"tcpIncomingData";
    QByteArray Data = tcpClientConnection->readAll();
    qDebug()<<Data;
}

void MainWindow::stopWorking(){
    device->stop();
    stopExternalSharedMemory();
    tcpTimer->stop();
    udpTimer->stop();
    shmTimer->stop();
    ui->tab_communication->setEnabled(true);
    tcpServer.close();
    udpSocket->close();
}

void MainWindow::updateConfiguration(){
    //Se debe recuperar el valor de filas y columnas despues del recorte
    //qDebug()<<"MainWindow::updateConfiguration()";
    HardwareConfigurations temp;

    shmConfig->lock();
    memcpy(&temp,shmConfig->data(),sizeof(HardwareConfigurations));
    //qDebug()<<temp.cols;
    //qDebug()<<temp.rows;
    configs.cols=temp.cols;
    configs.rows=temp.rows;
    configs.dataTotalSize=temp.dataTotalSize;
    //El resto de los parametros se carga desde la UI ()
    configs.selectCamera        =ui->cbAvailableCameras->currentIndex();
    configs.resizeTool          =ui->cbResize->checkState();
    configs.compressionTool     =ui->cbCompress->checkState();
    configs.resizeFactor        =ui->hsResize->value();
    configs.compressionFactor   =ui->hsCompression->value();
    configs.latestWhoWrote      = 1; //UI configuration!
    if(ui->rbJpg->isChecked())
        configs.compressionType =1;
    else if(ui->rbPng->isChecked())
        configs.compressionType =2;
    else
        configs.compressionType =-1;
    configs.stepSize        =(ui->leFPS->text()).toInt();
    memcpy(shmConfig->data(),&configs,sizeof(HardwareConfigurations));
    shmConfig->unlock();
    //emit newUIConfiguration(configs);
}

void MainWindow::paintData(){
    updateConfiguration();

    shmDataOut->lock();
    int imgSize= shmDataOut->size();
    cv::Mat imgToPrint(configs.rows,configs.cols,CV_8UC1);
    cv::Mat imgFromMem= cv::Mat(configs.rows,configs.cols,CV_8UC3,shmDataOut->data());
    if(imgSize>50){
        if(configs.compressionTool){
            //qDebug()<<"configs.compressionTool";
            cv::imdecode(imgFromMem, cv::IMREAD_COLOR, &imgToPrint);
        }
        else{
            //qDebug()<<"!configs.compressionTool";
           imgToPrint= imgFromMem;
        }

        QImage qimage = convertOpenCVMatToQtQImage(imgToPrint);          // convert from OpenCV Mat to Qt QImage
        ui->lblImageOutput->setPixmap(QPixmap::fromImage(qimage));       // show images on form labels           //
        ui->lblOriginal->setText("Image Memory Size: " + QString::number(configs.dataTotalSize));
        ui->lblResolution->setText("Image Resolution: "+QString::number(configs.rows)+"x"+ QString::number(configs.cols));
    }
    shmDataOut->unlock();
}

QImage MainWindow::convertOpenCVMatToQtQImage(cv::Mat mat){
    //Método no propio del Template
    if(mat.channels() == 1) {                   // if grayscale image
        return QImage((uchar*)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_Indexed8);     // declare and return a QImage
    } else if(mat.channels() == 3) {            // if 3 channel color image
        cv::cvtColor(mat, mat,cv::COLOR_BGR2RGB);     // invert BGR to RGB
        return QImage((uchar*)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);       // declare and return a QImage
    } else {
        qDebug() << "in convertOpenCVMatToQtQImage, image was not 1 channel or 3 channel, should never get here";
    }
    return QImage();        // return a blank QImage if the above did not work
}

void MainWindow::startExternalSharedMemory(){

    //emit startSharedMemory
    //ui->tab_device_configuration->setEnabled(false);
    ui->label_shm_status->setText("Shared Memory Communication ON");
    QString inputSHMname  = ui->lineEdit_shm_inputName->text();
    QString outputSHMname = ui->lineEdit_shm_outputName->text();
    QString configSHMname = ui->lineEdit_shm_configName->text();

    shmDataIn_native->setNativeKey(inputSHMname);
    shmDataOut_native->setNativeKey(outputSHMname);
    shmConfig_native->setNativeKey(configSHMname);

    if (!shmDataIn_native->create(HardwareInfo::maxInputDataSize))
        if(!shmDataIn_native->attach())
            qDebug()<<"Unable to create or attach to shared memory segment shmDataIn_native MainWindow.";
    if (!shmDataOut_native->create(HardwareInfo::maxOutputDataSize))
        if(!shmDataOut_native->attach())
            qDebug()<<"Unable to create or attach to shared memory segment shmDataOut_native MainWindow.";
    if (!shmConfig_native->create(HardwareInfo::configSize))
        if(!shmConfig_native->attach())
            qDebug()<<"Unable to create or attach to shared memory segment shmConfig_native MainWindow.";
    shmTimer->start(ui->lineEdit_shm_updateRate->text().toInt());

}

void MainWindow::stopExternalSharedMemory(){
        if (shmTimer->isActive())
          shmTimer->stop();
        //ui->tab_device_configuration->setEnabled(true);

        if(shmDataIn_native->isAttached()){
            shmDataIn_native->detach();
            shmDataOut_native->detach();
            shmConfig_native->detach();
        }
        ui->label_shm_status->setText("Shared Memory Communication OFF");
        //emit closeSharedMemory
}

void MainWindow::updateExternalSharedMemory(){
   //qDebug()<<"MainWindow::updateExternalSharedMemory()";
    try{
        if(shmDataIn->isAttached()&&shmDataIn_native->isAttached()){
        HardwareConfigurations externalConfig;
        memcpy(&externalConfig,shmConfig_native->data(),sizeof(HardwareConfigurations));

        shmDataIn->lock();
        shmDataOut->lock();
        shmConfig->lock();

        memcpy(shmDataIn->data(),shmDataIn_native->data(),shmDataIn->size());
        //qDebug()<<configs.dataTotalSize;
        //qDebug()<<shmDataOut_native->size();
        if(shmDataOut_native->size()>=configs.dataTotalSize)
            memcpy(shmDataOut_native->data(),shmDataOut->data(),configs.dataTotalSize);
        else
            qDebug()<<"ERROR Client memory<Output memory";

        if(externalConfig.latestWhoWrote==2){ //Si se escribio desde memoria compartida
            configs=externalConfig;
           // NO FUNCA!!!!!! memcpy(shmConfig->data(),&configs,shmConfig->size()); //Copio la nueva configuracion al buffer interno
        }else{ //Otro elemento actualizó,  refrezco la memoria compartida Externa
            //memcpy(shmConfig_native->data(),????????,shmConfig_native->size()); NO FUNCA!!!!!!
        }

        shmDataIn->unlock();
        shmDataOut->unlock();
        shmConfig->unlock();
        }
    }catch(exception& e){
            qDebug()<< e.what() << '\n';
    }
}

void MainWindow::writeTcpData(){
    if(1){ //preguntar si existe el tcpClientConnection
        shmDataOut->lock();
        shmConfig->lock();
        //vector<char> dataPackage((char*)shmDataOut->data(),(char*)shmDataOut->data()+shmDataOut->size());
        //dataPackage.insert(dataPackage.begin(),(char*)shmConfig->data(),(char*)shmConfig->data()+shmConfig->size()) ;
        //1º Creo un vector que contenga los datos
        vector<char> dataPackage((char*)shmConfig->data(),(char*)shmConfig->data()+shmConfig->size());
        //2ª agrego al vector, solo los datos de imagen comprimida. el resto no se copia
        dataPackage.insert(dataPackage.end(),(char*)shmDataOut->data(),(char*)shmDataOut->data()+configs.dataTotalSize) ;
        tcpClientConnection->write((char*)dataPackage.data(),dataPackage.size());
        //tcpClientConnection->write((char*)shmDataOut->data(),configs.dataTotalSize);//ANDA OK!
        shmConfig->unlock();
        shmDataOut->unlock();
    }
}
void MainWindow::writeUdpData(){
    if(1){
        shmDataOut->lock();
        shmConfig->lock();
        QByteArray datagram =QByteArray::fromRawData((const char*)shmDataOut->data(), shmDataOut->size());
        int remain=datagram.size();
        int size=0;
        while (remain>0){
            if(remain>512){
                size=512;
                udpSocket->writeDatagram(datagram.data()+(datagram.size()-remain),size,QHostAddress::Broadcast, ui->leUdpPort->text().toInt());
                remain-=512;
            }
            else{
                size=remain;
                udpSocket->writeDatagram(datagram.data()+(datagram.size()-remain),size,QHostAddress::Broadcast, ui->leUdpPort->text().toInt());
                remain=0;
          }
        }

        shmConfig->unlock();
        shmDataOut->unlock();
    }
}
