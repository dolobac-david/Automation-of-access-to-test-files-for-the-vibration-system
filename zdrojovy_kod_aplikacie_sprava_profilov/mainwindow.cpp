#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    string config = (QApplication::applicationDirPath() + "/config.txt").toStdString();

    ifstream file;
    file.open(config);
    QStringList list;
    string out;
    while(getline(file, out))
        list.push_back(out.c_str());

    QSqlDatabase::addDatabase("QSQLITE");
    QSqlDatabase::database().setDatabaseName(list[0]);

    QSqlDatabase::addDatabase("QSQLITE", "kniha_zakazek");
    QSqlDatabase::database("kniha_zakazek").setDatabaseName(list[2]);

    model = new QSqlTableModel(this);
    selectionModel = new QItemSelectionModel();
    delegate = new Delegate_random_prof(this);

    window = new CommonSettingsRandom;
    connect(this, SIGNAL(send_new_id(int)), window, SLOT(receive_new_id(int)));

    inquiery_window = new ProfileToInquiery;
    connect(this, SIGNAL(send_new_id(int)), inquiery_window, SLOT(receive_new_id(int)));

    ui->lineEdit_filter_inqr_id->setReadOnly(1);
    ui->lineEdit_filter_inqr_year->setReadOnly(1);
    ui->lineEdit_filter_inqr_index->setReadOnly(1);
    ui->lineEdit_filter_order_id->setReadOnly(1);
    ui->lineEdit_filter_order_year->setReadOnly(1);
    palette = new QPalette();
    palette->setColor(QPalette::Base,Qt::gray);
    palette->setColor(QPalette::Text,Qt::darkGray);
    ui->lineEdit_filter_inqr_id->setPalette(*palette);
    ui->lineEdit_filter_inqr_year->setPalette(*palette);
    ui->lineEdit_filter_inqr_index->setPalette(*palette);
    ui->lineEdit_filter_order_id->setPalette(*palette);
    ui->lineEdit_filter_order_year->setPalette(*palette);

    connect(ui->tableView_random->verticalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(receive_section_row()));
    connect(ui->tableView_random_new->verticalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(receive_section_row_bottom()));

    ui->pushButton_ramp->setDisabled(1);

    ramp_window = new Ramp;
    connect(this, SIGNAL(send_to_ramp(QList<QVariant>)), ramp_window, SLOT(receive_from_main_window(QList<QVariant>)));
    connect(ramp_window, SIGNAL(send_new_data_to_main_window(QList<QVariant>)), this, SLOT(receive_new_data_from_ramp(QList<QVariant>)));
}


MainWindow::~MainWindow()
{
    delete ui;
    delete model;
    delete selectionModel;
    delete delegate;
    delete window;
    delete inquiery_window;
    QSqlDatabase::database(0).close();
    delete ramp_window;
}


/********nastavenie mien stlpcov********/
void MainWindow::SetColumnNames(QAbstractItemModel *model_in, QTableView *table_view)
{
    //typ profilu random
    if (ui->comboBox->currentText() == "Random")
    {
        model_in->setHeaderData(0, Qt::Horizontal, "Profile ID", Qt::DisplayRole);
        model_in->setHeaderData(1, Qt::Horizontal, "Frequency", Qt::DisplayRole);
        model_in->setHeaderData(2, Qt::Horizontal, "Amplitude", Qt::DisplayRole);
        model_in->setHeaderData(3, Qt::Horizontal, "Warn limit positive", Qt::DisplayRole);
        model_in->setHeaderData(4, Qt::Horizontal, "Warn limit negative", Qt::DisplayRole);
        model_in->setHeaderData(5, Qt::Horizontal, "Abort limit positive", Qt::DisplayRole);
        model_in->setHeaderData(6, Qt::Horizontal, "Abort limit negative", Qt::DisplayRole);

        table_view->setColumnWidth(0, 60);
        table_view->setColumnWidth(3, 110);
        table_view->setColumnWidth(4, 120);
        table_view->setColumnWidth(5, 112);
        table_view->setColumnWidth(6, 120);
    }

    else if(ui->comboBox->currentText() == "Sine")
    {
        model_in->setHeaderData(0, Qt::Horizontal, "Profile ID", Qt::DisplayRole);
        model_in->setHeaderData(1, Qt::Horizontal, "Frequency", Qt::DisplayRole);
        model_in->setHeaderData(2, Qt::Horizontal, "Acceleration", Qt::DisplayRole);
        model_in->setHeaderData(3, Qt::Horizontal, "Warn limit positive", Qt::DisplayRole);
        model_in->setHeaderData(4, Qt::Horizontal, "Warn limit negative", Qt::DisplayRole);
        model_in->setHeaderData(5, Qt::Horizontal, "Abort limit positive", Qt::DisplayRole);
        model_in->setHeaderData(6, Qt::Horizontal, "Abort limit negative", Qt::DisplayRole);

        table_view->setColumnWidth(0, 60);
        table_view->setColumnWidth(3, 110);
        table_view->setColumnWidth(4, 120);
        table_view->setColumnWidth(5, 112);
        table_view->setColumnWidth(6, 120);
    }

    else if(ui->comboBox->currentText() == "Shocks")
    {
        model_in->setHeaderData(0, Qt::Horizontal, "Profile ID", Qt::DisplayRole);
        model_in->setHeaderData(1, Qt::Horizontal, "Curve type", Qt::DisplayRole);
        model_in->setHeaderData(2, Qt::Horizontal, "Amplitude", Qt::DisplayRole);
        model_in->setHeaderData(3, Qt::Horizontal, "Width", Qt::DisplayRole);
    }
}


/******nastavenie SQLite tabulky na model, podla typu testu*****/
void MainWindow::SetTable(QSqlTableModel *model_in)
{
    if (ui->comboBox->currentText() == "Random")
        model_in->setTable("profiles_random");

    else if(ui->comboBox->currentText() == "Sine")
        model_in->setTable("profiles_sine");

    else if(ui->comboBox->currentText() == "Shocks")
        model_in->setTable("profiles_shocks");
}


/*Nacitanie DB*/
void MainWindow::on_pushButton_load_random_clicked()
{
    qDebug() << QSqlDatabase::database().open() << endl;

    SetTable(model);
    SetColumnNames(model, ui->tableView_random);

    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->setFilter("");
    model->setSort(0, Qt::AscendingOrder);
    model->select();

    ui->tableView_random->setModel(model);
    ui->tableView_random->setItemDelegate(delegate);

    if (ui->comboBox->currentText() == "Sine" || ui->comboBox->currentText() == "Random")
    {
        if (ui->tableView_random->model() != nullptr)
        {
            int rows = model->rowCount();
            if(rows != 0)
            {
                if (ui->comboBox_limits->currentText() == "Limits in %")
                {
                    QModelIndex index;
                    double value_dB, value_percent;
                    for(int i = 0; i < rows; i++)
                    {
                        for(int j = 3; j < 7; j++)
                        {
                            index = model->index(i, j);
                            value_dB = model->data(index).toDouble();

                            //horne limity
                            if(j == 3 || j == 5)
                            {
                                if (ui->comboBox->currentText() == "Sine")
                                    value_percent = 100 * (pow(10, value_dB/20) - 1);

                                else if(ui->comboBox->currentText() == "Random")
                                    value_percent = 100 * (pow(10, value_dB/10) - 1);
                            }

                            //dolne limity
                            if(j == 4 || j == 6)
                            {
                                if (ui->comboBox->currentText() == "Sine")
                                    value_percent = 100 * (1 - pow(10, -value_dB/20));

                                else if(ui->comboBox->currentText() == "Random")
                                    value_percent = 100 * (1 - pow(10, -value_dB/10));
                            }

                            model->setData(index, value_percent);
                        }
                    }
                }
            }
        }
    }
}


/********* Potvrdenie zmien, prepocet z % na dB ak su limity v zobrazene v % **********/
void MainWindow::on_pushButton_submit_random_clicked()
{
    if (ui->comboBox->currentText() == "Sine" || ui->comboBox->currentText() == "Random")
    {
        int rows = model->rowCount();
        if(rows != 0)
        {
            //do DB ulozi hodnoty v dB
            if (ui->comboBox_limits->currentText() == "Limits in %")
            {
                ui->comboBox_limits->setCurrentText("Limits in dB");
                model->submitAll();
                ui->comboBox_limits->setCurrentText("Limits in %");
            }
            else
                model->submitAll();
        }
    }
    else
        model->submitAll();
}


/*Presun existujuceho profilu na tvorbu noveho*/
void MainWindow::on_pushButton_drag_profile_clicked()
{
    if(model->rowCount() != 0)
    {
        selectionModel = ui->tableView_random->selectionModel();
        QModelIndexList rows = selectionModel->selectedRows();

        if(rows.count() > 0)
        {
            QModelIndexList indexes = selectionModel->selectedIndexes();
            int columns = model->columnCount();
            std::vector<QVariant> tmp;

            for(int i = 0; i < indexes.count(); i++)
            {
                QVariant a = selectionModel->model()->data(indexes.at(i));
                tmp.push_back(a);
            }

            QStandardItemModel* model2 = new QStandardItemModel(rows.count(), columns);
            SetColumnNames(model2, ui->tableView_random_new);

            QModelIndex topLeft = model2->index(0, 0, QModelIndex());
            QModelIndex bottomRight = model2->index(rows.count() - 1, columns - 1, QModelIndex());

            ui->tableView_random_new->setModel(model2);
            QItemSelectionModel* selmodel = new QItemSelectionModel;
            selmodel = ui->tableView_random_new->selectionModel();

            QItemSelection selection(topLeft, bottomRight);
            selmodel->select(selection, QItemSelectionModel::Select);
            QModelIndexList indexes2 = selmodel->selectedIndexes();

            for(int i = 0; i < indexes2.count(); i++)
            {
                model2->setData(indexes2.at(i), tmp.at(i), Qt::EditRole);
            }

            ui->tableView_random_new->setModel(model2);
            ui->tableView_random_new->setItemDelegate(delegate);
        }
    }
}


/*Novy zaznam pri tvorbe noveho profilu*/
void MainWindow::on_pushButton_new_record_clicked()
{
    QStandardItemModel* model = new QStandardItemModel();
    model = static_cast<QStandardItemModel*>(ui->tableView_random_new->model());

    //ak uz je v tabulke nieco pretiahnute, teda je inicializovana
    if(model != nullptr)
    {
        QStandardItem* item = new QStandardItem;
        model->insertRow(model->rowCount(), item);
    }
}


/*vymazanie zaznamu(ov) z tabulky pre tvorbu noveho profilu*/
void MainWindow::on_pushButton_delete_new_record_clicked()
{
    QStandardItemModel* model = new QStandardItemModel;
    model = static_cast<QStandardItemModel*>(ui->tableView_random_new->model());
    if(model != nullptr)
    {
        selectionModel = ui->tableView_random_new->selectionModel();
        QModelIndexList indexes = selectionModel->selectedRows();

        if(!indexes.empty())
            model->removeRows(indexes.first().row(), indexes.count());
    }
}


/*vymazanie zaznamu(ov) z DB profilov*/
void MainWindow::on_pushButton_delete_record_clicked()
{
    if(model->rowCount() != 0)
    {
        selectionModel = ui->tableView_random->selectionModel();
        QModelIndexList indexes = selectionModel->selectedRows();

        if(!indexes.empty())
            model->removeRows(indexes.first().row(), indexes.count());
    }
}


/*nahranie novo vytvoreneho profilu do DB*/
void MainWindow::on_pushButton_submit_new_record_clicked()
{
    QStandardItemModel* model2 = new QStandardItemModel;
    model2 = static_cast<QStandardItemModel*>(ui->tableView_random_new->model());

    if(model2 != nullptr)
    {
        int rows = model2->rowCount();
        int columns = model2->columnCount();

        /*generovanie noveho ID*/
        QSqlTableModel* model_new_id = new QSqlTableModel;

        SetTable(model_new_id);
        model_new_id->setSort(0, Qt::AscendingOrder);
        model_new_id->select();

        QModelIndex last_id_pos = model_new_id->index(model_new_id->rowCount() - 1, 0);
        int last_id = last_id_pos.data(Qt::EditRole).toInt();
        new_id = last_id + 1;

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
            model->submitAll();     /*odstranit tento riadok? asi ano, na subnit tam je uz tlacitko tak nech sa to urobi pomocou neho**/
        }
    }
}


/*otvorenie okna common_settings*/
void MainWindow::on_pushButton_common_settings_clicked()
{
    emit send_new_id(new_id);
    window->show();
}


/*zrusenie zaznamov oznacenych na vymazanie*/
void MainWindow::on_pushButton_revert_random_clicked()
{
    model->revertAll();
}


/*otvorenie okna poptavky*/
void MainWindow::on_pushButton_inquiery_clicked()
{
    QSqlDatabase::database(0).close();
    emit send_new_id(new_id);
    inquiery_window->show();
}


/*filtrovanie podla poptavky*/
void MainWindow::on_OK_button_filter_main_profiles_clicked()
{
    //ak je zaskrtnuty checkbox a vyplnene filtre
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
            QString query_profilesX_text;
            if (ui->comboBox->currentText() == "Random")
                query_profilesX_text = "adProfileId IN (";

            else if(ui->comboBox->currentText() == "Sine")
                query_profilesX_text = "adProfileId IN (";

            else if (ui->comboBox->currentText() == "Shocks")
                query_profilesX_text = "profileId IN (";

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
            SetColumnNames(model, ui->tableView_random);

            model->setFilter(query_profilesX_text);
            model->select();
        }
    }

    //ak je zaskrtnuty zakazkovy filter checkbox a vyplnene filtre
    else if (ui->checkBox_order->isChecked() && !ui->lineEdit_filter_order_id->text().isEmpty() && !ui->lineEdit_filter_order_year->text().isEmpty())
    {
        QSqlDatabase::database("kniha_zakazek").open();

        QSqlQuery query_contract(QSqlDatabase::database("kniha_zakazek"));

        query_contract.prepare("SELECT customerInquiryYear, customerInquiryNumber, customerInquiryIndex FROM main_list "
                               "WHERE contractYear = ? AND contractNumber = ?");

        query_contract.bindValue(0, ui->lineEdit_filter_order_year->text());
        query_contract.bindValue(1, ui->lineEdit_filter_order_id->text());

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
            QString query_profilesX_text;
            if (ui->comboBox->currentText() == "Random")
                query_profilesX_text = "adProfileId IN (";

            else if(ui->comboBox->currentText() == "Sine")
                query_profilesX_text = "adProfileId IN (";

            else if (ui->comboBox->currentText() == "Shocks")
                query_profilesX_text = "profileId IN (";

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
            SetColumnNames(model, ui->tableView_random);

            model->setFilter(query_profilesX_text);
            model->select();
        }
    }
}


/*odomknutie filtru poptavky na pisanie*/
void MainWindow::on_checkBox_inqr_stateChanged()
{
    if(ui->checkBox_inqr->isChecked())
    {
        ui->lineEdit_filter_inqr_id->setReadOnly(0);
        ui->lineEdit_filter_inqr_year->setReadOnly(0);
        ui->lineEdit_filter_inqr_index->setReadOnly(0);
        palette->setColor(QPalette::Base,Qt::white);
        palette->setColor(QPalette::Text,Qt::black);
        ui->lineEdit_filter_inqr_id->setPalette(*palette);
        ui->lineEdit_filter_inqr_year->setPalette(*palette);
        ui->lineEdit_filter_inqr_index->setPalette(*palette);
    }

    else if(!ui->checkBox_inqr->isChecked())
    {
        ui->lineEdit_filter_inqr_id->setReadOnly(1);
        ui->lineEdit_filter_inqr_year->setReadOnly(1);
        ui->lineEdit_filter_inqr_index->setReadOnly(1);
        palette->setColor(QPalette::Base,Qt::gray);
        palette->setColor(QPalette::Text,Qt::darkGray);
        ui->lineEdit_filter_inqr_id->setPalette(*palette);
        ui->lineEdit_filter_inqr_year->setPalette(*palette);
        ui->lineEdit_filter_inqr_index->setPalette(*palette);
    }
}


/*odomknutie filtru zakazky na pisanie*/
void MainWindow::on_checkBox_order_stateChanged()
{
    if(ui->checkBox_order->isChecked())
        {
            ui->lineEdit_filter_order_id->setReadOnly(0);
            ui->lineEdit_filter_order_year->setReadOnly(0);
            palette->setColor(QPalette::Base,Qt::white);
            palette->setColor(QPalette::Text,Qt::black);
            ui->lineEdit_filter_order_id->setPalette(*palette);
            ui->lineEdit_filter_order_year->setPalette(*palette);
        }

        else if(!ui->checkBox_order->isChecked())
        {
            ui->lineEdit_filter_order_id->setReadOnly(1);
            ui->lineEdit_filter_order_year->setReadOnly(1);
            palette->setColor(QPalette::Base,Qt::gray);
            palette->setColor(QPalette::Text,Qt::darkGray);
            ui->lineEdit_filter_order_id->setPalette(*palette);
            ui->lineEdit_filter_order_year->setPalette(*palette);
    }
}


/********ak sa oznaci cely riadok v hornej tabulke*********/
void MainWindow::receive_section_row()
{
    if (ui->comboBox->currentText() == "Sine")
    {
        if (ui->tableView_random->selectionModel() != nullptr)
        {
            QItemSelectionModel* sel_model = ui->tableView_random->selectionModel();
            QModelIndexList indexes = sel_model->selectedIndexes();
            if (indexes.size() == 7)
            {
                double freq = indexes[1].data().toDouble();
                double accel = indexes[2].data().toDouble();

                double vel = accel / (2*atan(1)*4*freq);
                double d = (vel / (2*atan(1)*4*freq))*1000;
                ui->lineEdit_v->setText(to_string(vel).c_str());
                ui->lineEdit_d->setText(to_string(d).c_str());
            }
        }
    }
}


/********* vypocet zo zadanej rychlosti - horna tabulka ********/
void MainWindow::on_lineEdit_v_editingFinished()
{
    if (ui->comboBox->currentText() == "Sine")
    {
        if (ui->tableView_random->selectionModel() != nullptr)
        {
            QItemSelectionModel* sel_model = ui->tableView_random->selectionModel();
            QModelIndexList indexes = sel_model->selectedIndexes();
            if (indexes.size() == 7)
            {
                double freq = indexes[1].data().toDouble();
                double vel = ui->lineEdit_v->text().toDouble();

                double accel = vel*2*atan(1)*4*freq;
                ui->tableView_random->model()->setData(indexes[2], accel);
                double d = (vel / (2*atan(1)*4*freq))*1000;
                ui->lineEdit_d->setText(to_string(d).c_str());
            }
        }
    }
}


/********* vypocet zo zadanej vychylky - horna tabulka ********/
void MainWindow::on_lineEdit_d_editingFinished()
{
    if (ui->comboBox->currentText() == "Sine")
    {
        if (ui->tableView_random->selectionModel() != nullptr)
        {
            QItemSelectionModel* sel_model = ui->tableView_random->selectionModel();
            QModelIndexList indexes = sel_model->selectedIndexes();
            if (indexes.size() == 7)
            {
                double freq = indexes[1].data().toDouble();
                double d = ui->lineEdit_d->text().toDouble();

                double vel = (d/1000)*2*atan(1)*4*freq;
                ui->lineEdit_v->setText(to_string(vel).c_str());
                double accel = vel * (2*atan(1)*4*freq);
                ui->tableView_random->model()->setData(indexes[2], accel);
            }
        }
    }
}


/********ak sa oznaci cely riadok v spodnej tabulke*********/
void MainWindow::receive_section_row_bottom()
{
    if (ui->comboBox->currentText() == "Sine")
    {
        if (ui->tableView_random_new->selectionModel() != nullptr)
        {
            QItemSelectionModel* sel_model = ui->tableView_random_new->selectionModel();
            QModelIndexList indexes = sel_model->selectedIndexes();
            if (indexes.size() == 7)
            {
                double freq = indexes[1].data().toDouble();
                double accel = indexes[2].data().toDouble();

                double vel = accel / (2*atan(1)*4*freq);
                double d = (vel / (2*atan(1)*4*freq))*1000;
                ui->lineEdit_v_bottom->setText(to_string(vel).c_str());
                ui->lineEdit_d_bottom->setText(to_string(d).c_str());
            }
        }
    }
}


/********* vypocet zo zadanej rychlosti - dolna tabulka ********/
void MainWindow::on_lineEdit_v_bottom_editingFinished()
{
    if (ui->comboBox->currentText() == "Sine")
    {
        if (ui->tableView_random_new->selectionModel() != nullptr)
        {
            QItemSelectionModel* sel_model = ui->tableView_random_new->selectionModel();
            QModelIndexList indexes = sel_model->selectedIndexes();
            if (indexes.size() == 7)
            {
                double freq = indexes[1].data().toDouble();
                double vel = ui->lineEdit_v_bottom->text().toDouble();

                double accel = vel*2*atan(1)*4*freq;
                ui->tableView_random_new->model()->setData(indexes[2], accel);
                double d = (vel / (2*atan(1)*4*freq))*1000;
                ui->lineEdit_d_bottom->setText(to_string(d).c_str());
            }
        }
    }
}


/********* vypocet zo zadanej vychylky - dolna tabulka ********/
void MainWindow::on_lineEdit_d_bottom_editingFinished()
{
    if (ui->comboBox->currentText() == "Sine")
    {
        if (ui->tableView_random_new->selectionModel() != nullptr)
        {
            QItemSelectionModel* sel_model = ui->tableView_random_new->selectionModel();
            QModelIndexList indexes = sel_model->selectedIndexes();
            if (indexes.size() == 7)
            {
                double freq = indexes[1].data().toDouble();
                double d = ui->lineEdit_d_bottom->text().toDouble();

                double vel = (d/1000)*2*atan(1)*4*freq;
                ui->lineEdit_v_bottom->setText(to_string(vel).c_str());
                double accel = vel * (2*atan(1)*4*freq);
                ui->tableView_random_new->model()->setData(indexes[2], accel);
            }
        }
    }
}


/*******limits dB/% prepocet *********/
void MainWindow::on_comboBox_limits_currentTextChanged()
{
    if (ui->comboBox->currentText() == "Sine" || ui->comboBox->currentText() == "Random")
    {
        if (ui->tableView_random->model() != nullptr)
        {
            int rows = model->rowCount();
            if(rows != 0)
            {
                if (ui->comboBox_limits->currentText() == "Limits in %")
                {
                    QModelIndex index;
                    double value_dB, value_percent;
                    for(int i = 0; i < rows; i++)
                    {
                        for(int j = 3; j < 7; j++)
                        {
                            index = model->index(i, j);
                            value_dB = model->data(index).toDouble();

                            //horne limity
                            if(j == 3 || j == 5)
                            {
                                if (ui->comboBox->currentText() == "Sine")
                                    value_percent = 100 * (pow(10, value_dB/20) - 1);

                                else if(ui->comboBox->currentText() == "Random")
                                    value_percent = 100 * (pow(10, value_dB/10) - 1);
                            }

                            //dolne limity
                            if(j == 4 || j == 6)
                            {
                                if (ui->comboBox->currentText() == "Sine")
                                    value_percent = 100 * (1 - pow(10, -value_dB/20));

                                else if(ui->comboBox->currentText() == "Random")
                                    value_percent = 100 * (1 - pow(10, -value_dB/10));
                            }

                            model->setData(index, value_percent);
                        }
                    }
                }

                else if (ui->comboBox_limits->currentText() == "Limits in dB")
                {
                    QModelIndex index;
                    double value_percent, value_dB;
                    for(int i = 0; i < rows; i++)
                    {
                        for(int j = 3; j < 7; j++)
                        {
                            index = model->index(i, j);
                            value_percent = model->data(index).toDouble();

                            //horne limity
                            if(j == 3 || j == 5)
                            {
                                if (ui->comboBox->currentText() == "Sine")
                                    value_dB = 20 * log10(1 + value_percent/100);

                                else if(ui->comboBox->currentText() == "Random")
                                    value_dB = 10 * log10(1 + value_percent/100);
                            }

                            //dolne limity
                            if(j == 4 || j == 6)
                            {
                                if (ui->comboBox->currentText() == "Sine")
                                    value_dB = -20 * log10(1 - value_percent/100);

                                else if(ui->comboBox->currentText() == "Random")
                                    value_dB = -10 * log10(1 - value_percent/100);
                            }

                            model->setData(index, value_dB);
                        }
                    }
                }
            }
        }
    }

    /* pre spodnu tabulku */
    if (ui->comboBox->currentText() == "Sine" || ui->comboBox->currentText() == "Random")
    {
        if (ui->tableView_random_new->model() != nullptr)
        {
            int rows = ui->tableView_random_new->model()->rowCount();
            if(rows != 0)
            {
                if (ui->comboBox_limits->currentText() == "Limits in %")
                {
                    QModelIndex index;
                    double value_dB, value_percent;
                    for(int i = 0; i < rows; i++)
                    {
                        for(int j = 3; j < 7; j++)
                        {
                            index = ui->tableView_random_new->model()->index(i, j);
                            value_dB = ui->tableView_random_new->model()->data(index).toDouble();

                            //horne limity
                            if(j == 3 || j == 5)
                            {
                                if (ui->comboBox->currentText() == "Sine")
                                    value_percent = 100 * (pow(10, value_dB/20) - 1);

                                else if(ui->comboBox->currentText() == "Random")
                                    value_percent = 100 * (pow(10, value_dB/10) - 1);
                            }

                            //dolne limity
                            if(j == 4 || j == 6)
                            {
                                if (ui->comboBox->currentText() == "Sine")
                                    value_percent = 100 * (1 - pow(10, -value_dB/20));

                                else if(ui->comboBox->currentText() == "Random")
                                    value_percent = 100 * (1 - pow(10, -value_dB/10));
                            }

                            ui->tableView_random_new->model()->setData(index, value_percent);
                        }
                    }
                }

                else if (ui->comboBox_limits->currentText() == "Limits in dB")
                {
                    QModelIndex index;
                    double value_percent, value_dB;
                    for(int i = 0; i < rows; i++)
                    {
                        for(int j = 3; j < 7; j++)
                        {
                            index = ui->tableView_random_new->model()->index(i, j);
                            value_percent = ui->tableView_random_new->model()->data(index).toDouble();

                            //horne limity
                            if(j == 3 || j == 5)
                            {
                                if (ui->comboBox->currentText() == "Sine")
                                    value_dB = 20 * log10(1 + value_percent/100);

                                else if(ui->comboBox->currentText() == "Random")
                                    value_dB = 10 * log10(1 + value_percent/100);
                            }

                            //dolne limity
                            if(j == 4 || j == 6)
                            {
                                if (ui->comboBox->currentText() == "Sine")
                                    value_dB = -20 * log10(1 - value_percent/100);

                                else if(ui->comboBox->currentText() == "Random")
                                    value_dB = -10 * log10(1 - value_percent/100);
                            }

                            ui->tableView_random_new->model()->setData(index, value_dB);
                        }
                    }
                }
            }
        }
    }
}


/********** zobrazenie okna rampy *********/
void MainWindow::on_pushButton_ramp_clicked()
{
    if(ui->tableView_random_new->selectionModel() != nullptr)
    {
        QModelIndexList rows = ui->tableView_random_new->selectionModel()->selectedRows();
        QModelIndexList indexes = ui->tableView_random_new->selectionModel()->selectedIndexes();
        QList <QVariant> list;
        if(ui->comboBox->currentText() == "Random")
        {
            if(rows.count() == 1)
            {
                for(int i = 0; i < indexes.count(); i++)
                    list.push_back(indexes[i].data());

                emit send_to_ramp(list);
                ramp_window->show();
            }
        }
    }
}


/******* spristupnenie tlacitok podla typu vibr. skusky ******/
void MainWindow::on_comboBox_currentTextChanged()
{
    if(ui->comboBox->currentText() == "Random")
    {
        ui->pushButton_ramp->setEnabled(1);
        ui->comboBox_limits->setEnabled(1);
        ui->lineEdit_d->setDisabled(1);
        ui->lineEdit_d_bottom->setDisabled(1);
        ui->lineEdit_v->setDisabled(1);
        ui->lineEdit_v_bottom->setDisabled(1);
    }

    else if(ui->comboBox->currentText() == "Sine")
    {
        ui->pushButton_ramp->setDisabled(1);
        ui->comboBox_limits->setEnabled(1);
        ui->lineEdit_d->setEnabled(1);
        ui->lineEdit_d_bottom->setEnabled(1);
        ui->lineEdit_v->setEnabled(1);
        ui->lineEdit_v_bottom->setEnabled(1);
    }

    else if(ui->comboBox->currentText() == "Shocks")
    {
        ui->pushButton_ramp->setDisabled(1);
        ui->comboBox_limits->setDisabled(1);
        ui->lineEdit_d->setDisabled(1);
        ui->lineEdit_d_bottom->setDisabled(1);
        ui->lineEdit_v->setDisabled(1);
        ui->lineEdit_v_bottom->setDisabled(1);
    }
}


/******** nove data z karty rampa *******/
void MainWindow::receive_new_data_from_ramp(QList<QVariant> list)
{
    QStandardItem* item = nullptr;
    QStandardItemModel* model = new QStandardItemModel();
    model = static_cast<QStandardItemModel*>(ui->tableView_random_new->model());
    model->insertRow(model->rowCount(), item);

    for(int i = 0; i < model->columnCount(); i++)
    {
        QModelIndex index = model->index(model->rowCount() - 1, i);
        model->setData(index, list[i], Qt::EditRole);
    }

    //usporiadanie podla frekvencie vzostupne
    model->sort(1);
    ui->tableView_random_new->setModel(model);
}
