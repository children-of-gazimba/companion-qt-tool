#ifndef MISC_JSON_MIME_DATA_PARSER_H
#define MISC_JSON_MIME_DATA_PARSER_H

#include <QJsonDocument>
#include <QMimeData>
#include <QJsonObject>
#include <QList>
#include <QPainterPath>

#include "db/table_records.h"
#include "playlist/playlist_settings.h"

/*
* Class used for conversion of TableRecords to QMimeData.
* TableRecords will be referenced by a Json serialized string,
* inside the text() property of QMimeData.
* Also transfers QMimeData back to TableRecord.
*/

class JsonMimeDataParser
{
public:
    explicit JsonMimeDataParser();
    ~JsonMimeDataParser();

    /*
     * creates QMimeData with text set to a string
     * wrapping a json serialized object, referencing
     * given TableRecord.
     * returns 0 if parsing fails.
    */
    static QMimeData* toJsonMimeData(TableRecord*);

    /*
     * creates QMimeData with text set to a string
     * wrapping a json serialized object, referencing
     * given all given TableRecords.
     * returns 0 if parsing fails.
    */
    static QMimeData* toJsonMimeData(const QList<TableRecord*>&);

    /*
     * Extracts TableRecord from given QMimeData.
     * Parser assumes text in QMimeData is in Json format and.
     * (see toJsonMimeData(...))
     * Returns 0 if parsing fails.
     * Note: None of the returned pointers reference any Model::...
     * elements and should therefore be deleted after use.
    */
    static TableRecord* toTableRecord(const QMimeData*);

    /*
     * Extracts list of TableRecord from given QMimeData.
     * Parser assumes text in QMimeData is in Json format and.
     * (see toJsonMimeData(...))
     * Returns 0 if parsing fails.
     * Note: None of the returned pointers reference any Model::...
     * elements and should therefore be deleted after use.
    */
    static QList<TableRecord*> toTableRecordList(const QMimeData*);

    /*
    * Extracts TableRecord from given QJsonObject.
    * Returns 0 if parsing failes.
    * Note: None of the returned pointers reference any Model::...
    * elements and should therefore be deleted after use.
    */
    static TableRecord* toTableRecord(const QJsonObject&);

    /* Creates JsonObject from given TableRecord */
    static const QJsonObject toJsonObject(TableRecord*);

    /* Creates JsonObject from given SoundFileRecord */
    static const QJsonObject toJsonObject(SoundFileRecord*);

    /* Creates JsonObject from given Settings */
    static const QJsonObject toJsonObject(const PlaylistSettings&);

    static PlaylistSettings* toPlaylistSettings(const QJsonObject&);

    /** Creates JsonArray of QPainterPath::Elements */
    static const QJsonArray toJsonArray(const QPainterPath& path);

    /** Creates QPainterPath from JsonArray */
    static const QPainterPath toPainterPath(const QJsonArray& arr);

    /*
     * Creates JsonObject from given CategoryRecord.
     * Serializes only id,index,name,parent_id.
    */
    static const QJsonObject toJsonObject(CategoryRecord*);
};

#endif // MISC_JSON_MIME_DATA_PARSER_H
