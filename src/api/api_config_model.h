#ifndef API_CONFIG_MODEL_H
#define API_CONFIG_MODEL_H

#include <QAbstractTableModel>

#include <QMap>

#include "api_config.h"

class ApiConfigModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit ApiConfigModel(QObject *parent = nullptr);

    const ApiConfig operator [](const QString& name) const;
    const ApiConfig operator [](const QModelIndex& index) const;

    const QString getName(const QModelIndex& index) const;
    const QMap<QString, ApiConfig>& getMap() const;

    void setApiConfig(const QString& name, const ApiConfig& config);
    bool removeApiConfig(const QString& name);
    bool hasApiConfig(const QString& name) const;

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole) override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Editable:
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    // Remove data:
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool removeColumns(int column, int count, const QModelIndex &parent = QModelIndex()) override;

signals:
    void configurationAdded(const QString&);
    void configurationChanged(const QString&);
    void configurationRemoved(const QString&);

private:
    /* validates existance of given QModelIndex for this model **/
    bool indexIsValid(const QModelIndex&) const;

    QMap<QString, ApiConfig> configs_;

};

#endif // API_CONFIG_MODEL_H
