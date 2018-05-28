#include "plugin_handler.h"

#include <QDir>
#include <QPluginLoader>
#include <QDebug>
#include <QGenericPlugin>

PluginHandler::PluginHandler(const QString &plugin_path, QObject *parent)
    : QObject(parent)
    , plugin_path_(plugin_path)
    , plugin_name_("")
    , plugin_(0)
{}

PluginHandler::PluginHandler(const QString& plugin_path, const QString& plugin_name, QObject *parent)
    : QObject(parent)
    , plugin_path_(plugin_path)
    , plugin_name_(plugin_name)
    , plugin_(0)
{}

PluginHandler::~PluginHandler()
{
    if(plugin_)
        plugin_->deleteLater();
}

void PluginHandler::init()
{
    if(!plugin_)
        loadPlugin();
}

const QString &PluginHandler::getPluginPath() const
{
    return plugin_path_;
}

const QString PluginHandler::getPluginName() const
{
    if(plugin_)
        return plugin_->metaObject()->className();
    return plugin_name_;
}

void PluginHandler::loadPlugin()
{
    qDebug() << "LOADING PLUGIN @" << plugin_path_;
    QDir plugin_dir(plugin_path_);
    foreach (QString file_name, plugin_dir.entryList(QDir::Files)) {
        QPluginLoader loader(plugin_dir.absoluteFilePath(file_name));
        QObject *plugin_obj = loader.instance();
        if (plugin_obj) {
            QString c_name(plugin_obj->metaObject()->className());
            if(plugin_name_.size() == 0 || c_name.compare(plugin_name_) == 0) {
                qDebug() << "  > SUCCESS: loaded.\n"
                         << "    > file" << file_name << "\n"
                         << "    > class" << plugin_obj->metaObject()->className();
                plugin_ = qobject_cast<QGenericPlugin*>(plugin_obj);
                return;
            }
            else {
                qDebug() << "  > NOTIFICATION: skipped.\n"
                         << "    > file" << file_name << "had wrong plugin name."
                         << "    > found class" << c_name
                         << "    > searched for class" << plugin_name_;
            }
        }
        else {
            qDebug() << "  > NOTIFICATION: skipped.\n"
                     << "    > file" << file_name << "failed to load.";
        }
    }
}
