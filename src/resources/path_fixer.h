#ifndef RESOURCES_PATH_FIXER_H
#define RESOURCES_PATH_FIXER_H

#include <QObject>
#include <QList>

#include "db/handler.h"
#include "db/table_records.h"

namespace Resources {

class PathFixer : public QObject
{
    Q_OBJECT
public:
    explicit PathFixer(DB::Handler* handler, QObject *parent = 0);
    ~PathFixer();

    static void remove(DB::SoundFileRecord* rec, DB::Handler* handler);
    static bool check(DB::SoundFileRecord* rec);
signals:

public slots:

private:
    DB::Handler* handler_;
};

} // namespace Resources

#endif // RESOURCES_PATH_FIXER_H
