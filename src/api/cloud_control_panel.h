#ifndef CLOUD_CONTROL_PANEL_H
#define CLOUD_CONTROL_PANEL_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QTableView>

class CloudControlPanel : public QWidget
{
    Q_OBJECT
public:
    explicit CloudControlPanel(QWidget *parent = nullptr);

signals:

public slots:

private slots:
    void populateApiConfigForEdit(const QModelIndex&);
    void submitEdit();
    void deleteEdit();
    void applyDeleteEnabled(const QString& name);

private:
    void initWidgets();
    void initLayout();

    QLineEdit* name_edit_;
    QLineEdit* url_edit_;
    QLineEdit* token_edit_;
    QPushButton* button_submit_;
    QPushButton* button_delete_;
    QTableView* table_view_;
};

#endif // CLOUDCONTROLPANEL_H
