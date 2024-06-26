#ifndef FILEINTERFACE_H
#define FILEINTERFACE_H
#include "ShowResult.h"
class FileInterface {
public:
    virtual void openFile(const QString& aFileName,bool bNewFile = false) = 0;
    virtual void newFile() = 0;
    virtual QString recentOpenDir() =0;
};

#endif // FILEINTERFACE_H
