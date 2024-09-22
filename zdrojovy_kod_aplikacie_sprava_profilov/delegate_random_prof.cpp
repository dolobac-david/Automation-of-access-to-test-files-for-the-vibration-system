#include "delegate_random_prof.h"

Delegate_random_prof::Delegate_random_prof(QObject *parent): QStyledItemDelegate(parent)
{}


QWidget* Delegate_random_prof::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QDoubleSpinBox *editor = new QDoubleSpinBox(parent);
    editor->setMaximum(1000000);
    editor->setDecimals(15);
    return editor;
}


void Delegate_random_prof::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    double value = index.model()->data(index, Qt::EditRole).toDouble();
    QDoubleSpinBox* spinbox = static_cast<QDoubleSpinBox*>(editor);
    spinbox->setValue(value);
}


void Delegate_random_prof::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QDoubleSpinBox* spinbox = static_cast<QDoubleSpinBox*>(editor);
    spinbox->interpretText();
    double value = spinbox->value();
    model->setData(index, value, Qt::EditRole);
}


void Delegate_random_prof::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    editor->setGeometry(option.rect);
}


QString Delegate_random_prof::displayText(const QVariant &value, const QLocale &locale) const
{
    if (value.userType() == QVariant::Double )
        return locale.toString(value.toDouble(), 'g', 10);

    else
        return QStyledItemDelegate::displayText(value, locale);
}
