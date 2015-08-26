#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStringList>
#include <QAbstractItemView>
#include "QDebug"
#include "get_url.h"
#include "QTreeWidgetItem"
#include <QDesktopWidget>
#include "QProcess"
#include "download_dialog.h"
#include "QTimer"


QProcess mprocess;
QTreeWidgetItem *currentitem;
QTimer *mtimer;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);

    const QRect screen = QApplication::desktop()->screenGeometry();
    this->move( screen.center() - this->rect().center() );

    QTreeWidgetItem* header = new QTreeWidgetItem();
    header->setText(0,"File Name");
    header->setText(1,"Size");
    header->setText(2,"Status");
    header->setText(3,"URL");
    header->setText(4,"Save as");
    ui->treeWidget->setHeaderItem(header);
    ui->treeWidget->setColumnWidth(0,300);
    ui->treeWidget->setColumnWidth(1,100);
    ui->treeWidget->setColumnWidth(2,200);
    ui->treeWidget->setColumnWidth(3,500);
    ui->treeWidget->setColumnWidth(4,500);

    mtimer = new QTimer(this);
    connect(mtimer, SIGNAL(timeout()), (this), SLOT(reload_list()));
    mtimer->start(500);

}
//    Client *cl = new Client();
MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::reload_list(){
    int currentrow = 0;
    if(ui->treeWidget->topLevelItemCount() != 0 && ui->treeWidget->selectedItems().count() != 0){
        currentrow = ui->treeWidget->indexOfTopLevelItem(ui->treeWidget->selectedItems().first());
    }
    int oldsize = ui->treeWidget->topLevelItemCount();
    ui->treeWidget->clear();
    mprocess.start("sh", QStringList() << "-c" << "cat ~/.paria/.paria.list");
    mprocess.waitForFinished(-1);
    QString stdoutraw = mprocess.readAllStandardOutput();
    QStringList stdout = stdoutraw.split("\n");

    for(int i = 0;i<stdout.length();i++){
        if(!stdout.at(i).isEmpty()){
            QTreeWidgetItem* Item = new QTreeWidgetItem();
            Item->setText(0,getBetween(stdout.at(i),"#<url>#","#</url>#").split("/").at(getBetween(stdout.at(i),"#<url>#","#</url>#").split("/").length()-1));
            Item->setText(1,getBetween(stdout.at(i),"#<size>#","#</size>#"));
            Item->setText(2,getBetween(stdout.at(i),"#<status>#","#</status>#"));
            Item->setText(3,getBetween(stdout.at(i),"#<url>#","#</url>#"));
            Item->setText(4,getBetween(stdout.at(i),"#<saveas>#","#</saveas>#"));

            ui->treeWidget->insertTopLevelItem(i,Item);

        }
    }

    if(stdout.length() != 0){
        if(ui->treeWidget->topLevelItemCount() == oldsize){
                ui->treeWidget->topLevelItem(currentrow)->setSelected(true);
                currentitem = ui->treeWidget->topLevelItem(currentrow);
        }
    }
}

void MainWindow::on_btn_add_url_clicked()
{
    get_url *geturl = new get_url();
    geturl->show();
}

QString MainWindow::getBetween(QString str, QString open, QString close) {
    int startIndex = str.indexOf(open)+open.length();
    if(startIndex <= 0)return QString();
    int endIndex = str.indexOf(close,startIndex);
    if(endIndex <= 0)return QString();
    return str.mid(startIndex,endIndex - startIndex);
}

void MainWindow::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column)
{
    currentitem = item;
}



void MainWindow::on_btn_delete_clicked()
{
    if(ui->treeWidget->selectedItems().count() != 0){
    mprocess.start("sh", QStringList() << "-c" << "cat ~/.paria/.paria.list && rm ~/.paria/.paria.list");
    mprocess.waitForFinished(-1);
    QString stdoutraw = mprocess.readAllStandardOutput();
    QStringList stdout = stdoutraw.split("\n");

    for(int i = 0;i<stdout.length();i++){
        if(!stdout.at(i).isEmpty()){
            QString line = getBetween(stdout.at(i),"#<url>#","#</url>#");
            QString current_item_line = currentitem->text(3);
            if(line == current_item_line){
                continue;
            }else{
                mprocess.start("sh", QStringList() << "-c" << "echo '" + stdout.at(i) + "' >> ~/.paria/.paria.list" );
                mprocess.waitForFinished(-1);
            }
        }
    }
    reload_list();
    }
}


void MainWindow::on_btn_show_clicked()
{
    if(ui->treeWidget->selectedItems().count() >= 1){

        mprocess.start("sh", QStringList() << "-c" << "cat ~/.paria/.paria.list");
        mprocess.waitForFinished(-1);
        QString stdoutraw = mprocess.readAllStandardOutput();
        QStringList stdout = stdoutraw.split("\n");
        QString file_url,file_login,file_pass,file_saveas;
        for(int i = 0;i<stdout.length();i++){
            if(!stdout.at(i).isEmpty()){
                QString line = getBetween(stdout.at(i),"#<url>#","#</url>#");
                QString current_item_line = currentitem->text(3);
                if(line == current_item_line){
                    file_url = getBetween(stdout.at(i),"#<url>#","#</url>#");
                    file_login = getBetween(stdout.at(i),"#<login>#","#</login>#");
                    file_pass = getBetween(stdout.at(i),"#<pass>#","#</pass>#");
                    file_saveas = getBetween(stdout.at(i),"#<saveas>#","#</saveas>#");
                }
            }
        }


        download_dialog *dl = new download_dialog();

        dl->setData(file_url,file_login,file_pass,file_saveas);
        dl->show();
        dl->startDownload();
    }
}

void MainWindow::closeEvent(QCloseEvent *){
    mprocess.start("sh" ,QStringList() << "-c" << "pkill aria2c" );
    mprocess.waitForFinished(-1);
}
