#ifndef LINENUMBERAREA_H
#define LINENUMBERAREA_H
#include <QWidget>
class CodeEditor;
class LineNumberArea : public QWidget
{
public:
    LineNumberArea(CodeEditor *editor) ;
    QSize sizeHint() const override;
protected:
    void paintEvent(QPaintEvent *event) override ;

private:
    CodeEditor *codeEditor;
};
#endif // LINENUMBERAREA_H
