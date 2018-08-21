#ifndef IUDP_H
#define IUDP_H

#include <QObject>
#include <QUdpSocket>
#include <QList>
#include <QNetworkDatagram>

using namespace std;

#define PACK_STORED 10
#define MAXCHUNCKSIZE 1024

//#define HEADERSIZE 20
struct UDPdatagramHeader{
    unsigned pack_num;      //numero de paquete
    unsigned chunk;          //numero de pedazo
    unsigned total_chunks;     //cantidad total de pedazos
};
enum UDP_Instruction {CLIENT_READY,CLIENT_BUSY,RESEND,RECONECT,CLIENT_CTR};
enum State{st_NOTLOADED,st_LOAD,st_START,st_PAUSE,st_STOP,st_CLOSE,st_ERROR};
#define MAXPRAMS 2
#define INTERNAL_HEADER
struct InternalCmd{
    char header[4]={'s','t'};
    UDP_Instruction instruction;
    unsigned parameters[MAXPRAMS];
    //ejemplo 1: instruction=RESEND, parameters[0]=pack_num,parameters[1]=chunk
    //ejemplo 2: instruction=CLIENT_CTR, parameters[0]=State,parameters[1]=chunk

};

enum UDPError{NO_ERROR,WRONG_PACKAGE,WRONG_SIZE };

class UDPdata{
public:
    UDPdata();
    UDPError appendDatagram(const QByteArray &datagram);
    // verifica la cabecera y la remueve : si los numeros de dato o el tamaño no coinciden retorna
    //agrega un nuevo datagrama en orden segun el numero de pedazo (el indice = UDPdatagramHeader::chop)
    //Si el dato esta vacio el numero y tamaño del dato es el que contiene el datagrama. (reserva espacio en datagrams)
    //Si la cantidad de datos ingresados es igual a la cantidad de la cabecera señaliza _isComplete
    QHostAddress getAddres(); //devuelve la direccion desde donde fue generado el
    int getPort();            //devuelve el puerto por donde fue enviado el dato
    void readData(char *ptrDest, unsigned maxSize); // ordena los datagramas, les quita la cabecera y los copia en los primeros maxSize bytes en ptrDest
    void loadData(const char *ptrData, int size, unsigned packNumber); //toma un dato y lo convirte en datagramas. señaliza _isComplete;
    bool isComplete(); //si la cantidad de datos coincide con la informacion de la cabecera retorna true;
    unsigned packNumber(){return pack_num;}
    size_t size(){return _size;} //tamaño del dato
    int datagramCount(){return datagrams.size();} //tamaño del dato
    bool getDatagram(QByteArray &destDatagram, int idx); //retorna el idx datagrama.
    bool operator<(const UDPdata &) const;
    void clean();
private:
    bool sortQNetworkDatagram(const QNetworkDatagram &dat1,const QNetworkDatagram &dat2) const;
    QList<QByteArray> datagrams;
    unsigned pack_num;      //numero de paquete
    unsigned _size;          //tamaño del dato (se actualiza a medida que llegan datagramas)
    bool _isComplete;       //badera que indica si el dato esta completo (cuando se completan todos los datagramas)
    QHostAddress _senderAddress;
    int _senderPort;
};


class IUDP : public QObject
{
    Q_OBJECT
public:
    IUDP();
    IUDP(QHostAddress host, int port);
    ~IUDP();
    int read(char*, unsigned , QHostAddress &, unsigned &); //copia los primeros maxSize bytes en la direccion apuntada por ptrDest. Retira el dato de dataInBuffer
    bool write(const char *data, unsigned size, const QHostAddress &address, const unsigned &port);//envia los maxSize bytes apuntador por data a la direccion destino. Guarda una copia en dataOutBuffer
    int getSize();
    bool hasPendingData();
private slots:
    void readPendingDatagrams(); //lectura de datagrama, reconstruccion de  dato y
signals:
    void newIncomeData(); //señaliza que un dato esta completo.
private:
    QUdpSocket *udpSocket;
    //bool processTheDatagram(const QByteArray &datagram);
    UDPdata  _buffer;
    unsigned _packCounter;
    QHostAddress _addres;
    unsigned _port;
    InternalCmd _clientRequest;
};

#endif // IUDP_H
