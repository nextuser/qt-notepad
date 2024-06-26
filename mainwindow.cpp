#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tformdoc.h"
#include <QFile>
#include <QDir>
#include <QFileDialog>
#include <QCloseEvent>
#include <QPlainTextEdit>
#include <QInputDialog>
#include <QSettings>
TFormDoc * MainWindow::createFormDoc(){
    TFormDoc *doc = new TFormDoc(this,this);   //指定父窗口，必须在父窗口为Widget窗口提供一个显示区域;
    connect(doc->codeEditor,&CodeEditor::selectionChanged,this,&MainWindow::on_selectionChanged) ;
    connect(doc->codeEditor,&CodeEditor::copyAvailable,this,&MainWindow::on_copyAvailable) ;
    connect(doc->codeEditor,&CodeEditor::undoAvailable,this,&MainWindow::on_undoAvailable) ;
    connect(doc->codeEditor,&CodeEditor::redoAvailable,this,&MainWindow::on_redoAvailable) ;
    return doc;
}
void MainWindow::closeEvent(QCloseEvent *event)
{
    ui->mdiArea->closeAllSubWindows();  //关闭所有子窗口
    saveSettings();
    event->accept();
}
MainWindow::MainWindow(QWidget *parent,const QString& filePath)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),openPath(filePath)
{
    ui->setupUi(this);
    loadSettings();
    setCentralWidget(ui->mdiArea);
    switchViewMode(true);
    QFileInfo fileInfo(openPath);
    if(fileInfo.exists() && fileInfo.isFile()){
        openFile(fileInfo.absoluteFilePath());
    }
    else{
        newFile();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
#include <QClipboard>
#include <QApplication>
#include <QMimeData>
void MainWindow::updateActionState()
{
    TFormDoc *doc;
    bool activated = findActiveForm(doc);
    bool selected = false;
    if(activated ){
        selected = ! doc->selectedText().isEmpty();
        ui->actionRedo->setEnabled(doc->codeEditor->isUndoRedoEnabled());
        ui->actionUndo->setEnabled(doc->codeEditor->isUndoRedoEnabled());
    }
    auto mime = QApplication::clipboard()->mimeData();

    ui->actCut->setEnabled(selected);
    ui->actCopy->setEnabled(selected);
    ui->actPaste->setEnabled(activated && mime->hasText() );
    ui->actFont->setEnabled(activated);
    ui->actionGo_to->setEnabled(activated);
    ui->actFont->setEnabled(activated);
    ui->actionSearch_Next->setEnabled(activated);
    ui->actionSearch_Previous->setEnabled(activated);
    ui->actionGo_to->setEnabled(activated);
    ui->actionReplace->setEnabled(activated);
    ui->actionZoom_in->setEnabled(activated);
    ui->actionzoomout->setEnabled(activated);
    ui->actionsearch->setEnabled(activated);



}

void MainWindow::openFile(const QString& aFileName,bool bNewFile)
{
    if (aFileName.isEmpty())
        return;     //如果未选择文件，退出

    if(!bNewFile) recentPath = QFileInfo(aFileName).absoluteFilePath();
    TFormDoc    *formDoc = createFormDoc();
    ui->mdiArea->addSubWindow(formDoc);
    formDoc->loadFromFile(aFileName,bNewFile);   //打开文件
    formDoc->show();

    updateActionState();
}

void MainWindow::showResultMsg(QString msg)
{
    ui->statusBar->showMessage(msg);
}
void MainWindow::on_actDoc_Open_triggered()
{//打开文件
//必须先获取当前MDI子窗口，再使用打开文件对话框，否则无法获得活动的子窗口



    QString aFileName=QFileDialog::getOpenFileName(this,tr("打开一个文件"),this->recentOpenDir(),
                                                     "所有文件(*.*);;C程序文件(*.h *cpp);;文本文件(*.txt)");
    openFile(aFileName);
}

#include <QTemporaryFile>
#include "fileutil.h"

void MainWindow::newFile(){
    QString tempName = FileUtil::getNewFile(QDir::tempPath(), "未命名.txt");
    QFile aFile(tempName);
    aFile.open(QIODeviceBase::Text | QIODeviceBase::WriteOnly);
    aFile.close();
    openFile(aFile.fileName(),true);
}

QString MainWindow::recentOpenDir()
{
    return QFileInfo(this->recentPath).absolutePath();
}
void MainWindow::on_actDoc_New_triggered()
{ //新建文件

    newFile();

}

void MainWindow::on_actCut_triggered()
{ //cut
    TFormDoc *formDoc = nullptr;
    if(findActiveForm(formDoc)){

        formDoc->textCut();
    }
}

void MainWindow::on_actFont_triggered()
{//设置字体
    TFormDoc *formDoc = nullptr;
    if(findActiveForm(formDoc)){
        formDoc->setEditFont();
    }
}

void MainWindow::on_actCopy_triggered()
{//copy
    TFormDoc *formDoc = nullptr;
    if(findActiveForm(formDoc)){
        formDoc->textCopy();
    }
}

void MainWindow::on_actPaste_triggered()
{//paste
    TFormDoc *formDoc = nullptr;
    if(findActiveForm(formDoc)){
        formDoc->textPaste();
    }

}

void MainWindow::on_mdiArea_subWindowActivated(QMdiSubWindow *arg1)
{//当前活动子窗口切换时
    Q_UNUSED(arg1);
    QMdiSubWindow *activeWindow = ui->mdiArea->activeSubWindow();
    if (ui->mdiArea->subWindowList().size()==0 || activeWindow == nullptr)
    { //若子窗口个数为零

        ui->statusBar->clearMessage();
    }
    else
    {

        TFormDoc *formDoc = nullptr;
        if(findActiveForm(formDoc)){
            ui->statusBar->showMessage(formDoc->currentFileName()); //显示主窗口的文件名
        }

    }
    updateActionState();
}

void MainWindow::switchViewMode(bool isTabbed)
{
    //MDI 显示模式
    if (isTabbed) //Tab多页显示模式
        ui->mdiArea->setViewMode(QMdiArea::TabbedView); //Tab多页显示模式
    else //子窗口模式
        ui->mdiArea->setViewMode(QMdiArea::SubWindowView); //子窗口模式
    // ui->mdiArea->setTabsClosable(isTabbed);  //切换到多页模式下需重新设置
    // ui->actCascade->setEnabled(!isTabbed);   //子窗口模式下才有用
    // ui->actTile->setEnabled(!isTabbed);

}

void MainWindow::on_actCascade_triggered()
{ //窗口级联展开
    ui->mdiArea->setViewMode(QMdiArea::SubWindowView);
    ui->mdiArea->cascadeSubWindows();
}

void MainWindow::on_actTile_triggered()
{//平铺展开
    ui->mdiArea->setViewMode(QMdiArea::SubWindowView);
    ui->mdiArea->tileSubWindows();
}

void MainWindow::on_actCloseALL_triggered()
{//关闭全部子窗口
    ui->mdiArea->closeAllSubWindows();
}


void MainWindow::on_actDoc_Save_triggered()
{//保存

    TFormDoc *formDoc = nullptr;
    if(findActiveForm(formDoc)){
        formDoc->saveToFile(this);
    }
}


QPlainTextEdit * MainWindow::getActiveEdit(){
    QMdiSubWindow * activeWindow = ui->mdiArea->activeSubWindow();
    return (activeWindow == nullptr)?  nullptr : ((TFormDoc*)activeWindow->widget())->textEditor();

}


static const QString KEY_RECENT_PATH = "path/recent";
void MainWindow::loadSettings()
{
    QApplication::setOrganizationName("ljlhome");
    QApplication::setApplicationName("Note--");
    QSettings settings;
    recentPath = settings.value(KEY_RECENT_PATH).toString();
    if(openPath.isEmpty()){
        openPath = recentPath;
    }

}

void MainWindow::saveSettings()
{
    QSettings settings;
    settings.setValue(KEY_RECENT_PATH,QVariant(recentPath));
}


bool MainWindow::findActiveForm(TFormDoc *&doc){
    QMdiSubWindow *activeWindow = ui->mdiArea->activeSubWindow();
    doc = nullptr;
    if(activeWindow != nullptr){
        doc = (TFormDoc*)activeWindow->widget();
    }
    return doc != nullptr;
}

void MainWindow::on_actionGo_to_triggered()
{
    TFormDoc * doc;
    if(findActiveForm(doc)){
        doc->locateLine();
    }


}

void MainWindow::on_actionTabbedView_triggered(bool checked)
{
    Q_UNUSED(checked);
    switchViewMode(true);
}


void MainWindow::on_actionsearch_triggered()
{
    TFormDoc * doc;
    if(findActiveForm(doc)){
        doc->textFind();
    }
}


void MainWindow::on_actionZoom_in_triggered()
{
    TFormDoc * doc;
    if(findActiveForm(doc)){
        doc->zoomIn();
    }

}


void MainWindow::on_actionzoomout_triggered()
{
    TFormDoc * doc;
    if(findActiveForm(doc)){
        doc->zoomOut();
    }
}


void MainWindow::on_actionSearch_Next_triggered()
{

    TFormDoc * doc;
    if(findActiveForm(doc)){
        doc->findNext(this);
    }
}


void MainWindow::on_actionSearch_Previous_triggered()
{
    TFormDoc * doc;
    if(findActiveForm(doc)){
        doc->findPrev(this);
    }
}


void MainWindow::on_actionReplace_triggered()
{
    TFormDoc * doc;
    if(findActiveForm(doc)){
        doc->showTextReplace();
    }
}

void MainWindow::on_undoAvailable(bool b)
{
    Q_UNUSED(b);
    updateActionState();
}

void MainWindow::on_redoAvailable(bool b)
{
    Q_UNUSED(b);
    updateActionState();
}

void MainWindow::on_copyAvailable(bool b)
{
    Q_UNUSED(b);
    updateActionState();
}

void MainWindow::on_selectionChanged()
{
    updateActionState();
}


void MainWindow::on_actionUndo_triggered()
{
    TFormDoc * doc;
    if(findActiveForm(doc)){
        doc->codeEditor->undo();
    }
}



void MainWindow::on_actionRedo_triggered()
{
    TFormDoc * doc;
    if(findActiveForm(doc)){
        doc->codeEditor->redo();
    }
}

