#ifndef TRECENTFILES_H
#define TRECENTFILES_H
#include <QStringList>
class TRecentFiles
{
    const int MAX_COUNT = 100;
public:
    TRecentFiles();
    bool recordFileName(QString url);
    void clear();
    bool changeFileName(QString oldUrl,QString  newUrl);

    QStringList recentFiles();

private:
    QStringList m_files;

};

#endif // TRECENTFILES_H
