#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include    <QMdiSubWindow>
#include "fileinterface.h"
#include "ShowResult.h"

class QPlainTextEdit;
class TFormDoc;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}

QT_END_NAMESPACE

class MainWindow : public QMainWindow,FileInterface,public ShowResult
{
    Q_OBJECT

protected:
    void closeEvent(QCloseEvent *event) override; //主窗口关闭时关闭所有子窗口

public:
    MainWindow(QWidget *parent ,const QString& filePath);
    ~MainWindow();
    // FileInterface
    virtual void openFile(const QString& aFileName,bool bNewFile = false) override;
    virtual void newFile() override;
    virtual QString recentOpenDir() override;
    //ShowResult interface
    void showResultMsg(QString msg) override;

private slots:
    void on_actDoc_Open_triggered(); //打开文件

    void on_actDoc_New_triggered(); //新建文件

    void on_actCut_triggered();     //cut

    void on_actCopy_triggered();    //copy

    void on_actPaste_triggered();   //paste

    void on_actFont_triggered();    //设置字体

    void on_mdiArea_subWindowActivated(QMdiSubWindow *arg1);    //子窗口被激活

    void on_actCascade_triggered(); //级联形式显示子窗口

    void on_actTile_triggered();    //平铺形式显示子窗口

    void on_actCloseALL_triggered(); //关闭全部子窗口

    void on_actDoc_Save_triggered();

    void on_actionGo_to_triggered();


    void on_actionTabbedView_triggered(bool checked);

    void on_actionsearch_triggered();

    void on_actionZoom_in_triggered();

    void on_actionzoomout_triggered();

    void on_actionSearch_Next_triggered();

    void on_actionSearch_Previous_triggered();

    void on_actionReplace_triggered();

    void on_actionUndo_triggered();

    void on_actionRedo_triggered();

private :
    void switchViewMode(bool isTabbed);


    bool findActiveForm(TFormDoc *&doc);

    void updateActionState();




    QPlainTextEdit * getActiveEdit();
public slots:
    virtual void on_undoAvailable(bool b) ;
    virtual void on_redoAvailable(bool b) ;
    virtual void on_copyAvailable(bool b) ;
    virtual void on_selectionChanged() ;

private:
    TFormDoc * createFormDoc();
    Ui::MainWindow *ui;
    QString recentPath;
    QString openPath;

    void loadSettings();
    void saveSettings();

};
#endif // MAINWINDOW_H
