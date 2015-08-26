#include "get_url.h"
#include "ui_get_url.h"
#include "QMessageBox"
#include "download_info.h"
#include "QProcess"
#include "QDesktopWidget"
#include "QUrl"

get_url::get_url(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::get_url)
{
    ui->setupUi(this);

    const QRect screen = QApplication::desktop()->screenGeometry();
    this->move( screen.center() - this->rect().center() );
}

get_url::~get_url()
{
    delete ui;
}

void get_url::on_btn_cancel_clicked()
{
    this->close();
}

void get_url::on_btn_ok_clicked()
{
    if(ui->le_url->text().isEmpty()){
        QMessageBox msg;
        msg.setText("Please Enter URL !");
        msg.exec();
        return;
    }

    if(ui->use_auth->isChecked()){
        if(ui->le_login->text().isEmpty() | ui->le_pass->text().isEmpty()){
            QMessageBox msg;
            msg.setText("Please Enter Login and Password !");
            msg.exec();
            return;
        }
    }

    download_info *di = new download_info();
    di->show();

    di->setdata(QUrl::fromPercentEncoding(ui->le_url->text().toLatin1()),ui->le_login->text(),ui->le_pass->text());
    this->close();
}
