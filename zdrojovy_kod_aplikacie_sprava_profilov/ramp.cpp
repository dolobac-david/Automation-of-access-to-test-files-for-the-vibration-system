#include "ramp.h"
#include "ui_ramp.h"

Ramp::Ramp(QWidget *parent) :QWidget(parent), ui(new Ui::Ramp)
{
    ui->setupUi(this);

    ui->lineEdit_ramp_start_freq->setDisabled(1);
    ui->lineEdit_ramp_start_ampl->setDisabled(1);
    ui->lineEdit_ramp_target_ampl->setDisabled(1);
}

Ramp::~Ramp()
{
    delete ui;
}


/******* prijatie dat z main_window ******/
void Ramp::receive_from_main_window(QList<QVariant> list)
{
    list_ramp = list;
    ui->lineEdit_ramp_start_freq->setText(list_ramp[1].toString());
    ui->lineEdit_ramp_start_ampl->setText(list_ramp[2].toString());

}


/******vypocet rampy*******/
void Ramp::on_pushButton_ramp_calc_clicked()
{
    double H = 0;
    double k = 0;
    double f_start = ui->lineEdit_ramp_start_freq->text().toDouble();
    double f_end = ui->lineEdit_ramp_target_freq->text().toDouble();

    if(0 <= ui->lineEdit_ramp_dB->text().toDouble() && ui->lineEdit_ramp_dB->text().toDouble() <= 100)
    {
        if(ui->comboBox_ramp_incr_decr->currentText() == "/ Increasing")
            k = ui->lineEdit_ramp_dB->text().toDouble();
        else if(ui->comboBox_ramp_incr_decr->currentText() == "Decreasing")
            k = -ui->lineEdit_ramp_dB->text().toDouble();

        if(ui->comboBox_ramp_dec_oct->currentText() == "dB / Octave")
            H = (k * log10(f_end/f_start)) / log10(2);
        else if(ui->comboBox_ramp_dec_oct->currentText() == "dB / Decade")
            H = k * log10(f_end/f_start);

        double y_start = ui->lineEdit_ramp_start_ampl->text().toDouble();
        double y_end = y_start * pow(10, H/10);
        ui->lineEdit_ramp_target_ampl->setText(std::to_string(y_end).c_str());
    }
}


/****** odoslanie novych dat z rampy *******/
void Ramp::on_pushButton_ramp_ok_clicked()
{
    list_ramp[1] = ui->lineEdit_ramp_target_freq->text();
    list_ramp[2] = ui->lineEdit_ramp_target_ampl->text();

    emit send_new_data_to_main_window(list_ramp);
}
