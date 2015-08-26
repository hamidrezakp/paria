#ifndef GET_URL_H
#define GET_URL_H

#include <QWidget>

namespace Ui {
class get_url;
}

class get_url : public QWidget
{
    Q_OBJECT

public:
    explicit get_url(QWidget *parent = 0);
    ~get_url();

private slots:
    void on_btn_cancel_clicked();

    void on_btn_ok_clicked();

private:
    Ui::get_url *ui;
};

#endif // GET_URL_H
