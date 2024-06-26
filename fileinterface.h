#ifndef FILEINTERFACE_H
#define FILEINTERFACE_H
#include "ShowResult.h"
class FileInterface {
public:
    virtual void openFile(const QString& aFileName) = 0;

};

#endif // FILEINTERFACE_H
