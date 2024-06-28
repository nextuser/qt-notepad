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
#include "tsubwindow.h"
#include <QMessageBox>
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
    ///ui->mdiArea->closeAllSubWindows();  //关闭所有子窗口
    if(closeAllSubs()){
        saveSettings();
        event->accept();
    }
    else{
        event->ignore();
    }
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
    TSubWindow *subWin = new TSubWindow(ui->mdiArea,this);
    subWin->setWidget(formDoc);
    ui->mdiArea->addSubWindow(subWin);

    QString filePath = formDoc->loadFromFile(aFileName,bNewFile);   //打开文件
    if(m_recentFiles.recordFileName(filePath)){
        updateRecentMenu();
    }
    formDoc->show();

    updateActionState();
}

void MainWindow::showResultMsg(QString msg)
{
    ui->statusBar->showMessage(msg);
}



QMenu *MainWindow::createTitleMenu(QWidget *target)
{
    QMenu *menu = new QMenu(target);
    menu->addAction(ui->actionCloseTab);
    menu->addAction(ui->actionClose_other_files);
    menu->addAction(ui->actCloseALL);
    return menu;
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
    if (isTabbed) {//Tab多页显示模式
        ui->mdiArea->setViewMode(QMdiArea::TabbedView); //Tab多页显示模式
        ui->mdiArea->setTabsClosable(true);
        ui->mdiArea->setTabsMovable(true);
    }
    else{ //子窗口模式
        ui->mdiArea->setViewMode(QMdiArea::SubWindowView); //子窗口模式
    }
    //   //切换到多页模式下需重新设置
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
    closeAllSubs();
}

bool MainWindow::closeAllSubs(){
    QMessageBox::StandardButton button = QMessageBox::StandardButton::NoButton;
    QList<QMdiSubWindow *> wList = ui->mdiArea->subWindowList();
    bool bBatch = wList.size() > 1;
    for(auto &w : wList){
        button = closeSub(w,button,bBatch);
        if(button == QMessageBox::Cancel)  return false;
    }
    return true;
}

void MainWindow::saveFile(TFormDoc *doc,bool otherName){
    QString oldPath = doc->filePath();
    QString newPath = doc->saveToFile(this,otherName);

    if(m_recentFiles.changeFileName(oldPath,newPath)){
        updateRecentMenu();
    }


}

void MainWindow::updateRecentMenu()
{

    QMenu *menu = new QMenu();
    for(QString  path : this->m_recentFiles.recentFiles()){
        QFileInfo info(path);
        QAction *action = new QAction(info.fileName());
        action->setData(path);
        connect(action,&QAction::triggered,this,&MainWindow::on_recentFileOpen);
        menu->addAction(action);
    }
    ui->actionrecentfiles->setMenu(menu);
}

void MainWindow::on_recentFileOpen(bool checked){
    Q_UNUSED(checked);

    QAction * action = (QAction*) (sender());
    QString path = action->data().toString();

    for(auto subWin : ui->mdiArea->subWindowList()){
        QString openedPath = ((TFormDoc*)subWin->widget())->filePath();
        if(openedPath == path){
            subWin->activateWindow();
            return;
        }
    }

    this->openFile(path);
}

void MainWindow::on_actDoc_Save_triggered()
{//保存

    TFormDoc *formDoc = nullptr;
    if(findActiveForm(formDoc)){
        saveFile(formDoc);
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



QMessageBox::StandardButton MainWindow:: closeSub(QMdiSubWindow * subWindow, QMessageBox::StandardButton inButton,bool batch){
    if(subWindow == nullptr ) return inButton;


    TFormDoc *doc = (TFormDoc*)subWindow->widget();

    if(!doc->isWindowModified()){
        subWindow->close();
        return inButton;
    }

    if(inButton ==  QMessageBox::YesToAll){
        saveFile(doc);
        subWindow->close();
        return inButton;
    }
    else if(inButton == QMessageBox::NoToAll){
        subWindow->close();
        return inButton;
    }
    QMessageBox::StandardButtons buttons ;
    if(!batch){
        buttons =  QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel;
    }
    else{
        buttons = QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel
                  |QMessageBox::StandardButton::YesToAll | QMessageBox::StandardButton::NoToAll;
    }

    QMessageBox::StandardButton button = QMessageBox::question(this,"文件已经修改，正在关闭","是否需要保存?",
                                  buttons );

    if(button == QMessageBox::Cancel)
    {
        return button;
    }
    if(button == QMessageBox::Yes || button == QMessageBox::YesToAll){
        saveFile(doc);
        subWindow->close();
        return button;
    }
    if(button == QMessageBox::NoToAll || button == QMessageBox::No){
        subWindow->close();
        return button;
    }
    return inButton;
}
void MainWindow::on_actionCloseTab_triggered()
{
    QMdiSubWindow *activeWindow = ui->mdiArea->activeSubWindow();
    closeSub(activeWindow);
}


void MainWindow::on_actionClose_other_files_triggered()
{
    QMdiSubWindow *activeWindow = ui->mdiArea->activeSubWindow();
    QMessageBox::StandardButton button = QMessageBox::NoButton;
    int count = ui->mdiArea->subWindowList().size() ;
    bool leftMorethanOne = count - 1 > 1;
    for(auto &w :ui->mdiArea->subWindowList()){
        if( w != activeWindow){
            button = closeSub(w,button,leftMorethanOne);
            if(button == QMessageBox::Cancel)
                return ;
        }
    }
}


void MainWindow::on_actionSave_as_triggered()
{
    TFormDoc *formDoc = nullptr;
    if(findActiveForm(formDoc)){
        saveFile(formDoc,true);
    }
}


void MainWindow::on_actionSave_All_triggered()
{
   for(auto &w :ui->mdiArea->subWindowList()){
        ((TFormDoc* )w->widget())->saveToFile(this);
    }
}

