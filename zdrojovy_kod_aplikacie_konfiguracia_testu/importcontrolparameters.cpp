#include "importcontrolparameters.h"
#include "ui_importcontrolparameters.h"

ImportControlParameters::ImportControlParameters(QWidget *parent) :QWidget(parent), ui(new Ui::ImportControlParameters)
{
    ui->setupUi(this);

    model = new QSqlQueryModel(this);

    selectionModel = new QItemSelectionModel();
}

void ImportControlParameters::SetColumnNames()
{
    model->setHeaderData(0, Qt::Horizontal, "Test file year", Qt::DisplayRole);
    model->setHeaderData(1, Qt::Horizontal, "Test file number", Qt::DisplayRole);
    model->setHeaderData(2, Qt::Horizontal, "Test file name", Qt::DisplayRole);
    model->setHeaderData(3, Qt::Horizontal, "Customer inquiry year", Qt::DisplayRole);
    model->setHeaderData(4, Qt::Horizontal, "Customer inquiry number", Qt::DisplayRole);
    model->setHeaderData(5, Qt::Horizontal, "Customer inquiry index", Qt::DisplayRole);
    model->setHeaderData(6, Qt::Horizontal, "Profile Subtype", Qt::DisplayRole);
    model->setHeaderData(7, Qt::Horizontal, "Profile ID", Qt::DisplayRole);

    ui->tableView_import_control_parameters->setModel(model);

    ui->tableView_import_control_parameters->setColumnWidth(2, 180);
    ui->tableView_import_control_parameters->setColumnWidth(3, 160);
    ui->tableView_import_control_parameters->setColumnWidth(4, 160);
    ui->tableView_import_control_parameters->setColumnWidth(5, 160);
}


ImportControlParameters::~ImportControlParameters()
{
    delete ui;
    delete model;
    QSqlDatabase::database("tests_parameters", 0).close();
}


/*nacitanie existujucich suborov*/
void ImportControlParameters::on_pushButton_load_files_clicked()
{
    QSqlDatabase::database("tests_parameters").open();

    QSqlQuery query(QSqlDatabase::database("tests_parameters"));

    query.exec("SELECT testFileYear, testFileNumber, test_parts.testFileName, relCustomerInquiryYear, relCustomerInquiryNumber, "
               "relCustomerInquiryIndex, test_parts.profileSubtype, test_parts.relProfileId FROM main_tests_table INNER JOIN test_parts ON (main_tests_table.testFileYear = "
               "test_parts.adTestFileYear AND main_tests_table.testFileNumber = test_parts.adTestFileNumber)");

    model->setQuery(query);

    SetColumnNames();
}


/*presun test_file_year, test_file_number do main window*/
void ImportControlParameters::on_pushButton_import_parameters_clicked()
{
    selectionModel = ui->tableView_import_control_parameters->selectionModel();
    QModelIndexList indexes = SelectedRowData();

    if(!indexes.isEmpty())
    {
        int test_year = indexes[0].data().toInt();
        int test_number = indexes[1].data().toInt();
        int profile_type = indexes[6].data().toInt();
        emit send_test_file_name_year(test_year, test_number, profile_type);
    }
}


/*presun test_file_year, test_file_number do main window pre nastavenia snimacov*/
void ImportControlParameters::on_pushButton_import_input_channels_clicked()
{
    selectionModel = ui->tableView_import_control_parameters->selectionModel();
    QModelIndexList indexes = SelectedRowData();

    if(!indexes.isEmpty())
    {
        int test_year = indexes[0].data().toInt();
        int test_number = indexes[1].data().toInt();
        emit send_test_file_name_year_snimace(test_year, test_number);
    }
}


/* presun test_file_year, test_file_number do main window pre nastavenia level */
void ImportControlParameters::on_pushButton_import_level_clicked()
{
    selectionModel = ui->tableView_import_control_parameters->selectionModel();
    QModelIndexList indexes = SelectedRowData();

    if(!indexes.isEmpty())
    {
        int test_year = indexes[0].data().toInt();
        int test_number = indexes[1].data().toInt();
        emit send_test_file_name_year_level(test_year, test_number);
    }
}


/* presun test_file_year, test_file_number do main window pre nastavenia tolerance band */
void ImportControlParameters::on_pushButton_import_tolerance_clicked()
{
    selectionModel = ui->tableView_import_control_parameters->selectionModel();
    QModelIndexList indexes = SelectedRowData();

    if(!indexes.isEmpty())
    {
        int test_year = indexes[0].data().toInt();
        int test_number = indexes[1].data().toInt();
        emit send_test_file_name_year_tolerance_band(test_year, test_number);
    }
}

/*********** presun test_file_year, test_file_number, test file name a type profilu do karty export *************/
void ImportControlParameters::on_pushButton_import_info_clicked()
{
    selectionModel = ui->tableView_import_control_parameters->selectionModel();
    QModelIndexList indexes = SelectedRowData();

    if(!indexes.isEmpty())
    {
        int test_year = indexes.first().data().toInt();
        int test_number = indexes.at(1).data().toInt();
        QString test_file_name = indexes[2].data().toString();
        int profile_type = indexes.at(6).data().toInt();

        emit send_test_file_name_number_year_to_export(test_year, test_number, test_file_name, profile_type);
    }
}


/********ziska data z oznaceneho riadku*********/
QModelIndexList ImportControlParameters::SelectedRowData()
{
    selectionModel = ui->tableView_import_control_parameters->selectionModel();
    QModelIndexList indexes = selectionModel->selectedIndexes();

    if(!indexes.isEmpty())
    {
        int sel_row = indexes.first().row();
        int columns = model->columnCount();

        QModelIndex topLeft = model->index(sel_row, 0);
        QModelIndex bottomRight = model->index(sel_row, columns-1);
        QItemSelection selection(topLeft, bottomRight);

        selectionModel->select(selection, QItemSelectionModel::Select);
        QModelIndexList indexes_out = selectionModel->selectedIndexes();
        return indexes_out;
    }

    QModelIndexList indexes_empty;
    return indexes_empty;
}


/***********import profilu testovacieho suboru**********/
void ImportControlParameters::on_pushButton_import_profile_from_test_file_clicked()
{
    selectionModel = ui->tableView_import_control_parameters->selectionModel();
    QModelIndexList indexes = SelectedRowData();

    if(!indexes.isEmpty())
    {
        QVariant inqr_year = indexes[3].data();
        QVariant inqr_number = indexes[4].data();
        QVariant inqr_index = indexes[5].data();
        int profile_type = indexes[6].data().toInt();
        int profile_id = indexes[7].data().toInt();
        emit send_profile(profile_type, profile_id, inqr_year, inqr_number, inqr_index);
    }
}


/*********globalne tlacitko, nacita cely subor do uzivatelskeho rozhrania********/
void ImportControlParameters::on_pushButton_import_whole_file_clicked()
{
    ui->pushButton_import_info->clicked();
    ui->pushButton_import_profile_from_test_file->clicked();
    ui->pushButton_import_parameters->clicked();
    ui->pushButton_import_input_channels->clicked();

    selectionModel = ui->tableView_import_control_parameters->selectionModel();
    QModelIndexList indexes = SelectedRowData();
    if(!indexes.isEmpty())
    {
        int profile_type = indexes[6].data().toInt();

        if(profile_type == 1)
            ui->pushButton_import_level->clicked();
        else if(profile_type == 2)
            ui->pushButton_import_tolerance->clicked();
    }
}


/*****automaticke nacitanie zoznamu suborov, pripadne predfiltrovanie******/
void ImportControlParameters::receive_load_all(bool file_exists, QVariant test_year, QString company_name, QString company_name_new_file)
{
    ui->pushButton_load_files->clicked();

    if(file_exists)
    {
        ui->lineEdit_import_filter_year->setText(test_year.toString());
        ui->lineEdit_import_filter_company_name->setText(company_name);
        ui->lineEdit_import_filter_company_name->editingFinished();
    }

    else if(!file_exists)
    {
        ui->lineEdit_import_filter_year->setText("");
        ui->lineEdit_import_filter_company_name->setText(company_name_new_file);
        ui->lineEdit_import_filter_company_name->editingFinished();
    }
}


/*****funkcia oboch filtrov naraz****/
void ImportControlParameters::CombinedFilter()
{
    if (!ui->lineEdit_import_filter_year->text().isEmpty() && !ui->lineEdit_import_filter_company_name->text().isEmpty())
    {
        QSqlDatabase::database("tests_parameters").open();

        QSqlQuery query(QSqlDatabase::database("tests_parameters"));

        query.prepare("SELECT testFileYear, testFileNumber, test_parts.testFileName, relCustomerInquiryYear, relCustomerInquiryNumber, "
                   "relCustomerInquiryIndex, test_parts.profileSubtype, test_parts.relProfileId FROM main_tests_table "
                      "INNER JOIN test_parts ON (main_tests_table.testFileYear = "
                   "test_parts.adTestFileYear AND main_tests_table.testFileNumber = test_parts.adTestFileNumber AND "
                      "main_tests_table.testFileYear = ? AND test_parts.testFileName LIKE '%" + ui->lineEdit_import_filter_company_name->text() +"%')");

        query.bindValue(0, ui->lineEdit_import_filter_year->text());

        query.exec();

        model->setQuery(query);

        SetColumnNames();
    }
}


/*****filter iba podla roku, po ukonceni editu*******/
void ImportControlParameters::on_lineEdit_import_filter_year_editingFinished()
{
    CombinedFilter();

    if (!ui->lineEdit_import_filter_year->text().isEmpty() && ui->lineEdit_import_filter_company_name->text().isEmpty())
    {
        QSqlDatabase::database("tests_parameters").open();

        QSqlQuery query(QSqlDatabase::database("tests_parameters"));

        query.prepare("SELECT testFileYear, testFileNumber, test_parts.testFileName, relCustomerInquiryYear, relCustomerInquiryNumber, "
                   "relCustomerInquiryIndex, test_parts.profileSubtype, test_parts.relProfileId FROM main_tests_table INNER JOIN test_parts ON (main_tests_table.testFileYear = "
                   "test_parts.adTestFileYear AND main_tests_table.testFileNumber = test_parts.adTestFileNumber AND main_tests_table.testFileYear = ?)");

        query.bindValue(0, ui->lineEdit_import_filter_year->text());

        query.exec();

        model->setQuery(query);

        SetColumnNames();
    }
}


/********filter iba podla firmy, po ukonceni editu******/
void ImportControlParameters::on_lineEdit_import_filter_company_name_editingFinished()
{
    CombinedFilter();

    if (!ui->lineEdit_import_filter_company_name->text().isEmpty() && ui->lineEdit_import_filter_year->text().isEmpty())
    {
        QSqlDatabase::database("tests_parameters").open();

        QSqlQuery query(QSqlDatabase::database("tests_parameters"));

        query.exec("SELECT testFileYear, testFileNumber, test_parts.testFileName, relCustomerInquiryYear, relCustomerInquiryNumber, "
                   "relCustomerInquiryIndex, test_parts.profileSubtype, test_parts.relProfileId FROM main_tests_table INNER JOIN test_parts ON (main_tests_table.testFileYear = "
                   "test_parts.adTestFileYear AND main_tests_table.testFileNumber = test_parts.adTestFileNumber AND "
                   "test_parts.testFileName LIKE '%" + ui->lineEdit_import_filter_company_name->text() +"%')");

        model->setQuery(query);

        SetColumnNames();
    }
}
