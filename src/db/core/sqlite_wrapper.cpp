#include "sqlite_wrapper.h"

#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QElapsedTimer>
#include <QCoreApplication>

namespace DB {
namespace Core {

SqliteWrapper::SqliteWrapper(QString const& db_path, QObject* parent):
    QObject(parent)
  , db_()
{
    initDB(db_path);
}

QSqlRelationalTableModel* SqliteWrapper::getTable(TableIndex index)
{
    if(!db_.isOpen() || index == NONE) {
        qDebug() << "FAILURE: database not open";
        return 0;
    }

    QSqlRelationalTableModel* model = new QSqlRelationalTableModel(this);
    model->setTable(toString(index));
    model->select();

    QElapsedTimer timer;
    timer.start();
    while(model->canFetchMore()) {
        model->fetchMore();
        if(timer.elapsed() > 50) {
            QCoreApplication::processEvents();
            timer.start();
            qDebug() << "NOTIFICATION: fetching more rows";
            qDebug() << " > table name:" << model->tableName();
        }
    }

    return model;
}

const QList<QSqlRecord> SqliteWrapper::selectQuery(const QString &SELECT, const QString &FROM, const QString &WHERE)
{
    if(SELECT.size() == 0 || FROM.size() == 0) {
        qDebug() << "FAILURE: parameter in selectQuery() missing";
        qDebug() << " > SELECT:" << SELECT;
        qDebug() << " > FROM:" << FROM;
        qDebug() << " > WHERE:" << WHERE;
        return QList<QSqlRecord>();
    }

    QString qry_str = "SELECT " + SELECT + " FROM " + FROM;
    if(WHERE.size() > 0)
        qry_str += " WHERE " + WHERE;

    return executeQuery(qry_str);
}

const QList<QSqlRecord> SqliteWrapper::selectQuery(const QString &SELECT, TableIndex FROM, const QString &WHERE)
{
    return selectQuery(SELECT, toString(FROM), WHERE);
}

void SqliteWrapper::insertQuery(TableIndex index, const QString &value_block)
{
    if(index == NONE)
        return;

    QString qry_str = "INSERT INTO " + toString(index) + " ";
    qry_str += value_block;
    executeQuery(qry_str);
}

void SqliteWrapper::deleteQuery(TableIndex index, const QString &WHERE)
{
    QString qry = "DELETE FROM " + toString(index) + " WHERE " + WHERE;
    executeQuery(qry);
}

void SqliteWrapper::open()
{
    if(db_.isOpen()) {
        qDebug() << "NOTIFICATION: database is already open";
        return;
    }

    if(db_.open()) {
        qDebug() << "SUCCESS: connected to database";
    }
    else {
        qDebug() << "FAILURE: could not open database";
    }
}

void SqliteWrapper::close()
{
    if(db_.isOpen()) {
        db_.close();
    }
    else {
        qDebug() << "NOTIFICATION: database is not open";
    }
}

const QString SqliteWrapper::escape(const QString &str)
{
    QString temp(str);
    temp.replace("'", "''");
    return temp;
}

void SqliteWrapper::initDB(QString const& db_path)
{
    db_ = QSqlDatabase::addDatabase("QSQLITE");
    db_.setDatabaseName(db_path);

    qDebug() << " > initializing db from:" << db_path;

    open();
}

const QList<QSqlRecord> SqliteWrapper::executeQuery(const QString & qry_str)
{
    QList<QSqlRecord> results;
    if(!db_.isOpen()) {
        qDebug() << "FAILURE: Database not open";
        return results;
    }

    QSqlQuery qry;
    qry.prepare(qry_str);
    qry.setForwardOnly(true);
    if(qry.exec()) {
        QElapsedTimer timer;
        timer.start();
        int i = 0;
        while(qry.next()) {
            results.append(qry.record());
            ++i;
            if(timer.elapsed() > 50) {
                QCoreApplication::processEvents();
                timer.start();
                float progress = i / (float)qry.size();
                qDebug() << "SELECTING " +
                            QString::number(i) + "/" + QString::number(qry.size()) +
                            " (" + QString::number( (int) (progress * 100) ) + "%)";
            }
        }
    }
    else {
        qDebug() << "FAILURE: SQL Query failed to execute.";
        qDebug() << " > Query:" << qry_str;
        qDebug() << " > Error:" << qry.lastError().text();
    }

    return results;
}

} // namespace Core
} // namespace DB
