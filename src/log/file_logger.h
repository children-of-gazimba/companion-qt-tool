#ifndef LOG_FILE_LOGGER_H
#define LOG_FILE_LOGGER_H

#include <QString>

namespace Log {

class FileLogger
{
public:
    struct LogMessage {
        QString tag;
        QString message;
    };

public:
    FileLogger(const QString& tag);

    void write(const QString& msg);
    static void writeLogMessage(const LogMessage& msg);
    static void clearLogFile();
    QString const& getTag() const;
    void setTag(const QString& tag);
private:
    QString tag_;
};

} // namespace Log

#endif // LOG_FILE_LOGGER_H
