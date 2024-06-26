#include "searchdialog.h"
#include "ui_searchdialog.h"
#include "codeeditor.h"
#include <QPalette>
#include <QColor>
SearchDialog::SearchDialog(CodeEditor *parent , bool bReplace)
    : QDialog(parent)
    , ui(new Ui::SearchDialog),codeEditor(parent)
{
    ui->setupUi(this);
    ui->toolButtonShowReplace->setDefaultAction(ui->actionactionShowReplace);
    showReplace(bReplace);
    loadOptions();


    ui->labelStatus->setText("");

    QPalette plt ;
    plt.setColor(QPalette::ColorRole::WindowText,Qt::blue);
    ui->labelStatus->setPalette(plt);


}

void SearchDialog::loadOptions(){
    SearchOptions &options = codeEditor->searchOption;
    ui->checkBoxMachcase->setChecked(options.matchcase);
    ui->checkBoxReverse->setChecked(options.backward);
    ui->checkBoxRewind->setChecked(options.rewind);
    ui->checkBoxWholeWord->setChecked(options.wholeworld);
    QString text = codeEditor->textCursor().selectedText();
    if(text.isEmpty()) text = options.substr;
    ui->lineEditSearch->setText(text);
    ui->lineEditReplace->setText(options.toReplace);
}

void SearchDialog::saveOptions(){
    SearchOptions &options =  codeEditor->searchOption;
    options.matchcase = ui->checkBoxMachcase->isChecked();
    options.rewind = ui->checkBoxRewind->isChecked();
    options.wholeworld = ui->checkBoxWholeWord->isChecked();
    options.backward = ui->checkBoxReverse->isChecked();
    options.substr = ui ->lineEditSearch->text();
    options.toReplace = ui->lineEditReplace->text();
}


SearchDialog::~SearchDialog()
{
    delete ui;
}

void SearchDialog::showResultMsg(QString str)
{
    ui->labelStatus->setText(str);
}

void SearchDialog::on_toolButtonShowReplace_toggled(bool checked)
{
    showReplace(checked);
}

void SearchDialog::showReplace(bool show)
{
    ui->groupBoxReplace->setVisible(show);
    ui->toolButtonShowReplace->setChecked(show);

    if(show){

        QWidget::setTabOrder({ui->lineEditSearch,ui->lineEditReplace,ui->pushButtonReplace,
                              ui->pushButtonReplaceAll,ui->pushButtonFind,ui->checkBoxReverse,ui->checkBoxMachcase,
                              ui->checkBoxWholeWord,ui->checkBoxRewind,ui->toolButtonShowReplace});
    }
    else{
        QWidget::setTabOrder({ui->lineEditSearch,ui->pushButtonFind,
                              ui->checkBoxReverse,ui->checkBoxMachcase,
                              ui->checkBoxWholeWord,ui->checkBoxRewind,ui->toolButtonShowReplace});
    }


}

void SearchDialog::on_pushButtonFind_clicked()
{

    saveOptions();

    bool find = codeEditor->searchInEditor(ui->lineEditSearch->text());
    QString msg = find ? "" : codeEditor->getFindFailMsg();
    ui->labelStatus->setText(msg);
    if(find)
    {

        QPalette palette = codeEditor->palette();
        palette.setColor(QPalette::Highlight,palette.color(QPalette::Active,QPalette::Highlight));
        codeEditor->setPalette(palette);
    }
}

void SearchDialog::on_pushButtonReplace_clicked()
{
    saveOptions();
    codeEditor->replace(ui->lineEditSearch->text(),ui->lineEditReplace->text(),this);

}


void SearchDialog::on_pushButtonReplaceAll_clicked()
{
    saveOptions();
    codeEditor->replaceAll(ui->lineEditSearch->text(),ui->lineEditReplace->text(),this);
}

