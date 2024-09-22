#include "commonsettingsrandom.h"
#include "ui_commonsettingsrandom.h"


CommonSettingsRandom::CommonSettingsRandom(QWidget *parent) :QWidget(parent), ui(new Ui::CommonSettingsRandom)
{
    ui->setupUi(this);

    model = new QSqlTableModel(this);
    selectionModel = new QItemSelectionModel();
    delegate = new Delegate_random_prof(this);

    ui->lineEdit_inqr_id->setReadOnly(1);
    ui->lineEdit_inqr_year->setReadOnly(1);
    ui->lineEdit_inqr_index->setReadOnly(1);
    ui->lineEdit_contract_number->setReadOnly(1);
    ui->lineEdit_contract_year->setReadOnly(1);
    palette = new QPalette();
    palette->setColor(QPalette::Base,Qt::gray);
    palette->setColor(QPalette::Text,Qt::darkGray);
    ui->lineEdit_inqr_id->setPalette(*palette);
    ui->lineEdit_inqr_year->setPalette(*palette);
    ui->lineEdit_inqr_index->setPalette(*palette);
    ui->lineEdit_contract_number->setPalette(*palette);
    ui->lineEdit_contract_year->setPalette(*palette);
}


CommonSettingsRandom::~CommonSettingsRandom()
{
    delete ui;
    delete model;
    delete selectionModel;
    delete delegate;
}


void CommonSettingsRandom::receive_new_id(int new_profile_id)
{
  new_id = new_profile_id;
}


/*******nastavenie tabulky podla typu testu******/
void CommonSettingsRandom::SetTable(QSqlTableModel *model_in)
{
    if (ui->comboBox->currentText() == "Random")
        model_in->setTable("common_settings_random");

    else if (ui->comboBox->currentText() == "Sine")
        model_in->setTable("common_settings_sine");

    else if (ui->comboBox->currentText() == "Shocks")
        model_in->setTable("common_settings_shocks");
}


/******nastavenie mien stplcov********/
void CommonSettingsRandom::SetColumnNames(QAbstractItemModel *model_in, QTableView *table_view)
{
    if (ui->comboBox->currentText() == "Random")
    {
        model_in->setHeaderData(0, Qt::Horizontal, "Profile ID", Qt::DisplayRole);
        model_in->setHeaderData(1, Qt::Horizontal, "Profile note", Qt::DisplayRole);
        model_in->setHeaderData(2, Qt::Horizontal, "Test time per axis", Qt::DisplayRole);
        model_in->setHeaderData(3, Qt::Horizontal, "Clipping", Qt::DisplayRole);
        model_in->setHeaderData(4, Qt::Horizontal, "Degrees of freedom", Qt::DisplayRole);
        model_in->setHeaderData(5, Qt::Horizontal, "Read only flag", Qt::DisplayRole);

        table_view->setColumnWidth(2, 120);
        table_view->setColumnWidth(4, 120);
    }

    else if (ui->comboBox->currentText() == "Sine")
    {
        model_in->setHeaderData(0, Qt::Horizontal, "Profile ID", Qt::DisplayRole);
        model_in->setHeaderData(1, Qt::Horizontal, "Profile note", Qt::DisplayRole);
        model_in->setHeaderData(2, Qt::Horizontal, "Sweep Type", Qt::DisplayRole);
        model_in->setHeaderData(3, Qt::Horizontal, "Sweep velocity", Qt::DisplayRole);
        model_in->setHeaderData(4, Qt::Horizontal, "Test time per axis", Qt::DisplayRole);
        model_in->setHeaderData(5, Qt::Horizontal, "Limits unit", Qt::DisplayRole);
        model_in->setHeaderData(6, Qt::Horizontal, "Read only flag", Qt::DisplayRole);

        table_view->setColumnWidth(2, 120);
        table_view->setColumnWidth(4, 120);
        table_view->setItemDelegateForColumn(3, delegate);
        table_view->setItemDelegateForColumn(4, delegate);
    }

    else if (ui->comboBox->currentText() == "Shocks")
    {
        model_in->setHeaderData(0, Qt::Horizontal, "Profile ID", Qt::DisplayRole);
        model_in->setHeaderData(1, Qt::Horizontal, "Profile note", Qt::DisplayRole);
        model_in->setHeaderData(2, Qt::Horizontal, "Shock number per direction", Qt::DisplayRole);
        model_in->setHeaderData(3, Qt::Horizontal, "Shock distance", Qt::DisplayRole);
        model_in->setHeaderData(4, Qt::Horizontal, "Read only flag", Qt::DisplayRole);

        table_view->setColumnWidth(2, 230);
        table_view->setItemDelegateForColumn(3, delegate);
    }
}


/*Nacitanie DB*/
void CommonSettingsRandom::on_pushButton_load_common_random_clicked()
{
    QSqlDatabase::database().open();

    SetTable(model);
    SetColumnNames(model, ui->tableView_common_random);

    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->setSort(0, Qt::AscendingOrder);
    model->select();

    ui->tableView_common_random->setModel(model);
}


/*potvrdenie zmien a nastavenie prislusnych zaznamov na read only - toto sa nepodarilo*/
void CommonSettingsRandom::on_pushButton_submit_common_clicked()
{
    model->submitAll();
}


/*********vymazanie polozky z hornej tabulky*******/
void CommonSettingsRandom::on_pushButton_delete_clicked()
{
    if(model->rowCount() != 0)
    {
        selectionModel = ui->tableView_common_random->selectionModel();
        QModelIndexList indexes = selectionModel->selectedRows();

        if(!indexes.empty())
            model->removeRows(indexes.first().row(), indexes.count());
    }
}


/*****Presun existujuceho profilu na tvorbu noveho******/
void CommonSettingsRandom::on_pushButton_drag_profile_clicked()
{
    if(model->rowCount() != 0)
    {
        selectionModel = ui->tableView_common_random->selectionModel();
        QModelIndexList rows = selectionModel->selectedRows();

        if(rows.count() > 0)
        {
            QModelIndexList indexes = selectionModel->selectedIndexes();
            int columns = model->columnCount();
            std::vector<QVariant> tmp;

            for(int i = 0; i < indexes.count(); i++)
            {
                QVariant value = selectionModel->model()->data(indexes.at(i));
                tmp.push_back(value);
            }

            QStandardItemModel* model2 = new QStandardItemModel(rows.count(), columns);
            SetColumnNames(model2, ui->tableView_common_random_2);

            QModelIndex topLeft = model2->index(0, 0, QModelIndex());
            QModelIndex bottomRight = model2->index(rows.count() - 1, columns - 1, QModelIndex());

            ui->tableView_common_random_2->setModel(model2);
            QItemSelectionModel* selectionModel2 = new QItemSelectionModel;
            selectionModel2 = ui->tableView_common_random_2->selectionModel();

            QItemSelection selection(topLeft, bottomRight);
            selectionModel2->select(selection, QItemSelectionModel::Select);
            QModelIndexList indexes2 = selectionModel2->selectedIndexes();

            for(int i = 0; i < indexes2.count(); i++)
            {
                model2->setData(indexes2.at(i), tmp.at(i), Qt::EditRole);
            }

            ui->tableView_common_random_2->setModel(model2);
        }
    }
}


/****novy zaznam v spodnej tabulke*****/
void CommonSettingsRandom::on_pushButton_new_record_clicked()
{
    QStandardItemModel* model = new QStandardItemModel();
    model = static_cast<QStandardItemModel*>(ui->tableView_common_random_2->model());

    if(model != nullptr)
    {
        QStandardItem* item = new QStandardItem;
        model->insertRow(model->rowCount(), item);
    }
}


/******vymazanie zaznamu(ov) z tabulky pre tvorbu noveho profilu*******/
void CommonSettingsRandom::on_pushButton_delete_new_record_clicked()
{
    QStandardItemModel* model = new QStandardItemModel;
    model = static_cast<QStandardItemModel*>(ui->tableView_common_random_2->model());

    if(model != nullptr)
    {
        selectionModel = ui->tableView_common_random_2->selectionModel();
        QModelIndexList indexes = selectionModel->selectedRows();

        if(!indexes.empty())
            model->removeRows(indexes.first().row(), indexes.count());
    }
}


/*nahranie novo vytvoreneho profilu do DB*/
void CommonSettingsRandom::on_pushButton_submit_new_record_clicked()
{
    QStandardItemModel* model2 = new QStandardItemModel;
    model2 = static_cast<QStandardItemModel*>(ui->tableView_common_random_2->model());

    if(model2 != nullptr)
    {
        int rows = model2->rowCount();
        int columns = model2->columnCount();

        for(int i = 0; i < rows; i++)
        {
            QSqlRecord record = model->record();
            record.setValue(0, new_id);

            //j=1 => vynechanie prveho stlpca (profile_Id), ktory sa zapise samostatne
            for(int j = 1; j < columns; j++)
            {
                QModelIndex id_index = model2->index(i, 0);
                model2->setData(id_index, new_id);

                QModelIndex index = model2->index(i, j);
                QVariant value = index.data(Qt::EditRole);
                record.setValue(j, value);
            }
            model->insertRecord(model->rowCount(), record);
            model->submitAll();
        }
    }
}


/******zrusit zmeny v hornej tabulke*****/
void CommonSettingsRandom::on_pushButton_revert_common_clicked()
{
    model->revertAll();
}


/*filtracia podla poptavky*/
void CommonSettingsRandom::on_pushButton_OK_filter_common_clicked()
{
    //ak je zaskrtnuty checkbox a vyplnene filtre
    if (ui->checkBox_inqr->isChecked() && !ui->lineEdit_inqr_id->text().isEmpty() && !ui->lineEdit_inqr_year->text().isEmpty() && !ui->lineEdit_inqr_index->text().isEmpty())
    {
        QSqlDatabase::database().open();

        QSqlQuery query_relations(QSqlDatabase::database());
        query_relations.prepare("SELECT adProfileId FROM relations_inquires_vs_profiles_vs_types WHERE adCustomerInquiryYear = ? "
                                "AND adCustomerInquiryNumber = ? AND adProfileSubtype = ? AND adProfileType = ? AND adCustomerInquiryIndex = ?");

        query_relations.bindValue(0, ui->lineEdit_inqr_year->text());
        query_relations.bindValue(1, ui->lineEdit_inqr_id->text());
        query_relations.bindValue(2, ui->comboBox->currentIndex());
        query_relations.bindValue(3, 0);
        query_relations.bindValue(4, ui->lineEdit_inqr_index->text());

        QStringList tmp;
        query_relations.exec();

        while(query_relations.next())
        {
            QString profile_id = query_relations.value(0).toString();
            tmp.push_back(profile_id);
        }

        //ak sa nasiel aspon jeden profil priradeny danej objednavke, zacne sa vytvarat sqlite query
        if (tmp.size() > 0)
        {
            QString query_profilesX_text = "profileId IN (";
            QString query_profilesX_values;
            for(int i = 0; i < tmp.size(); i++)
            {
                query_profilesX_values += tmp.at(i);

                //ak nie sme na konci kontajnera s profile id-ckami, tak pridame ciarku a pokracujeme
                if (tmp.at(i) != tmp.back())
                {
                    query_profilesX_values += ", ";
                }

                //inak query zakoncime
                else
                    query_profilesX_values += ")";
            }

            //vysledny dotaz
            query_profilesX_text = query_profilesX_text + query_profilesX_values;

            SetTable(model);
            SetColumnNames(model, ui->tableView_common_random);

            model->setFilter(query_profilesX_text);
            model->select();
        }
    }

    //ak je zaskrtnuty zakazkovy filter checkbox a vyplnene filtre
    else if (ui->checkBox_contract->isChecked() && !ui->lineEdit_contract_number->text().isEmpty() && !ui->lineEdit_contract_year->text().isEmpty())
    {
        QSqlDatabase::database("kniha_zakazek").open();

        QSqlQuery query_contract(QSqlDatabase::database("kniha_zakazek"));

        query_contract.prepare("SELECT customerInquiryYear, customerInquiryNumber, customerInquiryIndex FROM main_list "
                               "WHERE contractYear = ? AND contractNumber = ?");

        query_contract.bindValue(0, ui->lineEdit_contract_year->text());
        query_contract.bindValue(1, ui->lineEdit_contract_number->text());

        query_contract.exec();
        query_contract.first();
        QString inqr_year = query_contract.value(0).toString();
        QString inqr_number = query_contract.value(1).toString();
        QString inqr_index = query_contract.value(2).toString();

        QSqlDatabase::database("kniha_zakazek").close();

        QSqlDatabase::database().open();

        QSqlQuery query_relations(QSqlDatabase::database());
        query_relations.prepare("SELECT adProfileId FROM relations_inquires_vs_profiles_vs_types WHERE adCustomerInquiryYear = ? "
                      "AND adCustomerInquiryNumber = ? AND adProfileSubtype = ? AND adProfileType = ? AND adCustomerInquiryIndex = ?");

        query_relations.bindValue(0, inqr_year);
        query_relations.bindValue(1, inqr_number);
        query_relations.bindValue(2, ui->comboBox->currentIndex());
        query_relations.bindValue(3, 0);
        query_relations.bindValue(4, inqr_index);

        QStringList tmp;
        query_relations.exec();

        while(query_relations.next())
        {
            QString profile_id = query_relations.value(0).toString();
            tmp.push_back(profile_id);
        }

        //ak sa nasiel aspon jeden profil priradeny danej objednavke, zacne sa vytvarat sqlite query
        if (tmp.size() > 0)
        {
            QString query_profilesX_text = "profileId IN (";
            QString query_profilesX_values;
            for(int i = 0; i < tmp.size(); i++)
            {
                query_profilesX_values += tmp.at(i);

                //ak nie sme na konci kontajnera s profile id-ckami, tak pridame ciarku a pokracujeme
                if (tmp.at(i) != tmp.back())
                {
                    query_profilesX_values += ", ";
                }

                //inak query zakoncime
                else
                    query_profilesX_values += ")";
            }

            //vysledny dotaz
            query_profilesX_text = query_profilesX_text + query_profilesX_values;

            SetTable(model);
            SetColumnNames(model, ui->tableView_common_random);

            model->setFilter(query_profilesX_text);
            model->select();
        }
    }
}


/****aktviacia filtru ponuky*****/
void CommonSettingsRandom::on_checkBox_inqr_stateChanged()
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


/*****aktivacia filtru zakazky****/
void CommonSettingsRandom::on_checkBox_contract_stateChanged()
{
    if(ui->checkBox_contract->isChecked())
    {
        ui->lineEdit_contract_number->setReadOnly(0);
        ui->lineEdit_contract_year->setReadOnly(0);
        palette->setColor(QPalette::Base,Qt::white);
        palette->setColor(QPalette::Text,Qt::black);
        ui->lineEdit_contract_number->setPalette(*palette);
        ui->lineEdit_contract_year->setPalette(*palette);
    }

    else
    {
        ui->lineEdit_contract_number->setReadOnly(1);
        ui->lineEdit_contract_year->setReadOnly(1);
        palette->setColor(QPalette::Base,Qt::gray);
        palette->setColor(QPalette::Text,Qt::darkGray);
        ui->lineEdit_contract_number->setPalette(*palette);
        ui->lineEdit_contract_year->setPalette(*palette);
    }
}
