#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSql>
#include <QDebug>
#include <QStandardItem>
#include <QTableView>
#include "delegate_random_prof.h"
#include "commonsettingsrandom.h"
#include "profiletoinquiery.h"
#include <fstream>
#include <math.h>
#include "ramp.h"

using namespace std;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    void SetColumnNames(QAbstractItemModel* model_in, QTableView* table_view);
    void SetTable(QSqlTableModel *model_in);
    ~MainWindow();

    signals:
    void send_new_id(int new_id);//*
    void send_to_ramp(QList<QVariant> list);//*

private slots:
    void on_pushButton_load_random_clicked();//*
    void on_pushButton_submit_random_clicked();//*
    void on_pushButton_drag_profile_clicked();//*
    void on_pushButton_new_record_clicked();//*
    void on_pushButton_delete_new_record_clicked();//*
    void on_pushButton_submit_new_record_clicked();//*
    void on_pushButton_delete_record_clicked();//*
    void on_pushButton_common_settings_clicked();//*
    void on_pushButton_revert_random_clicked();//*
    void on_pushButton_inquiery_clicked();//*
    void on_OK_button_filter_main_profiles_clicked();//*
    void on_checkBox_inqr_stateChanged();//*
    void on_checkBox_order_stateChanged();//*
    void receive_section_row();//*
    void on_lineEdit_v_editingFinished();//*
    void on_lineEdit_d_editingFinished();//*
    void receive_section_row_bottom();//*
    void on_lineEdit_v_bottom_editingFinished();//*
    void on_lineEdit_d_bottom_editingFinished();//*
    void on_comboBox_limits_currentTextChanged();//*
    void on_pushButton_ramp_clicked();//*
    void on_comboBox_currentTextChanged();//*
    void receive_new_data_from_ramp(QList<QVariant> list);//*

private:
    Ui::MainWindow *ui;
    QSqlTableModel* model;
    Delegate_random_prof* delegate;
    QItemSelectionModel* selectionModel;
    CommonSettingsRandom* window;
    int new_id;
    ProfileToInquiery* inquiery_window;
    QPalette *palette;
    Ramp* ramp_window;
};

#endif // MAINWINDOW_H
