#include "fileutil.h"
#include <QFile>
#include <QTextCodec>
#include <QTextStream>
#include <QDesktopServices>
#include <QList>
#include <QUrl>
#include <QFileInfo>
const QString FileUtil::TIME_FORMAT = "yyyy-MM-dd hh:mm:ss";

/**
 * 读取文件的编码
 * @brief FileUtil::FileCharacterEncoding
 * @param fileName
 * @return
 */
EncodingFormat FileUtil::FileCharacterEncoding(const QString &fileName)
{
    //假定默认编码utf8
    EncodingFormat code = EncodingFormat::UTF8;

    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly))
    {
        //读取3字节用于判断
        QByteArray buffer = file.read(3);
        quint8 sz1st = buffer.at(0);
        quint8 sz2nd = buffer.at(1);
        quint8 sz3rd = buffer.at(2);
        if (sz1st == 0xFF && sz2nd == 0xFE)
        {
            code = EncodingFormat::UTF16LE;
        }
        else if (sz1st == 0xFE && sz2nd == 0xFF)
        {
            code = EncodingFormat::UTF16BE;
        }
        else if (sz1st == 0xEF && sz2nd == 0xBB && sz3rd == 0xBF)
        {
            code = EncodingFormat::UTF8BOM;
        }
        else
        {
            //尝试用utf8转换,如果无效字符数大于0,则表示是ansi编码
            QTextCodec::ConverterState cs;
            QTextCodec* tc = QTextCodec::codecForName("utf-8");
            tc->toUnicode(buffer.constData(), (int)buffer.size(), &cs);
            code = (cs.invalidChars > 0) ? EncodingFormat::ANSI : EncodingFormat::UTF8;
        }

        file.close();
    }

    return code;
}




/**
 * 用于读取文件，清除文件append到文本框中。
 * @brief FileUtil::readFileToShow
 * @param filePath
 * @param ap
 */
void FileUtil::readFileToShow(QString filePath,AppendInterface *ap)
{

    if (ap == nullptr) {
        ///qDebug() << "ap is null!";
        return;
    }

    //读取ansi编码格式文件
    ap->clear();

    //    filePath = "E:/work/ImageManageSys/utf8/0000_051623_162252_05_004_00001_00008_00.txt";
    EncodingFormat code = FileCharacterEncoding(filePath);
    ////qDebug() << "code=" << (int)code;

    //读取ANSI编码格式文件
    if(code == EncodingFormat::ANSI)
    {
        QString txtFile = filePath.left(filePath.size() -3);
        txtFile += "txt";
        //
        QFile file(filePath);
        if(file.open(QIODevice::ReadOnly)) {
            //        qDebug() << file.errorString();
            QTextCodec::setCodecForLocale(QTextCodec::codecForName("gb2312"));//中文转码声明
            QString temStr;
            while(!file.atEnd())
            {
                QByteArray arr = file.readAll();
                arr.replace(0x0B,0x0D);
                temStr = QString::fromLocal8Bit(arr, arr.length());//Window下的QByteArray转QString
                ap->append(temStr);
            }
        }
        file.close();
    }

    //读取UTF-8编码格式文件
    if(code == EncodingFormat::UTF8)
    {

        QFile file(filePath);
        if(!file.open(QIODevice::ReadOnly)) {
            ///qDebug() << file.errorString();
            return;
        }

        QTextStream in(&file);
        ///in.setCodec("UTF-8");  // 设置编码为UTF-8
        in.setAutoDetectUnicode(true);

        while(!in.atEnd()) {
            QString line = in.readLine();
            ap->append(line); // 添加到QTextEdit控件中
        }

        file.close();
    }
}

void  FileUtil::defaultOpen(QString filepaths)
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(filepaths));
}

QString FileUtil::sizeFormat(qint64 size)
{
    const qint64 GB = 1 << 30;
    const qint64 MB = 1 << 20;
    const qint64 KB = 1 << 10;
    QString ret = "";
    if(size > GB){
        ret += QString::asprintf("%3.3fGB",size / (float)GB);
    }
    else if(size > MB){
        ret += QString::asprintf("%3.3fMB",size / (float)MB);
    }
    else if(size > KB){
        ret += QString::asprintf("%3.3fKB",size / (float)KB);
    }
    else{
        ret += QString::asprintf("%dB",(int)size );
    }
    return ret;
}

void showHardwareInfo(){

    // qDebug() << "MachineName: " << GetHardWare::getMachineName();
    // qDebug() << "CpuInfo: " << GetHardWare::getCpuInfo();

    // qDebug() << "MemoryInfo: " << GetHardWare::getMemoryInfo();
    // qDebug() << "OSInfo: " << GetHardWare::getOSInfo();
    // qDebug() << "DiskSerial: " << GetHardWare::getDiskSerial();
    // qDebug() << "MacAddress: " << GetHardWare::getMacAddress();
    // qDebug() << "InternetState: " << GetHardWare::getInternetState();
    // qDebug() << "DiskInfo: " << GetHardWare::getDiskInfo();

    // qDebug() << "CpuSerial: " << GetHardWare::getCpuSerial();
    // qDebug() << "BiosSerial: " << GetHardWare::getBiosSerial();

}

/**
 * 生成目录的层级目录列表 一直到filePath目录
 * @brief FileUtil::generatePathUrls
 * @param filePath
 * @return
 */
QList<NameUrl> FileUtil::generatePathUrls(QString filePath){

    QList<NameUrl> retList;

    QFileInfo fileInfo(filePath);
    QString curPath;
    QString parentPath;
    do{
        curPath = fileInfo.absoluteFilePath();
        parentPath = fileInfo.absolutePath();
        retList.push_front(std::make_pair(fileInfo.fileName(),curPath));
        fileInfo = QFileInfo(parentPath);
    }while(curPath != parentPath);
    return retList;
}

/**
 * 生成可以再label中各层目录的链接。 每个层级一个链接
 * <a href="..." >/</a>&nbsp;&gt;&nbsp;<a href="" >Users</a>&nbsp;&gt;&nbsp;<a href="">jinglinli</a>
 * @brief FileUtil::generateFileLink
 * @param filePath
 * @return
 */
QString FileUtil::generateFileLink(QString filePath){
    static QString linkTemplate = R"(<a href="%1">%2</a>)";
    //QString sep = QDir::separator();
    QString sep = "&nbsp;&gt;&nbsp;";
    QList<NameUrl>  nameUrls = generatePathUrls(filePath);
    QString ret = "";
    for(NameUrl &nameUrl : nameUrls){
        QString name = nameUrl.first;
        QFileInfo info(nameUrl.second);
        if(info.isDir() ){
            if (info.absolutePath() != info.absoluteFilePath()){
                name = info.fileName() + sep;
            }
            ret += linkTemplate.arg(nameUrl.second).arg(name);
        }
        else{
            ret += nameUrl.first;
        }
    }
    return ret;
}


/*
 * generate a new file , if filename exists, add one int to filename. /home/ljl/abc.txt => /home/ljl/abc0.txt
*/
QString FileUtil::getNewFile(QString path, QString fileName)
{
    QString newPath = path + "/" + fileName;
    QFile* currFile = new QFile(newPath);
    int i = 0;
    while(currFile->exists()){
        delete currFile;
        int index = (int)fileName.lastIndexOf(".");
        QString baseName = fileName;
        QString extName = "";
        if(index >= 0 ){

            baseName = fileName.left(index);
            extName = fileName.right(fileName.size() - index - 1);
        }
        newPath = path + "/" + baseName + QString::number(i++) ;
        if(!extName.isEmpty()) newPath += "." +  extName;
        currFile = new QFile(newPath);
    }

    delete currFile;
    return newPath;
}
