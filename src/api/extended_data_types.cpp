#include "extended_data_types.h"

QualifiedSoundData::QualifiedSoundData(const SoundData &sound, const QString &server)
    : SoundData(sound)
    , server(server)
{}

QualifiedSoundData::QualifiedSoundData(const QJsonObject &obj)
    : SoundData()
    , server()
{
    setFromJsonObject(obj);
}

const QJsonObject QualifiedSoundData::toJsonObject() const
{
    QJsonObject obj(SoundData::toJsonObject());
    obj["server"] = server;
    return obj;
}

void QualifiedSoundData::setFromJsonObject(const QJsonObject &obj)
{
    SoundData::setFromJsonObject(obj);
    if(obj.contains("server") && obj["server"].isString())
        server = obj["server"].toString();
}
