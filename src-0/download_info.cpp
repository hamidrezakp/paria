#include "download_info.h"
#include "ui_download_info.h"
#include "QProcess"
#include "QDebug"
#include "category.h"
#include "QFileDialog"
#include "QDesktopWidget"
#include "download_dialog.h"

QString login , pass, url;
QStringList categorys;
QStringList categoryimport;
QProcess process;

download_info::download_info(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::download_info)
{
    ui->setupUi(this);

    const QRect screen = QApplication::desktop()->screenGeometry();
    this->move( screen.center() - this->rect().center() );

    reload_lists();
}

void download_info::reload_lists(){
    ui->cb_category->clear();
    categorys.clear();
    categoryimport.clear();
    process.start("sh", QStringList() << "-c" << "cat ~/.paria/.paria.category");
    process.waitForFinished(-1);
    QString stdoutraw = process.readAllStandardOutput();
    QStringList stdout = stdoutraw.split("\n");

    for(int i = 0;i<stdout.length();i++){
        if(!stdout.at(i).isEmpty()){
        categorys.append(getBetween(stdout.at(i),"#<folder>#","#</folder>#") + "|||" + getBetween(stdout.at(i),"#<types>#","#</types>#"));
        }
    }

    for(int i = 0;i< stdout.length();i++){
        if(!stdout.at(i).isEmpty()){
            ui->cb_category->addItem(getBetween(stdout.at(i),"#<name>#","#</name>#"));
        }
    }

}

download_info::~download_info()
{
    delete ui;
}

void download_info::setdata(QString rawurl,QString rawlogin,QString rawpass){
    url = rawurl;
    login = rawlogin;
    pass = rawpass;

    ui->le_url->setText(url);

    if(!login.isEmpty() && !pass.isEmpty()){
        process.start("sh", QStringList() << "-c" << "curl --user " + login + ":" + pass + " --head " + url);
    }else{
        process.start("sh", QStringList() << "-c" << "curl --head " + url);
    }
    process.waitForFinished(-1);
    QString stdoutraw = process.readAllStandardOutput();
    QStringList stdout = stdoutraw.split("\n");

    for(int i = 0;i<stdout.length();i++){
        if(stdout.at(i).contains("Content-Length:")){
            QString string = stdout.at(i);
            string.replace(0,15,QLatin1String(""));
            ui->lbl_size->setText(size_human(string.toFloat()));
        }
    }

    if(ui->cb_category->count() > 0 && ui->cb_category->currentIndex() >= 0){
        QString filename = url.split("/").at(url.split("/").length()-1);
        for (int i = 0 ; i < ui->cb_category->count();i++){
            QStringList types = categorys.at(i).split("|||").at(1).split(",");
            for(int y = 0;y < types.length();y++){
                if(filename.split(".").at(filename.split(".").length()-1) == types.at(y)){
                    ui->cb_category->setCurrentIndex(i);
                }
            }
        }

    }
    QStringList current_category = categorys.at(ui->cb_category->currentIndex()).split("|||");
    QString file_name = url.split("/").at(url.split("/").length()-1);
    ui->le_saveas->setText(current_category.at(0) + "/" + file_name);
}

QString download_info::size_human(float number)
{
    float num = number;
    QStringList list;
    list << "KB" << "MB" << "GB" << "TB";

    QStringListIterator i(list);
    QString unit("bytes");

    while(num >= 1024.0 && i.hasNext())
     {
        unit = i.next();
        num /= 1024.0;
    }
    return QString().setNum(num,'f',2)+" "+unit;
}

void download_info::on_btn_add_category_clicked()
{
    category *cat = new category();
    cat->show();
}

QString download_info::getBetween(QString str, QString open, QString close) {
    int startIndex = str.indexOf(open)+open.length();
    if(startIndex <= 0)return QString();
    int endIndex = str.indexOf(close,startIndex);
    if(endIndex <= 0)return QString();
    return str.mid(startIndex,endIndex - startIndex);
}

void download_info::on_btn_cancel_clicked()
{
    process.terminate();
    this->close();
}

void download_info::on_btn_download_later_clicked()
{
    process.start("sh", QStringList() << "-c" << "cat ~/.paria/.paria.list && rm ~/.paria/.paria.list");
    process.waitForFinished(-1);
    QString stdoutraw = process.readAllStandardOutput();
    QStringList stdout = stdoutraw.split("\n");

    for(int i = 0;i<stdout.length();i++){
        if(!stdout.at(i).isEmpty()){
            QString line = getBetween(stdout.at(i),"#<url>#","#</url>#") + getBetween(stdout.at(i),"#<login>#","#</login>#") + getBetween(stdout.at(i),"#<pass>#","#</pass>#");
            QString current_line = url + login + pass;
            if(line == current_line){
                process.start("sh", QStringList() << "-c" << "echo '#<url>#" + url + "#</url>##<login>#" + login + "#</login>##<pass>#" + pass + "#</pass>##<size>##</size>##<status>##</status>##<saveas>#" + ui->le_saveas->text() + "#</saveas>#' >> ~/.paria/.paria.list" );
                process.waitForFinished(-1);
            }else{
                process.start("sh", QStringList() << "-c" << "echo '" + stdout.at(i) + "' >> ~/.paria/.paria.list" );
                process.waitForFinished(-1);
            }
        }
    }
    if(!stdoutraw.contains(url)){
        process.start("sh", QStringList() << "-c" << "echo '#<url>#" + url + "#</url>##<login>#" + login + "#</login>##<pass>#" + pass + "#</pass>##<size>##</size>##<status>##</status>##<saveas>#" + ui->le_saveas->text() + "#</saveas>#' >> ~/.paria/.paria.list" );
        process.waitForFinished(-1);
    }
    process.terminate();
    this->close();
}

void download_info::on_cb_category_currentIndexChanged(const QString &arg1)
{
    if(ui->cb_category->currentIndex() >= 0){
        QStringList current_category = categorys.at(ui->cb_category->currentIndex()).split("|||");
        QString file_name = url.split("/").at(url.split("/").length()-1);
        ui->le_saveas->setText(current_category.at(0) + "/" + file_name);
    }

}

void download_info::on_btn_browse_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                "/home",
                                                QFileDialog::ShowDirsOnly
                                                | QFileDialog::DontResolveSymlinks);
    QString file_name = url.split("/").at(url.split("/").length()-1);
    ui->le_saveas->setText(dir + "/" + file_name);

}

void download_info::on_btn_reload_categor_list_clicked()
{
    reload_lists();
}

void download_info::on_btn_start_download_clicked()
{
    process.start("sh", QStringList() << "-c" << "cat ~/.paria/.paria.list && rm ~/.paria/.paria.list");
    process.waitForFinished(-1);
    QString stdoutraw = process.readAllStandardOutput();
    QStringList stdout = stdoutraw.split("\n");

    for(int i = 0;i<stdout.length();i++){
        if(!stdout.at(i).isEmpty()){
            QString line = getBetween(stdout.at(i),"#<url>#","#</url>#") + getBetween(stdout.at(i),"#<login>#","#</login>#") + getBetween(stdout.at(i),"#<pass>#","#</pass>#");
            QString current_line = url + login + pass;
            if(line == current_line){
                process.start("sh", QStringList() << "-c" << "echo '#<url>#" + url + "#</url>##<login>#" + login + "#</login>##<pass>#" + pass + "#</pass>##<size>##</size>##<status>##</status>##<saveas>#" + ui->le_saveas->text() + "#</saveas>#' >> ~/.paria/.paria.list" );
                process.waitForFinished(-1);
            }else{
                process.start("sh", QStringList() << "-c" << "echo '" + stdout.at(i) + "' >> ~/.paria/.paria.list" );
                process.waitForFinished(-1);
            }
        }
    }
    if(!stdoutraw.contains(url)){
        process.start("sh", QStringList() << "-c" << "echo '#<url>#" + url + "#</url>##<login>#" + login + "#</login>##<pass>#" + pass + "#</pass>##<size>##</size>##<status>##</status>##<saveas>#" + ui->le_saveas->text() + "#</saveas>#' >> ~/.paria/.paria.list" );
        process.waitForFinished(-1);
    }
    download_dialog *dl = new download_dialog();
    dl->show();
    dl->setData(url,login,pass,ui->le_saveas->text());
    dl->startDownload();
    process.terminate();
    this->close();
}
