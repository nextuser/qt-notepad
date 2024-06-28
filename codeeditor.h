#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QWidget>

#include <QWidget>
#include <QPlainTextEdit>
#include <QMargins>
#include "searchoptions.h"
#include <QTextDocument>
class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    CodeEditor(QWidget *parent );
    ~CodeEditor();

    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();

    bool searchInEditorOriginal(QString text);
    void findNext();
    void findPrev();
    void textFind();
    void showGoto();
    void showFind();
    void showTextReplace();

    QString getFindFailMsg();
    QTextDocument::FindFlags getFindFlags();
    bool searchInEditor(QString text);
    void delete_current_line();
    void duplicate_line();
    QString replaceAll(QString findStr, QString replaceStr);
    bool replaceCurrent(QString findStr,QString replaceStr);
    QString replace(QString findStr,QString replaceStr);
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
    SearchOptions searchOption;
    friend class SearchDialog;

    // QWidget interface
protected:
    void dropEvent(QDropEvent *event) override;

Q_SIGNALS:
    void dropFile(QString filePath);

    void statusMessageChange(QString msg);
};

#endif // CODEEDITOR_H
