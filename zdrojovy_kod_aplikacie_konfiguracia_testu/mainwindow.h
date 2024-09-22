#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSql>
#include <QDebug>
#include <QStandardItem>
#include <QMessageBox>
#include "importcontrolparameters.h"
#include "random.h"
#include "sine.h"
#include "shock.h"
#include "snimace.h"
#include "testitemselection.h"
#include <math.h>
#include <ctime>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    bool isTestExisting();
    void SendFiltersToImport();
    vector<CRandomProfiles> CollectRandomProfiles();
    CRandomVibrations CollectRandomVibrations();
    vector<CLevels> CollectLevels();
    vector<CSineProfiles> CollectSineProfiles();
    CSineVibrations CollectSineVibrations();
    CShockProfiles CollectShockProfiles();
    CShocks CollectShocks();
    vector<CToleranceBand> CollectToleranceBand();
    CVibrationsCommon CollectVibrationsCommon();
    void PushButtonTextChangeInsertUpdate();
    void LabelLastTestRefresh();
    void DuplicateEnableDisable();
    void BlockButtonsWhenNoContract();
    QString FilePath();
    void CreateFileTest(QString file_path);
    void BlockButtonsTimesAndLoad();
    QString PadZeros(QVariant number,int order);
    QString FileNameFromMacro();
    ~MainWindow();

signals:
void send_is_test_item(int value, QString year_inqr, QString number_inqr, QString index_inqr);
void send_is_test_arrangement(int value, QString year_inqr, QString number_inqr, QString index_inqr);
void send_to_import_load(bool, QVariant test_year, QString company_name, QString company_name_new_file);

private slots:
    void on_pushButton_load_profiles_clicked();//done
    void on_OK_button_filter_profiles_clicked();//done
    void on_checkBox_inqr_stateChanged();//done
    void on_checkBox_contract_stateChanged();//done
    void on_pushButton_clicked();//done
    void on_pushButton_import_control_parameters_random_clicked();//done
    void receive_test_file_year_number(int test_year, int test_number, int profile_type);//done
    void on_pushButton_sine_import_clicked();//done
    void on_pushButton_sine_calculate_clicked();//done
    void on_pushButton_shock_import_clicked();//done
    void on_pushButton_shock_calculate_clicked();//done
    void on_pushButton_snimace_import_clicked();//done
    void receive_test_file_year_number_snimace(int test_year, int test_number);//done
    void on_pushButton_snimace_refresh_clicked();//done
    void on_pushButton_level_new_clicked();//done
    void on_pushButton_level_delete_clicked();//done
    void on_pushButton_level_import_clicked();//done
    void receive_test_file_year_number_level(int test_year, int test_number);//done
    void on_pushButton_tolerance_new_clicked();//done
    void on_pushButton_tolerance_delete_clicked();//done
    void on_pushButton_tolerance_import_clicked();//done
    void receive_test_file_year_number_tolerance_band(int test_year, int test_number);//done
    void on_pushButton_export_create_test_file_clicked();//done
    void on_pushButton_export_test_item_selection_clicked();//done
    void receive_test_item_year_number(QVariant year, QVariant number);//done
    void receive_test_arrangement_order(int arrang_order);//done
    void on_pushButton_export_generate_clicked();//done
    void on_pushButton_export_insert_info_to_test_parameters_clicked();//done
    void on_pushButton_export_load_info_from_test_parameters_clicked();//done
    void receive_test_file_name_number_year(int test_year, int test_number, QString test_file_name, int profile_type);//done
    void on_pushButton_export_logbook_times_clicked();//done
    void on_pushButton_export_save_actual_parameters_clicked();//done
    void receive_profile(QVariant profile_type, QVariant profile_id, QVariant inqr_year, QVariant inqr_number, QVariant inqr_index);
    void on_comboBox_currentTextChanged();
    void on_pushButton_profily_import_karta_clicked();
    void on_pushButton_export_date_clicked();
    void on_tabWidget_tabBarClicked(int index);

    void on_lineEdit_export_test_year_textChanged();

    void on_lineEdit_export_test_number_textChanged();

    //duplikovanie
    void on_pushButton_export_duplicate_clicked();

    void on_lineEdit_export_company_order_textChanged();

private:
    Ui::MainWindow *ui;
    QSqlTableModel* model;
    QSqlTableModel* model_common;
    QPalette *palette;
    QPalette *palette_enabled;
    QItemSelectionModel* selectionModel;
    QItemSelectionModel* selectionModel_common;
    ImportControlParameters* window_import;
    int test_file_year, test_file_number;
    QList<QComboBox*> list_type_of_input;
    QList<QLineEdit*> list_sensitivity;
    QList<QComboBox*> list_unit;
    QList<QLineEdit*> list_sensitivity_unit;
    QList<QLineEdit*> list_limits_lower;
    QList<QLineEdit*> list_limits_lower_unit;
    QList<QLineEdit*> list_limits_upper;
    QList<QLineEdit*> list_limits_upper_unit;
    QList<QLineEdit*> list_info_text;
    QStandardItemModel* model_level;
    QStandardItemModel* model_tolerance;
    QString file_default_random, file_default_sine, file_default_shock;
    QString file_new_base_path, file_new_base_path_no_order;
    int test_file_year_new, test_file_number_new;
    QString company_order, company_name;
    QString inqr_year, inqr_number, inqr_index;
    TestItemSelection* window_test_item;
    QVariant company_id;
    QString previous_file_path;
};

QVariantList FetchInquiry(int test_year, int test_number);
QVariantList FetchContractAndCompanyID(int inquiry_year, int inquiry_number, int inquiry_index);
QString FetchCompanyName(QVariant company_id);
QVariantList FetchUsingContract(QVariant contract_year, QVariant contract_number);

#endif // MAINWINDOW_H
