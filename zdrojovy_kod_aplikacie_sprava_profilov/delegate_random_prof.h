#ifndef DELEGATE_RANDOM_PROF_H
#define DELEGATE_RANDOM_PROF_H

#include <QStyledItemDelegate >
#include <QModelIndex>
#include <QObject>
#include <QDoubleSpinBox>

class Delegate_random_prof : public QStyledItemDelegate
{
    Q_OBJECT

public:
    Delegate_random_prof(QObject *parent = nullptr);
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QString displayText(const QVariant &value, const QLocale &locale) const override;
};

#endif // DELEGATE_RANDOM_PROF_H
