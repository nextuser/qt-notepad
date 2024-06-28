#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QPlainTextEdit>
#include "searchoptions.h"
#include "ShowResult.h"
class CodeEditor;
namespace Ui {
class SearchDialog;
}

class SearchDialog : public QDialog,public ShowResult
{
    Q_OBJECT

public:
    explicit SearchDialog(CodeEditor *parent , bool bReplace);
    ~SearchDialog();
    virtual void showResultMsg(QString str) override;
private slots:
    void on_toolButtonShowReplace_toggled(bool checked);

    void on_pushButtonFind_clicked();

    void on_pushButtonReplace_clicked();

    void on_pushButtonReplaceAll_clicked();

private:
    Ui::SearchDialog *ui;
    SearchOptions     *options;
    CodeEditor        *codeEditor;
    void loadOptions();
    void saveOptions();
    void showReplace(bool show);



};

#endif // DIALOG_H
