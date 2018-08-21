#ifndef SHAREDMEMORYCONTAINER_H
#define SHAREDMEMORYCONTAINER_H
#include <QString>
#include <QSharedMemory>
struct SHM_descriptor{
    QSharedMemory* ptr_shm;
    int ID;
};

class SharedMemoryContainer
{
public:
   SharedMemoryContainer();
   void removeAll();
   void closeAll();
   void removeFromID(int theID);
   int insert(QSharedMemory* theShareMemory);
   QSharedMemory* getFromID(int theID);
   QSharedMemory* getFromName(QString theName);
   int getNameID(QString theName);
   QString getNames(int);
   int getSize();
   char *getList();
private:
   std::vector<SHM_descriptor> _allShareMemorys;
   std::vector<QString> _names;//The id corresponds to the index where the name is found in _names.
};

#endif // SHAREDMEMORYCONTAINER_H
