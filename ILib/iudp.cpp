#include "iudp.h"
#include <iostream>
#include <QTest>
using namespace std;

UDPdata::UDPdata(){
    clean();
}
UDPError UDPdata::appendDatagram(const QByteArray &datagram){
    UDPdatagramHeader *header=(UDPdatagramHeader *) datagram.data();
    if(header->chunk ==1){
        pack_num=header->pack_num;
        _size=0;
        datagrams.clear();
        _isComplete=false;
    }
    _size+=datagram.size()-sizeof(UDPdatagramHeader);
    datagrams.append(datagram);
    //std::sort(datagrams.begin(),datagrams.end(),sortQNetworkDatagram); ERROR!!!!!!!!!!!!!
    if(datagrams.size()==(int)header->total_chunks){
        _isComplete=true;
    }
    return NO_ERROR;
}

void UDPdata::readData(char *ptrDest, unsigned maxSize){
    QByteArray auxByteArray;
    int offset=0;
    if(isComplete()){
        int size =min(maxSize,_size);
        for(int i=0;i<datagrams.size();i++){
            auxByteArray=datagrams[i];
            auxByteArray.remove(0,sizeof(UDPdatagramHeader));
            if(offset+auxByteArray.size()<=size){
                memcpy(ptrDest+offset,auxByteArray.data(),auxByteArray.size());
                offset+=auxByteArray.size();
            }
            else{
                memcpy(ptrDest+offset,auxByteArray.data(),size-offset);
                offset+=size-offset;
            }
        }
    }
}
void UDPdata::clean(){
    datagrams.clear();
    pack_num=-1;
    _size=-1;
    _isComplete=false;
    _senderAddress=QHostAddress::Null;
    _senderPort=-1;
}
void UDPdata::loadData(const char *ptrData, int size, unsigned packNumber=0){
    const int payLoad = MAXCHUNCKSIZE-sizeof(UDPdatagramHeader);//512-10=502
    QByteArray auxByteArray;
    _size=size;
    UDPdatagramHeader auxHeader;
    char *ptrAux=(char*)&auxHeader;
    int numChunk=size/payLoad;
    if(size%payLoad){
        numChunk++;
    }
    auxHeader.total_chunks=numChunk;
    auxHeader.pack_num=packNumber;
    int offset=0;
    int remain=0;
    auxHeader.chunk=0;
    for (int i=0;i<numChunk;i++){
        auxByteArray.clear();
        auxHeader.chunk=i+1;
        auxByteArray.insert(0,ptrAux,sizeof(UDPdatagramHeader));
        remain=size-offset;
        if(remain>=payLoad){
            auxByteArray.insert(sizeof(UDPdatagramHeader),ptrData+offset,payLoad);
            offset+=payLoad;
        }
        else{
            auxByteArray.insert(sizeof(UDPdatagramHeader),ptrData+offset,remain);
            offset+=remain;
        }
        datagrams.append(auxByteArray);
    }
}


QHostAddress UDPdata::getAddres(){
    if(datagrams.size()>0)
        return _senderAddress;
    else
        return QHostAddress::Null;
}

int UDPdata::getPort(){
    if(datagrams.size()>0)
        return _senderPort;
    else
        return -1;
}


bool UDPdata::isComplete(){
    if(datagrams.size()>0){
        UDPdatagramHeader *head=(UDPdatagramHeader*)datagrams[0].data();
        if(head->total_chunks==(unsigned)datagrams.size()){
            _isComplete=true;
            return true;
        }
    }
    return false;
}

bool UDPdata::operator<(const UDPdata & dat) const{

    return(_size<dat._size);
}

bool UDPdata::getDatagram(QByteArray &destDatagram,int idx){
    if(datagrams.size()>idx){
        destDatagram=datagrams[idx];
        return true;
    }
    return false;
}

bool UDPdata::sortQNetworkDatagram(const QNetworkDatagram &dat1,const QNetworkDatagram &dat2) const{
    UDPdatagramHeader *h1=(UDPdatagramHeader *) dat1.data().data();
    UDPdatagramHeader *h2=(UDPdatagramHeader *) dat2.data().data();
    return (h1->chunk<h2->chunk);
}

/********************************IUDP**********************************/
IUDP::IUDP(){
    memset(&_clientRequest,0,sizeof(_clientRequest));
    _clientRequest.instruction=CLIENT_READY;
    _packCounter=0;
    udpSocket = new QUdpSocket(this);
    udpSocket->bind();
    //udpSocket->setSocketOption(QAbstractSocket::LowDelayOption,1);
    connect(udpSocket, SIGNAL(readyRead()),this, SLOT(readPendingDatagrams()),Qt::DirectConnection);
    _addres=QHostAddress::Any;
    _port=1234;
}

IUDP::IUDP(QHostAddress host, int port){
    memset(&_clientRequest,0,sizeof(_clientRequest));
    _clientRequest.instruction=CLIENT_READY;
    _packCounter=0;
    udpSocket = new QUdpSocket(this);
    udpSocket->bind(host, port);
    _addres=host;
    _port=port;
    connect(udpSocket, SIGNAL(readyRead()),this, SLOT(readPendingDatagrams()),Qt::DirectConnection);
}

IUDP::~IUDP(){
    delete udpSocket;
}

bool IUDP::hasPendingData(){
    if(_buffer.isComplete())
        return true;
    return false;
}

int IUDP::getSize(){
    if(_buffer.size()>0)
        return _buffer.size();
    else
        return -1;
}

void IUDP::readPendingDatagrams(){
    QByteArray dataArray;
    InternalCmd *ptr;
    QHostAddress host;
    quint16  hostPort;
    InternalCmd cmd;
    QByteArray aux;
    cmd.header[0]='s';
    cmd.header[1]='t';
    cmd.header[2]=0;
    cmd.header[3]=0;

    cout<<"readPendingDatagrams"<<endl;
    while (udpSocket->hasPendingDatagrams()) {
        dataArray.resize(udpSocket->pendingDatagramSize());
        if(udpSocket->readDatagram(dataArray.data(),dataArray.size(),&host,&hostPort)<0){
            cout<<"Error reading Datagram"<<endl;
        }
        ptr =(InternalCmd *)dataArray.data();
        if(ptr->header[0]=='s' && ptr->header[1]=='t'){
            cout<<"mensaje del cliente"<<endl;
            memcpy(&_clientRequest,ptr,sizeof(InternalCmd));
            _clientRequest.instruction = ptr->instruction;
            cout<<"client Instruction: "<<_clientRequest.instruction<<endl;
        }
        else{
            cout<<"not header"<<endl;
            UDPdatagramHeader *header=(UDPdatagramHeader *) dataArray.data();
            if(header->chunk ==1){ //recibiendo un nuevo paquete. Señalizar busy
                cmd.instruction=CLIENT_BUSY;
                aux.append((char*)&cmd,sizeof(InternalCmd));
                cout<<"mensaje del cliente al servidor"<<endl;
                cout<<"Header: "<<((InternalCmd*)aux.data())->header<<endl;
                cout<<"instruction "<<((InternalCmd*)aux.data())->instruction<<endl;
                udpSocket->writeDatagram(aux,host,hostPort);
            }
            _buffer.appendDatagram(dataArray);
            if(_buffer.isComplete()){
                cout<<"is comeplete"<<endl;
                cmd.instruction=CLIENT_READY;
                aux.append((char*)&cmd,sizeof(InternalCmd ));
                udpSocket->writeDatagram(aux,host,hostPort);
                emit newIncomeData();
            }
        }
    }
}

int IUDP::read(char *data, unsigned maxSize, QHostAddress &address, unsigned &port){
    int size=-1;
    if(_buffer.isComplete()){
        address=_buffer.getAddres();
        port=_buffer.getPort();
        size=min(_buffer.size(),maxSize);
        _buffer.readData(data,maxSize);
        _buffer.clean();
    }
    return size;
}

bool IUDP::write(const char *data, unsigned size, const QHostAddress &address=QHostAddress::Null,const unsigned &port=0){
    _packCounter++;
    QHostAddress clientAddress=address;
    unsigned clientPort=port;
    if(address==QHostAddress::Null){
        clientAddress=QHostAddress::Broadcast;
    }
    if(port==0){
        clientPort=_port;
    }
    UDPdata auxData;
    QByteArray auxDatagram;

    auxData.loadData(data,size,_packCounter);
    if(udpSocket->hasPendingDatagrams()){
        readPendingDatagrams();
    }
    if(_clientRequest.instruction== CLIENT_READY){
        for(int i=0;i<auxData.datagramCount();i++){
            auxData.getDatagram(auxDatagram,i);
            if(udpSocket->writeDatagram(auxDatagram,clientAddress,clientPort)<0){
                cout<<"Error enviando"<<endl;
                return false;
            }
        }
    }else{
        cout<<"_clientRequest: "<<_clientRequest.instruction<<endl;
        return false;
    }
    return true;
}
