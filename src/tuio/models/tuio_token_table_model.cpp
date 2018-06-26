#include "tuio_token_table_model.h"

#include <QSet>

TuioTokenTableModel::TuioTokenTableModel(QObject *parent)
    : QAbstractTableModel(parent)
    , tokens_()
{}

int TuioTokenTableModel::rowCount(const QModelIndex &parent) const
{
    if(parent.isValid())
        return 0;
    return tokens_.size();
}

int TuioTokenTableModel::columnCount(const QModelIndex &parent) const
{
    if(parent.isValid())
        return 0;
    return 10;
}

QVariant TuioTokenTableModel::data(const QModelIndex &index, int role) const
{
    if(role != Qt::DisplayRole || !isValidDataIndex(index))
        return QVariant();
    return data(tokens_[tokens_.keys()[index.row()]], index.column());
}

QVariant TuioTokenTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role != Qt::DisplayRole)
        return QVariant();
    if(orientation == Qt::Vertical) {
        if(section >= rowCount())
            return QVariant();
        return QVariant(section);
    }
    else {
        if(section >= columnCount())
            return QVariant();
        return horizontalHeaderData(section);
    }
}

Qt::ItemFlags TuioTokenTableModel::flags(const QModelIndex &index) const
{
    if(!isValidDataIndex(index) && !isValidHeaderIndex(index))
        return Qt::NoItemFlags;
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

int TuioTokenTableModel::getRow(int id) const
{
    if(!hasToken(id))
        return -1;
    return tokens_.keys().indexOf(id);
}

int TuioTokenTableModel::getRow(const QTuioToken &t) const
{
    return getRow(t.id());
}

bool TuioTokenTableModel::hasToken(int id) const
{
    return tokens_.contains(id);
}

bool TuioTokenTableModel::addToken(const QTuioToken &t)
{
    if(hasToken(t.id()))
        return false;
    internalAddToken(t);
    return true;
}

bool TuioTokenTableModel::addToken(int id)
{
    if(hasToken(id))
        return false;
    QTuioToken t(id);
    internalAddToken(t);
    return true;
}

bool TuioTokenTableModel::updateToken(const QTuioToken &t)
{
    if(!hasToken(t.id()))
        return false;
    internalUpdateToken(t);
    return true;
}

bool TuioTokenTableModel::removeToken(int id)
{
    if(!hasToken(id))
        return false;
    internalRemoveToken(id);
    return true;
}

void TuioTokenTableModel::onTokenEvent(const QMap<int, QTuioToken> &active_tokens, const QVector<QTuioToken> &dead_tokens)
{
    foreach(auto c, active_tokens.values()) {
        if(hasToken(c.id()))
            updateToken(c);
        else
            addToken(c);
    }
    foreach(auto c, dead_tokens) {
        if(hasToken(c.id()))
            removeToken(c.id());
    }
}

QVariant TuioTokenTableModel::data(const QTuioToken &token, int field) const
{
    switch(field) {
        case 0: return QVariant(token.id());
        case 1: return QVariant(token.classId());
        case 2: return QVariant(token.x());
        case 3: return QVariant(token.y());
        case 4: return QVariant(token.vx());
        case 5: return QVariant(token.vy());
        case 6: return QVariant(token.acceleration());
        case 7: return QVariant(token.angle());
        case 8: return QVariant(token.angularVelocity());
        case 9: return QVariant(token.angularAcceleration());
        default: return QVariant();
    }
}

QVariant TuioTokenTableModel::horizontalHeaderData(int field) const
{
    switch(field) {
        case 0: return QVariant("ID");
        case 1: return QVariant("class_ID");
        case 2: return QVariant("X");
        case 3: return QVariant("Y");
        case 4: return QVariant("VX");
        case 5: return QVariant("VY");
        case 6: return QVariant("Acceleration");
        case 7: return QVariant("Angle");
        case 8: return QVariant("Angular Velocity");
        case 9: return QVariant("Angular Acceleration");
        default: return QVariant();
    }
}

void TuioTokenTableModel::internalAddToken(const QTuioToken &t)
{
    QSet<int> keys = tokens_.keys().toSet();
    keys.insert(t.id());
    int row = keys.toList().indexOf(t.id());
    beginInsertRows(QModelIndex(), row, row);
    tokens_[t.id()] = t;
    endInsertRows();
}

void TuioTokenTableModel::internalUpdateToken(const QTuioToken &t)
{
    tokens_[t.id()].setClassId(t.classId());
    tokens_[t.id()].setX(t.x());
    tokens_[t.id()].setY(t.y());
    tokens_[t.id()].setVX(t.vx());
    tokens_[t.id()].setVY(t.vy());
    tokens_[t.id()].setAcceleration(t.acceleration());
    tokens_[t.id()].setAngle(t.angle());
    tokens_[t.id()].setAngularVelocity(t.angularVelocity());
    tokens_[t.id()].setAngularAcceleration(t.angularAcceleration());
    int row = getRow(t.id());
    emit dataChanged(index(row, 0), index(row, columnCount()-1));
}

void TuioTokenTableModel::internalRemoveToken(int id)
{
    int row = getRow(id);
    beginRemoveRows(QModelIndex(), row, row);
    tokens_.remove(id);
    endRemoveRows();
}

bool TuioTokenTableModel::isValidDataIndex(const QModelIndex &idx) const
{
    if(!idx.isValid())
        return false;
    if(idx.row() >= rowCount() || idx.row() < 0)
        return false;
    if(idx.column() >= columnCount() || idx.row() < 0)
        return false;
    return true;
}

bool TuioTokenTableModel::isValidHeaderIndex(const QModelIndex &idx) const
{
    if(!idx.isValid())
        return false;
    return (idx.row() >= 0 && idx.row() < rowCount() && idx.column() == -1) ||
           (idx.column() >= 0 && idx.column() < columnCount() && idx.row() == -1);
}
