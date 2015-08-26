#ifndef DOWNLOAD_COMPLETE_H
#define DOWNLOAD_COMPLETE_H

#include <QWidget>

namespace Ui {
class download_complete;
}

class download_complete : public QWidget
{
    Q_OBJECT

public:
    explicit download_complete(QWidget *parent = 0);
    ~download_complete();
public slots:
    void setdata(QString,QString,QString);

private slots:
    void on_btn_close_clicked();

    void on_btn_open_clicked();

    void on_btn_open_folder_clicked();


private:
    Ui::download_complete *ui;
};

#endif // DOWNLOAD_COMPLETE_H
