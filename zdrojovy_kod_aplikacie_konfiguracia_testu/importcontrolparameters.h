#ifndef IMPORTCONTROLPARAMETERS_H
#define IMPORTCONTROLPARAMETERS_H

#include <QWidget>
#include <QtSql>
#include <QDebug>
#include <QStandardItem>

namespace Ui {
class ImportControlParameters;
}

class ImportControlParameters : public QWidget
{
    Q_OBJECT

public:
    explicit ImportControlParameters(QWidget *parent = nullptr);
    void SetColumnNames();
    void CombinedFilter();
    QModelIndexList SelectedRowData();
    ~ImportControlParameters();

signals:
void send_test_file_name_year(int test_file_year, int test_file_number, int profile_type);
void send_test_file_name_year_snimace(int test_file_year, int test_file_number);
void send_test_file_name_year_level(int test_file_year, int test_file_number);
void send_test_file_name_year_tolerance_band(int test_file_year, int test_file_number);
void send_test_file_name_number_year_to_export(int test_file_year, int test_file_number, QString test_file_name, int profile_type);
void send_profile(QVariant profile_type, QVariant profile_id, QVariant inqr_year, QVariant inqr_number, QVariant inqr_index);

private slots:
    void on_pushButton_load_files_clicked();//done
    void on_pushButton_import_parameters_clicked();//done
    void on_pushButton_import_input_channels_clicked();//done
    void on_pushButton_import_level_clicked();//done
    void on_pushButton_import_tolerance_clicked();//done
    void on_pushButton_import_info_clicked();//done
    void on_pushButton_import_profile_from_test_file_clicked();
    void on_pushButton_import_whole_file_clicked();
    void receive_load_all(bool, QVariant test_year, QString company_name, QString company_name_new_file);
    void on_lineEdit_import_filter_year_editingFinished();
    void on_lineEdit_import_filter_company_name_editingFinished();

private:
    Ui::ImportControlParameters *ui;
    QSqlQueryModel* model;
    QItemSelectionModel* selectionModel;
};

#endif // IMPORTCONTROLPARAMETERS_H
