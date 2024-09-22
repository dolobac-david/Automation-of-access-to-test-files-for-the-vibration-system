#include "profiletoinquiery.h"
#include "ui_profiletoinquiery.h"


ProfileToInquiery::ProfileToInquiery(QWidget *parent) : QWidget(parent), ui(new Ui::ProfileToInquiery)
{
    ui->setupUi(this);

    model = new QSqlQueryModel(this);
    selectionModel = new QItemSelectionModel;

    ui->lineEdit_manual_profile->setReadOnly(1);
    palette = new QPalette();
    palette->setColor(QPalette::Base,Qt::gray);
    palette->setColor(QPalette::Text,Qt::darkGray);
    ui->lineEdit_manual_profile->setPalette(*palette);

    ui->lineEdit_inqr_id->setReadOnly(1);
    ui->lineEdit_inqr_year->setReadOnly(1);
    ui->lineEdit_inqr_index->setReadOnly(1);
    palette = new QPalette();
    palette->setColor(QPalette::Base,Qt::gray);
    palette->setColor(QPalette::Text,Qt::darkGray);
    ui->lineEdit_inqr_id->setPalette(*palette);
    ui->lineEdit_inqr_year->setPalette(*palette);
    ui->lineEdit_inqr_index->setPalette(*palette);

    ui->lineEdit_contract_id->setReadOnly(1);
    ui->lineEdit_contract_year->setReadOnly(1);
    palette = new QPalette();
    palette->setColor(QPalette::Base,Qt::gray);
    palette->setColor(QPalette::Text,Qt::darkGray);
    ui->lineEdit_contract_id->setPalette(*palette);
    ui->lineEdit_contract_year->setPalette(*palette);
}


ProfileToInquiery::~ProfileToInquiery()
{
    delete ui;
    delete model;
    delete palette;
    QSqlDatabase::database("kniha_zakazek", 0).close();
}


/******nastavenie mien stlpcov*******/
void ProfileToInquiery::SetColumnNames(QSqlQueryModel *model_in, QTableView *table_view)
{
    model_in->setHeaderData(0, Qt::Horizontal, "Inquiry year", Qt::DisplayRole);
    model_in->setHeaderData(1, Qt::Horizontal, "Inquiry number", Qt::DisplayRole);
    model_in->setHeaderData(2, Qt::Horizontal, "Inquiry index", Qt::DisplayRole);
    model_in->setHeaderData(3, Qt::Horizontal, "Contract year", Qt::DisplayRole);
    model_in->setHeaderData(4, Qt::Horizontal, "Contract number", Qt::DisplayRole);
    model_in->setHeaderData(5, Qt::Horizontal, "Company ID", Qt::DisplayRole);
    model_in->setHeaderData(6, Qt::Horizontal, "Inquiry description", Qt::DisplayRole);
    model_in->setHeaderData(7, Qt::Horizontal, "Company name", Qt::DisplayRole);
    model_in->setHeaderData(8, Qt::Horizontal, "Company name 2", Qt::DisplayRole);

    table_view->setColumnWidth(6, 150);
    table_view->setColumnWidth(7, 200);
    table_view->setColumnWidth(8, 150);
}


/****nacitanie DB*****/
void ProfileToInquiery::on_pushButton_load_inquiry_clicked()
{
    QSqlDatabase::database("kniha_zakazek").open();

    QSqlQuery query(QSqlDatabase::database("kniha_zakazek"));
    query.exec("SELECT customerInquiryYear, customerInquiryNumber, customerInquiryIndex, contractYear, contractNumber, main_list.relCompanyIdNumber,"
               " customerInquiryDescription, companies.companyNameRow1, companies.companyNameRow2 FROM main_list LEFT OUTER JOIN companies ON main_list.relCompanyIdNumber = "
               "companies.companyIdNumber");

    model->setQuery(query);

    SetColumnNames(model, ui->tableView_inquiry);

    ui->tableView_inquiry->setModel(model);
}


/*****ziskanie ID z hlavneho okna******/
void ProfileToInquiery::receive_new_id(int new_profile_id)
{
    new_id = new_profile_id;
}



/*******priradenie profilu k ponuke******/
void ProfileToInquiery::on_pushButton_assign_inquiry_clicked()
{
    if(model->rowCount() != 0)
    {
        selectionModel = ui->tableView_inquiry->selectionModel();
        QModelIndexList rows = selectionModel->selectedRows();

        if(rows.count() == 1)
        {
            QModelIndexList indexes = selectionModel->selectedIndexes();
            std::vector<QVariant> tmp;

            for(QModelIndex index: indexes)
            {
                QVariant value = selectionModel->model()->data(index);
                tmp.push_back(value);
            }

            QSqlDatabase::database().open();

            QSqlQuery query(QSqlDatabase::database());
            query.prepare("INSERT INTO relations_inquires_vs_profiles_vs_types VALUES(:adCustomerInquiryYear, "
                          ":adCustomerInquiryNumber, :adCustomerInquiryIndex, :adProfileType, :adProfileSubtype, :adProfileId)");

            query.bindValue(":adCustomerInquiryYear", tmp.at(0));
            query.bindValue(":adCustomerInquiryNumber", tmp.at(1));
            query.bindValue(":adCustomerInquiryIndex", tmp.at(2));
            query.bindValue(":adProfileType", 0);
            query.bindValue(":adProfileSubtype", ui->comboBox->currentIndex());

            if (ui->checkBox_profile_manual->isChecked())
            {
                if (!(ui->lineEdit_manual_profile->text().isEmpty()))
                    query.bindValue(":adProfileId", ui->lineEdit_manual_profile->text());
            }

            else
                query.bindValue(":adProfileId", new_id);

            query.exec();
        }
    }
}


/*****aktivacia manualneho zadania profilu**********/
void ProfileToInquiery::on_checkBox_profile_manual_stateChanged()
{
    if(ui->checkBox_profile_manual->isChecked())
    {
        ui->lineEdit_manual_profile->setReadOnly(0);
        palette->setColor(QPalette::Base,Qt::white);
        palette->setColor(QPalette::Text,Qt::black);
        ui->lineEdit_manual_profile->setPalette(*palette);
    }

    else
    {
        ui->lineEdit_manual_profile->setReadOnly(1);
        palette->setColor(QPalette::Base,Qt::gray);
        palette->setColor(QPalette::Text,Qt::darkGray);
        ui->lineEdit_manual_profile->setPalette(*palette);
    }
}


/*filter podla poptavky/zakazky*/
void ProfileToInquiery::on_pushButton_filter_inqr_clicked()
{
    //ak je zaskrtnuty checkbox a vyplnene polia poptavky
    if (ui->checkBox_inqr->isChecked() && !ui->lineEdit_inqr_id->text().isEmpty() && !ui->lineEdit_inqr_year->text().isEmpty() && !ui->lineEdit_inqr_index->text().isEmpty())
    {
        QSqlDatabase::database("kniha_zakazek").open();

        QSqlQuery query(QSqlDatabase::database("kniha_zakazek"));
        query.prepare("SELECT customerInquiryYear, customerInquiryNumber, customerInquiryIndex, contractYear, contractNumber, main_list.relCompanyIdNumber,"
                   " customerInquiryDescription, companies.companyNameRow1, companies.companyNameRow2 FROM main_list LEFT OUTER JOIN companies ON main_list.relCompanyIdNumber = "
                   "companies.companyIdNumber WHERE main_list.customerInquiryYear = ? AND main_list.customerInquiryNumber = ? AND main_list.customerInquiryIndex = ?");

        query.bindValue(0, ui->lineEdit_inqr_year->text());
        query.bindValue(1, ui->lineEdit_inqr_id->text());
        query.bindValue(2, ui->lineEdit_inqr_index->text());

        query.exec();
        model->setQuery(query);
    }

    /*zakazka*/
    else if(ui->checkBox_contract->isChecked() && !(ui->lineEdit_contract_id->text().isEmpty() && ui->lineEdit_contract_year->text().isEmpty()))
    {
        QSqlDatabase::database("kniha_zakazek").open();

        QSqlQuery query(QSqlDatabase::database("kniha_zakazek"));
        query.prepare("SELECT customerInquiryYear, customerInquiryNumber, customerInquiryIndex, contractYear, contractNumber, main_list.relCompanyIdNumber,"
                   " customerInquiryDescription, companies.companyNameRow1, companies.companyNameRow2 FROM main_list LEFT OUTER JOIN companies ON main_list.relCompanyIdNumber = "
                   "companies.companyIdNumber WHERE main_list.contractYear = ? AND main_list.contractNumber = ?");

        query.bindValue(0, ui->lineEdit_contract_year->text());
        query.bindValue(1, ui->lineEdit_contract_id->text());

        query.exec();
        model->setQuery(query);
    }

    SetColumnNames(model, ui->tableView_inquiry);
    ui->tableView_inquiry->setModel(model);
}


/****aktivacia filtru ponuky****/
void ProfileToInquiery::on_checkBox_inqr_stateChanged()
{
    if(ui->checkBox_inqr->isChecked())
    {
        ui->lineEdit_inqr_id->setReadOnly(0);
        ui->lineEdit_inqr_year->setReadOnly(0);
        ui->lineEdit_inqr_index->setReadOnly(0);
        palette->setColor(QPalette::Base,Qt::white);
        palette->setColor(QPalette::Text,Qt::black);
        ui->lineEdit_inqr_id->setPalette(*palette);
        ui->lineEdit_inqr_year->setPalette(*palette);
        ui->lineEdit_inqr_index->setPalette(*palette);
    }

    else
    {
        ui->lineEdit_inqr_id->setReadOnly(1);
        ui->lineEdit_inqr_year->setReadOnly(1);
        ui->lineEdit_inqr_index->setReadOnly(1);
        palette->setColor(QPalette::Base,Qt::gray);
        palette->setColor(QPalette::Text,Qt::darkGray);
        ui->lineEdit_inqr_id->setPalette(*palette);
        ui->lineEdit_inqr_year->setPalette(*palette);
        ui->lineEdit_inqr_index->setPalette(*palette);
    }
}


/****aktivacia filtru zakazky****/
void ProfileToInquiery::on_checkBox_contract_stateChanged()
{
    if(ui->checkBox_contract->isChecked())
    {
        ui->lineEdit_contract_id->setReadOnly(0);
        ui->lineEdit_contract_year->setReadOnly(0);
        palette->setColor(QPalette::Base,Qt::white);
        palette->setColor(QPalette::Text,Qt::black);
        ui->lineEdit_contract_id->setPalette(*palette);
        ui->lineEdit_contract_year->setPalette(*palette);
    }

    else
    {
        ui->lineEdit_contract_id->setReadOnly(1);
        ui->lineEdit_contract_year->setReadOnly(1);
        palette->setColor(QPalette::Base,Qt::gray);
        palette->setColor(QPalette::Text,Qt::darkGray);
        ui->lineEdit_contract_id->setPalette(*palette);
        ui->lineEdit_contract_year->setPalette(*palette);
    }
}
