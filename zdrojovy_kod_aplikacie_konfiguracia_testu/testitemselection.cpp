#include "testitemselection.h"
#include "ui_testitemselection.h"

TestItemSelection::TestItemSelection(QWidget *parent) :QWidget(parent), ui(new Ui::TestItemSelection)
{
    ui->setupUi(this);

    model_test_item = new QSqlQueryModel(this);

    selectionModel = new QItemSelectionModel();

    model_test_arrangement = new QSqlQueryModel(this);
}

TestItemSelection::~TestItemSelection()
{
    delete ui;
    delete model_test_item;
    delete selectionModel;
    delete model_test_arrangement;
}


/*******ziskanie ID protokolu z knihy zakaziek********/
int FetchProtocolID(QString year_inqr, QString number_inqr, QString index_inqr)
{
    QSqlDatabase::database("kniha_zakazek").open();

    QSqlQuery query_protocols(QSqlDatabase::database("kniha_zakazek"));
    query_protocols.prepare("SELECT testItemsSetRegistrationProtocolId FROM test_items_sets_registration_protocols "
                            "WHERE relCustomerInquiryYear = ? AND relCustomerInquiryNumber = ? AND "
                            "relCustomerInquiryIndex = ?");

    query_protocols.bindValue(0, year_inqr);
    query_protocols.bindValue(1, number_inqr);
    query_protocols.bindValue(2, index_inqr);

    query_protocols.exec();
    QVariant protocol_id;

    while(query_protocols.next())
        protocol_id = query_protocols.value(0);

    QSqlDatabase::database("kniha_zakazek").close();

    return protocol_id.toInt();
}


/********** zobrazenie testovacich poloziek **************/
void TestItemSelection::receive_is_test_item(int value, QString year_inqr, QString number_inqr, QString index_inqr)
{
    if(value == 1)
    {
        int protocol_id = FetchProtocolID(year_inqr, number_inqr, index_inqr);

        QSqlQuery query_test_item(QSqlDatabase::database("kniha_zakazek"));

        query_test_item.prepare("SELECT testItemYear, testItemNumber, testItemMainDescriptionCZ, adTestItemsSetRegistrationProtocolId "
                                "FROM test_items_list WHERE adTestItemsSetRegistrationProtocolId = ? ");

        query_test_item.bindValue(0, protocol_id);

        query_test_item.exec();

        model_test_item->setQuery(query_test_item);
        model_test_item->setHeaderData(0, Qt::Horizontal, "Test item year", Qt::DisplayRole);
        model_test_item->setHeaderData(1, Qt::Horizontal, "Test item number", Qt::DisplayRole);
        model_test_item->setHeaderData(2, Qt::Horizontal, "Test Item Main Description CZ", Qt::DisplayRole);
        model_test_item->setHeaderData(3, Qt::Horizontal, "Test Items Set Registration Protocol ID", Qt::DisplayRole);

        ui->tableView_test_item_selection->setModel(model_test_item);
        ui->tableView_test_item_selection->setColumnWidth(2, 170);
        ui->tableView_test_item_selection->setColumnWidth(3, 180);

        QSqlDatabase::database("kniha_zakazek").close();
    }
}


/**** presun vybranej polozky do export karty *********/
void TestItemSelection::on_pushButton_test_item_select_clicked()
{
    selectionModel = ui->tableView_test_item_selection->selectionModel();
    QModelIndexList indexes = selectionModel->selectedIndexes();

    QVariant test_item_year = indexes.first().data();
    QVariant test_item_number = indexes.at(1).data();

    emit send_test_item_year_number(test_item_year, test_item_number);
}


/************** zobrazenie testovacich zostav aj s polozkami *****************/
void TestItemSelection::receive_is_test_arrangement(int value, QString year_inqr, QString number_inqr, QString index_inqr)
{
    if(value == 1)
    {
        QSqlDatabase::database("kniha_zakazek").open();

        int protocol_id = FetchProtocolID(year_inqr, number_inqr, index_inqr);

        QSqlQuery query_test_arrangement(QSqlDatabase::database("kniha_zakazek"));

        query_test_arrangement.prepare("SELECT test_arrangements_list.adTestArrangementId, "
                                       "test_arrangements_list.adTestArrangementOrder, "
                                       "test_arrangements_list.positionInTestArrangement, "
                                       "test_items_list.testItemYear, test_items_list.testItemNumber, "
                                       "test_items_list.testItemMainDescriptionCZ, "
                                       "test_items_list.adTestItemsSetRegistrationProtocolId FROM "
                                       "test_items_list INNER JOIN test_arrangements_list ON "
                                       "(test_items_list.testItemYear = test_arrangements_list.relTestItemYear AND "
                                       "test_items_list.testItemNumber = test_arrangements_list.relTestItemNumber AND "
                                       "test_items_list.adTestItemsSetRegistrationProtocolId = ?)");

        query_test_arrangement.bindValue(0, protocol_id);

        query_test_arrangement.exec();

        model_test_arrangement->setQuery(query_test_arrangement);

        model_test_arrangement->setHeaderData(0, Qt::Horizontal, "Test arrangement ID", Qt::DisplayRole);
        model_test_arrangement->setHeaderData(1, Qt::Horizontal, "Test arrangement order", Qt::DisplayRole);
        model_test_arrangement->setHeaderData(2, Qt::Horizontal, "Position in test arrangement order", Qt::DisplayRole);
        model_test_arrangement->setHeaderData(3, Qt::Horizontal, "Test item year", Qt::DisplayRole);
        model_test_arrangement->setHeaderData(4, Qt::Horizontal, "Test item number", Qt::DisplayRole);
        model_test_arrangement->setHeaderData(5, Qt::Horizontal, "Test Item Main Description CZ", Qt::DisplayRole);
        model_test_arrangement->setHeaderData(6, Qt::Horizontal, "Test Items Set Registration Protocol ID", Qt::DisplayRole);

        ui->tableView_test_item_selection->setModel(model_test_arrangement);

        ui->tableView_test_item_selection->setColumnWidth(0, 140);
        ui->tableView_test_item_selection->setColumnWidth(1, 140);
        ui->tableView_test_item_selection->setColumnWidth(2, 190);
        ui->tableView_test_item_selection->setColumnWidth(3, 150);
        ui->tableView_test_item_selection->setColumnWidth(4, 150);
        ui->tableView_test_item_selection->setColumnWidth(5, 185);
        ui->tableView_test_item_selection->setColumnWidth(6, 190);

        QSqlDatabase::database("kniha_zakazek").close();
    }
}


/********presun poradia zostavy do export karty*************/
void TestItemSelection::on_pushButton_test_arrangement_select_clicked()
{
    int arrangement_order = 0;
    for(int i = 0; i < model_test_arrangement->rowCount(); i++)
    {
        QModelIndex index = model_test_arrangement->index(i, 1);
        int value = model_test_arrangement->data(index).toInt();
        if (value > arrangement_order)
            arrangement_order = value;
    }

    emit send_test_arrangement_order(arrangement_order);
}
