#ifndef TAG_CONTEXT_MENU_H
#define TAG_CONTEXT_MENU_H

#include <QMenu>
#include <QCheckBox>

#include <db/handler.h>

#include <db/table_records.h>
#include "tag_action.h"

class TagContextMenu : public QMenu
{
    Q_OBJECT
public:
    explicit TagContextMenu(DB::Handler *db_handler, QWidget *parent = nullptr);

    void setTagModel(DB::Model::TagTableModel *model);
    void addTagAction(DB::TagRecord *tag, Qt::CheckState = Qt::CheckState::Unchecked);
    void deleteTagAction(DB::TagRecord *tag);

    void updateSelectedRecords(QList<DB::SoundFileRecord*> selected_records);
signals:

public slots:
    void onToggleTag(DB::TagRecord* tag, Qt::CheckState state);
    void onAboutToShow();

private:
    void init();
    void closeEvent(QCloseEvent* e);

    QList<DB::SoundFileRecord*> selected_records_;
    QMap<DB::TagRecord*,TagAction*> tag_actions_;
    QMap<DB::TagRecord*, QList<DB::SoundFileRecord*>> sf_per_tag_;
    DB::Handler *db_handler_;
    DB::Model::TagTableModel *tag_model_;

};

#endif // TAG_CONTEXT_MENU_H
