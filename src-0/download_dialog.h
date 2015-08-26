#ifndef DOWNLOAD_DIALOG_H
#define DOWNLOAD_DIALOG_H

#include <QWidget>
#include "maiaXmlRpcClient.h"

namespace Ui {
class download_dialog;
}

class download_dialog : public QWidget
{
    Q_OBJECT

public:
    explicit download_dialog(QWidget *parent = 0);
    ~download_dialog();
public slots:
    void setData(QString,QString,QString,QString);

    void startDownload();

    QString generate_gid();

    void closeEvent(QCloseEvent*);

    void relaod_information();

    QString size_human(float);

    void save_status();

    QString getBetween(QString, QString, QString);


    QVariant pause(QString);

    QVariant unpause(QString);

    QVariant addUri(QString);

    QVariant remove(QString);

    QVariant pauseAll();

    QVariant tellStatus(QString);

    QVariant getUris(QString);

    QVariant getFiles(QString);

    QVariant getPeers(QString);

    QVariant getServers(QString);

    QVariant getOption(QString);

    QVariant changeOption(QString ,QString );

    void getResponse(QVariant &);

    void getFault(int, const QString &) ;

    QString generate_port();

private slots:
    void on_btn_pause_resume_clicked();

    void on_btn_cancel_clicked();

private:
    Ui::download_dialog *ui;
    MaiaXmlRpcClient *rpc;
};

#endif // DOWNLOAD_DIALOG_H
