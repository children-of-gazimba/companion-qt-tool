#include "file_logger.h"

#include <QFile>
#include <QTextStream>

#include "resources/lib.h"

namespace Log {

FileLogger::FileLogger(const QString& tag)
    : tag_(tag)
{}

void FileLogger::write(const QString &msg)
{
    LogMessage m;
    m.tag = tag_;
    m.message = msg;
    writeLogMessage(m);
}

void FileLogger::writeLogMessage(const LogMessage& msg)
{
    if ( Resources::Lib::LOG_FILE.open(QIODevice::ReadWrite | QIODevice::Append) )
    {
        QTextStream stream(&Resources::Lib::LOG_FILE);
        stream << " > " << msg.tag << ": " << msg.message << endl;
        Resources::Lib::LOG_FILE.close();
    }
}

void FileLogger::clearLogFile()
{
    Resources::Lib::LOG_FILE.resize(0);
}

const QString &FileLogger::getTag() const
{
    return tag_;
}

void FileLogger::setTag(const QString &tag)
{
    tag_ = tag;
}

} // namespace Log
