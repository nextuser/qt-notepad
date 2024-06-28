#include "trecentfiles.h"

TRecentFiles::TRecentFiles() {

}

bool TRecentFiles::recordFileName(QString filePath){
    return changeFileName(filePath,filePath);
}

QStringList TRecentFiles::recentFiles(){
    return m_files;
}

bool TRecentFiles::changeFileName(QString oldfilePath,QString  newfilePath){
    if(oldfilePath.isEmpty() || newfilePath.isEmpty()){
        return false;
    }

    int index = m_files.indexOf(oldfilePath);
    if(index >= 0){
        m_files.remove(index);
    }

    if(m_files.size() > MAX_COUNT)  m_files.pop_front();
    m_files.append(newfilePath);
    return true;
}

void TRecentFiles::clear(){
    m_files.clear();
}
