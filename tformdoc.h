#ifndef TFORMDOC_H
#define TFORMDOC_H
#include "highlighter.h"
#include <QWidget>
#include <codeeditor.h>
#include "fileinterface.h"
#include "ShowResult.h"

namespace Ui {
class TFormDoc;
}

class TFormDoc : public QWidget
{
    Q_OBJECT

public:
    explicit TFormDoc(QWidget *parent );
    ~TFormDoc();

private:
    Ui::TFormDoc *ui;
    CodeEditor   *codeEditor;

    QString m_filename;             //当前文件
    bool    m_fileOpened=false;     //文件已打开
    bool    m_isNewFile;
    Highlighter *highlighter;
    friend class MainWindow;
    void addTextArea(const QFont &font);
    void afterFileSaved(const QString& filePath,ShowResult *sr);

public:

    QString    loadFromFile(const QString& aFileName,bool bNewFile);   //打开文件
    QString currentFileName();  //返回当前文件名
    bool    isFileOpened();     //文件已经打开
    QString    saveToFile(ShowResult *sr,bool needOtherName = false);     //保存文件

    void    setEditFont();      //设置字体
    void    textCut();          //cut
    void    textCopy();         //copy
    void    textPaste();
    void    showTextReplace();
    void    locateLine();
    void    textFind();
    void    zoomIn();
    void    zoomOut();
    void    findNext(ShowResult *sr);
    void    findPrev(ShowResult *sr);
    QString selectedText();
    QString  filePath() {
        return m_filename;
    }
    inline  CodeEditor * textEditor(){
        return codeEditor;
    }
};

#endif // TFORMDOC_H
