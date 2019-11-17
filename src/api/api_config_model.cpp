#include "api_config_model.h"

ApiConfigModel::ApiConfigModel(QObject *parent)
    : QAbstractTableModel(parent)
    , configs_()
{
}

const ApiConfig ApiConfigModel::operator [](const QString &name) const
{
    return configs_[name];
}

const ApiConfig ApiConfigModel::operator [](const QModelIndex &index) const
{
    if(!indexIsValid(index))
        return ApiConfig();
    return configs_[getName(index)];
}

const QString ApiConfigModel::getName(const QModelIndex &idx) const
{
    if(!indexIsValid(idx))
        return "";
    return data(index(idx.row(), 0)).toString();
}

const QMap<QString, ApiConfig> &ApiConfigModel::getMap() const
{
    return configs_;
}

void ApiConfigModel::setApiConfig(const QString &name, const ApiConfig &config)
{
    if(configs_.contains(name)) {
        configs_[name] = config;
        emit configurationChanged(name);
    }
    else {
        configs_[name] = config;
        emit configurationAdded(name);
        emit layoutAboutToBeChanged();
        emit layoutChanged();
    }

    emit dataChanged(index(0,0), index(rowCount(), columnCount()));
}

bool ApiConfigModel::removeApiConfig(const QString &name)
{
    if(!configs_.contains(name))
        return false;
    configs_.remove(name);
    emit configurationRemoved(name);
    emit layoutAboutToBeChanged();
    emit layoutChanged();
    emit dataChanged(index(0,0), index(rowCount(), columnCount()));
    return true;
}

bool ApiConfigModel::hasApiConfig(const QString &name) const
{
    return configs_.contains(name);
}

QVariant ApiConfigModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    // horizontal header (attribute names)
    if(orientation == Qt::Horizontal) {
        if(section < 0 || section >= columnCount())
            return QVariant();

        if(role == Qt::DisplayRole || role == Qt::EditRole) {
            if(section == 0)
                return QVariant("Name");
            else if(section == 1)
                return QVariant("Url");
            else if(section == 2)
                return QVariant("Access Token");
        }
    }

    // vertical header (number of record)
    else {
        if(section < 0 || section >= rowCount())
            return QVariant();
        if(role == Qt::DisplayRole || role == Qt::EditRole)
            return QVariant(section);
    }

    return QVariant();
}

bool ApiConfigModel::setHeaderData(int, Qt::Orientation, const QVariant &, int)
{
    return false;
}


int ApiConfigModel::rowCount(const QModelIndex &) const
{
    return configs_.size();
}

int ApiConfigModel::columnCount(const QModelIndex &) const
{
    return 3; // name, url, access_token
}

QVariant ApiConfigModel::data(const QModelIndex &index, int role) const
{
    if(!indexIsValid(index))
        return QVariant();

    auto key = configs_.keys()[index.row()];

    if(role == Qt::DisplayRole || role == Qt::EditRole) {
        if(index.column() == 0)
            return QVariant(key);
        else if(index.column() == 1)
            return QVariant(configs_[key].server_url);
        else if(index.column() == 2)
            return QVariant(configs_[key].access_token);
    }

    return QVariant();
}

bool ApiConfigModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    return false;
}

Qt::ItemFlags ApiConfigModel::flags(const QModelIndex &index) const
{
    if(!indexIsValid(index))
        return Qt::ItemIsEnabled;

    return QAbstractTableModel::flags(index);
}

bool ApiConfigModel::removeRows(int, int, const QModelIndex &)
{
    return false;
}

bool ApiConfigModel::removeColumns(int, int, const QModelIndex &)
{
    return false;
}

bool ApiConfigModel::indexIsValid(const QModelIndex &index) const
{
    return index.isValid() && index.row() < rowCount() && index.column() < columnCount();
}
