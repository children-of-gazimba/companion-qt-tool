#ifndef RESOURCES_PATH_FIXER_H
#define RESOURCES_PATH_FIXER_H

#include <QObject>
#include <QList>

#include "db/database_handler.h"
#include "db/table_records.h"

namespace Resources {

class PathFixer : public QObject
{
    Q_OBJECT
public:
    explicit PathFixer(DatabaseHandler* handler, QObject *parent = 0);
    ~PathFixer();

    static void remove(SoundFileRecord* rec, DatabaseHandler* handler);
    static bool check(SoundFileRecord* rec);
signals:

public slots:

private:
    DatabaseHandler* handler_;
};

} // namespace Resources

#endif // RESOURCES_PATH_FIXER_H
