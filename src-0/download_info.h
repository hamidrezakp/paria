#ifndef DOWNLOAD_INFO_H
#define DOWNLOAD_INFO_H

#include <QWidget>

namespace Ui {
class download_info;
}

class download_info : public QWidget
{
    Q_OBJECT

public:
    explicit download_info(QWidget *parent = 0);
    ~download_info();

public slots:
    void setdata(QString,QString,QString);

    QString size_human(float);

    QString getBetween(QString, QString, QString);

    void reload_lists();

private slots:
    void on_btn_add_category_clicked();

    void on_btn_cancel_clicked();

    void on_btn_download_later_clicked();

    void on_cb_category_currentIndexChanged(const QString &arg1);

    void on_btn_browse_clicked();

    void on_btn_reload_categor_list_clicked();

    void on_btn_start_download_clicked();

private:
    Ui::download_info *ui;
};

#endif // DOWNLOAD_INFO_H
