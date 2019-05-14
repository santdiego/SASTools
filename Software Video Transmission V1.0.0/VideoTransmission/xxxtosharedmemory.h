#ifndef XXXTOSHAREDMEMORY_H
#define XXXTOSHAREDMEMORY_H

#include <QObject>
#include <QSharedMemory>

class XXXtoSharedMemory : public QObject
{
    Q_OBJECT
public:
    explicit XXXtoSharedMemory(QObject *parent = nullptr);

signals:
    void newOutbox(); //signa
public slots:
    void newInbox();
private:
    QSharedMemory *inbox;
    QSharedMemory *outbox;

};

#endif // XXXTOSHAREDMEMORY_H
