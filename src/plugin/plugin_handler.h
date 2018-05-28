#ifndef PLUGIN_HANDLER_H
#define PLUGIN_HANDLER_H

#include <QObject>

class QGenericPlugin;

class PluginHandler : public QObject
{
    Q_OBJECT
public:
    explicit PluginHandler(const QString& plugin_path, QObject *parent = nullptr);
    explicit PluginHandler(const QString& plugin_path, const QString& plugin_name, QObject *parent = nullptr);
    virtual ~PluginHandler();

    virtual void init();

    const QString& getPluginPath() const;
    const QString getPluginName() const;

signals:

protected:
    virtual void loadPlugin();

    QString plugin_path_;
    QString plugin_name_;
    QGenericPlugin* plugin_;
};

#endif // PLUGIN_HANDLER_H
