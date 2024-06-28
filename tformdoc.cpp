#include "tformdoc.h"
#include "ui_tformdoc.h"
#include "codeeditor.h"
#include <QVBoxLayout>
#include <QFileInfo>
#include <QFontDialog>
#include <QSaveFile>
#include <QException>
#include <mainwindow.h>

TFormDoc::TFormDoc(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TFormDoc)

{
    ui->setupUi(this);
    addTextArea(this->font());

    this->setAttribute(Qt::WA_DeleteOnClose);   //关闭时自动删除

    connect(codeEditor, &CodeEditor::modificationChanged,
            this, &QWidget::setWindowModified);
    connect(codeEditor,&CodeEditor::dropFile,(MainWindow*)parent,&MainWindow::on_editorDropFile);
}


void TFormDoc::addTextArea(const QFont &font)
{
    QVBoxLayout *verticalLayout = new QVBoxLayout(this);
    verticalLayout->setSpacing(0);
    verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
    verticalLayout->setContentsMargins(0, 0, 0, 0);
    codeEditor = new CodeEditor(this);
    codeEditor->setObjectName(QString::fromUtf8("CodeEditor"));
    codeEditor->setFont(font);
    highlighter = new Highlighter(codeEditor->document());
    verticalLayout->addWidget(codeEditor);


}

TFormDoc::~TFormDoc()
{
    delete ui;
}


QString TFormDoc::loadFromFile(const QString &aFileName,bool bNewFile)
{//打开文件
    QFile aFile(aFileName);     //以文件方式读出
    QString ret = "";
    if (aFile.open(QIODevice::ReadOnly | QIODevice::Text)) //以只读文本方式打开文件
    {
        QTextStream aStream(&aFile);    //用文本流读取文件
        codeEditor->clear();
        codeEditor->setPlainText(aStream.readAll()); //读取文本文件
        aFile.close();

        m_filename=aFileName;             //保存当前文件名
        QFileInfo   fileInfo(aFileName);    //文件信息
        QString str=fileInfo.fileName();    //去除路径后的文件名
        this->setToolTip(fileInfo.absoluteFilePath());
        this->setWindowTitle(str+"[*]");
        m_fileOpened=true;

        ret = m_filename;
    }
    this->m_isNewFile = bNewFile;
    return ret;

}

QString TFormDoc::currentFileName()
{
    return  m_filename;
}

bool TFormDoc::isFileOpened()
{ //文件是否已打开
    return m_fileOpened;
}

void TFormDoc::afterFileSaved(const QString& filePath,ShowResult *sr)
{
    QFileInfo fileInfo(filePath);
    this->setWindowTitle(fileInfo.fileName());
    this->setToolTip(fileInfo.absoluteFilePath());
    this->setWindowModified(false);
    this->m_isNewFile = false;
     sr->showResultMsg(QString("保存成功：%1").arg(fileInfo.absoluteFilePath()));
}
#include <QFileDialog>
QString TFormDoc::saveToFile(ShowResult *sr,bool needOtherName )
{
    QString filePath(m_filename);
    QString name = QFileInfo(filePath).fileName();

    if(this->m_isNewFile || needOtherName){

        filePath = QFileDialog::getSaveFileName(this, tr("Save File"),
                                           filePath,
                                           tr("All files(*.*);;Text files (*.txt);;Xml files(*.xml *.xsd);;cpp files(*.c *.cc *.cpp *.h *.hpp)"));

    }

    if(filePath.isEmpty()) return "";

    QSaveFile   aFile(filePath);
    if (!aFile.open(QIODevice::WriteOnly | QIODevice::Text))
        return "";


    aFile.setDirectWriteFallback(false);    //使用临时文件
    try
    {
        QString str=codeEditor->toPlainText();  //整个内容作为字符串
        QByteArray  strBytes=str.toUtf8();              //转换为字节数组, UTF-8编码
        aFile.write(strBytes,strBytes.length());        //写入文件
        aFile.commit();
        afterFileSaved(filePath,sr);
        return m_filename;
    }
    catch (QException &e)
    {
        qDebug("保存文件的过程发生了错误");
        aFile.cancelWriting();      //出现异常时取消写入
        sr->showResultMsg("保存文件的过程发生了错误");
        return "";
    }
}

void TFormDoc::setEditFont()
{
    QFont   font;
    bool    ok;
    font=codeEditor->font();

    font=QFontDialog::getFont(&ok,font);
    codeEditor->setFont(font);
}

void TFormDoc::textCut()
{
    codeEditor->cut();
}

void TFormDoc::textCopy()
{
    codeEditor->copy();
}

void TFormDoc::textPaste()
{
    codeEditor->paste();
}

#include <QInputDialog>
void TFormDoc::locateLine()
{
    codeEditor->showGoto();
}


const int FONT_MIN_POINT = 6;
const int FONT_MAX_PIONT = 100;
const int FONT_STEP = 2;
void TFormDoc::zoomIn()
{
    QFont   font;

    font= codeEditor->font();
    int newSize = font.pointSize() + FONT_STEP;
    if(newSize < FONT_MAX_PIONT){
        font.setPointSize(newSize);
        codeEditor->setFont(font);
    }
}

void TFormDoc::zoomOut()
{
    QFont   font;

    font= codeEditor->font();
    int newSize = font.pointSize() - FONT_STEP;
    if(newSize >= FONT_MIN_POINT){
        font.setPointSize(newSize);

        codeEditor->setFont(font);
    }
}

void TFormDoc::findNext(ShowResult * sr)
{
    codeEditor->findNext(sr);
}

void TFormDoc::findPrev(ShowResult * sr)
{
    codeEditor->findPrev(sr);
}

QString TFormDoc::selectedText()
{
    return codeEditor->textCursor().selectedText();
}

void TFormDoc::textFind()
{
    codeEditor->showFind();
}

void TFormDoc::showTextReplace()
{
    codeEditor->showTextReplace();
}
