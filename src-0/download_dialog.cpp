#include "download_dialog.h"
#include "ui_download_dialog.h"
#include "download_complete.h"
#include "QDesktopWidget"
#include "QProcess"
#include "QDebug"
#include "QTimer"
#include "QMessageBox"
#include <ctime>
#include <iostream>

QString surl,slogin,spass,sfolder,newgid,port,pid;
int dc_opens=0;
QProcess sprocess;
QTimer *timer;
QVariant result2;


download_dialog::download_dialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::download_dialog)
{
    ui->setupUi(this);

    const QRect screen = QApplication::desktop()->screenGeometry();
    this->move( screen.center() - this->rect().center() );

    port = generate_port();
    newgid = generate_gid();

    rpc = new MaiaXmlRpcClient(QUrl("http://localhost:" + port + "/rpc"), this);
}

download_dialog::~download_dialog()
{
    delete ui;
}

void download_dialog::setData(QString rawurl,QString rawlogin,QString rawpass,QString rawfolder){
    ui->lbl_url->setText(QUrl::fromPercentEncoding(rawurl.toLatin1()));
    surl = rawurl;
    slogin = rawlogin;
    spass = rawpass;
    sfolder = rawfolder;

}

void download_dialog::startDownload(){

    QString cmnd;
    char *uh = getenv("HOME");
    QString userHome(QString::fromStdString(uh));

    if(slogin.isEmpty() | spass.isEmpty()){
        cmnd = "aria2c --rpc-listen-port=" + port + " --log-level=notice -c -x16 -s16 -j2 -k 1M --dir=" + userHome + "/.paria/.temp" + " --log=" + userHome + "/.paria/logs/LOG_OF_" + surl.split("/").last() + ".log --enable-rpc --rpc-listen-all=true --rpc-allow-origin-all --gid="+ newgid + " '" + surl + "'";
    }
    else{
        if(surl.split("/").first().contains("ftp")){
            cmnd = "aria2c --log-level=notice -c -x16 -s16 -j2 -k 1M --dir=" + userHome + "/.paria/.temp" + " --log=" + userHome + "/.paria/logs/LOG_OF_" + surl.split("/").last() + " --ftp-user=" + slogin + " --ftp-passwd=" + spass + " --enable-rpc  --rpc-allow-origin-all --gid="+ newgid + " --rpc-listen-port=" + port + " '" + surl + "'";
        }else{
        cmnd = "aria2c --rpc-listen-port=" + port + " --log-level=notice -c -x16 -s16 -j2 -k 1M --dir=" + userHome + "/.paria/.temp" + " --log=" + userHome + "/.paria/logs/LOG_OF_" + surl.split("/").last() + " --http-user=" + slogin + " --http-passwd=" + spass + " --enable-rpc --rpc-listen-all=true --rpc-allow-origin-all --gid="+ newgid + " '" + surl + "'";
        }
    }
    int pidstatus;
    sprocess.start("sh", QStringList() << "-c" << "pidof aria2c" );
    sprocess.waitForFinished(-1);
    if(sprocess.readAllStandardOutput().count() == 0){
        pidstatus = 0;
    }else {
        pidstatus = 1;
    }
    sprocess.startDetached("sh", QStringList() << "-c" << cmnd );
    if(pidstatus == 0){
        sprocess.start("sh", QStringList() << "-c" << "pidof aria2c" );
        sprocess.waitForFinished(-1);
        pid = sprocess.readAllStandardOutput().trimmed();

    }else if(pidstatus == 1){
        sprocess.start("sh", QStringList() << "-c" << "pidof aria2c" );
        sprocess.waitForFinished(-1);
        pid = sprocess.readAllStandardOutput().split(' ').first().trimmed();
    }
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), (this), SLOT(relaod_information()));
    timer->start(300);
}

void download_dialog::relaod_information(){
    if(ui->lbl_status->text() != "complete"){
        QMap<QString,QVariant> status = tellStatus(newgid).toMap();
        ui->lbl_status->setText(status.value("status").toString());
        if(ui->lbl_status->text() == "active"){ui->lbl_status->setText("downloading...");}
        ui->lbl_file_size->setText(size_human(status.value("totalLength").toFloat()));
        float downloaded_len = (status.value("completedLength").toFloat() * 100) / status.value("totalLength").toFloat();
        ui->lbl_downloaded->setText(size_human(status.value("completedLength").toFloat()) + " " + QString::number(downloaded_len) + "%");
        ui->progressBar_download->setValue(downloaded_len);
        ui->lbl_transferRate->setText(size_human(status.value("downloadSpeed").toFloat()) + "/sec");
        float left_time = (status.value("totalLength").toFloat() - status.value("completedLength").toFloat()) / status.value("downloadSpeed").toFloat();
        ui->lbl_left->setText(QDateTime::fromTime_t(left_time).toUTC().toString("hh:mm:ss"));
        if(ui->lbl_status->text() == "downloading...")
        {
            this->setWindowTitle("Downloading " + ui->lbl_downloaded->text());}
        else if(ui->lbl_status->text() == "paused"){
            this->setWindowTitle("Paused");
        }else if(ui->lbl_status->text() == "error"){
            this->setWindowTitle("Error in download");
        }else{
            this->setWindowTitle(ui->lbl_status->text());
        }
        save_status();
    }
}

void download_dialog::save_status(){
    QString download_status;
    if(ui->lbl_status->text() == "downloading..." | ui->lbl_status->text() == "paused"){
        download_status = QString::number(ui->progressBar_download->value()) + "%";
    }
    else{
        download_status = ui->lbl_status->text();
    }

    sprocess.start("sh", QStringList() << "-c" << "cat ~/.paria/.paria.list && rm ~/.paria/.paria.list");
    sprocess.waitForFinished(-1);
    QString stdoutraw = sprocess.readAllStandardOutput();
    QStringList stdout = stdoutraw.split("\n");

    for(int i = 0;i<stdout.length();i++){
        if(!stdout.at(i).isEmpty()){
            QString line = getBetween(stdout.at(i),"#<url>#","#</url>#") + getBetween(stdout.at(i),"#<login>#","#</login>#") + getBetween(stdout.at(i),"#<pass>#","#</pass>#");
            QString current_line = surl + slogin + spass ;
            if(line == current_line){
                sprocess.start("sh", QStringList() << "-c" << "echo '#<url>#" + surl + "#</url>##<login>#" + slogin + "#</login>##<pass>#" + spass + "#</pass>##<size>#" + ui->lbl_file_size->text() + "#</size>##<status>#" + download_status + "#</status>##<saveas>#" + sfolder + "#</saveas>#' >> ~/.paria/.paria.list" );
                sprocess.waitForFinished(-1);
            }else{
                sprocess.start("sh", QStringList() << "-c" << "echo '" + stdout.at(i) + "' >> ~/.paria/.paria.list" );
                sprocess.waitForFinished(-1);
            }
        }
    }
    if(ui->lbl_status->text() == "complete"){
//        sprocess.start("kill" , QStringList() << sprocess.pid());

        timer->stop();
        char *uh = getenv("HOME");
        QString userHome(QString::fromStdString(uh));
        QMessageBox msgBox;
        msgBox.setText("Please wait while copying file");
        msgBox.setWindowTitle("Copying file.");
        msgBox.setStandardButtons(0);
        if(dc_opens < 1){
        msgBox.show();
        sprocess.start("sh" , QStringList() << "-c" << "mv " + userHome + "/.paria/.temp/" + surl.split("/").last() + " " + sfolder );
        sprocess.waitForFinished(-1);
        msgBox.close();
        download_complete *dc = new download_complete();
        dc->setdata(surl,sfolder,ui->lbl_file_size->text());
        dc->show();
        dc_opens++;
        }
        this->close();
    }
}



void download_dialog::on_btn_pause_resume_clicked()
{
    if(ui->lbl_status->text() == "downloading..."){
        pause(newgid);
        ui->btn_pause_resume->setText("Resume");
    }
    if(ui->lbl_status->text() == "paused")
    {
        unpause(newgid);
        ui->btn_pause_resume->setText("Pause");
    }
}

QString download_dialog::generate_gid(){
    QString numalpa = "1.2.3.4.5.6.7.8.9.0.a.b.c.d.e.f";
    QStringList hex = numalpa.split(".");
    QString res;
    srand(time(NULL));
    for(int i = 0;i<16;i++){
        res.append(hex.at(rand() % 16));
    }
    return res;
}

QString download_dialog::generate_port(){
    srand(time(NULL));
    return QString::number( rand()%(65535-1024 + 1) + 1024 );
}

void download_dialog::closeEvent(QCloseEvent *event)
{
    sprocess.start("sh" ,QStringList() << "-c" << "kill " + pid );
    sprocess.waitForFinished(-1);
    timer->stop();
}

QString download_dialog::size_human(float number)
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


void download_dialog::on_btn_cancel_clicked()
{
    sprocess.start("sh" ,QStringList() << "-c" << "kill " + pid );
    sprocess.waitForFinished(-1);
    timer->stop();
    this->close();
}

QString download_dialog::getBetween(QString str, QString open, QString close) {
    int startIndex = str.indexOf(open)+open.length();
    if(startIndex <= 0)return QString();
    int endIndex = str.indexOf(close,startIndex);
    if(endIndex <= 0)return QString();
    return str.mid(startIndex,endIndex - startIndex);
}


QVariant download_dialog::pause(QString gid) {
    QVariantList args;
    args << gid;
    rpc->call("aria2.pause", args,this, SLOT(getResponse(QVariant &)),this,SLOT(getFault(int, const QString &)));
    return result2;
}

QVariant download_dialog::unpause(QString gid) {
    QVariantList args;
    args << gid;
    rpc->call("aria2.unpause", args,this, SLOT(getResponse(QVariant &)),this, SLOT(getFault(int, const QString &)));
    return result2;
}

QVariant download_dialog::addUri(QString uri) {
    QVariantList args;
    args << uri;
    rpc->call("aria2.addUri", args,this, SLOT(getResponse(QVariant &)),this, SLOT(getFault(int, const QString &)));
    return result2;
}
QVariant download_dialog::remove(QString gid) {
    QVariantList args;
    args << gid;
    rpc->call("aria2.remove", args,this, SLOT(getResponse(QVariant &)),this, SLOT(getFault(int, const QString &)));
    return result2;
}
QVariant download_dialog::pauseAll() {
    QVariantList args;
    rpc->call("aria2.pauseAll", args,this, SLOT(getResponse(QVariant &)),this, SLOT(getFault(int, const QString &)));
    return result2;
}
QVariant download_dialog::tellStatus(QString gid) {
    QVariantList args;
    args << gid;
    rpc->call("aria2.tellStatus", args,this, SLOT(getResponse(QVariant &)),this, SLOT(getFault(int, const QString &)));

    return result2;
}
QVariant download_dialog::getUris(QString gid) {
    QVariantList args;
    args << gid;
    rpc->call("aria2.getUris", args,this, SLOT(getResponse(QVariant &)),this, SLOT(getFault(int, const QString &)));
    return result2;
}
QVariant download_dialog::getFiles(QString gid) {
    QVariantList args;
    args << gid;
    rpc->call("aria2.getFiles", args,this, SLOT(getResponse(QVariant &)),this, SLOT(getFault(int, const QString &)));
    return result2;
}
QVariant download_dialog::getPeers(QString gid) {
    QVariantList args;
    args << gid;
    rpc->call("aria2.getPeers", args,this, SLOT(getResponse(QVariant &)),this, SLOT(getFault(int, const QString &)));
    return result2;
}
QVariant download_dialog::getServers(QString gid) {
    QVariantList args;
    args << gid;
    rpc->call("aria2.getServers", args,this, SLOT(getResponse(QVariant &)),this, SLOT(getFault(int, const QString &)));
    return result2;
}
QVariant download_dialog::getOption(QString gid) {
    QVariantList args;
    args << gid;
    rpc->call("aria2.getOption", args,this, SLOT(getResponse(QVariant &)),this, SLOT(getFault(int, const QString &)));
    return result2;
}
QVariant download_dialog::changeOption(QString gid,QString options) {
    QVariantList args;
    args << gid << options;
    rpc->call("aria2.changeOption", args,this, SLOT(getResponse(QVariant &)),this, SLOT(getFault(int, const QString &)));
    return result2;
}

void download_dialog::getResponse(QVariant &arg) {
        result2 =  arg;
}

void download_dialog::getFault(int error, const QString &message) {
        qDebug() << "EEE:" << error << "-" << message;
}


