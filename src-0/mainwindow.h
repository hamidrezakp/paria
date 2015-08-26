#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "QTreeWidgetItem"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_btn_add_url_clicked();

    void reload_list();

    QString getBetween(QString, QString, QString);

    void on_treeWidget_itemClicked(QTreeWidgetItem *item, int column);

    void on_btn_delete_clicked();

    void on_btn_show_clicked();

    void closeEvent(QCloseEvent*);


private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
