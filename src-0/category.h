#ifndef CATEGORY_H
#define CATEGORY_H

#include <QWidget>
#include "QTreeWidgetItem"

namespace Ui {
class category;
}

class category : public QWidget
{
    Q_OBJECT

public:
    explicit category(QWidget *parent = 0);
    ~category();

private slots:
    void on_btn_add_clicked();

    void reload();

    QString getBetween(QString, QString, QString);

    void on_btn_clear_clicked();

    void on_treeWidget_itemClicked(QTreeWidgetItem *item, int column);

    void on_btn_remove_clicked();

    void on_pushButton_clicked();

private:
    Ui::category *ui;
};

#endif // CATEGORY_H
