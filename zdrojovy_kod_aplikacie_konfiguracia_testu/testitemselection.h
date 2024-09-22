#ifndef TESTITEMSELECTION_H
#define TESTITEMSELECTION_H

#include <QWidget>
#include <QtSql>
#include <QDebug>

namespace Ui {
class TestItemSelection;
}

class TestItemSelection : public QWidget
{
    Q_OBJECT

public:
    explicit TestItemSelection(QWidget *parent = nullptr);
    ~TestItemSelection();

signals:
void send_test_item_year_number(QVariant year, QVariant number);
void send_test_arrangement_order(int arrang_order);

private slots:
    void receive_is_test_item(int value, QString year_inqr, QString number_inqr, QString index_inqr);//done
    void on_pushButton_test_item_select_clicked();//done
    void receive_is_test_arrangement(int value, QString year_inqr, QString number_inqr, QString index_inqr);//done
    void on_pushButton_test_arrangement_select_clicked();//done

private:
    Ui::TestItemSelection *ui;
    QSqlQueryModel* model_test_item;
    QItemSelectionModel* selectionModel;
    QSqlQueryModel* model_test_arrangement;
};

int FetchProtocolID(QString year_inqr, QString number_inqr, QString index_inqr);

#endif // TESTITEMSELECTION_H
