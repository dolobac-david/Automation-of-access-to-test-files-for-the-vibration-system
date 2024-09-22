#include "mainwindow.h"
#include "ui_mainwindow.h"

/*********konstruktor, pociatocna inicializacia uzivatelskeho rozhrania***********/
MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    /*******spracovanie databaz z config suboru******/
    string config = (QApplication::applicationDirPath() + "/config.txt").toStdString();

    ifstream file;
    file.open(config);
    QStringList list;
    string out;
    while(getline(file, out))
        list.push_back(out.c_str());

    QSqlDatabase::addDatabase("QSQLITE");
    QSqlDatabase::database().setDatabaseName(list[0]);

    QSqlDatabase::addDatabase("QSQLITE", "tests_parameters");
    QSqlDatabase::database("tests_parameters").setDatabaseName(list[1]);

    QSqlDatabase::addDatabase("QSQLITE", "kniha_zakazek");
    QSqlDatabase::database("kniha_zakazek").setDatabaseName(list[2]);

    model = new QSqlTableModel(this);
    model_common = new QSqlTableModel(this);

    ui->lineEdit_filter_inqr_id->setReadOnly(1);
    ui->lineEdit_filter_inqr_year->setReadOnly(1);
    ui->lineEdit_filter_inqr_index->setReadOnly(1);
    ui->lineEdit_filter_contract_number->setReadOnly(1);
    ui->lineEdit_filter_contract_year->setReadOnly(1);
    palette = new QPalette();
    palette->setColor(QPalette::Base,Qt::gray);
    palette->setColor(QPalette::Text,Qt::darkGray);
    ui->lineEdit_filter_inqr_id->setPalette(*palette);
    ui->lineEdit_filter_inqr_year->setPalette(*palette);
    ui->lineEdit_filter_inqr_index->setPalette(*palette);
    ui->lineEdit_filter_contract_number->setPalette(*palette);
    ui->lineEdit_filter_contract_year->setPalette(*palette);

    selectionModel = new QItemSelectionModel();
    selectionModel_common = new QItemSelectionModel();

    window_import = new ImportControlParameters;
    connect(window_import, SIGNAL(send_test_file_name_year(int, int, int)), this, SLOT(receive_test_file_year_number(int, int, int)));

    /*approx. test time*/
    ui->lineEdit_shock_test_time->setReadOnly(1);

    connect(window_import, SIGNAL(send_test_file_name_year_snimace(int, int)), this, SLOT(receive_test_file_year_number_snimace(int, int)));

    list_type_of_input = ui->groupBox_snimace_type_of_input->findChildren<QComboBox*>();
    list_sensitivity = ui->groupBox_snimace_sensitivity->findChildren<QLineEdit*>();
    list_unit = ui->groupBox_snimace_unit->findChildren<QComboBox*>();
    list_sensitivity_unit = ui->groupBox_snimace_sensitivity_unit->findChildren<QLineEdit*>();
    list_limits_lower = ui->groupBox_snimace_limits_lower->findChildren<QLineEdit*>();
    list_limits_lower_unit = ui->groupBox_snimace_limits_lower_unit->findChildren<QLineEdit*>();
    list_limits_upper = ui->groupBox_snimace_limits_upper->findChildren<QLineEdit*>();
    list_limits_upper_unit = ui->groupBox_snimace_limits_upper_unit->findChildren<QLineEdit*>();
    list_info_text = ui->groupBox_snimace_info_text->findChildren<QLineEdit*>();

    palette_enabled = new QPalette();
    palette_enabled->setColor(QPalette::Base,Qt::white);
    palette_enabled->setColor(QPalette::Text,Qt::black);

    model_level = new QStandardItemModel(0,2);
    model_level->setHeaderData(0, Qt::Horizontal, "Level [dB]", Qt::DisplayRole);
    model_level->setHeaderData(1, Qt::Horizontal, "Time", Qt::DisplayRole);
    ui->tableView_level->setModel(model_level);

    connect(window_import, SIGNAL(send_test_file_name_year_level(int, int)), this, SLOT(receive_test_file_year_number_level(int, int)));

    model_tolerance = new QStandardItemModel(0, 3);
    model_tolerance->setHeaderData(0, Qt::Horizontal, "Time [%]", Qt::DisplayRole);
    model_tolerance->setHeaderData(1, Qt::Horizontal, "Abort pos. [%]", Qt::DisplayRole);
    model_tolerance->setHeaderData(2, Qt::Horizontal, "Abort neg. [%]", Qt::DisplayRole);
    ui->tableView_tolerance->setModel(model_tolerance);

    connect(window_import, SIGNAL(send_test_file_name_year_tolerance_band(int, int)), this, SLOT(receive_test_file_year_number_tolerance_band(int, int)));

    file_default_random = QApplication::applicationDirPath() + "/Default.rau";
    file_default_sine = QApplication::applicationDirPath() + "/Default.sin";
    file_default_shock = QApplication::applicationDirPath() + "/Default.shk";

    file_new_base_path = list[3];
    file_new_base_path_no_order = list[4];

    window_test_item = new TestItemSelection;

    connect(this, SIGNAL(send_is_test_item(int, QString, QString, QString)), window_test_item, SLOT(receive_is_test_item(int, QString, QString, QString)));
    connect(window_test_item, SIGNAL(send_test_item_year_number(QVariant, QVariant)), this, SLOT(receive_test_item_year_number(QVariant, QVariant)));

    connect(this, SIGNAL(send_is_test_arrangement(int, QString, QString, QString)), window_test_item, SLOT(receive_is_test_arrangement(int, QString, QString, QString)));
    connect(window_test_item, SIGNAL(send_test_arrangement_order(int)), this, SLOT(receive_test_arrangement_order(int)));

    connect(window_import, SIGNAL(send_test_file_name_number_year_to_export(int, int, QString, int)), this, SLOT(receive_test_file_name_number_year(int, int, QString, int)));

    connect(window_import, SIGNAL(send_profile(QVariant, QVariant, QVariant, QVariant, QVariant)), this, SLOT(receive_profile(QVariant, QVariant, QVariant, QVariant, QVariant)));

    ui->checkBox_export_common->setChecked(1);

    ui->tabWidget->setTabEnabled(1, 0);
    ui->tabWidget->setTabEnabled(3, 0);
    ui->tabWidget->setTabEnabled(5, 0);
    ui->tabWidget->setTabEnabled(6, 0);

    connect(this, SIGNAL(send_to_import_load(bool, QVariant, QString, QString)), window_import, SLOT(receive_load_all(bool, QVariant, QString, QString)));

    LabelLastTestRefresh();
    ui->pushButton_export_duplicate->setDisabled(1);

    BlockButtonsWhenNoContract();
    ui->pushButton_export_generate->clicked();

    ui->lineEdit_export_file_name->setText("#testNumber#_#testYear#_#company&order#_#axis#_#XXX#_#testItem000#testArrangementOrder#");
    ui->lineEdit_export_postfix->setText(".SIN");
}


/*****true = jedna sa o stavajuci subor****/
bool MainWindow::isTestExisting()
{
    QSqlDatabase::database("tests_parameters").open();

    QSqlQuery query(QSqlDatabase::database("tests_parameters"));

    query.prepare("SELECT testFileYear, testFileNumber FROM main_tests_table WHERE testFileYear = ? AND testFileNumber = ?");

    query.bindValue(0, ui->lineEdit_export_test_year->text());
    query.bindValue(1, ui->lineEdit_export_test_number->text());

    query.exec();

    if(query.next())
        return 1;
    else
        return 0;
}


/*********poslanie filtrov do import karty na predvyplnenie********/
void MainWindow::SendFiltersToImport()
{
    bool test_exists = isTestExisting();
    QVariantList tmp;
    QString company_name;
    if(!test_exists && ui->checkBox_inqr->isChecked())
    {
        tmp = FetchContractAndCompanyID(ui->lineEdit_filter_inqr_year->text().toInt(), ui->lineEdit_filter_inqr_id->text().toInt(), ui->lineEdit_filter_inqr_index->text().toInt());
        if(!tmp.isEmpty())
            company_name = FetchCompanyName(tmp[0]);
    }
    emit send_to_import_load(isTestExisting(), ui->lineEdit_export_test_year->text().toInt(), ui->lineEdit_export_company_name->text(), company_name);
}


MainWindow::~MainWindow()
{
    delete ui;
    delete model;
    delete model_common;
    delete palette;
    delete selectionModel;
    delete selectionModel_common;
    delete window_import;
    delete palette_enabled;
    delete model_level;
    delete model_tolerance;
    QSqlDatabase::database(0).close();
    QSqlDatabase::database("tests_parameters", 0).close();
    QSqlDatabase::database("Z_31_Kniha_zakazek_ZL1", 0).close();
    delete window_test_item;
}


/*nacitanie profilov a common_settings*/
void MainWindow::on_pushButton_load_profiles_clicked()
{
    QSqlDatabase::database().open();

    //typ profilu random
    if (ui->comboBox->currentText() == "Random")
    {
        model->setTable("profiles_random");

        model->setHeaderData(0, Qt::Horizontal, "Profile ID", Qt::DisplayRole);
        model->setHeaderData(1, Qt::Horizontal, "Frequency", Qt::DisplayRole);
        model->setHeaderData(2, Qt::Horizontal, "Amplitude", Qt::DisplayRole);
        model->setHeaderData(3, Qt::Horizontal, "Warn limit positive", Qt::DisplayRole);
        model->setHeaderData(4, Qt::Horizontal, "Warn limit negative", Qt::DisplayRole);
        model->setHeaderData(5, Qt::Horizontal, "Abort limit positive", Qt::DisplayRole);
        model->setHeaderData(6, Qt::Horizontal, "Abort limit negative", Qt::DisplayRole);

        ui->tableView_profiles->setColumnWidth(0, 60);
        ui->tableView_profiles->setColumnWidth(3, 110);
        ui->tableView_profiles->setColumnWidth(4, 120);
        ui->tableView_profiles->setColumnWidth(5, 112);
        ui->tableView_profiles->setColumnWidth(6, 120);


        model_common->setTable("common_settings_random");

        model_common->setHeaderData(0, Qt::Horizontal, "Profile ID", Qt::DisplayRole);
        model_common->setHeaderData(1, Qt::Horizontal, "Profile note", Qt::DisplayRole);
        model_common->setHeaderData(2, Qt::Horizontal, "Test time per axis", Qt::DisplayRole);
        model_common->setHeaderData(3, Qt::Horizontal, "Clipping", Qt::DisplayRole);
        model_common->setHeaderData(4, Qt::Horizontal, "Degrees of freedom", Qt::DisplayRole);
        model_common->setHeaderData(5, Qt::Horizontal, "Read only flag", Qt::DisplayRole);

        ui->tableView_common->setColumnWidth(2, 120);
        ui->tableView_common->setColumnWidth(4, 120);
    }

    else if(ui->comboBox->currentText() == "Sine")
    {
        model->setTable("profiles_sine");

        model->setHeaderData(0, Qt::Horizontal, "Profile ID", Qt::DisplayRole);
        model->setHeaderData(1, Qt::Horizontal, "Frequency", Qt::DisplayRole);
        model->setHeaderData(2, Qt::Horizontal, "Acceleration", Qt::DisplayRole);
        model->setHeaderData(3, Qt::Horizontal, "Warn limit positive", Qt::DisplayRole);
        model->setHeaderData(4, Qt::Horizontal, "Warn limit negative", Qt::DisplayRole);
        model->setHeaderData(5, Qt::Horizontal, "Abort limit positive", Qt::DisplayRole);
        model->setHeaderData(6, Qt::Horizontal, "Abort limit negative", Qt::DisplayRole);

        ui->tableView_profiles->setColumnWidth(0, 60);
        ui->tableView_profiles->setColumnWidth(3, 110);
        ui->tableView_profiles->setColumnWidth(4, 120);
        ui->tableView_profiles->setColumnWidth(5, 112);
        ui->tableView_profiles->setColumnWidth(6, 120);


        model_common->setTable("common_settings_sine");

        model_common->setHeaderData(0, Qt::Horizontal, "Profile ID", Qt::DisplayRole);
        model_common->setHeaderData(1, Qt::Horizontal, "Profile note", Qt::DisplayRole);
        model_common->setHeaderData(2, Qt::Horizontal, "Sweep Type", Qt::DisplayRole);
        model_common->setHeaderData(3, Qt::Horizontal, "Sweep velocity", Qt::DisplayRole);
        model_common->setHeaderData(4, Qt::Horizontal, "Test time per axis", Qt::DisplayRole);
        model_common->setHeaderData(5, Qt::Horizontal, "Limits unit", Qt::DisplayRole);
        model_common->setHeaderData(6, Qt::Horizontal, "Read only flag", Qt::DisplayRole);

        ui->tableView_common->setColumnWidth(2, 120);
        ui->tableView_common->setColumnWidth(4, 120);
    }

    else if(ui->comboBox->currentText() == "Shocks")
    {
        model->setTable("profiles_shocks");

        model->setHeaderData(0, Qt::Horizontal, "Profile ID", Qt::DisplayRole);
        model->setHeaderData(1, Qt::Horizontal, "Curve type", Qt::DisplayRole);
        model->setHeaderData(2, Qt::Horizontal, "Amplitude", Qt::DisplayRole);
        model->setHeaderData(3, Qt::Horizontal, "Width", Qt::DisplayRole);


        model_common->setTable("common_settings_shocks");

        model_common->setHeaderData(0, Qt::Horizontal, "Profile ID", Qt::DisplayRole);
        model_common->setHeaderData(1, Qt::Horizontal, "Profile note", Qt::DisplayRole);
        model_common->setHeaderData(2, Qt::Horizontal, "Shock number per direction", Qt::DisplayRole);
        model_common->setHeaderData(3, Qt::Horizontal, "Shock distance", Qt::DisplayRole);
        model_common->setHeaderData(4, Qt::Horizontal, "Read only flag", Qt::DisplayRole);

        ui->tableView_common->setColumnWidth(2, 230);
    }

    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->setFilter("");
    model->setSort(0, Qt::AscendingOrder);
    model->select();

    ui->tableView_profiles->setModel(model);

    model_common->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model_common->setFilter("");
    model_common->setSort(0, Qt::AscendingOrder);
    model_common->select();

    ui->tableView_common->setModel(model_common);
}


/*filtrovanie profilov a common_settings podla poptavky*/
void MainWindow::on_OK_button_filter_profiles_clicked()
{
    //ak je zaskrtnuty checkbox a vyplnene oba filtre
    if (ui->checkBox_inqr->isChecked() && !ui->lineEdit_filter_inqr_id->text().isEmpty() && !ui->lineEdit_filter_inqr_year->text().isEmpty() && !ui->lineEdit_filter_inqr_index->text().isEmpty())
    {
        QSqlDatabase::database().open();

        QSqlQuery query_relations(QSqlDatabase::database());
        query_relations.prepare("SELECT adProfileId FROM relations_inquires_vs_profiles_vs_types WHERE adCustomerInquiryYear = ? "
                      "AND adCustomerInquiryNumber = ? AND adProfileSubtype = ? AND adProfileType = ? AND adCustomerInquiryIndex = ?");

        query_relations.bindValue(0, ui->lineEdit_filter_inqr_year->text());
        query_relations.bindValue(1, ui->lineEdit_filter_inqr_id->text());
        query_relations.bindValue(2, ui->comboBox->currentIndex());
        query_relations.bindValue(3, 0);
        query_relations.bindValue(4, ui->lineEdit_filter_inqr_index->text());

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
            QString query_profilesX_text, query_common_settingsX_text;
            if (ui->comboBox->currentText() == "Random")
            {
                query_profilesX_text = "adProfileId IN (";
                query_common_settingsX_text = "profileId IN (";
            }

            else if(ui->comboBox->currentText() == "Sine")
            {
                query_profilesX_text = "adProfileId IN (";
                query_common_settingsX_text = "profileId IN (";
            }


            else if (ui->comboBox->currentText() == "Shocks")
            {
                query_profilesX_text = "profileId IN (";
                query_common_settingsX_text = "profileId IN (";
            }

            QString query_profilesX_values, query_common_settingsX_values;
            for(int i = 0; i < tmp.size(); i++)
            {
                query_profilesX_values += tmp.at(i);
                query_common_settingsX_values += tmp.at(i);

                //ak nie sme na konci kontajnera s profile id-ckami, tak pridame ciarku a pokracujeme
                if (tmp.at(i) != tmp.back())
                {
                    query_profilesX_values += ", ";
                    query_common_settingsX_values += ", ";
                }

                //inak query zakoncime
                else
                {
                    query_profilesX_values += ")";
                    query_common_settingsX_values += ")";
                }
            }

            //vysledny dotaz
            query_profilesX_text = query_profilesX_text + query_profilesX_values;
            query_common_settingsX_text = query_common_settingsX_text + query_common_settingsX_values;

            model->setFilter(query_profilesX_text);
            model->select();

            model_common->setFilter(query_common_settingsX_text);
            model_common->select();
        }
    }

    //ak je zaskrtnuty zakazkovy filter checkbox a vyplnene filtre
    else if (ui->checkBox_contract->isChecked() && !ui->lineEdit_filter_contract_number->text().isEmpty() && !ui->lineEdit_filter_contract_year->text().isEmpty())
    {
        QSqlDatabase::database("kniha_zakazek").open();

        QSqlQuery query_contract(QSqlDatabase::database("kniha_zakazek"));

        query_contract.prepare("SELECT customerInquiryYear, customerInquiryNumber, customerInquiryIndex FROM main_list "
                               "WHERE contractYear = ? AND contractNumber = ?");

        query_contract.bindValue(0, ui->lineEdit_filter_contract_year->text());
        query_contract.bindValue(1, ui->lineEdit_filter_contract_number->text());

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
            QString query_profilesX_text, query_common_settingsX_text;
            if (ui->comboBox->currentText() == "Random")
            {
                query_profilesX_text = "adProfileId IN (";
                query_common_settingsX_text = "profileId IN (";
            }

            else if(ui->comboBox->currentText() == "Sine")
            {
                query_profilesX_text = "adProfileId IN (";
                query_common_settingsX_text = "profileId IN (";
            }


            else if (ui->comboBox->currentText() == "Shocks")
            {
                query_profilesX_text = "profileId IN (";
                query_common_settingsX_text = "profileId IN (";
            }

            QString query_profilesX_values, query_common_settingsX_values;
            for(int i = 0; i < tmp.size(); i++)
            {
                query_profilesX_values += tmp.at(i);
                query_common_settingsX_values += tmp.at(i);

                //ak nie sme na konci kontajnera s profile id-ckami, tak pridame ciarku a pokracujeme
                if (tmp.at(i) != tmp.back())
                {
                    query_profilesX_values += ", ";
                    query_common_settingsX_values += ", ";
                }

                //inak query zakoncime
                else
                {
                    query_profilesX_values += ")";
                    query_common_settingsX_values += ")";
                }
            }

            //vysledny dotaz
            query_profilesX_text = query_profilesX_text + query_profilesX_values;
            query_common_settingsX_text = query_common_settingsX_text + query_common_settingsX_values;

            model->setFilter(query_profilesX_text);
            model->select();

            model_common->setFilter(query_common_settingsX_text);
            model_common->select();
        }
    }
}


/*odomknutie/uzamknutie filtrov poptavky*/
void MainWindow::on_checkBox_inqr_stateChanged()
{
    if(ui->checkBox_inqr->isChecked())
    {
        ui->lineEdit_filter_inqr_id->setReadOnly(0);
        ui->lineEdit_filter_inqr_year->setReadOnly(0);
        ui->lineEdit_filter_inqr_index->setReadOnly(0);
        ui->lineEdit_filter_inqr_id->setPalette(*palette_enabled);
        ui->lineEdit_filter_inqr_year->setPalette(*palette_enabled);
        ui->lineEdit_filter_inqr_index->setPalette(*palette_enabled);
    }

    else if(!ui->checkBox_inqr->isChecked())
    {
        ui->lineEdit_filter_inqr_id->setReadOnly(1);
        ui->lineEdit_filter_inqr_year->setReadOnly(1);
        ui->lineEdit_filter_inqr_index->setReadOnly(1);
        ui->lineEdit_filter_inqr_id->setPalette(*palette);
        ui->lineEdit_filter_inqr_year->setPalette(*palette);
        ui->lineEdit_filter_inqr_index->setPalette(*palette);
    }
}


/*odomknutie/uzamknutie filtrov zakazky*/
void MainWindow::on_checkBox_contract_stateChanged()
{
    if(ui->checkBox_contract->isChecked())
    {
        ui->lineEdit_filter_contract_number->setReadOnly(0);
        ui->lineEdit_filter_contract_year->setReadOnly(0);
        ui->lineEdit_filter_contract_number->setPalette(*palette_enabled);
        ui->lineEdit_filter_contract_year->setPalette(*palette_enabled);
    }

    else if(!ui->checkBox_contract->isChecked())
    {
        ui->lineEdit_filter_contract_number->setReadOnly(1);
        ui->lineEdit_filter_contract_year->setReadOnly(1);
        ui->lineEdit_filter_contract_number->setPalette(*palette);
        ui->lineEdit_filter_contract_year->setPalette(*palette);
    }
}


/*vyber profilu na export*/
void MainWindow::on_pushButton_clicked()
{   
    selectionModel = ui->tableView_profiles->selectionModel();
    QModelIndexList indexes = selectionModel->selectedIndexes();;

    if(!indexes.isEmpty())
    {
        int sel_row = indexes.first().row();

        QVariant profile_id = model->index(sel_row, 0).data();

        QString query_profilesX_text;
        QString query_common_settingsX_text = "profileId IN (";

        if (ui->comboBox->currentText() == "Random" || ui->comboBox->currentText() == "Sine")
            query_profilesX_text = "adProfileId IN (";

        else if (ui->comboBox->currentText() == "Shocks")
            query_profilesX_text = "profileId IN (";

        query_profilesX_text += profile_id.toString() + ")";
        model->setFilter(query_profilesX_text);
        model->select();

        if(ui->checkBox_export_common->isChecked())
        {
            query_common_settingsX_text += profile_id.toString() + ")";
            model_common->setFilter(query_common_settingsX_text);
            model_common->select();

            if (ui->comboBox->currentText() == "Random" && model_common->rowCount() != 0)
            {
                /*ak je clipping vyplneny v ramci profilov*/
                QModelIndex index_clipping = model_common->index(0, 3);
                QVariant clipping = model_common->data(index_clipping);

                if (clipping == 2.5)
                    ui->radioButton_sigma->setChecked(1);

                else if (clipping == 3)
                    ui->radioButton_3->setChecked(1);

                else if (clipping == 3.5)
                    ui->radioButton_3_5->setChecked(1);

                else if (clipping == 4)
                    ui->radioButton_4->setChecked(1);

                else if (clipping == 5)
                    ui->radioButton_5->setChecked(1);

                else if (clipping == 6)
                    ui->radioButton_Aus->setChecked(1);


                /*ak boli DOF vyplnene v ramci profilov*/
                QModelIndex index_dof = model_common->index(0, 4);
                QVariant dof = model_common->data(index_dof);
                ui->lineEdit_dof->setText(dof.toString());
            }

            else if (ui->comboBox->currentText() == "Sine" && model_common->rowCount() != 0)
            {
                /*ak sa vyplnil sweep type pri profiloch*/
                QModelIndex index_sweep_type = model_common->index(0, 2);
                QVariant sweep_type = model_common->data(index_sweep_type);
                if (sweep_type == 0)
                    ui->radioButton_sine_sweep_log->setChecked(1);

                else if (sweep_type == 1)
                    ui->radioButton_sine_sweep_linear->setChecked(1);

                /*ak bol vyplnen sweep velocity pri profiloch*/
                QModelIndex index_sweep_v = model_common->index(0, 3);
                QVariant sweep_v = model_common->data(index_sweep_v);
                ui->lineEdit_sine_sweep_v->setText(sweep_v.toString());

                //ak sa vyplnil test time pri profiloch
                QModelIndex index_test_time = model_common->index(0, 4);
                int test_time = model_common->data(index_test_time).toInt();

                int days = test_time/86400;
                int hours = (test_time - days*86400)/3600;
                int minutes = (test_time - hours*3600 - days*86400)/60;
                int seconds = test_time - minutes*60 - hours*3600 - days*86400;

                ui->lineEdit_sine_day->setText(QString(to_string(days).c_str()));
                ui->lineEdit_sine_hour->setText(QString(to_string(hours).c_str()));
                ui->lineEdit_sine_min->setText(QString(to_string(minutes).c_str()));
                ui->lineEdit_sine_sec->setText(QString(to_string(seconds).c_str()));
            }

            else if (ui->comboBox->currentText() == "Shocks" && model_common->rowCount() != 0)
            {
                /*vyplneny shock number v profiloch*/
                QModelIndex index_shock_number = model_common->index(0, 2);
                QVariant shock_number = model_common->data(index_shock_number);
                ui->lineEdit_shock_number->setText(shock_number.toString());

                /*vyplneny shock distance v profiloch*/
                QModelIndex index_shock_distance = model_common->index(0, 3);
                QVariant shock_distance = model_common->data(index_shock_distance);
                ui->lineEdit_shock_distance->setText(shock_distance.toString());
            }
        }

        else if(!ui->checkBox_export_common->isChecked())
        {
            query_common_settingsX_text += ")";
            model_common->setFilter(query_common_settingsX_text);
            model_common->select();
        }
    }
}


/*okno import control parameters - prechod z karty control parameters random*/
void MainWindow::on_pushButton_import_control_parameters_random_clicked()
{
    window_import->show();
    SendFiltersToImport();
}


/*ziskanie vybraneho test_year, test_number a predvyplnenie nastaveni Control parameters*/
void MainWindow::receive_test_file_year_number(int test_year, int test_number, int profile_type)
{
    test_file_year = test_year;
    test_file_number = test_number;
    sqlite3* DB;
    qDebug() << sqlite3_open(QSqlDatabase::database("tests_parameters").databaseName().toStdString().c_str(), &DB) << endl;

    ui->comboBox->setCurrentIndex(profile_type);

    /*Control parameters Random*/
    if (ui->comboBox->currentText() == "Random")
    {
        CRandomVibrations tmp;
        tmp.FetchFromRandomVibrations(DB, test_file_year, test_file_number);
        qDebug() << sqlite3_close(DB) << endl;

        /*Clipping*/
        if(!ui->checkBox_export_common->isChecked() || model_common->rowCount() == 0)
        {
            if (tmp.Clipping == 2.5)
                ui->radioButton_sigma->setChecked(1);

            else if (tmp.Clipping == 3)
                ui->radioButton_3->setChecked(1);

            else if (tmp.Clipping == 3.5)
                ui->radioButton_3_5->setChecked(1);

            else if (tmp.Clipping == 4)
                ui->radioButton_4->setChecked(1);

            else if (tmp.Clipping == 5)
                ui->radioButton_5->setChecked(1);

            else if (tmp.Clipping == 6)
                ui->radioButton_Aus->setChecked(1);
        }

        else if (ui->checkBox_export_common->isChecked() && model_common->rowCount() != 0)
        {
            /*ak je clipping a DOF vyplnene v ramci profilov*/
            QModelIndex index_clipping = model_common->index(0, 3);
            QVariant clipping = model_common->data(index_clipping);

            if (clipping == 2.5)
                ui->radioButton_sigma->setChecked(1);

            else if (clipping == 3)
                ui->radioButton_3->setChecked(1);

            else if (clipping == 3.5)
                ui->radioButton_3_5->setChecked(1);

            else if (clipping == 4)
                ui->radioButton_4->setChecked(1);

            else if (clipping == 5)
                ui->radioButton_5->setChecked(1);

            else if (clipping == 6)
                ui->radioButton_Aus->setChecked(1);


            /*ak boli DOF vyplnene v ramci profilov*/
            QModelIndex index_dof = model_common->index(0, 4);
            QVariant dof = model_common->data(index_dof);
            ui->lineEdit_dof->setText(dof.toString());
        }

        if (model_common->rowCount() != 0)
        {
            /*ak je clipping vyplneny v ramci profilov*/
            QModelIndex index_clipping = model_common->index(0, 3);
            QVariant clipping = model_common->data(index_clipping);

            if (clipping == 2.5)
                ui->radioButton_sigma->setChecked(1);

            else if (clipping == 3)
                ui->radioButton_3->setChecked(1);

            else if (clipping == 3.5)
                ui->radioButton_3_5->setChecked(1);

            else if (clipping == 4)
                ui->radioButton_4->setChecked(1);

            else if (clipping == 5)
                ui->radioButton_5->setChecked(1);

            else if (clipping == 6)
                ui->radioButton_Aus->setChecked(1);
        }

        /*Multi channel*/
        if (tmp.MultiChannelControl == 0)
            ui->radioButton_average->setChecked(1);

        else if (tmp.MultiChannelControl == 1)
            ui->radioButton_extremal->setChecked(1);

        /*mass test item/fixture*/
        ui->lineEdit_mass_test_item->setText(QString(to_string(tmp.MassTestItems).c_str()));
        ui->lineEdit_mass_test_fixture->setText(QString(to_string(tmp.MassTestFixture).c_str()));

        /*connection with slip table*/
        ui->checkBox_slip_table->setChecked(tmp.ConnectionWithSlipTable);

        /*abort, warn limits*/
        ui->lineEdit_abort_limit->setText(QString(to_string(tmp.AbortLimitWithLines).c_str()));
        ui->lineEdit_warn_limit->setText(QString(to_string(tmp.WarnLimitWithLines).c_str()));

        /*level shift*/
        if (tmp.LevelShift == 0)
            ui->radioButton_manual->setChecked(1);

        else if (tmp.LevelShift == 1)
            ui->radioButton_auto->setChecked(1);

        else if (tmp.LevelShift == 2)
            ui->radioButton_optimized->setChecked(1);

        /*DOF*/
        if(!ui->checkBox_export_common->isChecked() || model_common->rowCount() == 0)
            ui->lineEdit_dof->setText(QString(to_string(tmp.DegreesOfFreedom).c_str()));

        /*Pretest level*/
        ui->lineEdit_pretest_level->setText(QString(to_string(tmp.PreTestLevel).c_str()));

        /*enable pos dB values*/
        ui->checkBox_random_enable_pos->setChecked(tmp.EnablePosValues);

        /*selection of bandwidth*/
        if (tmp.SelectionOfBandwidth == 100)
            ui->comboBox_random_selection_of->setCurrentIndex(0);

        else if (tmp.SelectionOfBandwidth == 200)
            ui->comboBox_random_selection_of->setCurrentIndex(1);

        else if (tmp.SelectionOfBandwidth == 500)
            ui->comboBox_random_selection_of->setCurrentIndex(2);

        else if (tmp.SelectionOfBandwidth == 1000)
            ui->comboBox_random_selection_of->setCurrentIndex(3);

        else if (tmp.SelectionOfBandwidth == 2000)
            ui->comboBox_random_selection_of->setCurrentIndex(4);

        else if (tmp.SelectionOfBandwidth == 5000)
            ui->comboBox_random_selection_of->setCurrentIndex(5);

        /*Curve save*/
        /*vynulovanie checkboxov*/
        ui->checkBox_cyclic_every->setChecked(0);
        ui->checkBox_with_abort_level->setChecked(0);
        ui->checkBox_at_abort->setChecked(0);
        ui->checkBox_with_RMS_limit->setChecked(0);
        ui->checkBox_with_abort_level_cyclic_savings->setChecked(0);
        ui->checkBox_at_abort_cyclic_savings->setChecked(0);
        ui->checkBox_with_RMS_cyclic_savings->setChecked(0);

        /*nastavenie podla tests_parameters*/
        if (tmp.CurveSave == 1.401298e-45)
            ui->checkBox_cyclic_every->setChecked(1);

        else if (tmp.CurveSave == 2.662467e-44)
        {
            ui->checkBox_cyclic_every->setChecked(1);
            ui->checkBox_with_abort_level->setChecked(1);
            ui->checkBox_with_abort_level_cyclic_savings->setChecked(1);
        }

        else if (tmp.CurveSave == 4.203895e-45)
        {
            ui->checkBox_cyclic_every->setChecked(1);
            ui->checkBox_with_abort_level->setChecked(1);
        }

        else if (tmp.CurveSave == 1.022948e-43)
        {
            ui->checkBox_cyclic_every->setChecked(1);
            ui->checkBox_at_abort->setChecked(1);
            ui->checkBox_at_abort_cyclic_savings->setChecked(1);
        }

        else if (tmp.CurveSave == 1.261169e-44)
        {
            ui->checkBox_cyclic_every->setChecked(1);
            ui->checkBox_at_abort->setChecked(1);
        }

        else if (tmp.CurveSave == 5.184804e-44)
        {
            ui->checkBox_cyclic_every->setChecked(1);
            ui->checkBox_with_RMS_limit->setChecked(1);
            ui->checkBox_with_RMS_cyclic_savings->setChecked(1);
        }

        else if (tmp.CurveSave == 7.006492e-45)
        {
            ui->checkBox_cyclic_every->setChecked(1);
            ui->checkBox_with_RMS_limit->setChecked(1);
        }

        else if (tmp.CurveSave == 1.275182e-43)
        {
            ui->checkBox_cyclic_every->setChecked(1);
            ui->checkBox_with_abort_level->setChecked(1);
            ui->checkBox_at_abort->setChecked(1);
            ui->checkBox_with_abort_level_cyclic_savings->setChecked(1);
            ui->checkBox_at_abort_cyclic_savings->setChecked(1);
        }

        else if (tmp.CurveSave == 1.541428e-44)
        {
            ui->checkBox_cyclic_every->setChecked(1);
            ui->checkBox_with_abort_level->setChecked(1);
            ui->checkBox_at_abort->setChecked(1);
        }

        else if (tmp.CurveSave == 7.707142e-44)
        {
            ui->checkBox_cyclic_every->setChecked(1);
            ui->checkBox_with_abort_level->setChecked(1);
            ui->checkBox_with_RMS_limit->setChecked(1);
            ui->checkBox_with_abort_level_cyclic_savings->setChecked(1);
            ui->checkBox_with_RMS_cyclic_savings->setChecked(1);
        }

        else if (tmp.CurveSave == 9.809089e-45)
        {
            ui->checkBox_cyclic_every->setChecked(1);
            ui->checkBox_with_abort_level->setChecked(1);
            ui->checkBox_with_RMS_limit->setChecked(1);
        }

        else if (tmp.CurveSave == 1.527415e-43)
        {
            ui->checkBox_cyclic_every->setChecked(1);
            ui->checkBox_at_abort->setChecked(1);
            ui->checkBox_with_RMS_limit->setChecked(1);
            ui->checkBox_at_abort_cyclic_savings->setChecked(1);
            ui->checkBox_with_RMS_cyclic_savings->setChecked(1);
        }

        else if (tmp.CurveSave == 1.821688e-44)
        {
            ui->checkBox_cyclic_every->setChecked(1);
            ui->checkBox_at_abort->setChecked(1);
            ui->checkBox_with_RMS_limit->setChecked(1);
        }

        else if (tmp.CurveSave == 1.779649e-43)
        {
            ui->checkBox_cyclic_every->setChecked(1);
            ui->checkBox_with_abort_level->setChecked(1);
            ui->checkBox_at_abort->setChecked(1);
            ui->checkBox_with_RMS_limit->setChecked(1);
            ui->checkBox_with_abort_level_cyclic_savings->setChecked(1);
            ui->checkBox_at_abort_cyclic_savings->setChecked(1);
            ui->checkBox_with_RMS_cyclic_savings->setChecked(1);
        }

        else if (tmp.CurveSave == 2.101948e-44)
        {
            ui->checkBox_cyclic_every->setChecked(1);
            ui->checkBox_with_abort_level->setChecked(1);
            ui->checkBox_at_abort->setChecked(1);
            ui->checkBox_with_RMS_limit->setChecked(1);
        }

        /*Curve save time*/
        int hours = tmp.CurveSaveTime/3600;
        int minutes = (tmp.CurveSaveTime - hours*3600)/60;
        int seconds = tmp.CurveSaveTime - minutes*60 - hours*3600;

        ui->lineEdit_random_h->setText(QString(to_string(hours).c_str()));
        ui->lineEdit_random_min->setText(QString(to_string(minutes).c_str()));
        ui->lineEdit_random_sec->setText(QString(to_string(seconds).c_str()));
    }


    /*Control Parameters Sine*/
    else if (ui->comboBox->currentText() == "Sine")
    {
        CSineVibrations tmp;
        tmp.FetchFromSineVibrations(DB, test_file_year, test_file_number);
        qDebug() << sqlite3_close(DB) << endl;

        /*Amplitude type*/
        if (tmp.AmplitudeType == 0)
            ui->radioButton_sine_amplitude_band_limited->setChecked(1);

        else if (tmp.AmplitudeType == 1)
            ui->radioButton_sine_amplitude_rms_average->setChecked(1);

        else if (tmp.AmplitudeType == 2)
            ui->radioButton_sine_amplitude_peak_value->setChecked(1);

        /*Sweep type*/
        /*ak nebol vyplneny v ramci profilov*/
        if (!ui->checkBox_export_common->isChecked() || model_common->rowCount() == 0)
        {
            if (tmp.SweepType == 0)
                ui->radioButton_sine_sweep_log->setChecked(1);

            else if (tmp.SweepType == 1)
                ui->radioButton_sine_sweep_linear->setChecked(1);
        }

        else if (ui->checkBox_export_common->isChecked() && model_common->rowCount() != 0)
        {
            /*ak sa vyplnil sweep type, v a test time pri profiloch*/
            QModelIndex index_sweep_type = model_common->index(0, 2);
            QVariant sweep_type = model_common->data(index_sweep_type);
            if (sweep_type == 0)
                ui->radioButton_sine_sweep_log->setChecked(1);

            else if (sweep_type == 1)
                ui->radioButton_sine_sweep_linear->setChecked(1);

            QModelIndex index_sweep_v = model_common->index(0, 3);
            QVariant sweep_v = model_common->data(index_sweep_v);
            ui->lineEdit_sine_sweep_v->setText(sweep_v.toString());

            QModelIndex index_test_time = model_common->index(0, 4);
            int test_time = model_common->data(index_test_time).toInt();

            int days = test_time/86400;
            int hours = (test_time - days*86400)/3600;
            int minutes = (test_time - hours*3600 - days*86400)/60;
            int seconds = test_time - minutes*60 - hours*3600 - days*86400;

            ui->lineEdit_sine_day->setText(QString(to_string(days).c_str()));
            ui->lineEdit_sine_hour->setText(QString(to_string(hours).c_str()));
            ui->lineEdit_sine_min->setText(QString(to_string(minutes).c_str()));
            ui->lineEdit_sine_sec->setText(QString(to_string(seconds).c_str()));
        }

        /*Multi Channel*/
        if (tmp.MultiChannelControl == 0)
            ui->radioButton_sine_average->setChecked(1);

        else if (tmp.MultiChannelControl == 1)
            ui->radioButton_sine_extremal->setChecked(1);

        /*Start Frequency*/
        if (tmp.StartFrequency == 0)
            ui->radioButton_sine_freq_lower->setChecked(1);

        else if (tmp.StartFrequency == 1)
            ui->radioButton_sine_freq_upper->setChecked(1);

        /*abort, warn limit violations*/
        ui->lineEdit_sine_warn_limit->setText(QString(to_string(tmp.NumberOfWarnLimitViolations).c_str()));
        ui->lineEdit_sine_abort_limit->setText(QString(to_string(tmp.NumberOfAbortLimitViolations).c_str()));

        /*Pretest level*/
        ui->lineEdit_sine_pretest_level->setText(QString(to_string(tmp.PreTestLevel).c_str()));

        /*connection with slip table*/
        ui->checkBox_sine_slip_table->setChecked(tmp.ConnectionWithSlipTable);

        /*mass test item/fixture*/
        ui->lineEdit_sine_mass_item->setText(QString(to_string(tmp.MassTestItems).c_str()));
        ui->lineEdit_sine_mass_fixture->setText(QString(to_string(tmp.MassTestFixture).c_str()));

        /*sweep velocity*/
        if (!ui->checkBox_export_common->isChecked() || model_common->rowCount() == 0)
            ui->lineEdit_sine_sweep_v->setText(QString(to_string(tmp.SweepVelocity).c_str()));

        /*test time*/
        if (!ui->checkBox_export_common->isChecked() || model_common->rowCount() == 0)
        {
            int days = tmp.TestTime / 86400;
            int hours = (tmp.TestTime - days*86400) / 3600;
            int minutes = (tmp.TestTime - hours * 3600 - days*86400) / 60;
            int seconds = tmp.TestTime - minutes*60 - hours*3600 - days*86400;

            ui->lineEdit_sine_day->setText(QString(to_string(days).c_str()));
            ui->lineEdit_sine_hour->setText(QString(to_string(hours).c_str()));
            ui->lineEdit_sine_min->setText(QString(to_string(minutes).c_str()));
            ui->lineEdit_sine_sec->setText(QString(to_string(seconds).c_str()));
        }

        /*Test duration boxes*/
        if(tmp.TestDurationBoxes == 0)
        {
            ui->checkBox_sine_sweep_v->setChecked(0);
            ui->checkBox_sine_numb_of_cycles->setChecked(0);
            ui->checkBox_sine_time_per->setChecked(0);
            ui->checkBox_sine_test_time->setChecked(0);
        }

        else if(tmp.TestDurationBoxes == 101)
        {
            ui->checkBox_sine_sweep_v->setChecked(0);
            ui->checkBox_sine_numb_of_cycles->setChecked(0);
            ui->checkBox_sine_time_per->setChecked(1);
            ui->checkBox_sine_test_time->setChecked(1);
        }

        else if(tmp.TestDurationBoxes == 110)
        {
            ui->checkBox_sine_sweep_v->setChecked(0);
            ui->checkBox_sine_numb_of_cycles->setChecked(1);
            ui->checkBox_sine_time_per->setChecked(1);
            ui->checkBox_sine_test_time->setChecked(0);
        }

        else if(tmp.TestDurationBoxes == 1010)
        {
            ui->checkBox_sine_sweep_v->setChecked(1);
            ui->checkBox_sine_numb_of_cycles->setChecked(1);
            ui->checkBox_sine_time_per->setChecked(0);
            ui->checkBox_sine_test_time->setChecked(0);
        }

        else if(tmp.TestDurationBoxes == 1001)
        {
            ui->checkBox_sine_sweep_v->setChecked(1);
            ui->checkBox_sine_numb_of_cycles->setChecked(0);
            ui->checkBox_sine_time_per->setChecked(0);
            ui->checkBox_sine_test_time->setChecked(1);
        }

        /*Curve save*/
        /*reset checkboxov*/
        ui->checkBox_sine_cyclic_savings_every->setChecked(0);
        ui->checkBox_sine_with_abort_level->setChecked(0);
        ui->checkBox_sine_at_abort->setChecked(0);
        ui->checkBox_sine_with_acceleration_limit->setChecked(0);
        ui->checkBox_sine_with_abort_level_cyclic_savings->setChecked(0);
        ui->checkBox_sine_at_abort_cyclic_savings->setChecked(0);
        ui->checkBox_sine_with_acceleration_cyclic_savings->setChecked(0);

        if (tmp.CurveSave == 1010.00006103516)
            ui->checkBox_sine_cyclic_savings_every->setChecked(1);

        else if (tmp.CurveSave == 1010.00018310547)
        {
            ui->checkBox_sine_cyclic_savings_every->setChecked(1);
            ui->checkBox_sine_with_abort_level->setChecked(1);
        }

        else if (tmp.CurveSave == 1010.00030517578)
        {
            ui->checkBox_sine_cyclic_savings_every->setChecked(1);
            ui->checkBox_sine_with_acceleration_limit->setChecked(1);
        }

        else if (tmp.CurveSave == 1010.00042724609)
        {
            ui->checkBox_sine_cyclic_savings_every->setChecked(1);
            ui->checkBox_sine_with_abort_level->setChecked(1);
            ui->checkBox_sine_with_acceleration_limit->setChecked(1);
        }

        else if (tmp.CurveSave == 1010.00054931641)
        {
            ui->checkBox_sine_cyclic_savings_every->setChecked(1);
            ui->checkBox_sine_at_abort->setChecked(1);
        }

        else if (tmp.CurveSave == 1010.00067138672)
        {
            ui->checkBox_sine_cyclic_savings_every->setChecked(1);
            ui->checkBox_sine_with_abort_level->setChecked(1);
            ui->checkBox_sine_at_abort->setChecked(1);
        }

        else if (tmp.CurveSave == 1010.00079345703)
        {
            ui->checkBox_sine_cyclic_savings_every->setChecked(1);
            ui->checkBox_sine_with_acceleration_limit->setChecked(1);
            ui->checkBox_sine_at_abort->setChecked(1);
        }

        else if (tmp.CurveSave == 1010.00091552734)
        {
            ui->checkBox_sine_cyclic_savings_every->setChecked(1);
            ui->checkBox_sine_with_abort_level->setChecked(1);
            ui->checkBox_sine_at_abort->setChecked(1);
            ui->checkBox_sine_with_acceleration_limit->setChecked(1);
        }

        /*Curve save cycle*/
        ui->lineEdit_sine_cycle->setText(QString(to_string(tmp.CurveSaveCycle).c_str()));
    }


    /*Control parameters shock*/
    else if (ui->comboBox->currentText() == "Shocks")
    {
        CShocks tmp;
        tmp.FetchFromShocks(DB, test_file_year, test_file_number);
        qDebug() << sqlite3_close(DB) << endl;

        /*Shock number*/
        if(!ui->checkBox_export_common->isChecked() || model_common->rowCount() == 0)
            ui->lineEdit_shock_number->setText(QString(to_string(tmp.ShockNumberPerDirection).c_str()));

        /*vyplneny shock number a distance v profiloch*/
        else if (ui->checkBox_export_common->isChecked() && model_common->rowCount() != 0)
        {
            /*vyplneny shock number v profiloch*/
            QModelIndex index_shock_number = model_common->index(0, 2);
            QVariant shock_number = model_common->data(index_shock_number);
            ui->lineEdit_shock_number->setText(shock_number.toString());

            QModelIndex index_shock_distance = model_common->index(0, 3);
            QVariant shock_distance = model_common->data(index_shock_distance);
            ui->lineEdit_shock_distance->setText(shock_distance.toString());
        }

        /*Shock Distance*/
        if(!ui->checkBox_export_common->isChecked() || model_common->rowCount() == 0)
            ui->lineEdit_shock_distance->setText(QString(to_string(tmp.ShockDistance).c_str()));

        /*warn/abort limit violations*/
        ui->lineEdit_shock_warn_limit_violations->setText(QString(to_string(tmp.NumberOfWarnLimitViolations).c_str()));
        ui->lineEdit_shock_abort_limit->setText(QString(to_string(tmp.NumberOfAbortLimitViolations).c_str()));

        /*mass test fixture/item*/
        ui->lineEdit_shock_mass_test_fixture->setText(QString(to_string(tmp.MassTestFixture).c_str()));
        ui->lineEdit_shock_mass_test_item->setText(QString(to_string(tmp.MassTestItems).c_str()));

        /*slip table*/
        ui->checkBox_shock_slip_table->setChecked(tmp.ConnectionWithSlipTable);

        /*pretest level*/
        ui->lineEdit_shock_pre_test_level->setText(QString(to_string(tmp.PreTestLevel).c_str()));

        /*pre-shock level*/
        if(tmp.PreShockLevel == 0)
            ui->radioButton_shock_0_75->setChecked(1);

        if(tmp.PreShockLevel == 1)
            ui->radioButton_shock_1_5->setChecked(1);

        if(tmp.PreShockLevel == 2)
            ui->radioButton_shock_3->setChecked(1);

        if(tmp.PreShockLevel == 3)
            ui->radioButton_shock_6->setChecked(1);

        /*Curve Save*/
        /*reset checkboxov*/
        ui->checkBox_shock_cyclic_savings_every->setChecked(0);
        ui->checkBox_shock_with_abort_level->setChecked(0);
        ui->checkBox_shock_at_abort->setChecked(0);
        ui->checkBox_shock_with_acceleration_limit->setChecked(0);
        ui->checkBox_shock_with_abort_level_cyclic_savings->setChecked(0);
        ui->checkBox_shock_at_abort_cyclic_savings->setChecked(0);
        ui->checkBox_shock_with_acceleration_cyclic_savings->setChecked(0);

        if (tmp.CurveSave == -431599648)
            ui->checkBox_shock_cyclic_savings_every->setChecked(1);

        else if (tmp.CurveSave == -431600096)
        {
            ui->checkBox_shock_cyclic_savings_every->setChecked(1);
            ui->checkBox_shock_with_abort_level->setChecked(1);
            ui->checkBox_shock_at_abort->setChecked(1);
            ui->checkBox_shock_with_acceleration_limit->setChecked(1);
        }

        else if (tmp.CurveSave == -431599712)
        {
            ui->checkBox_shock_cyclic_savings_every->setChecked(1);
            ui->checkBox_shock_with_abort_level->setChecked(1);
        }

        else if (tmp.CurveSave == -431599776)
        {
            ui->checkBox_shock_cyclic_savings_every->setChecked(1);
            ui->checkBox_shock_with_acceleration_limit->setChecked(1);
        }

        else if (tmp.CurveSave == -431599904)
        {
            ui->checkBox_shock_cyclic_savings_every->setChecked(1);
            ui->checkBox_shock_at_abort->setChecked(1);
        }

        else if (tmp.CurveSave == -431599968)
        {
            ui->checkBox_shock_cyclic_savings_every->setChecked(1);
            ui->checkBox_shock_at_abort->setChecked(1);
            ui->checkBox_shock_with_abort_level->setChecked(1);
        }

        else if (tmp.CurveSave == -431600032)
        {
            ui->checkBox_shock_cyclic_savings_every->setChecked(1);
            ui->checkBox_shock_with_acceleration_limit->setChecked(1);
            ui->checkBox_shock_at_abort->setChecked(1);
        }

        else if (tmp.CurveSave == -431599840)
        {
            ui->checkBox_shock_cyclic_savings_every->setChecked(1);
            ui->checkBox_shock_with_abort_level->setChecked(1);
            ui->checkBox_shock_with_acceleration_limit->setChecked(1);
        }

        /*curve save shocks*/
        ui->lineEdit_shock_curve_save_shocks->setText(QString(to_string(tmp.CurveSaveShocks).c_str()));

        /*shock inverted*/
        ui->checkBox_shock_inverted->setChecked(tmp.ShockInverted);

        /*positive/negative pre/post shock amplitude*/
        ui->lineEdit_shock_positive_pre_shock->setText(QString(to_string(tmp.PosPreShock).c_str()));
        ui->lineEdit_shock_positive_post_shock->setText(QString(to_string(tmp.PosPostShock).c_str()));
        ui->lineEdit_shock_negative_pre_shock->setText(QString(to_string(tmp.NegPreShock).c_str()));
        ui->lineEdit_shock_negative_post_shock->setText(QString(to_string(tmp.NegPostShock).c_str()));

        /*standard*/
        ui->comboBox_shock_standard->setCurrentIndex(tmp.Standard);

        /*warn Limit*/
        ui->lineEdit_shock_warn_limit->setText(QString(to_string(tmp.WarnLimit).c_str()));
    }
}


/*import pre sinus - prechod z control parameters sine*/
void MainWindow::on_pushButton_sine_import_clicked()
{
    window_import->show();
    SendFiltersToImport();
}


/*vypocet number of cycles a time per cycle pre sinus*/
void MainWindow::on_pushButton_sine_calculate_clicked()
{
    if (ui->comboBox->currentText() == "Sine")
    {
        double lower_freq = model->index(0, 1).data().toDouble();
        double upper_freq = model->index(model->rowCount() - 1, 1).data().toDouble();

        int days = ui->lineEdit_sine_day->text().toInt();
        int hours = ui->lineEdit_sine_hour->text().toInt();
        int minutes = ui->lineEdit_sine_min->text().toInt();
        int seconds = ui->lineEdit_sine_sec->text().toInt();

        int test_time = seconds + minutes*60 + hours*3600 + days*86400;

        /*pre log sweep*/
        if(ui->radioButton_sine_sweep_log->isChecked())
        {
            double time_per_cycle = (120 * log10(upper_freq/lower_freq) / log10(2)) / ui->lineEdit_sine_sweep_v->text().toDouble();
            double number_of_cycles = test_time / time_per_cycle;

            ui->lineEdit_sine_time_per->setText(QString(to_string(time_per_cycle/60).c_str()));
            ui->lineEdit_sine_numb_of_cycles->setText(QString(to_string(number_of_cycles).c_str()));
        }

        /*linear sweep*/
        else if (ui->radioButton_sine_sweep_linear->isChecked())
        {
            double time_per_cycle = (2*(upper_freq - lower_freq)) / ui->lineEdit_sine_sweep_v->text().toDouble();
            double number_of_cycles = test_time / time_per_cycle;

            ui->lineEdit_sine_time_per->setText(QString(to_string(time_per_cycle/60).c_str()));
            ui->lineEdit_sine_numb_of_cycles->setText(QString(to_string(number_of_cycles).c_str()));
        }
    }
}


/*karta import - prechod z control param. shocks*/
void MainWindow::on_pushButton_shock_import_clicked()
{
    window_import->show();
    SendFiltersToImport();
}


/*vypocet shock number per second a approx. test time*/
void MainWindow::on_pushButton_shock_calculate_clicked()
{
    int test_time = ui->lineEdit_shock_number->text().toInt() * ui->lineEdit_shock_distance->text().toDouble();

    int days = test_time/86400;
    int hours = (test_time - days*86400)/3600;
    int minutes = (test_time - hours*3600 - days*86400)/60;
    int seconds = test_time - minutes*60 - hours*3600 - days*86400;

    ui->lineEdit_shock_test_time->setText(QString(to_string(days).c_str()) + " days " + QString(to_string(hours).c_str()) + " hours " + QString(to_string(minutes).c_str()) + " minutes " + QString(to_string(seconds).c_str()) + " seconds ");

    double number_per_sec = 1 / ui->lineEdit_shock_distance->text().toDouble();

    ui->lineEdit_shock_number_per_sec->setText(QString(to_string(number_per_sec).c_str()));
}


/*karta import - prechod z input channels*/
void MainWindow::on_pushButton_snimace_import_clicked()
{
    window_import->show();
    SendFiltersToImport();
}


/*import nastaveni snimacov*/
void MainWindow::receive_test_file_year_number_snimace(int test_year, int test_number)
{
    test_file_year = test_year;
    test_file_number = test_number;
    sqlite3* DB;
    sqlite3_open(QSqlDatabase::database("tests_parameters").databaseName().toStdString().c_str(), &DB);

    CVibrationsCommon tmp;
    tmp.FetchFromVibrationsCommon(DB, test_file_number, test_file_year);
    sqlite3_close(DB);

    for (int i = 0; i < 8; i++)
    {
        list_type_of_input.at(i)->setCurrentIndex(tmp.TypeOfInput[i]);
        list_sensitivity.at(i)->setText(to_string(tmp.Sensitivity[i]).c_str());
        list_unit.at(i)->setCurrentIndex(tmp.Unit[i]);
        list_limits_lower.at(i)->setText(to_string(tmp.LowerLimit[i]).c_str());
        list_limits_upper.at(i)->setText(to_string(tmp.UpperLimit[i]).c_str());
        list_info_text.at(i)->setText(tmp.InfoText[i].c_str());
    }
}


/*uzivatelske rozhranie snimacov*/
void MainWindow::on_pushButton_snimace_refresh_clicked()
{
    for (int i = 0; i < 8; i++)
    {
        if(ui->comboBox->currentText() == "Sine")
        {
            list_unit.at(i)->setEnabled(1);
            list_unit.at(i)->setPalette(*palette_enabled);

            if(list_unit.at(i)->currentText() == "a")
                list_sensitivity_unit.at(i)->setText("mV/m/s^2");
            else if(list_unit.at(i)->currentText() == "v")
                list_sensitivity_unit.at(i)->setText("mV/m/s");
            else if(list_unit.at(i)->currentText() == "d")
                list_sensitivity_unit.at(i)->setText("mV/mm");
            else if(list_unit.at(i)->currentText() == "?")
                list_sensitivity_unit.at(i)->setText("EU/mV");

            list_sensitivity_unit.at(i)->setReadOnly(1);
            list_limits_lower_unit.at(i)->setReadOnly(1);
            list_limits_upper_unit.at(i)->setReadOnly(1);
        }

        else if(ui->comboBox->currentText() == "Random" || ui->comboBox->currentText() == "Shock")
        {
            list_sensitivity_unit.at(i)->setText("mV/m/s^2");
            list_sensitivity_unit.at(i)->setReadOnly(1);
            list_unit.at(i)->setEnabled(0);
            list_unit.at(i)->setPalette(*palette);

            list_limits_lower_unit.at(i)->setReadOnly(1);
            list_limits_upper_unit.at(i)->setReadOnly(1);
        }

        if(list_type_of_input.at(i)->currentText() == "Abort")
        {
            list_unit.at(i)->setEnabled(0);
            list_unit.at(i)->setPalette(*palette);
            list_sensitivity.at(i)->setReadOnly(1);
            list_sensitivity.at(i)->setPalette(*palette);
            list_limits_lower_unit.at(i)->setText("V");
            list_limits_upper_unit.at(i)->setText("V");
            list_limits_lower_unit.at(i)->setReadOnly(1);
            list_limits_upper_unit.at(i)->setReadOnly(1);
        }

        else if(list_type_of_input.at(i)->currentText() == "Control" || list_type_of_input.at(i)->currentText() == "Measure")
        {
            list_sensitivity.at(i)->setReadOnly(0);
            list_sensitivity.at(i)->setPalette(*palette_enabled);
            list_limits_lower_unit.at(i)->setText("m/s^2");
            list_limits_upper_unit.at(i)->setText("m/s^2");
            if (ui->comboBox->currentText() == "Sine")
            {
                list_unit.at(i)->setEnabled(1);
                list_unit.at(i)->setPalette(*palette_enabled);
            }
        }
    }
}


/*level tabulka - pridat novy zaznam*/
void MainWindow::on_pushButton_level_new_clicked()
{
    QString test_time = ui->lineEdit_level_days->text() + ":" + ui->lineEdit_level_hours->text() + ":" + ui->lineEdit_level_minutes->text() + ":" + ui->lineEdit_level_seconds->text();

    QStandardItem* item_level = new QStandardItem;
    model_level->insertRow(model_level->rowCount(), item_level);

    QModelIndex index = model_level->index(model_level->rowCount() - 1, 0);
    model_level->setData(index, ui->lineEdit_level->text());

    index = model_level->index(model_level->rowCount() - 1, 1);
    model_level->setData(index, test_time);
}


/*level tabulka delete*/
void MainWindow::on_pushButton_level_delete_clicked()
{
    QItemSelectionModel* selectionModel = ui->tableView_level->selectionModel();
    QModelIndexList indexes = selectionModel->selectedRows();

    if(!indexes.isEmpty())
        model_level->removeRows(indexes.first().row(), indexes.count());
}


/*/ importovacie okno - prechod z karty level /*/
void MainWindow::on_pushButton_level_import_clicked()
{
    window_import->show();
    SendFiltersToImport();
}


/* import nastaveni level karty */
void MainWindow::receive_test_file_year_number_level(int test_year, int test_number)
{
    test_file_year = test_year;
    test_file_number = test_number;
    sqlite3* DB;
    sqlite3_open(QSqlDatabase::database("tests_parameters").databaseName().toStdString().c_str(), &DB);

    CLevels tmp;
    vector<CLevels> vec_levels;
    vec_levels = tmp.FetchFromLevels(DB, test_file_year, test_file_number);
    sqlite3_close(DB);

    if(model_level->rowCount() != 0)
        model_level->removeRows(0, model_level->rowCount());

    QStandardItem* item = new QStandardItem;
    QModelIndex index;

    int test_time;
    int days;
    int hours;
    int minutes;
    int seconds;

    for(size_t i = 0; i < vec_levels.size(); i++)
    {
        model_level->insertRow(model_level->rowCount(), item);

        index = model_level->index(model_level->rowCount() - 1, 0);
        model_level->setData(index, vec_levels.at(i).Level);

        test_time = vec_levels.at(i).Time;
        days = test_time/86400;
        hours = (test_time - days*86400)/3600;
        minutes = (test_time - hours*3600 - days*86400)/60;
        seconds = test_time - minutes*60 - hours*3600 - days*86400;

        QString test_time_str = QString(to_string(days).c_str()) + ":" + QString(to_string(hours).c_str()) + ":" + QString(to_string(minutes).c_str()) + ":" + QString(to_string(seconds).c_str());

        index = model_level->index(model_level->rowCount() - 1, 1);
        model_level->setData(index, test_time_str);
    }
}


/*tolerance band tabulka - pridat novy zaznam*/
void MainWindow::on_pushButton_tolerance_new_clicked()
{
    QStandardItem* item = new QStandardItem;
    model_tolerance->insertRow(model_tolerance->rowCount(), item);

    QModelIndex index = model_tolerance->index(model_tolerance->rowCount() - 1, 0);
    model_tolerance->setData(index, ui->lineEdit_tolerance_time->text());

    index = model_tolerance->index(model_tolerance->rowCount() - 1, 1);
    model_tolerance->setData(index, ui->lineEdit_tolerance_abort_pos->text());

    index = model_tolerance->index(model_tolerance->rowCount() - 1, 2);
    model_tolerance->setData(index, ui->lineEdit_tolerance_abort_neg->text());
}


/*tolerance band tabulka delete*/
void MainWindow::on_pushButton_tolerance_delete_clicked()
{
    QItemSelectionModel* selectionModel = ui->tableView_tolerance->selectionModel();
    QModelIndexList indexes = selectionModel->selectedRows();

    if(!indexes.isEmpty())
        model_tolerance->removeRows(indexes.first().row(), indexes.count());
}


/*/ importovacie okno - prechod z karty level /*/
void MainWindow::on_pushButton_tolerance_import_clicked()
{
    window_import->show();
    SendFiltersToImport();
}


/* import nastaveni tolerance band karty */
void MainWindow::receive_test_file_year_number_tolerance_band(int test_year, int test_number)
{
    test_file_year = test_year;
    test_file_number = test_number;
    sqlite3* DB;
    sqlite3_open(QSqlDatabase::database("tests_parameters").databaseName().toStdString().c_str(), &DB);

    CToleranceBand tmp;
    vector<CToleranceBand> vec_tolerance;
    vec_tolerance = tmp.FetchFromToleranceBand(DB, test_file_number, test_file_year);
    sqlite3_close(DB);

    if(model_tolerance->rowCount() != 0)
        model_tolerance->removeRows(0, model_tolerance->rowCount());

    QStandardItem* item = new QStandardItem;
    QModelIndex index;

    for(size_t i = 0; i < vec_tolerance.size(); i++)
    {
        model_tolerance->insertRow(model_tolerance->rowCount(), item);

        index = model_tolerance->index(model_tolerance->rowCount() - 1, 0);
        model_tolerance->setData(index, vec_tolerance.at(i).Time);

        index = model_tolerance->index(model_tolerance->rowCount() - 1, 1);
        model_tolerance->setData(index, vec_tolerance.at(i).AbortLimitPos);

        index = model_tolerance->index(model_tolerance->rowCount() - 1, 2);
        model_tolerance->setData(index, vec_tolerance.at(i).AbortLimitNeg);
    }
}


/************** generovanie test number a year*************/
void MainWindow::on_pushButton_export_generate_clicked()
{
    QSqlDatabase::database("tests_parameters").open();

    QSqlQuery query_number_year(QSqlDatabase::database("tests_parameters"));
    query_number_year.prepare("SELECT testFileYear, testFileNumber FROM main_tests_table WHERE ROWID IN ( SELECT max( ROWID ) FROM main_tests_table )");
    query_number_year.exec();
    query_number_year.first();

    test_file_year_new = query_number_year.value(0).toInt();
    test_file_number_new = query_number_year.value(1).toInt() + 1;

    const int MAXLEN = 80;
    char s[MAXLEN];
    time_t now = time(0);
    strftime(s, MAXLEN, "%Y", localtime(&now));

    if(test_file_year_new != stoi(s))
    {
        test_file_year_new = stoi(s);
        test_file_number_new = 1;
    }

    ui->lineEdit_export_test_year->setText(QString(to_string(test_file_year_new).c_str()));
    ui->lineEdit_export_test_number->setText(QString(to_string(test_file_number_new).c_str()));

    QSqlDatabase::database("tests_parameters").close();

    if(ui->lineEdit_export_file_name->text().isEmpty())
        ui->lineEdit_export_file_name->setText("#testNumber#_#testYear#_#company&order#_#axis#_#XXX#_#testItem000#testArrangementOrder#");

    ui->pushButton_export_duplicate->setDisabled(1);
}


/*******funkcia na vytiahnutie poptavky priradenej testu z main_tests_table*******/
QVariantList FetchInquiry(int test_year, int test_number)
{
    QSqlDatabase::database("tests_parameters").open();
    QSqlQuery query(QSqlDatabase::database("tests_parameters"));
    query.prepare("SELECT relCustomerInquiryYear, relCustomerInquiryNumber, relCustomerInquiryIndex FROM main_tests_table WHERE "
                               "testFileYear = ? AND testFileNumber = ?");
    query.bindValue(0, test_year);
    query.bindValue(1, test_number);

    query.exec();
    QVariantList tmp;
    while(query.next())
    {
        tmp.push_back(query.value(0));
        tmp.push_back(query.value(1));
        tmp.push_back(query.value(2));
    }

    QSqlDatabase::database("tests_parameters").close();
    return tmp;
}


/*****funkcia na vytiahnutie cisla zakazky a ID firmy z main_list, DB kniha zakaziek podla ponuky******/
QVariantList FetchContractAndCompanyID(int inquiry_year, int inquiry_number, int inquiry_index)
{
    QSqlDatabase::database("kniha_zakazek").open();
    QSqlQuery query(QSqlDatabase::database("kniha_zakazek"));
    query.prepare("SELECT relCompanyIdNumber, contractOrderForCompanyInYear, contractYear, contractNumber FROM main_list WHERE "
                               "customerInquiryYear = ? AND customerInquiryNumber = ? AND customerInquiryIndex = ?");

    query.bindValue(0, inquiry_year);
    query.bindValue(1, inquiry_number);
    query.bindValue(2, inquiry_index);

    query.exec();
    QVariantList tmp;

    while(query.next())
    {
        tmp.push_back(query.value(0));
        tmp.push_back(query.value(1));
        tmp.push_back(query.value(2));
        tmp.push_back(query.value(3));
    }
    QSqlDatabase::database("kniha_zakazek").close();

    return tmp;
}


/*******ziskanie mena firmy********/
QString FetchCompanyName(QVariant company_id)
{
    QSqlQuery query(QSqlDatabase::database("kniha_zakazek"));
    query.prepare("SELECT companyNameForFolders FROM companies WHERE companyIdNumber = ?");

    query.bindValue(0, company_id);

    query.exec();
    QString company_name;

    while (query.next())
        company_name = query.value(0).toString();

    QSqlDatabase::database("kniha_zakazek").close();

    return company_name;
}


/*******ziskanie udajov z knihy zakaziek podla zakazky********/
QVariantList FetchUsingContract(QVariant contract_year, QVariant contract_number)
{
    QSqlDatabase::database("kniha_zakazek").open();
    QSqlQuery query(QSqlDatabase::database("kniha_zakazek"));

    query.prepare("SELECT relCompanyIdNumber, contractOrderForCompanyInYear, customerInquiryYear, customerInquiryNumber, customerInquiryIndex FROM main_list WHERE "
                               "contractYear = ? AND contractNumber = ?");
    query.bindValue(0, contract_year);
    query.bindValue(1, contract_number);

    query.exec();
    QVariantList tmp;

    while(query.next())
    {
        tmp.push_back(query.value(0));
        tmp.push_back(query.value(1));
        tmp.push_back(query.value(2));
        tmp.push_back(query.value(3));
        tmp.push_back(query.value(4));
    }
    QSqlDatabase::database("kniha_zakazek").close();

    return tmp;
}


/**********prijatie a nacitanie profilu zo suboru*******/
void MainWindow::receive_profile(QVariant profile_type, QVariant profile_id, QVariant inqr_year, QVariant inqr_number, QVariant inqr_index)
{
    ui->comboBox->setCurrentIndex(profile_type.toInt());
    ui->pushButton_load_profiles->clicked();

    QString filter_profiles;
    QString filter_common = "profileId IN (" + profile_id.toString() + ")";

    if(ui->comboBox->currentText() == "Sine" || ui->comboBox->currentText() == "Random")
    {
        filter_profiles = "adProfileId IN (" + profile_id.toString() + ")";
    }

    else if(ui->comboBox->currentText() == "Shocks")
    {
        filter_profiles = "profileId IN (" + profile_id.toString() + ")";
    }

    model->setFilter(filter_profiles);
    model_common->setFilter(filter_common);
    ui->checkBox_export_common->setChecked(0);
    if(model_common->rowCount() != 0)
        ui->checkBox_export_common->setChecked(1);

    if(model->rowCount() != 0)
    {
        selectionModel = ui->tableView_profiles->selectionModel();
        QModelIndex topLeft = model->index(0, 0);
        selectionModel->select(topLeft, QItemSelectionModel::Select);
    }

    ui->checkBox_contract->setChecked(0);
    ui->checkBox_inqr->setChecked(1);
    ui->lineEdit_filter_inqr_year->setText(inqr_year.toString());
    ui->lineEdit_filter_inqr_id->setText(inqr_number.toString());
    ui->lineEdit_filter_inqr_index->setText(inqr_index.toString());
}


/************ okno vyberu skusobnej polozky/zostavy ***************/
void MainWindow::on_pushButton_export_test_item_selection_clicked()
{
    window_test_item->show();

    emit send_is_test_item(ui->radioButton_export_test_item->isChecked(), ui->lineEdit_export_inquiry_year->text(), ui->lineEdit_export_inquiry_number->text(), ui->lineEdit_export_inquiry_index->text());
    emit send_is_test_arrangement(ui->radioButton_export_test_arrangement->isChecked(), ui->lineEdit_export_inquiry_year->text(), ui->lineEdit_export_inquiry_number->text(), ui->lineEdit_export_inquiry_index->text());
}


/********** zapis cisla a roku skusobnej polozky do line editu ************/
void MainWindow::receive_test_item_year_number(QVariant year, QVariant number)
{
    ui->lineEdit_export_test_item_year->setText(year.toString());
    ui->lineEdit_export_test_item_number->setText(number.toString());
}


/********** zapis poradia skusobnej zostavy do line editu ************/
void MainWindow::receive_test_arrangement_order(int arrang_order)
{
    ui->lineEdit_export_test_arrangement_order->setText(QString(to_string(arrang_order).c_str()));
}


/********* zapis suboru do sqlite databazy, insert alebo update ************/
void MainWindow::on_pushButton_export_insert_info_to_test_parameters_clicked()
{
    sqlite3* DB;
    sqlite3_open(QSqlDatabase::database("tests_parameters").databaseName().toStdString().c_str(), &DB);

    bool test_exists = isTestExisting();
    int test_year = ui->lineEdit_export_test_year->text().toInt();
    int test_number = ui->lineEdit_export_test_number->text().toInt();

    if(test_exists)
        ui->pushButton_export_create_test_file->setText("Nahradit soubor");

    else if(!test_exists)
        ui->pushButton_export_create_test_file->setText("Vytvořit soubor");


    //nastavenia testu
    if(ui->comboBox->currentText() == "Sine")
        {
            /*Control Parameters Sine*/
            CSineVibrations tmp_sine_vibr;
            tmp_sine_vibr = CollectSineVibrations();

            if(!test_exists)
                tmp_sine_vibr.InsertToSineVibrations(DB);

            else if(test_exists)
            {
                CSineVibrations tmp_old;
                tmp_old.FetchFromSineVibrations(DB, test_year, test_number);
                tmp_sine_vibr.UpdateSineVibrations(DB, tmp_old);
            }
        }

    else if(ui->comboBox->currentText() == "Random")
        {
            /*control parameters random*/
            CRandomVibrations tmp_rand_vibr;
            tmp_rand_vibr = CollectRandomVibrations();

            /*levels*/
            CLevels tmp_class_level;
            vector<CLevels> tmp_vec_level;
            tmp_vec_level = CollectLevels();

            if(!test_exists)
            {
                tmp_rand_vibr.InsertToRandomVibrations(DB);
                tmp_class_level.InsertToLevels(DB, tmp_vec_level);
            }

            else if(test_exists)
            {
                CRandomVibrations tmp_old;
                tmp_old.FetchFromRandomVibrations(DB, test_year, test_number);
                tmp_rand_vibr.UpdateRandomVibrations(DB, tmp_old);
            }
        }

    else if(ui->comboBox->currentText() == "Shocks")
        {
            /********control parameters shock*********/
            CShocks tmp_shocks;
            tmp_shocks = CollectShocks();

            /*****************Tolerance band*************/
            CToleranceBand tmp_class_tolerance_band;
            vector<CToleranceBand> tmp_vec_tolerance_band;
            tmp_vec_tolerance_band = CollectToleranceBand();

            if(!test_exists)
            {
                tmp_shocks.InsertToShocks(DB);
                tmp_class_tolerance_band.InsertToToleranceBand(DB, tmp_vec_tolerance_band);
            }

            else if(test_exists)
            {
                CShocks tmp_old;
                tmp_old.FetchFromShocks(DB, test_year, test_number);
                tmp_shocks.UpdateShocks(DB, tmp_old);
            }
        }

    CVibrationsCommon tmp_snimace;
    tmp_snimace = CollectVibrationsCommon();

    if(!test_exists)
        tmp_snimace.InsertToVibrationsCommon(DB);

    else if(test_exists)
    {
        CVibrationsCommon tmp_snimace_old;
        tmp_snimace_old.FetchFromVibrationsCommon(DB, test_number, test_year);

        if(ui->comboBox->currentText() == "Sine")
            tmp_snimace.UpdateVibrationsCommon(DB, tmp_snimace_old, 1);

        else if(ui->comboBox->currentText() != "Sine")
            tmp_snimace.UpdateVibrationsCommon(DB, tmp_snimace_old, 0);
    }

    sqlite3_close(DB);

    QSqlDatabase::database("tests_parameters").open();

    //ak nie je cislo suboru v  databaze, insert
    if(!test_exists)
    {
        /*******main_test_table*********/
        QSqlQuery query_main_tests_table(QSqlDatabase::database("tests_parameters"));
        query_main_tests_table.prepare("INSERT INTO main_tests_table VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");

        query_main_tests_table.bindValue(0, ui->lineEdit_export_test_year->text());
        query_main_tests_table.bindValue(1, ui->lineEdit_export_test_number->text());
        query_main_tests_table.bindValue(2, ui->lineEdit_export_inquiry_year->text());
        query_main_tests_table.bindValue(3, ui->lineEdit_export_inquiry_number->text());
        query_main_tests_table.bindValue(4, ui->lineEdit_export_inquiry_index->text());
        query_main_tests_table.bindValue(6, ui->lineEdit_export_test_axis->text());

        if (ui->comboBox->currentText() == "Sine")
            query_main_tests_table.bindValue(7, 7);

        else if (ui->comboBox->currentText() == "Random")
            query_main_tests_table.bindValue(7, 10);

        else if (ui->comboBox->currentText() == "Shocks")
            query_main_tests_table.bindValue(7, 13);

        QString date_ = ui->lineEdit_export_date->text();
        date_ = date_.replace(4, 1, '/');
        date_ = date_.replace(7, 1, '/');
        query_main_tests_table.bindValue(9, date_);

        query_main_tests_table.bindValue(10, ui->lineEdit_export_test_item_year->text());
        query_main_tests_table.bindValue(11, ui->lineEdit_export_test_item_number->text());
        query_main_tests_table.bindValue(12, ui->lineEdit_export_test_arrangement_order->text());

        query_main_tests_table.exec();

        /*******test_parts*********/
        QSqlQuery query_test_parts(QSqlDatabase::database("tests_parameters"));
        query_test_parts.prepare("INSERT INTO test_parts VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");

        query_test_parts.bindValue(0, ui->lineEdit_export_test_year->text());
        query_test_parts.bindValue(1, ui->lineEdit_export_test_number->text());
        query_test_parts.bindValue(2, 0);

        if (ui->comboBox->currentText() == "Sine")
            query_test_parts.bindValue(3, 0);

        else if (ui->comboBox->currentText() == "Random")
            query_test_parts.bindValue(3, 1);

        else if (ui->comboBox->currentText() == "Shocks")
            query_test_parts.bindValue(3, 2);

        query_test_parts.bindValue(4, 1);
        query_test_parts.bindValue(5, 1);

        QString test_file_name = ui->lineEdit_export_file_name->text() + ui->lineEdit_export_postfix->text();

        query_test_parts.bindValue(8, test_file_name);

        if(model->rowCount() != 0)
        {
            QModelIndex index = model->index(0,0);
            query_test_parts.bindValue(9, model->data(index));
        }

        query_test_parts.exec();
    }

    //ak je subor v db update
    else if(test_exists)
    {
        QSqlQuery query_info_update(QSqlDatabase::database("tests_parameters"));

        query_info_update.prepare("UPDATE main_tests_table SET testAxis = ?, directoryTestDate = ?, relCustomerInquiryYear = ?,"
                           " relCustomerInquiryNumber = ?, relCustomerInquiryIndex = ?, relTestItemYear = ?, relTestItemNumber = ?, relTestArrangementOrder = ? WHERE "
                           "testFileYear = ? AND testFileNumber = ?");

        query_info_update.bindValue(0, ui->lineEdit_export_test_axis->text());
        query_info_update.bindValue(1, ui->lineEdit_export_date->text());
        query_info_update.bindValue(2, ui->lineEdit_export_inquiry_year->text());
        query_info_update.bindValue(3, ui->lineEdit_export_inquiry_number->text());
        query_info_update.bindValue(4, ui->lineEdit_export_inquiry_index->text());
        query_info_update.bindValue(5, ui->lineEdit_export_test_item_year->text());
        query_info_update.bindValue(6, ui->lineEdit_export_test_item_number->text());
        query_info_update.bindValue(7, ui->lineEdit_export_test_arrangement_order->text());
        query_info_update.bindValue(8, ui->lineEdit_export_test_year->text());
        query_info_update.bindValue(9, ui->lineEdit_export_test_number->text());

        query_info_update.exec();

        query_info_update.prepare("UPDATE test_parts SET testFileName = ?, relProfileId = ? WHERE "
                           "adTestFileYear = ? AND adTestFileNumber = ?");

        query_info_update.bindValue(0, ui->lineEdit_export_file_name->text() + ui->lineEdit_export_postfix->text());

        if(model->rowCount() != 0)
        {
            QModelIndex index = model->index(0,0);
            query_info_update.bindValue(1, model->data(index));
        }

        query_info_update.bindValue(2, ui->lineEdit_export_test_year->text());
        query_info_update.bindValue(3, ui->lineEdit_export_test_number->text());

        query_info_update.exec();
    }

    QSqlDatabase::database("tests_parameters").close();

    //po zapise suboru sa nazov tlacitka zmeni na "aktualizovat", refreshne sa label...
    ui->pushButton_export_insert_info_to_test_parameters->setText("Aktualizovat soubor v databáze");
    LabelLastTestRefresh();
    DuplicateEnableDisable();
    ui->pushButton_export_create_test_file->setEnabled(1);
    ui->pushButton_export_logbook_times->setEnabled(1);
    ui->pushButton_export_save_actual_parameters->setEnabled(1);
    ui->pushButton_export_duplicate->setEnabled(1);
}


/*******zlozenie nazvu suboru z makra********/
QString MainWindow::FileNameFromMacro()
{
    QSqlDatabase::database("tests_parameters").open();

    QSqlQuery query_file_name(QSqlDatabase::database("tests_parameters"));
    query_file_name.exec("SELECT testFileName FROM test_parts WHERE adTestFileYear = " + ui->lineEdit_export_test_year->text()
                         + " AND adTestFileNumber = " + ui->lineEdit_export_test_number->text());

    query_file_name.first();
    QString test_file_name_macro = query_file_name.value(0).toString();

    QSqlDatabase::database("tests_parameters").close();

    QString test_file_name = "";

    if(!test_file_name_macro.isEmpty())
    {
        QStringList divided_text = test_file_name_macro.split('#');
        int instruction;

        for (int i = 0; i < divided_text.size(); i++)
        {
            instruction = 0;

            if (divided_text[i] == "testNumber")
            {
                instruction = 1;
                test_file_name += PadZeros(ui->lineEdit_export_test_number->text(), 4);
            }

            if (divided_text[i] == "testYear")
            {
                instruction = 2;
                test_file_name += ui->lineEdit_export_test_year->text();
            }

            if (divided_text[i] == "company&order")
            {
                instruction = 3;
                test_file_name += ui->lineEdit_export_company_name->text();
                test_file_name += ui->lineEdit_export_company_order->text();
            }

            if (divided_text[i] == "axis")
            {
                instruction = 4;
                test_file_name += ui->lineEdit_export_test_axis->text();
            }

            if (divided_text[i] == "testItem000")
            {
                instruction = 5;
                test_file_name += PadZeros(ui->lineEdit_export_test_item_number->text(), 3);
            }

            if (divided_text[i] == "testArrangementOrder")
            {
                instruction = 6;
                test_file_name += ui->lineEdit_export_test_arrangement_order->text();
                test_file_name += divided_text.last();
            }

            if (instruction == 0)
                test_file_name += divided_text[i];
        }
    }

    int pos = test_file_name.lastIndexOf('.');
    test_file_name = test_file_name.mid(0, pos);
    qDebug() << test_file_name << endl;
    return test_file_name;
}


/*******zlozenie suborovej cesty******/
QString MainWindow::FilePath()
{
    QString file_path_1;

    int inqr_year = ui->lineEdit_export_inquiry_year->text().toInt();
    int inqr_number = ui->lineEdit_export_inquiry_number->text().toInt();
    int inqr_index = ui->lineEdit_export_inquiry_index->text().toInt();

    QVariantList tmp = FetchContractAndCompanyID(inqr_year, inqr_number, inqr_index);

    if (ui->lineEdit_export_company_order->text().isEmpty())
        file_path_1 = file_new_base_path_no_order + ui->lineEdit_export_company_name->text();

    else if (!ui->lineEdit_export_company_order->text().isEmpty())
        file_path_1 = file_new_base_path + "Z1_" + tmp[3].toString() + "_" + tmp[2].toString() + "_" + ui->lineEdit_export_company_name->text() + ui->lineEdit_export_company_order->text();

    QString date_ = ui->lineEdit_export_date->text();
    date_ = date_.replace(4, 1, '_');
    date_ = date_.replace(7, 1, '_');

    QString file_path_2 = file_path_1 + "/" + date_;

    QString file_path_final = file_path_2 + "/" + FileNameFromMacro() + ui->lineEdit_export_postfix->text();

    return file_path_final;
}


/******vytvori testovaci subor a jeho priecinky******/
void MainWindow::CreateFileTest(QString file_path)
{
    QString directory = file_path;
    int pos = file_path.lastIndexOf('/');
    directory.truncate(pos);
    QDir().mkpath(directory);

    if(ui->comboBox->currentText() == "Random")
        CreateTestFile(file_default_random.toStdString(), file_path.toStdString());

    else if(ui->comboBox->currentText() == "Sine")
        CreateTestFile(file_default_sine.toStdString(), file_path.toStdString());

    else if(ui->comboBox->currentText() == "Shocks")
        CreateTestFile(file_default_shock.toStdString(), file_path.toStdString());
}


/*************** vytvorenie testovacieho suboru *************/
void MainWindow::on_pushButton_export_create_test_file_clicked()
{
    QString file_path_final = FilePath();

    if(ui->pushButton_export_create_test_file->text() == "Vytvořit soubor")
    {
         if(ui->checkBox_export_create_file_out->isChecked())
            file_path_final = QDir::homePath() + "/" +  ui->lineEdit_export_file_name->text();
    }

    else if(ui->pushButton_export_create_test_file->text() == "Nahradit soubor")
        QDir().remove(previous_file_path);

    CreateFileTest(file_path_final);

    string file_path_final_mdb;

    /*Random*/
    if(ui->comboBox->currentText() == "Random")
    {
        file_path_final_mdb = RenameToMDB(file_path_final.toStdString());

        CRandomProfiles tmp_class_rand_profiles;
        vector<CRandomProfiles> tmp_vec_profiles;
        tmp_vec_profiles = CollectRandomProfiles();
        if(model->rowCount() != 0)
            tmp_class_rand_profiles.InsertFrom_RandomProfiles_ToTestFileRandom(file_path_final_mdb, tmp_vec_profiles);

        /*control parameters random*/
        CRandomVibrations tmp_rand_vibr;
        tmp_rand_vibr = CollectRandomVibrations();
        tmp_rand_vibr.InsertFrom_RandomVibrations_ToTestFileRandom(file_path_final_mdb);

        /*karta Level*/
        CLevels tmp_class_level;
        vector<CLevels> tmp_vec_level;
        tmp_vec_level = CollectLevels();
        if(model_level->rowCount() != 0)
            tmp_class_level.InsertFrom_Levels_ToTestFileRandom(file_path_final_mdb, tmp_vec_level);
    }

    /******** Sine **********/
    else if(ui->comboBox->currentText() == "Sine")
    {
        file_path_final_mdb = RenameToMDB(file_path_final.toStdString());

        CSineProfiles tmp_class_sine_profiles;
        vector<CSineProfiles> tmp_vec_sine_profiles;
        tmp_vec_sine_profiles = CollectSineProfiles();
        if(model->rowCount() != 0)
            tmp_class_sine_profiles.InsertFrom_SineProfiles_ToTestFileSine(file_path_final_mdb, tmp_vec_sine_profiles);

        /*Control Parameters Sine*/
        CSineVibrations tmp_sine_vibr;
        tmp_sine_vibr = CollectSineVibrations();
        tmp_sine_vibr.InsertFrom_SineVibrations_ToTestFileSine(file_path_final_mdb);
    }

    /******** Shocks **********/
    else if(ui->comboBox->currentText() == "Shocks")
    {
        file_path_final_mdb = RenameToMDB(file_path_final.toStdString());

        /**********nastavenia profilu shock***********/
        CShockProfiles tmp_shock_profiles;
        tmp_shock_profiles = CollectShockProfiles();
        if(model->rowCount() != 0)
            tmp_shock_profiles.InsertFrom_ShockProfiles_ToTestFileShock(file_path_final_mdb);

        /********control parameters shock*********/
        CShocks tmp_shocks;
        tmp_shocks = CollectShocks();
        tmp_shocks.InsertFrom_Shocks_ToTestFileShock(file_path_final_mdb);

        /*****************Tolerance band*************/
        CToleranceBand tmp_class_tolerance_band;
        vector<CToleranceBand> tmp_vec_tolerance_band;
        tmp_vec_tolerance_band = CollectToleranceBand();

        /*tolerance band sa presuva do suboru iba ak je standard = free*/
        if(ui->comboBox_shock_standard->currentText() == "free" && model_tolerance->rowCount() != 0)
            tmp_class_tolerance_band.InsertFrom_ToleranceBand_ToTestFileShock(file_path_final_mdb, tmp_vec_tolerance_band);
    }

    /*nastavenia snimacov*/
    CVibrationsCommon tmp_snimace;
    tmp_snimace = CollectVibrationsCommon();

    if(ui->comboBox->currentText() == "Random")
    {
        tmp_snimace.InsertFrom_VibrationsCommon_ToTestFileRandom(file_path_final_mdb);
        RenameToRau(file_path_final_mdb);
    }

    else if(ui->comboBox->currentText() == "Sine")
    {
        tmp_snimace.InsertFrom_VibrationsCommon_ToTestFileSine(file_path_final_mdb);
        RenameToSin(file_path_final_mdb);
    }

    else if(ui->comboBox->currentText() == "Shocks")
    {
        tmp_snimace.InsertFrom_VibrationsCommon_ToTestFileShock(file_path_final_mdb);
        RenameToShk(file_path_final_mdb);
    }

    ui->pushButton_export_create_test_file->setText("Nahradit soubor");
    previous_file_path = FilePath();
}


/*********zozbieranie random profilov z rozhrania*******/
vector<CRandomProfiles> MainWindow::CollectRandomProfiles()
{
    CRandomProfiles tmp_class_rand_profiles;
    vector<CRandomProfiles> tmp_vec_profiles;

    if(model->rowCount() != 0)
    {
        for(int i = 0; i < model->rowCount(); i++)
        {
            QModelIndex index = model->index(i, 0);
            tmp_class_rand_profiles.ProfileID = index.data().toInt();

            index = model->index(i, 1);
            tmp_class_rand_profiles.Frequency = index.data().toDouble();

            index = model->index(i, 2);
            tmp_class_rand_profiles.Amplitude = index.data().toDouble();

            index = model->index(i, 3);
            tmp_class_rand_profiles.WarnLimitPositive = index.data().toDouble();

            index = model->index(i, 4);
            tmp_class_rand_profiles.WarnLimitNegative = index.data().toDouble();

            index = model->index(i, 5);
            tmp_class_rand_profiles.AbortLimitPositive = index.data().toDouble();

            index = model->index(i, 6);
            tmp_class_rand_profiles.AbortLimitNegative = index.data().toDouble();

            tmp_vec_profiles.push_back(tmp_class_rand_profiles);
        }

        return tmp_vec_profiles;
    }

    else
        return tmp_vec_profiles;
}


/*********zozbieranie random control parameters z rozhrania*******/
CRandomVibrations MainWindow::CollectRandomVibrations()
{
    CRandomVibrations tmp_rand_vibr;

    tmp_rand_vibr.TestFileYear = ui->lineEdit_export_test_year->text().toInt();
    tmp_rand_vibr.TestFileNumber = ui->lineEdit_export_test_number->text().toInt();

    /*Clipping*/
    if (ui->radioButton_sigma->isChecked())
        tmp_rand_vibr.Clipping = 2.5;

    else if (ui->radioButton_3->isChecked())
        tmp_rand_vibr.Clipping = 3;

    else if (ui->radioButton_4->isChecked())
        tmp_rand_vibr.Clipping = 4;

    else if (ui->radioButton_5->isChecked())
        tmp_rand_vibr.Clipping = 5;

    else if (ui->radioButton_Aus->isChecked())
        tmp_rand_vibr.Clipping = 6;

    /*Multi channel*/
    if (ui->radioButton_average->isChecked())
        tmp_rand_vibr.MultiChannelControl = 0;

    else if (ui->radioButton_extremal->isChecked())
        tmp_rand_vibr.MultiChannelControl = 1;

    /*mass test item/fixture*/
    tmp_rand_vibr.MassTestItems = ui->lineEdit_mass_test_item->text().toDouble();
    tmp_rand_vibr.MassTestFixture = ui->lineEdit_mass_test_fixture->text().toDouble();

    /*connection with slip table*/
    tmp_rand_vibr.ConnectionWithSlipTable = ui->checkBox_slip_table->isChecked();

    /*abort, warn limits*/
    tmp_rand_vibr.AbortLimitWithLines = ui->lineEdit_abort_limit->text().toInt();
    tmp_rand_vibr.WarnLimitWithLines = ui->lineEdit_warn_limit->text().toInt();

    /*level shift*/
    if (ui->radioButton_manual->isChecked())
        tmp_rand_vibr.LevelShift = 0;

    else if (ui->radioButton_auto->isChecked())
        tmp_rand_vibr.LevelShift = 1;

    else if (ui->radioButton_optimized->isChecked())
        tmp_rand_vibr.LevelShift = 2;

    /*DOF*/
    tmp_rand_vibr.DegreesOfFreedom = ui->lineEdit_dof->text().toInt();

    /*Pretest level*/
    tmp_rand_vibr.PreTestLevel = ui->lineEdit_pretest_level->text().toInt();

    /*enable pos dB values*/
    tmp_rand_vibr.EnablePosValues = ui->checkBox_random_enable_pos->isChecked();

    /*selection of bandwidth*/
    if (ui->comboBox_random_selection_of->currentIndex() == 0)
        tmp_rand_vibr.SelectionOfBandwidth = 100;

    else if (ui->comboBox_random_selection_of->currentIndex() == 1)
        tmp_rand_vibr.SelectionOfBandwidth = 200;

    else if (ui->comboBox_random_selection_of->currentIndex() == 2)
        tmp_rand_vibr.SelectionOfBandwidth = 500;

    else if (ui->comboBox_random_selection_of->currentIndex() == 3)
        tmp_rand_vibr.SelectionOfBandwidth = 1000;

    else if (ui->comboBox_random_selection_of->currentIndex() == 4)
        tmp_rand_vibr.SelectionOfBandwidth = 2000;

    else if (ui->comboBox_random_selection_of->currentIndex() == 5)
        tmp_rand_vibr.SelectionOfBandwidth = 5000;

    /*Curve save*/

    if (ui->checkBox_cyclic_every->isChecked())
        tmp_rand_vibr.CurveSave = 1.401298e-45;

    else if (ui->checkBox_cyclic_every->isChecked() && ui->checkBox_with_abort_level->isChecked() &&
             ui->checkBox_with_abort_level_cyclic_savings->isChecked())
        tmp_rand_vibr.CurveSave = 2.662467e-44;

    else if (ui->checkBox_cyclic_every->isChecked() && ui->checkBox_with_abort_level->isChecked())
        tmp_rand_vibr.CurveSave = 4.203895e-45;

    else if (ui->checkBox_cyclic_every->isChecked() && ui->checkBox_at_abort->isChecked() &&
             ui->checkBox_at_abort_cyclic_savings->isChecked())
        tmp_rand_vibr.CurveSave = 1.022948e-43;

    else if (ui->checkBox_cyclic_every->isChecked() && ui->checkBox_at_abort->isChecked())
        tmp_rand_vibr.CurveSave = 1.261169e-44;

    else if (ui->checkBox_cyclic_every->isChecked() && ui->checkBox_with_RMS_limit->isChecked() &&
             ui->checkBox_with_RMS_cyclic_savings->isChecked())
        tmp_rand_vibr.CurveSave = 5.184804e-44;

    else if (ui->checkBox_cyclic_every->isChecked() && ui->checkBox_with_RMS_limit->isChecked())
        tmp_rand_vibr.CurveSave = 7.006492e-45;

    else if (ui->checkBox_cyclic_every->isChecked() && ui->checkBox_with_abort_level->isChecked() &&
             ui->checkBox_at_abort->isChecked() && ui->checkBox_with_abort_level_cyclic_savings->isChecked() &&
             ui->checkBox_at_abort_cyclic_savings->isChecked())
        tmp_rand_vibr.CurveSave = 1.275182e-43;

    else if (ui->checkBox_cyclic_every->isChecked() && ui->checkBox_with_abort_level->isChecked() &&
             ui->checkBox_at_abort->isChecked())
        tmp_rand_vibr.CurveSave = 1.541428e-44;

    else if (ui->checkBox_cyclic_every->isChecked() && ui->checkBox_with_abort_level->isChecked() &&
             ui->checkBox_with_RMS_limit->isChecked() && ui->checkBox_with_abort_level_cyclic_savings->isChecked() &&
             ui->checkBox_with_RMS_cyclic_savings->isChecked())
        tmp_rand_vibr.CurveSave = 7.707142e-44;

    else if (ui->checkBox_cyclic_every->isChecked() && ui->checkBox_with_abort_level->isChecked() &&
             ui->checkBox_with_RMS_limit->isChecked())
        tmp_rand_vibr.CurveSave = 9.809089e-45;

    else if (ui->checkBox_cyclic_every->isChecked() && ui->checkBox_at_abort->isChecked() &&
             ui->checkBox_with_RMS_limit->isChecked() && ui->checkBox_at_abort_cyclic_savings->isChecked() &&
             ui->checkBox_with_RMS_cyclic_savings->isChecked())
        tmp_rand_vibr.CurveSave = 1.527415e-43;

    else if (ui->checkBox_cyclic_every->isChecked() && ui->checkBox_at_abort->isChecked() &&
             ui->checkBox_with_RMS_limit->isChecked())
        tmp_rand_vibr.CurveSave = 1.821688e-44;

    else if (ui->checkBox_cyclic_every->isChecked() && ui->checkBox_with_abort_level->isChecked() &&
             ui->checkBox_at_abort->isChecked() && ui->checkBox_with_RMS_limit->isChecked() &&
             ui->checkBox_with_abort_level_cyclic_savings->isChecked() &&
             ui->checkBox_at_abort_cyclic_savings->isChecked() && ui->checkBox_with_RMS_cyclic_savings->isChecked())
        tmp_rand_vibr.CurveSave = 1.779649e-43;

    else if (ui->checkBox_cyclic_every->isChecked() && ui->checkBox_with_abort_level->isChecked() &&
             ui->checkBox_at_abort->isChecked() && ui->checkBox_with_RMS_limit->isChecked())
        tmp_rand_vibr.CurveSave = 2.101948e-44;

    /*Curve save time*/
    int hours = ui->lineEdit_random_h->text().toInt();
    int minutes = ui->lineEdit_random_min->text().toInt();
    int seconds = ui->lineEdit_random_sec->text().toInt();

    tmp_rand_vibr.CurveSaveTime = seconds + minutes*60 + hours*3600;

    return tmp_rand_vibr;
}


/*********zozbieranie levels nastaveni z rozhrania*******/
vector<CLevels> MainWindow::CollectLevels()
{
    CLevels tmp_class_level;
    vector<CLevels> tmp_vec_level;

    if(model_level->rowCount() != 0)
    {
        for(int i = 0; i < model_level->rowCount(); i++)
        {
            tmp_class_level.TestYear = ui->lineEdit_export_test_year->text().toInt();
            tmp_class_level.TestNumber = ui->lineEdit_export_test_number->text().toInt();

            QModelIndex index = model_level->index(i, 0);
            tmp_class_level.Level = index.data().toInt();

            index = model_level->index(i, 1);
            QString test_time_str = index.data().toString();
            QRegExp rx("[:]");
            QStringList list_test_time_str = test_time_str.split(rx);

            int days = list_test_time_str.at(0).toInt();
            int hours = list_test_time_str.at(1).toInt();
            int minutes = list_test_time_str.at(2).toInt();
            int seconds = list_test_time_str.at(3).toInt();

            tmp_class_level.Time = seconds + minutes*60 + hours*3600 + days*86400;

            tmp_vec_level.push_back(tmp_class_level);
        }

        return tmp_vec_level;
    }

    else
        return tmp_vec_level;
}


/*********zozbieranie sine profilov z rozhrania*******/
vector<CSineProfiles> MainWindow::CollectSineProfiles()
{
    CSineProfiles tmp_class_sine_profiles;
    vector<CSineProfiles> tmp_vec_sine_profiles;

    if(model->rowCount() != 0)
    {
        for(int i = 0; i < model->rowCount(); i++)
        {
            QModelIndex index = model->index(i, 0);
            tmp_class_sine_profiles.ProfileID = index.data().toInt();

            index = model->index(i, 1);
            tmp_class_sine_profiles.Frequency = index.data().toDouble();

            index = model->index(i, 2);
            tmp_class_sine_profiles.Acceleration = index.data().toDouble();

            index = model->index(i, 3);
            tmp_class_sine_profiles.WarnLimitPositive = index.data().toDouble();

            index = model->index(i, 4);
            tmp_class_sine_profiles.WarnLimitNegative = index.data().toDouble();

            index = model->index(i, 5);
            tmp_class_sine_profiles.AbortLimitPositive = index.data().toDouble();

            index = model->index(i, 6);
            tmp_class_sine_profiles.AbortLimitNegative = index.data().toDouble();

            tmp_vec_sine_profiles.push_back(tmp_class_sine_profiles);
        }

        return tmp_vec_sine_profiles;
    }

    else
        return tmp_vec_sine_profiles;
}


/*********zozbieranie control param sine z rozhrania*******/
CSineVibrations MainWindow::CollectSineVibrations()
{
    CSineVibrations tmp_sine_vibr;

    tmp_sine_vibr.TestFileYear = ui->lineEdit_export_test_year->text().toInt();
    tmp_sine_vibr.TestFileNumber = ui->lineEdit_export_test_number->text().toInt();

    /*Amplitude type*/
    if (ui->radioButton_sine_amplitude_band_limited->isChecked())
        tmp_sine_vibr.AmplitudeType = 0;

    else if (ui->radioButton_sine_amplitude_rms_average->isChecked())
        tmp_sine_vibr.AmplitudeType = 1;

    else if (ui->radioButton_sine_amplitude_peak_value->isChecked())
        tmp_sine_vibr.AmplitudeType = 2;

    /*Sweep type*/
    if (ui->radioButton_sine_sweep_log->isChecked())
        tmp_sine_vibr.SweepType = 0;

    else if (ui->radioButton_sine_sweep_linear->isChecked())
        tmp_sine_vibr.SweepType = 1;

    /*Multi Channel*/
    if (ui->radioButton_sine_average->isChecked())
        tmp_sine_vibr.MultiChannelControl = 0;

    else if (ui->radioButton_sine_extremal->isChecked())
        tmp_sine_vibr.MultiChannelControl = 1;

    /*Start Frequency*/
    if (ui->radioButton_sine_freq_lower->isChecked())
        tmp_sine_vibr.StartFrequency = 0;

    else if (ui->radioButton_sine_freq_upper->isChecked())
        tmp_sine_vibr.StartFrequency = 1;

    /*abort, warn limit violations*/
    tmp_sine_vibr.NumberOfWarnLimitViolations = ui->lineEdit_sine_warn_limit->text().toInt();
    tmp_sine_vibr.NumberOfAbortLimitViolations = ui->lineEdit_sine_abort_limit->text().toInt();

    /*Pretest level*/
    tmp_sine_vibr.PreTestLevel = ui->lineEdit_sine_pretest_level->text().toInt();

    /*connection with slip table*/
    tmp_sine_vibr.ConnectionWithSlipTable = ui->checkBox_sine_slip_table->text().toInt();

    /*mass test item/fixture*/
    tmp_sine_vibr.MassTestItems = ui->lineEdit_sine_mass_item->text().toDouble();
    tmp_sine_vibr.MassTestFixture = ui->lineEdit_sine_mass_fixture->text().toDouble();

    /*sweep velocity*/
    tmp_sine_vibr.SweepVelocity = ui->lineEdit_sine_sweep_v->text().toDouble();

    /*test time*/
    int days = ui->lineEdit_sine_day->text().toInt();
    int hours = ui->lineEdit_sine_hour->text().toInt();
    int minutes = ui->lineEdit_sine_min->text().toInt();
    int seconds = ui->lineEdit_sine_sec->text().toInt();

    tmp_sine_vibr.TestTime = seconds + minutes*60 + hours*3600 + days*86400;

    /*Test duration boxes*/
    if(ui->checkBox_sine_sweep_v->isChecked() && ui->checkBox_sine_numb_of_cycles->isChecked() &&
            ui->checkBox_sine_time_per->isChecked() && ui->checkBox_sine_test_time->isChecked())
        tmp_sine_vibr.TestDurationBoxes = 0;

    else if(ui->checkBox_sine_sweep_v->isChecked() && ui->checkBox_sine_numb_of_cycles->isChecked() &&
            ui->checkBox_sine_time_per->isChecked() && ui->checkBox_sine_test_time->isChecked())
        tmp_sine_vibr.TestDurationBoxes = 101;

    else if(ui->checkBox_sine_sweep_v->isChecked() && ui->checkBox_sine_numb_of_cycles->isChecked() &&
            ui->checkBox_sine_time_per->isChecked() && ui->checkBox_sine_test_time->isChecked())
        tmp_sine_vibr.TestDurationBoxes = 110;

    else if(ui->checkBox_sine_sweep_v->isChecked() && ui->checkBox_sine_numb_of_cycles->isChecked() &&
            ui->checkBox_sine_time_per->isChecked() && ui->checkBox_sine_test_time->isChecked())
        tmp_sine_vibr.TestDurationBoxes = 1010;

    else if(ui->checkBox_sine_sweep_v->isChecked() && ui->checkBox_sine_numb_of_cycles->isChecked() &&
            ui->checkBox_sine_time_per->isChecked() && ui->checkBox_sine_test_time->isChecked())
        tmp_sine_vibr.TestDurationBoxes = 1001;

    /*Curve save*/

    if (ui->checkBox_sine_cyclic_savings_every->isChecked() && !ui->checkBox_sine_with_abort_level->isChecked() &&
            !ui->checkBox_sine_at_abort->isChecked() && !ui->checkBox_sine_with_acceleration_limit->isChecked())
        tmp_sine_vibr.CurveSave = 1010.00006103516;

    else if (ui->checkBox_sine_cyclic_savings_every->isChecked() && ui->checkBox_sine_with_abort_level->isChecked() &&
             !ui->checkBox_sine_at_abort->isChecked() && !ui->checkBox_sine_with_acceleration_limit->isChecked())
        tmp_sine_vibr.CurveSave = 1010.00018310547;

    else if (ui->checkBox_sine_cyclic_savings_every->isChecked() && ui->checkBox_sine_with_acceleration_limit->isChecked() &&
             !ui->checkBox_sine_with_abort_level->isChecked() && !ui->checkBox_sine_at_abort->isChecked())
        tmp_sine_vibr.CurveSave = 1010.00030517578;

    else if (ui->checkBox_sine_cyclic_savings_every->isChecked() && ui->checkBox_sine_with_abort_level->isChecked() &&
             ui->checkBox_sine_with_acceleration_limit->isChecked() && !ui->checkBox_sine_at_abort->isChecked())
        tmp_sine_vibr.CurveSave = 1010.00042724609;

    else if (ui->checkBox_sine_cyclic_savings_every->isChecked() && ui->checkBox_sine_at_abort->isChecked() &&
             !ui->checkBox_sine_with_abort_level->isChecked() && !ui->checkBox_sine_with_acceleration_limit->isChecked())
        tmp_sine_vibr.CurveSave = 1010.00054931641;

    else if (ui->checkBox_sine_cyclic_savings_every->isChecked() && ui->checkBox_sine_with_abort_level->isChecked() &&
             ui->checkBox_sine_at_abort->isChecked() && !ui->checkBox_sine_with_acceleration_limit->isChecked())
        tmp_sine_vibr.CurveSave = 1010.00067138672;

    else if (ui->checkBox_sine_cyclic_savings_every->isChecked() && ui->checkBox_sine_with_acceleration_limit->isChecked() &&
             ui->checkBox_sine_at_abort->isChecked() && !ui->checkBox_sine_with_abort_level->isChecked())
        tmp_sine_vibr.CurveSave = 1010.00079345703;

    else if (ui->checkBox_sine_cyclic_savings_every->isChecked() && ui->checkBox_sine_with_abort_level->isChecked() &&
             ui->checkBox_sine_at_abort->isChecked() && ui->checkBox_sine_with_acceleration_limit->isChecked())
        tmp_sine_vibr.CurveSave = 1010.00091552734;

    /*Curve save cycle*/
    tmp_sine_vibr.CurveSaveCycle = ui->lineEdit_sine_cycle->text().toInt();

    return tmp_sine_vibr;
}


/********zozbieranie shock profilov z rohrania********/
CShockProfiles MainWindow::CollectShockProfiles()
{
    CShockProfiles tmp_shock_profiles;

    if(model->rowCount() != 0)
    {
        QModelIndex index = model->index(0, 0);
        tmp_shock_profiles.ProfileID = index.data().toInt();

        index = model->index(0, 1);
        tmp_shock_profiles.CurveType = index.data().toInt();

        index = model->index(0, 2);
        tmp_shock_profiles.Amplitude = index.data().toDouble();

        index = model->index(0, 3);
        tmp_shock_profiles.Width = index.data().toDouble();

        return tmp_shock_profiles;
    }

    else
        return tmp_shock_profiles;
}


/***************zozbieranie shock control param z rozhrania************/
CShocks MainWindow::CollectShocks()
{
    CShocks tmp_shocks;

    tmp_shocks.TestFileYear = ui->lineEdit_export_test_year->text().toInt();
    tmp_shocks.TestFileNumber = ui->lineEdit_export_test_number->text().toInt();

    /*Shock number*/
    tmp_shocks.ShockNumberPerDirection = ui->lineEdit_shock_number->text().toInt();

    /*Shock Distance*/
    tmp_shocks.ShockDistance = ui->lineEdit_shock_distance->text().toDouble();

    /*warn/abort limit violations*/
    tmp_shocks.NumberOfWarnLimitViolations = ui->lineEdit_shock_warn_limit_violations->text().toInt();
    tmp_shocks.NumberOfAbortLimitViolations = ui->lineEdit_shock_abort_limit->text().toInt();

    /*mass test fixture/item*/
    tmp_shocks.MassTestFixture = ui->lineEdit_shock_mass_test_fixture->text().toDouble();
    tmp_shocks.MassTestItems = ui->lineEdit_shock_mass_test_item->text().toDouble();

    /*slip table*/
    tmp_shocks.ConnectionWithSlipTable = ui->checkBox_shock_slip_table->isChecked();

    /*pretest level*/
    tmp_shocks.PreTestLevel = ui->lineEdit_shock_pre_test_level->text().toInt();

    /*pre-shock level*/
    if(ui->radioButton_shock_0_75->isChecked())
        tmp_shocks.PreShockLevel = 0;

    else if(ui->radioButton_shock_1_5->isChecked())
        tmp_shocks.PreShockLevel = 1;

    else if(ui->radioButton_shock_3->isChecked())
        tmp_shocks.PreShockLevel = 2;

    else if(ui->radioButton_shock_6->isChecked())
        tmp_shocks.PreShockLevel = 3;

    /*Curve Save*/
    if (ui->checkBox_shock_cyclic_savings_every->isChecked() && !ui->checkBox_shock_with_abort_level->isChecked() &&
            !ui->checkBox_shock_at_abort->isChecked() && !ui->checkBox_shock_with_acceleration_limit->isChecked())
        tmp_shocks.CurveSave = -431599648;

    else if (ui->checkBox_shock_cyclic_savings_every->isChecked() && ui->checkBox_shock_with_abort_level->isChecked() &&
             ui->checkBox_shock_at_abort->isChecked() && ui->checkBox_shock_with_acceleration_limit->isChecked())
        tmp_shocks.CurveSave = -431600096;

    else if (ui->checkBox_shock_cyclic_savings_every->isChecked() && ui->checkBox_shock_with_abort_level->isChecked() &&
             !ui->checkBox_shock_at_abort->isChecked() && !ui->checkBox_shock_with_acceleration_limit->isChecked())
        tmp_shocks.CurveSave = -431599712;

    else if (ui->checkBox_shock_cyclic_savings_every->isChecked() && ui->checkBox_shock_with_acceleration_limit->isChecked() &&
             !ui->checkBox_shock_with_abort_level->isChecked() && !ui->checkBox_shock_at_abort->isChecked())
        tmp_shocks.CurveSave = -431599776;

    else if (ui->checkBox_shock_cyclic_savings_every->isChecked() && ui->checkBox_shock_at_abort->isChecked() &&
             !ui->checkBox_shock_with_acceleration_limit->isChecked() && !ui->checkBox_shock_with_abort_level->isChecked())
        tmp_shocks.CurveSave = -431599904;

    else if (ui->checkBox_shock_cyclic_savings_every->isChecked() && ui->checkBox_shock_at_abort->isChecked() &&
             ui->checkBox_shock_with_abort_level->isChecked() && !ui->checkBox_shock_with_acceleration_limit->isChecked())
        tmp_shocks.CurveSave = -431599968;

    else if (ui->checkBox_shock_cyclic_savings_every->isChecked() && ui->checkBox_shock_with_acceleration_limit->isChecked() &&
             ui->checkBox_shock_at_abort->isChecked() && !ui->checkBox_shock_with_abort_level->isChecked())
        tmp_shocks.CurveSave = -431600032;

    else if (ui->checkBox_shock_cyclic_savings_every->isChecked() && ui->checkBox_shock_with_abort_level->isChecked() &&
             ui->checkBox_shock_with_acceleration_limit->isChecked() && !ui->checkBox_shock_at_abort->isChecked())
        tmp_shocks.CurveSave = -431599840;

    /*curve save shocks*/
    tmp_shocks.CurveSaveShocks = ui->lineEdit_shock_curve_save_shocks->text().toInt();

    /*shock inverted*/
    tmp_shocks.ShockInverted = ui->checkBox_shock_inverted->isChecked();

    /*positive/negative pre/post shock amplitude*/
    tmp_shocks.PosPreShock = ui->lineEdit_shock_positive_pre_shock->text().toInt();
    tmp_shocks.PosPostShock = ui->lineEdit_shock_positive_post_shock->text().toInt();
    tmp_shocks.NegPreShock = ui->lineEdit_shock_negative_pre_shock->text().toInt();
    tmp_shocks.NegPostShock = ui->lineEdit_shock_negative_post_shock->text().toInt();

    /*standard*/
    tmp_shocks.Standard = ui->comboBox_shock_standard->currentIndex();

    /*warn Limit*/
    tmp_shocks.WarnLimit = ui->lineEdit_shock_warn_limit->text().toInt();

    return tmp_shocks;
}


/**********zozbieranie tolerance band z rozhrania **********/
vector<CToleranceBand> MainWindow::CollectToleranceBand()
{
    CToleranceBand tmp_class_tolerance_band;
    vector<CToleranceBand> tmp_vec_tolerance_band;

    if(model_tolerance->rowCount() != 0)
    {
        for(int i = 0; i < model_tolerance->rowCount(); i++)
        {
            tmp_class_tolerance_band.adTestFileYear = test_file_year_new;
            tmp_class_tolerance_band.adTestFileNumber = test_file_number_new;

            QModelIndex index = model_tolerance->index(i, 0);
            tmp_class_tolerance_band.Time = index.data().toDouble();

            index = model_tolerance->index(i, 1);
            tmp_class_tolerance_band.AbortLimitPos = index.data().toDouble();

            index = model_tolerance->index(i, 2);
            tmp_class_tolerance_band.AbortLimitNeg = index.data().toDouble();

            tmp_vec_tolerance_band.push_back(tmp_class_tolerance_band);
        }

        return tmp_vec_tolerance_band;
    }

    return tmp_vec_tolerance_band;
}


/********zozbieranie snimacov z rozhrania *********/
CVibrationsCommon MainWindow::CollectVibrationsCommon()
{
    CVibrationsCommon tmp_snimace;

    tmp_snimace.TestFileYear = ui->lineEdit_export_test_year->text().toInt();
    tmp_snimace.TestFileNumber = ui->lineEdit_export_test_number->text().toInt();

    for (int i = 0; i < 8; i++)
    {
        tmp_snimace.TypeOfInput[i] = list_type_of_input.at(i)->currentIndex();
        tmp_snimace.Sensitivity[i] = list_sensitivity.at(i)->text().toDouble();
        tmp_snimace.Unit[i] = list_unit.at(i)->currentIndex();
        tmp_snimace.LowerLimit[i] = list_limits_lower.at(i)->text().toDouble();
        tmp_snimace.UpperLimit[i] = list_limits_upper.at(i)->text().toDouble();
        tmp_snimace.InfoText[i] = list_info_text.at(i)->text().toStdString();
    }

    return tmp_snimace;
}


/*********** prechod na import kartu z export karty **********/
void MainWindow::on_pushButton_export_load_info_from_test_parameters_clicked()
{
    window_import->show();
    SendFiltersToImport();
}


/******** import informacii o teste na kartu export ********/
void MainWindow::receive_test_file_name_number_year(int test_year, int test_number, QString test_file_name, int profile_type)
{
    ui->lineEdit_export_test_number->setText(QString(to_string(test_number).c_str()));
    ui->lineEdit_export_test_year->setText(QString(to_string(test_year).c_str()));
    ui->comboBox->setCurrentIndex(profile_type);

    QSqlDatabase::database("tests_parameters").open();

    QSqlQuery query_info(QSqlDatabase::database("tests_parameters"));
    query_info.prepare("SELECT relCustomerInquiryYear, relCustomerInquiryNumber, relCustomerInquiryIndex, testAxis, directoryTestDate,"
                       " relTestItemYear, relTestItemNumber, relTestArrangementOrder FROM main_tests_table WHERE "
                       "testFileYear = ? AND testFileNumber = ?");

    query_info.bindValue(0, test_year);
    query_info.bindValue(1, test_number);

    query_info.exec();

    while(query_info.next())
    {
        ui->lineEdit_export_inquiry_year->setText(query_info.value(0).toString());
        ui->lineEdit_export_inquiry_number->setText(query_info.value(1).toString());
        ui->lineEdit_export_inquiry_index->setText(query_info.value(2).toString());
        ui->lineEdit_export_test_axis->setText(query_info.value(3).toString());
        ui->lineEdit_export_date->setText(query_info.value(4).toString());
        ui->lineEdit_export_test_item_year->setText(query_info.value(5).toString());
        ui->lineEdit_export_test_item_number->setText(query_info.value(6).toString());
        ui->lineEdit_export_test_arrangement_order->setText(query_info.value(7).toString());
    }

    QSqlDatabase::database("tests_parameters").close();

    int inquiry_year = ui->lineEdit_export_inquiry_year->text().toInt();
    int inquiry_number = ui->lineEdit_export_inquiry_number->text().toInt();
    int inquiry_index = ui->lineEdit_export_inquiry_index->text().toInt();

    QVariantList tmp = FetchContractAndCompanyID(inquiry_year, inquiry_number, inquiry_index);

    ui->lineEdit_export_company_order->setText(tmp[1].toString());

    QString company_name = FetchCompanyName(tmp[0]);

    ui->lineEdit_export_company_name->setText(company_name);

    int pos = test_file_name.indexOf('.');
    qDebug() << test_file_name << " " << pos << endl;
    QString postfix = test_file_name.mid(pos);
    test_file_name = test_file_name.mid(0, pos);
    qDebug() << test_file_name << postfix << endl;

    ui->lineEdit_export_file_name->setText(test_file_name);
    ui->lineEdit_export_postfix->setText(postfix);

    //ulozenie suborovej cesty na pripadne prepisanie
    previous_file_path = FilePath();
}


/*******ziskanie start/stop casu *********/
void MainWindow::on_pushButton_export_logbook_times_clicked()
{
    QString file_path = FilePath();

    string file_path_mdb = RenameToMDB(file_path.toStdString());

    vector<string> tmp_times = FetchLogbookTimes(file_path_mdb);

    if(ui->comboBox->currentText() == "Sine")
        RenameToSin(file_path_mdb);
    else if(ui->comboBox->currentText() == "Random")
        RenameToRau(file_path_mdb);
    else if(ui->comboBox->currentText() == "Shocks")
        RenameToShk(file_path_mdb);

    QString start_time = QString(tmp_times.at(0).c_str());
    QString stop_time = QString(tmp_times.at(1).c_str());

    QSqlDatabase::database("tests_parameters").open();

    QSqlQuery query_logbook_times(QSqlDatabase::database("tests_parameters"));
    query_logbook_times.prepare("UPDATE test_parts SET testStartTimestamp = ?, testEndTimestamp = ? WHERE adTestFileYear "
                                "= ? AND adTestFileNumber = ?");

    query_logbook_times.bindValue(0, start_time);

    if(stop_time != "")
        query_logbook_times.bindValue(1, stop_time);

    query_logbook_times.bindValue(2, ui->lineEdit_export_test_year->text());
    query_logbook_times.bindValue(3, ui->lineEdit_export_test_number->text());

    query_logbook_times.exec();

    QSqlDatabase::database("tests_parameters").close();
}


/******* ulozia sa skutocne parametre testu do DB tests_parameters (control parameters a snimace), stavajuce neaktualne
 * nastavenia sa v DB prepisu ***********/
void MainWindow::on_pushButton_export_save_actual_parameters_clicked()
{
    QString file_path = FilePath();

    string file_path_mdb = RenameToMDB(file_path.toStdString());

    sqlite3* DB;
    sqlite3_open(QSqlDatabase::database("tests_parameters").databaseName().toStdString().c_str(), &DB);

    int test_year = ui->lineEdit_export_test_year->text().toInt();
    int test_number = ui->lineEdit_export_test_number->text().toInt();

    if(ui->comboBox->currentText() == "Sine")
    {
        CSineVibrations tmp_old;
        tmp_old.FetchFromSineVibrations(DB, test_year, test_number);

        CSineVibrations tmp_new;
        tmp_new.FetchFrom_TestFileSine_ToSineVibrations(file_path_mdb);

        QString change = tmp_new.UpdateSineVibrations(DB, tmp_old).c_str();

        CVibrationsCommon tmp_snimace_old;
        tmp_snimace_old.FetchFromVibrationsCommon(DB, test_number, test_year);

        CVibrationsCommon tmp_snimace_new;
        tmp_snimace_new.FetchFrom_TestFileSine_ToVibrationsCommon(file_path_mdb);

        QString change_snimace = QString(tmp_snimace_new.UpdateVibrationsCommon(DB, tmp_snimace_old, 1).c_str());

        ui->lineEdit_export_change_alert->setText(change + change_snimace);

        RenameToSin(file_path_mdb);
    }

    else if(ui->comboBox->currentText() == "Random")
    {
        CRandomVibrations tmp_old;
        tmp_old.FetchFromRandomVibrations(DB, test_year, test_number);

        CRandomVibrations tmp_new;
        tmp_new.FetchFrom_TestFileRandom_ToRandomVibrations(file_path_mdb);

        QString change = tmp_new.UpdateRandomVibrations(DB, tmp_old).c_str();

        CVibrationsCommon tmp_snimace_old;
        tmp_snimace_old.FetchFromVibrationsCommon(DB, test_number, test_year);

        CVibrationsCommon tmp_snimace_new;
        tmp_snimace_new.FetchFrom_TestFileRandom_ToVibrationsCommon(file_path_mdb);

        QString change_snimace = tmp_snimace_new.UpdateVibrationsCommon(DB, tmp_snimace_old, 0).c_str();

        ui->lineEdit_export_change_alert->setText(change + change_snimace);

        RenameToRau(file_path_mdb);
    }

    else if(ui->comboBox->currentText() == "Shocks")
    {
        CShocks tmp_old;
        tmp_old.FetchFromShocks(DB, test_year, test_number);

        CShocks tmp_new;
        tmp_new.FetchFrom_TestFileShock_ToShocks(file_path_mdb);

        QString change = tmp_new.UpdateShocks(DB, tmp_old).c_str();

        CVibrationsCommon tmp_snimace_old;
        tmp_snimace_old.FetchFromVibrationsCommon(DB, test_number, test_year);

        CVibrationsCommon tmp_snimace_new;
        tmp_snimace_new.FetchFrom_TestFileShock_ToVibrationsCommon(file_path_mdb);

        QString change_snimace = tmp_snimace_new.UpdateVibrationsCommon(DB, tmp_snimace_old, 0).c_str();

        ui->lineEdit_export_change_alert->setText(change + change_snimace);

        RenameToShk(file_path_mdb);
    }

    sqlite3_close(DB);
}


//******disable kariet********//
void MainWindow::on_comboBox_currentTextChanged()
{
    if(ui->comboBox->currentText() == "Sine")
    {
        ui->tabWidget->setTabEnabled(2, 1);

        ui->tabWidget->setTabEnabled(1, 0);
        ui->tabWidget->setTabEnabled(3, 0);
        ui->tabWidget->setTabEnabled(5, 0);
        ui->tabWidget->setTabEnabled(6, 0);

        ui->lineEdit_export_postfix->setText(".SIN");
    }

    else if(ui->comboBox->currentText() == "Random")
    {
        ui->tabWidget->setTabEnabled(1, 1);
        ui->tabWidget->setTabEnabled(5, 1);

        ui->tabWidget->setTabEnabled(2, 0);
        ui->tabWidget->setTabEnabled(3, 0);
        ui->tabWidget->setTabEnabled(6, 0);

        ui->lineEdit_export_postfix->setText(".RAU");
    }

    else if(ui->comboBox->currentText() == "Shocks")
    {
        ui->tabWidget->setTabEnabled(3, 1);
        ui->tabWidget->setTabEnabled(6, 1);

        ui->tabWidget->setTabEnabled(1, 0);
        ui->tabWidget->setTabEnabled(2, 0);
        ui->tabWidget->setTabEnabled(5, 0);

        ui->lineEdit_export_postfix->setText(".SHK");
    }
}


/******prechod z profilov na kartu import******/
void MainWindow::on_pushButton_profily_import_karta_clicked()
{
    window_import->show();
    SendFiltersToImport();
}


/*******aktualny datum**********/
void MainWindow::on_pushButton_export_date_clicked()
{
    const int MAXLEN = 80;
    char s[MAXLEN];
    time_t now = time(0);
    strftime(s, MAXLEN, "%Y/%m/%d", localtime(&now));
    ui->lineEdit_export_date->setText(s);
}


/*********automaticke nacitanie ponuky, zakazky, mena firmy do export karty**********/
void MainWindow::on_tabWidget_tabBarClicked(int index)
{
    if(index == 7)
    {
        int test_year = ui->lineEdit_export_test_year->text().toInt();
        int test_number = ui->lineEdit_export_test_number->text().toInt();

        QVariantList tmp_inquiry;
        tmp_inquiry = FetchInquiry(test_year, test_number);

        if(!tmp_inquiry.isEmpty())
        {
            ui->lineEdit_export_inquiry_year->setText(tmp_inquiry[0].toString());
            ui->lineEdit_export_inquiry_number->setText(tmp_inquiry[1].toString());
            ui->lineEdit_export_inquiry_index->setText(tmp_inquiry[2].toString());

            QVariantList tmp_contract = FetchContractAndCompanyID(tmp_inquiry[0].toInt(), tmp_inquiry[1].toInt(), tmp_inquiry[2].toInt());
            ui->lineEdit_export_company_order->setText(tmp_contract[1].toString());

            QString company_name = FetchCompanyName(tmp_contract[0]);
            ui->lineEdit_export_company_name->setText(company_name);
        }

        //ak sa jedna o novy subor, vtedy sa pocita s filtrovanim
        else if(tmp_inquiry.isEmpty())
        {
            /**ziskanie poptavky z filtru (ak sa filtrovalo podla poptavky)******/
            if (ui->checkBox_inqr->isChecked() && !ui->lineEdit_filter_inqr_id->text().isEmpty() &&
                    !ui->lineEdit_filter_inqr_year->text().isEmpty() && !ui->lineEdit_filter_inqr_index->text().isEmpty())
            {
                inqr_year = ui->lineEdit_filter_inqr_year->text();
                inqr_number =  ui->lineEdit_filter_inqr_id->text();
                inqr_index =  ui->lineEdit_filter_inqr_index->text();

                ui->lineEdit_export_inquiry_year->setText(inqr_year);
                ui->lineEdit_export_inquiry_number->setText(inqr_number);
                ui->lineEdit_export_inquiry_index->setText(inqr_index);

                QVariantList tmp_contract = FetchContractAndCompanyID(inqr_year.toInt(), inqr_number.toInt(), inqr_index.toInt());
                ui->lineEdit_export_company_order->setText(tmp_contract[1].toString());

                QString company_name = FetchCompanyName(tmp_contract[0]);
                ui->lineEdit_export_company_name->setText(company_name);
            }

            if(!ui->checkBox_inqr->isChecked())
            {
                /********ak sa filtrovalo podla zakazky tak sa vybera contractOrderForCompanyInYear podla contract year a number vo filtri *********/
                if(ui->checkBox_contract->isChecked() && !ui->lineEdit_filter_contract_number->text().isEmpty() && !ui->lineEdit_filter_contract_year->text().isEmpty())
                {
                    QVariant contract_year = ui->lineEdit_filter_contract_year->text();
                    QVariant contract_number = ui->lineEdit_filter_contract_number->text();
                    QVariantList tmp = FetchUsingContract(contract_year, contract_number);
                    QString company_name = FetchCompanyName(tmp[0]);

                    ui->lineEdit_export_inquiry_year->setText(tmp[2].toString());
                    ui->lineEdit_export_inquiry_number->setText(tmp[3].toString());
                    ui->lineEdit_export_inquiry_index->setText(tmp[4].toString());
                    ui->lineEdit_export_company_order->setText(tmp[1].toString());
                    ui->lineEdit_export_company_name->setText(company_name);
                }
            }
        }

        LabelLastTestRefresh();
        DuplicateEnableDisable();
        BlockButtonsWhenNoContract();
    }
}


/****blokovanie tlacitok ak nie je zakazka*******/
void MainWindow::BlockButtonsWhenNoContract()
{
    if (ui->lineEdit_export_company_order->text().isEmpty())
    {
        ui->pushButton_export_insert_info_to_test_parameters->setDisabled(1);
        ui->pushButton_export_duplicate->setDisabled(1);
        ui->pushButton_export_create_test_file->setEnabled(1);
    }

    else if (!ui->lineEdit_export_company_order->text().isEmpty())
    {
        ui->pushButton_export_insert_info_to_test_parameters->setEnabled(1);
        ui->pushButton_export_duplicate->setEnabled(1);
    }
}


/****** refresh labelu s novym cislom a rokom testu*****/
void MainWindow::LabelLastTestRefresh()
{
    QSqlDatabase::database("tests_parameters").open();

    QSqlQuery query_number_year(QSqlDatabase::database("tests_parameters"));
    query_number_year.prepare("SELECT testFileYear, testFileNumber FROM main_tests_table WHERE ROWID IN ( SELECT max( ROWID ) FROM main_tests_table )");
    query_number_year.exec();
    query_number_year.first();

    ui->label_actual_test_number_year->setText("Poslední test " + query_number_year.value(0).toString() + "_" + PadZeros(query_number_year.value(1), 4));

    QSqlDatabase::database("tests_parameters").close();
}


/*****blokovanie duplicate tlacitka*/
void MainWindow::DuplicateEnableDisable()
{
    bool test_exists = isTestExisting();
    if(!test_exists || ui->lineEdit_export_test_year->text().isEmpty() || ui->lineEdit_export_test_number->text().isEmpty())
        ui->pushButton_export_duplicate->setDisabled(1);

    else if(test_exists)
        ui->pushButton_export_duplicate->setEnabled(1);
}


/********** dynamicke zmeny nazvu tlacitka ulozit novy zaznam/ aktualizovat, pri zmene cisla**********/
void MainWindow::on_lineEdit_export_test_year_textChanged()
{
    PushButtonTextChangeInsertUpdate();
    DuplicateEnableDisable();
    BlockButtonsWhenNoContract();
    BlockButtonsTimesAndLoad();
}


/********** dynamicke zmeny nazvu tlacitka ulozit ako novy zaznam/ aktualizovat, pri zmene roku**********/
void MainWindow::on_lineEdit_export_test_number_textChanged()
{
    PushButtonTextChangeInsertUpdate();
    DuplicateEnableDisable();
    BlockButtonsWhenNoContract();
    BlockButtonsTimesAndLoad();
}


/*******zmena textu v push button******/
void MainWindow::PushButtonTextChangeInsertUpdate()
{
    bool test_exists = isTestExisting();
    if(test_exists)
    {
        ui->pushButton_export_insert_info_to_test_parameters->setText("Aktualizovat soubor v databáze");
        ui->pushButton_export_create_test_file->setEnabled(1);
        ui->pushButton_export_create_test_file->setText("Nahradit soubor");
    }
    else if(!test_exists)
    {
        ui->pushButton_export_create_test_file->setDisabled(1);
        ui->pushButton_export_insert_info_to_test_parameters->setText("Uložit nový záznam " +
            PadZeros(ui->lineEdit_export_test_number->text(), 4) + "_" + ui->lineEdit_export_test_year->text());
        ui->pushButton_export_create_test_file->setText("Vytvořit soubor");
    }
}


/*******duplikovanie(vytvorenie noveho) zaznamu*********/
void MainWindow::on_pushButton_export_duplicate_clicked()
{
    ui->pushButton_export_generate->clicked();
    ui->pushButton_export_date->clicked();
    ui->pushButton_export_insert_info_to_test_parameters->clicked();
}


void MainWindow::on_lineEdit_export_company_order_textChanged()
{
    BlockButtonsWhenNoContract();
}


/****blokovanie tlacitok ulozenia casov a skutocnych paramtrov******/
void MainWindow::BlockButtonsTimesAndLoad()
{
    bool test_exists = isTestExisting();
    if(test_exists)
    {
        ui->pushButton_export_logbook_times->setEnabled(1);
        ui->pushButton_export_save_actual_parameters->setEnabled(1);
    }
    else if(!test_exists)
    {
        ui->pushButton_export_logbook_times->setDisabled(1);
        ui->pushButton_export_save_actual_parameters->setDisabled(1);
    }
}


/*****vyplnenie nul pred cislom ********/
QString MainWindow::PadZeros(QVariant number, int order)
{
    int number_size = number.toString().length();
    QString zeros(order - number_size, '0');
    return zeros + number.toString();
}
