#ifndef EXTENDED_DATA_TYPES_H
#define EXTENDED_DATA_TYPES_H

#include "data_types.h"

struct QualifiedSoundData: SoundData {
    QualifiedSoundData(const SoundData &sound, const QString& server);
    QualifiedSoundData(const QJsonObject& obj);
    QString server;

    virtual const QJsonObject toJsonObject() const override;
    virtual void setFromJsonObject(const QJsonObject &obj) override;
};

#endif // EXTENDED_DATA_TYPES_H
