#include "category.h"
#include "ui_category.h"
#include "QMessageBox"
#include "QProcess"
#include "QDebug"
#include "download_info.h"
#include "QDesktopWidget"
#include "QFileDialog"

int mode = 0;
QTreeWidgetItem *current;
category::category(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::category)
{
    ui->setupUi(this);

    const QRect screen = QApplication::desktop()->screenGeometry();
    this->move( screen.center() - this->rect().center() );

    QTreeWidgetItem* header = new QTreeWidgetItem();
    header->setText(0,"Category Name");
    header->setText(1,"Folder");
    header->setText(2,"Types");
    ui->treeWidget->setHeaderItem(header);
    ui->treeWidget->setColumnWidth(0,100);
    ui->treeWidget->setColumnWidth(1,200);
    ui->treeWidget->setColumnWidth(2,300);

    reload();

}

category::~category()
{
    delete ui;
}

void category::on_btn_add_clicked()
{
    if(ui->le_folder->text().isEmpty() | ui->le_name->text().isEmpty() | ui->le_types->toPlainText().isEmpty()){
        QMessageBox *msgbox = new QMessageBox();
        msgbox->setText("Please enter name,folder and types!");
        msgbox->exec();
        return;
    }
    if(mode == 0){
        QProcess process;
        process.start("sh", QStringList() << "-c" << "echo '#<name>#" + ui->le_name->text() + "#</name>##<folder>#" + ui->le_folder->text() + "#</folder>##<types>#" + ui->le_types->toPlainText() + "#</types>#' >> ~/.paria/.paria.category" );
        process.waitForFinished(-1);
    }
    if(mode == 1){
        QProcess Process;
        Process.start("sh", QStringList() << "-c" << "cat ~/.paria/.paria.category && rm ~/.paria/.paria.category");
        Process.waitForFinished(-1);
        QString stdoutraw = Process.readAllStandardOutput();
        QStringList stdout = stdoutraw.split("\n");

        for(int i = 0;i<stdout.length();i++){
            if(!stdout.at(i).isEmpty()){
                QString line = getBetween(stdout.at(i),"#<name>#","#</name>#") + getBetween(stdout.at(i),"#<folder>#","#</folder>#") + getBetween(stdout.at(i),"#<types>#","#</types>#");
                QString current_line = current->text(0) + current->text(1) + current->text(2);
                if(line == current_line){
                    QProcess process;
                    process.start("sh", QStringList() << "-c" << "echo '#<name>#" + ui->le_name->text() + "#</name>##<folder>#" + ui->le_folder->text() + "#</folder>##<types>#" + ui->le_types->toPlainText() + "#</types>#' >> ~/.paria/.paria.category" );
                    process.waitForFinished(-1);
                }else{
                    QProcess process;
                    process.start("sh", QStringList() << "-c" << "echo '" + stdout.at(i) + "' >> ~/.paria/.paria.category" );
                    process.waitForFinished(-1);
                }
            }
        }
        QTreeWidgetItem* null = new QTreeWidgetItem();
        null->setText(0,"");
        null->setText(1,"");
        null->setText(2,"");
        current = null;
    }
    reload();
}

void category::reload(){
    ui->treeWidget->clear();
    QProcess Process;
    Process.start("sh", QStringList() << "-c" << "cat ~/.paria/.paria.category");
    Process.waitForFinished(-1);
    QString stdoutraw = Process.readAllStandardOutput();
    QStringList stdout = stdoutraw.split("\n");

    for(int i = 0;i<stdout.length();i++){
        if(!stdout.at(i).isEmpty()){
        QTreeWidgetItem* Item = new QTreeWidgetItem();
        Item->setText(0,getBetween(stdout.at(i),"#<name>#","#</name>#"));
        Item->setText(1,getBetween(stdout.at(i),"#<folder>#","#</folder>#"));
        Item->setText(2,getBetween(stdout.at(i),"#<types>#","#</types>#"));
        ui->treeWidget->insertTopLevelItem(i,Item);
        }
    }
}

QString category::getBetween(QString str, QString open, QString close) {
    int startIndex = str.indexOf(open)+open.length();
    if(startIndex <= 0)return QString();
    int endIndex = str.indexOf(close,startIndex);
    if(endIndex <= 0)return QString();
    return str.mid(startIndex,endIndex - startIndex);
}

void category::on_btn_clear_clicked()
{
    ui->le_folder->clear();
    ui->le_name->clear();
    ui->le_types->clear();
    mode=0;
}

void category::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column)
{
    mode=1;
    current = item;
    ui->le_name->setText(item->text(0));
    ui->le_folder->setText(item->text(1));
    ui->le_types->setText(item->text(2));
}

void category::on_btn_remove_clicked()
{
    if(ui->treeWidget->selectedItems().length() > 0){
    QProcess Process;
    Process.start("sh", QStringList() << "-c" << "cat ~/.paria/.paria.category && rm ~/.paria/.paria.category");
    Process.waitForFinished(-1);
    QString stdoutraw = Process.readAllStandardOutput();
    QStringList stdout = stdoutraw.split("\n");

    for(int i = 0;i<stdout.length();i++){
        if(!stdout.at(i).isEmpty()){
            QString line = getBetween(stdout.at(i),"#<name>#","#</name>#") + getBetween(stdout.at(i),"#<folder>#","#</folder>#") + getBetween(stdout.at(i),"#<types>#","#</types>#");
            QString current_item_line = current->text(0) + current->text(1) + current->text(2);
            if(line == current_item_line){
                continue;
            }else{
                QProcess process;
                process.start("sh", QStringList() << "-c" << "echo '" + stdout.at(i) + "' >> ~/.paria/.paria.category" );
                process.waitForFinished(-1);
            }
        }
    }
    reload();
    }
}

void category::on_pushButton_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                "/home",
                                                QFileDialog::ShowDirsOnly
                                                | QFileDialog::DontResolveSymlinks);
    ui->le_folder->setText(dir);
}
