#ifndef COMMONSETTINGSRANDOM_H
#define COMMONSETTINGSRANDOM_H

#include <QWidget>
#include <QtSql>
#include <QDebug>
#include <QStandardItem>
#include <QTableView>
#include "delegate_random_prof.h"

namespace Ui {
class CommonSettingsRandom;
}

class CommonSettingsRandom : public QWidget
{
    Q_OBJECT

public:
    explicit CommonSettingsRandom(QWidget *parent = nullptr);
    void SetColumnNames(QAbstractItemModel* model_in, QTableView* table_view);
    void SetTable(QSqlTableModel *model_in);
    ~CommonSettingsRandom();

private slots:
    void receive_new_id(int new_profile_id);//*
    void on_pushButton_load_common_random_clicked();//*
    void on_pushButton_submit_common_clicked();//*
    void on_pushButton_delete_clicked();//*
    void on_pushButton_drag_profile_clicked();//*
    void on_pushButton_new_record_clicked();//*
    void on_pushButton_delete_new_record_clicked();//*
    void on_pushButton_submit_new_record_clicked();//*
    void on_pushButton_revert_common_clicked();//*
    void on_pushButton_OK_filter_common_clicked();//*
    void on_checkBox_inqr_stateChanged();//*
    void on_checkBox_contract_stateChanged();//*

private:
    Ui::CommonSettingsRandom *ui;
    QSqlTableModel* model;
    Delegate_random_prof* delegate;
    QItemSelectionModel* selectionModel;
    int new_id;
    QPalette *palette;
};

#endif // COMMONSETTINGSRANDOM_H
