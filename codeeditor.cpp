#include "codeeditor.h"
#include "linenumberarea.h"
#include "QPainter"
#include "QTextBlock"
#include <QPlainTextEdit>
#include <QTextCursor>
#include <QTextDocumentFragment>
#include <QMessageBox>

#include "searchoptions.h"
#include "searchdialog.h"

CodeEditor::CodeEditor(QWidget *parent,FileInterface * fi)
    : QPlainTextEdit(parent),fileInterface(fi)
{
    lineNumberArea = new LineNumberArea(this);

    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
    connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));
    lnaMargins.setLeft(4);
    lnaMargins.setRight(4);
    updateLineNumberAreaWidth(0);
    highlightCurrentLine();
}

CodeEditor::~CodeEditor()
{
    delete lineNumberArea;
}
// In the constructor we connect our slots to signals in QPlainTextEdit. It is necessary to calculate the line number area width and highlight the first line when the editor is created.

int CodeEditor::lineNumberAreaWidth()
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    int space = lnaMargins.left() + lnaMargins.right() + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;

    return space;
}
// The lineNumberAreaWidth() function calculates the width of the LineNumberArea widget. We take the number of digits in the last line of the editor and multiply that with the maximum width of a digit.

void CodeEditor::updateLineNumberAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}
// When we update the width of the line number area, we simply call QAbstractScrollArea::setViewportMargins().

void CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

#include <QMimeData>
#include <QUrl>
void CodeEditor::dropEvent(QDropEvent *event)
{
    if(event->mimeData()->hasUrls()){
        for(QUrl &url: event->mimeData()->urls()){
            if(url.isLocalFile()){
                fileInterface->openFile(url.toLocalFile());
            }
        }
    }
    else{
        QPlainTextEdit::dropEvent(event);
    }
}
// This slot is invoked when the editors viewport has been scrolled. The QRect given as argument is the part of the editing area that is do be updated (redrawn). dy holds the number of pixels the view has been scrolled vertically.

void CodeEditor::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}
// When the size of the editor changes, we also need to resize the line number area.

void CodeEditor::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        QColor lineColor = QColor(Qt::yellow).lighter(160);

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}
// When the cursor position changes, we highlight the current line, i.e., the line containing the cursor.

// QPlainTextEdit gives the possibility to have more than one selection at the same time. we can set the character format (QTextCharFormat) of these selections. We clear the cursors selection before setting the new new QPlainTextEdit::ExtraSelection, else several lines would get highlighted when the user selects multiple lines with the mouse.


// One sets the selection with a text cursor. When using the FullWidthSelection property, the current cursor text block (line) will be selected. If you want to select just a portion of the text block, the cursor should be moved with QTextCursor::movePosition() from a position set with setPosition().

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), Qt::lightGray);
// The lineNumberAreaPaintEvent() is called from LineNumberArea whenever it receives a paint event. We start off by painting the widget's background.

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();
// We will now loop through all visible lines and paint the line numbers in the extra area for each line. Notice that in a plain text edit each line will consist of one QTextBlock; though, if line wrapping is enabled, a line may span several rows in the text edit's viewport.

// We get the top and bottom y-coordinate of the first text block, and adjust these values by the height of the current text block in each iteration in the loop.

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::black);
            painter.drawText(0, top, lineNumberArea->width() - lnaMargins.right(), fontMetrics().height(),
                             Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }
}

int searchString(QString toFind, QString substr, bool forward)
{
    if(forward){
        return toFind.indexOf(substr);
    }
    else{
        return toFind.lastIndexOf(substr);
    }
}

bool CodeEditor::searchInEditorOriginal(QString text)
{
    QString toFind = this->toPlainText();
    QString substr = text;
    SearchOptions &options = searchOption;
    bool forward = !options.backward;
    if(substr.isEmpty()) return false;

    if(!options.matchcase){
        toFind = toFind.toLower();
        substr = substr.toLower();
    }
    int len = toFind.length();
    int pos = this->textCursor().position();
    int nextPos = pos + 1;
    QString head = (pos > 0) ? toFind.first(pos - 1) : "";
    QString tail = (nextPos < len ) && (pos >= 0)? toFind.last(len - nextPos) : "";

    int startpos = 0;
    int index = -1;
    if(forward){
        index = searchString(tail,substr,true);
        startpos = nextPos;
    }
    else{
        index = searchString(head,substr,false);
        startpos = 0;
    }
    //not find, rewind to find
    if(index < 0 && options.rewind){
        if(forward){
            index = searchString(head,substr,true);
            startpos = 0;
        }
        else{
            index = searchString(tail,substr,false);
            startpos = nextPos;

        }
    }

    if(index < 0 ) {
        ////QMessageBox::information(this,"Find", QString("Fail to find %1").arg(substr));
        return false;

    }


    ////this->extraSelections().clear();
    int selectPos = startpos + index;
    QTextCursor cursor = this->textCursor();
    cursor.setPosition(selectPos, QTextCursor::MoveAnchor); //移到key起始位置
    cursor.movePosition(QTextCursor::NoMove, QTextCursor::KeepAnchor,substr.length());
    cursor.select(QTextCursor::WordUnderCursor);
    this->setTextCursor(cursor);

    return true;
}

QTextDocument::FindFlags CodeEditor::getFindFlags(){

    QTextDocument::FindFlags  ret;
    if(searchOption.matchcase){
        ret |= QTextDocument::FindFlag::FindCaseSensitively;
    }

    if(searchOption.backward){
        ret |= QTextDocument::FindFlag::FindBackward;
    }

    if(searchOption.wholeworld){
        ret |= QTextDocument::FindFlag::FindWholeWords;
    }
    return ret;
}
bool CodeEditor::searchInEditor(QString text)
{
    QTextDocument::FindFlags findFlags = getFindFlags();
    bool find = this->find(text,findFlags);
    QTextCursor cursor = this->textCursor();
    int len = this->toPlainText().length();
    if(!find && searchOption.rewind){
        if(searchOption.backward && len > 0){
            cursor.setPosition(len -1 );
        }
        else {
            cursor.setPosition(0);
        }
        setTextCursor(cursor);
        find = this->find(text,findFlags);
    }

    return find;

}




void CodeEditor::showFind(){

    SearchDialog dlg(this,false);
    dlg.exec();
}

void CodeEditor::showTextReplace()
{
    SearchDialog dlg(this,true);
    dlg.exec();
}

void CodeEditor::findPrev(ShowResult *sr)
{
    this->searchOption.backward = true;
    QString text = this->textCursor().selectedText();

    if(!text.isEmpty()){
        bool find = searchInEditor(text);
        sr->showResultMsg(find ? "" : this->getFindFailMsg());
    }
    else{
        showFind();

    }
}

void CodeEditor::findNext(ShowResult *sr)
{
    this->searchOption.backward = false;
    QString text = this->textCursor().selectedText();

    if(!text.isEmpty()){

        bool find = searchInEditor(text);
        sr->showResultMsg(find ? "" : this->getFindFailMsg());
    }
    else{
        showFind();
    }
}
// void CodeEditor::textFind()
// {
//     QString text = this->textCursor().selectedText();
//     if(!text.isEmpty()){
//         this->searchOption.forward = true;
//         searchInEditor(text);
//     }
// }
#include <QInputDialog>
void CodeEditor::showGoto(){
    bool ok = false;
    int ln = QInputDialog::getInt(this, "转到", "行号： ", 1, 1, this->document()->lineCount(), 1, &ok);//第一步

    if(ok)
    {
        QString text = this->toPlainText();
        QTextCursor c = this->textCursor();

        int pos = 0;
        int next = -1;

        for(int i=0; i<ln; i++)//第二步
        {
            pos = next + 1;//最后一个换行符的下一个字符就是目标行的第一个字符

            next = text.indexOf('\n', pos);
        }

        c.setPosition(pos);//第三步

        this->setTextCursor(c);
    }
}

QString CodeEditor::getFindFailMsg()
{
    QString ret;
    SearchOptions & options = searchOption;
    if(options.rewind){
        ret = QString("文件中无法找到'%1'").arg(options.substr);
    }
    else{
        if(!options.backward){
            ret = QString("已经寻找到文件尾部，无法找到'%1'").arg(options.substr);
        }
        else{
            ret = QString("已经寻找到文件首部，无法找到'%1'").arg(options.substr);
        }
    }
    return ret;
}

void CodeEditor::duplicate_line() {
    QTextCursor cursor = textCursor();
    int pos(cursor.position());
    cursor.beginEditBlock();
    cursor.clearSelection();
    cursor.movePosition(QTextCursor::EndOfLine);
    cursor.select(QTextCursor::LineUnderCursor);
    QTextDocumentFragment text( cursor.selection() );
    cursor.clearSelection();
    cursor.insertText( QString(QChar::LineSeparator) );
    cursor.insertFragment( text );
    cursor.setPosition(pos);
    cursor.endEditBlock();
}


void CodeEditor::delete_current_line() {
    QTextCursor cursor = textCursor();
    cursor.beginEditBlock();
    cursor.clearSelection();
    cursor.movePosition(QTextCursor::StartOfLine);
    int pos(cursor.position());
    cursor.select(QTextCursor::LineUnderCursor);
    // remove line (and line feed char)
    cursor.removeSelectedText();
    cursor.deleteChar();
    // goto start of next line
    cursor.setPosition(pos);
    cursor.endEditBlock();
}


void CodeEditor::selectCursor2(int pos,int len)
{
    Q_UNUSED(pos);
    QList<QTextEdit::ExtraSelection> extraSelections;

    QTextEdit::ExtraSelection selection;

    QColor lineColor = QColor(Qt::blue).lighter(160);

    selection.format.setBackground(lineColor);
    ///selection.format.setProperty(QTextFormat::FullWidthSelection, true);
    QTextCursor cursor = textCursor();


    cursor.movePosition(QTextCursor::NoMove, QTextCursor::KeepAnchor,len);
    selection.cursor = cursor;

    /////selection.cursor.clearSelection();
    extraSelections.append(selection);
    setExtraSelections(extraSelections);


}


void CodeEditor::selectCursor(int pos,int len)
{
    QTextCursor cursor = textCursor();
    cursor.setPosition(pos);
    cursor.movePosition(QTextCursor::NoMove, QTextCursor::KeepAnchor,len);
    cursor.select(QTextCursor::SelectionType::WordUnderCursor);
    setTextCursor(cursor);

}

void CodeEditor::replaceAll(QString findStr, QString replaceStr)
{
    QString text = findStr;
    if (text.isEmpty()) {
        return;
    }

    auto flags = getFindFlags();

    // Count instances
    int found = 0;

    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::Start);
    forever {
        cursor = this->document()->find(text, cursor, flags);
        if (!cursor.isNull()) {
            found++;
        } else {
            break;
        }
    }

    if (found) {

        if (QMessageBox::question(this, tr("Question"), tr("Replace %n instance(s)?", "", found), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No) {
            return;
        }
    } else {
        QMessageBox::information(this, tr("Sorry"), tr("Phrase not found."));
        return;
    }

    // Replace instances
    QTextCursor start_cursor = textCursor();
    forever {
        cursor = this->document()->find(text, cursor, flags);
        if (!cursor.isNull()) {
            cursor.insertText(replaceStr);
            setTextCursor(cursor);
        } else {
            break;
        }
    }
    setTextCursor(start_cursor);
}

void CodeEditor::replace(QString findStr, QString replaceStr)
{
    bool find = searchInEditor(findStr);

    ////ui->labelStatus->setText(msg);
    if(find)
    {
        QTextCursor cursor = textCursor();
        int pos = cursor.position();
        cursor.beginEditBlock();
        cursor.insertText(replaceStr);
        cursor.endEditBlock();
        setTextCursor(cursor);

        selectCursor(pos,replaceStr.length());
        QPalette palette = this->palette();
        palette.setColor(QPalette::Highlight,palette.color(QPalette::Active,QPalette::Highlight));
        setPalette(palette);
    }
    QString msg = find ? "" : getFindFailMsg();
}



