#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QWidget>

#include <QWidget>
#include <QPlainTextEdit>
#include <QMargins>
#include "fileinterface.h"
#include "searchoptions.h"
#include "ShowResult.h"
#include <QTextDocument>
class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    CodeEditor(QWidget *parent ,FileInterface *fi);
    ~CodeEditor();

    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();

    bool searchInEditorOriginal(QString text);
    void findNext(ShowResult *sr);
    void findPrev(ShowResult *sr);
    void textFind();
    void showGoto();
    void showFind();
    void showTextReplace();

    QString getFindFailMsg();
    QTextDocument::FindFlags getFindFlags();
    bool searchInEditor(QString text);
    void delete_current_line();
    void duplicate_line();
    void replaceAll(QString findStr, QString replaceStr,ShowResult *sr);
    bool replaceCurrent(QString findStr,QString replaceStr);
    void replace(QString findStr,QString replaceStr,ShowResult *sr);
    void selectCursor(int pos,int len);
    void selectCursor2(int pos,int len);
protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &, int);



private:
    QWidget *lineNumberArea;
    QMargins lnaMargins;//lineNumberArea margins
    FileInterface *fileInterface;
    SearchOptions searchOption;
    friend class SearchDialog;
    // QWidget interface
protected:
    void dropEvent(QDropEvent *event) override;
};

#endif // CODEEDITOR_H
