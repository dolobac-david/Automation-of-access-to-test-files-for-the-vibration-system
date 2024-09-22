#ifndef RAMP_H
#define RAMP_H

#include <QWidget>
#include <QDebug>
#include <QStandardItem>

namespace Ui {
class Ramp;
}

class Ramp : public QWidget
{
    Q_OBJECT

public:
    explicit Ramp(QWidget *parent = nullptr);
    ~Ramp();

signals:
    void send_new_data_to_main_window(QList<QVariant> list);

private slots:
    void receive_from_main_window(QList<QVariant> list);
    void on_pushButton_ramp_ok_clicked();

    void on_pushButton_ramp_calc_clicked();

private:
    Ui::Ramp *ui;
    QList<QVariant> list_ramp;
};

#endif // RAMP_H
