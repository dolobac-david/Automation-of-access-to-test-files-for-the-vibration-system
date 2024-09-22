#ifndef PROFILETOINQUIERY_H
#define PROFILETOINQUIERY_H

#include <QWidget>
#include <QtSql>
#include <QDebug>
#include <QTableView>
#include <QStandardItem>

namespace Ui {
class ProfileToInquiery;
}

class ProfileToInquiery : public QWidget
{
    Q_OBJECT

public:
    explicit ProfileToInquiery(QWidget *parent = nullptr);
    void SetColumnNames(QSqlQueryModel* model_in, QTableView* table_view);
    ~ProfileToInquiery();

    private slots:
        void on_pushButton_load_inquiry_clicked();//done
        void on_pushButton_assign_inquiry_clicked();//done
        void receive_new_id(int new_profile_id);//done
        void on_checkBox_profile_manual_stateChanged();//done
        void on_checkBox_inqr_stateChanged();//done
        void on_pushButton_filter_inqr_clicked();//done
        void on_checkBox_contract_stateChanged();//done

private:
    Ui::ProfileToInquiery *ui;
    QSqlQueryModel* model;
    QItemSelectionModel* selectionModel;
    int new_id;
    QPalette *palette;
};

#endif // PROFILETOINQUIERY_H
