#include "download_complete.h"
#include "ui_download_complete.h"
#include "QDesktopServices"
#include "QUrl"
#include "QDesktopWidget"

QString furl,ffolder,ffile_size;
download_complete::download_complete(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::download_complete)
{
    ui->setupUi(this);

    const QRect screen = QApplication::desktop()->screenGeometry();
    this->move( screen.center() - this->rect().center() );
}

download_complete::~download_complete()
{
    delete ui;
}

void download_complete::setdata(QString url,QString folder,QString file_size){
    furl = url;
    ffolder = folder;
    ffile_size = file_size;

    ui->lbl_saved_as->setText(ffolder);
    ui->lbl_size_of_downlaod->setText("Downloaded " + ffile_size);
    ui->lbl_url->setText(furl);
}

void download_complete::on_btn_close_clicked()
{
    this->close();
}

void download_complete::on_btn_open_clicked()
{
    QDesktopServices::openUrl(QUrl("file:///" + ffolder));
    this->close();
}

void download_complete::on_btn_open_folder_clicked()
{
    QString file_name = ffolder.split("/").last();
    QString folder_location = ffolder.replace(file_name,"");
    QDesktopServices::openUrl(QUrl("file:///" + folder_location));
    this->close();
}


