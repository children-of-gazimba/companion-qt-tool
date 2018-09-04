#include "tag_context_menu.h"

#include <QDebug>

TagContextMenu::TagContextMenu(DB::Handler *db_handler, QWidget *parent)
    : QMenu(tr("Assign Tags"),parent)
    , selected_records_()
    , tag_actions_()
    , db_handler_(db_handler)
    , tag_model_(nullptr)
{
    setTagModel(db_handler_->getTagTableModel());
    init();
}

void TagContextMenu::setTagModel(DB::Model::TagTableModel *model)
{
    tag_model_ = model;
}

void TagContextMenu::addTagAction(DB::TagRecord *tag, Qt::CheckState state)
{
    TagAction* ta = new TagAction(tag, this);
    ta->setCheckState(state);

    tag_actions_.insert(tag,ta);
    this->addAction(ta);
}

void TagContextMenu::updateSelectedRecords(QList<DB::SoundFileRecord *> selected_records)
{
    selected_records_ = selected_records;
}

void TagContextMenu::onToggleTag(DB::TagRecord* tag, Qt::CheckState state)
{
    qDebug() << "about to assign tag" << tag->name << state << " to ";
    if(db_handler_ == nullptr){
        return;
    }
    if(db_handler_->getTagTableModel()->getTagByName(tag->name) == nullptr){
        return;
    }
    qDebug() << " selected sound files with length:" << selected_records_.length();
    foreach(auto sf, selected_records_){
        qDebug() << "sf:" << sf->name;
        if(db_handler_->getSoundFileTableModel()->getSoundFileById(sf->id) == nullptr){
            continue;
        }

        if (state == Qt::CheckState::Checked){
            db_handler_->addSoundFileTag(sf->id, tag->id);
        } else {
            db_handler_->deleteSoundFileTag(sf->id, tag->id);
        }

    }
}

void TagContextMenu::onAboutToShow()
{
    QList<DB::TagRecord*> tags = tag_model_->getTags();

    // check associations between soundfiles and tags
    QMap<DB::TagRecord*, QList<DB::SoundFileRecord*>> sf_per_tag;
    foreach(auto tag, tags){
        sf_per_tag[tag] = QList<DB::SoundFileRecord*>();
    }
    foreach (auto sf, selected_records_){
        foreach (auto tag, db_handler_->getTagRecordsBySoundFileId(sf->id)){
            sf_per_tag[tag].append(sf);
        }
    }

    // check which tag checkboxes need to be checked
    QMap<DB::TagRecord*, Qt::CheckState> tag_states;
    foreach(auto t, sf_per_tag.keys()){
        if (sf_per_tag[t].length() == selected_records_.length()){
            tag_states.insert(t, Qt::CheckState::Checked);
        } else if (sf_per_tag[t].length() == 0){
            tag_states.insert(t, Qt::CheckState::Unchecked);
        } else{
            tag_states.insert(t, Qt::CheckState::PartiallyChecked);
        }
    }

    // update tag actions checkbox state
    foreach (DB::TagRecord* tag, tags) {
        TagAction* ta = tag_actions_.value(tag, nullptr);
        if (ta != nullptr){
            tag_actions_.value(tag)->setCheckState(tag_states[tag]);
        } else {
            addTagAction(tag, Qt::CheckState::Unchecked);
        }
    }

    // connect actions to toggle signal
    foreach(auto ta, tag_actions_){
        connect(ta, &TagAction::toggledTag,
                this, &TagContextMenu::onToggleTag);
    }
}


void TagContextMenu::init()
{
    QList<DB::TagRecord*> tags = tag_model_->getTags();
    foreach(auto tag, tags){
        addTagAction(tag, Qt::CheckState::Unchecked);
    }

    connect(this, &QMenu::aboutToShow,
            this, &TagContextMenu::onAboutToShow);
}

void TagContextMenu::closeEvent(QCloseEvent* e)
{
    qDebug() << "disconnect and close";
    foreach(auto ta, tag_actions_){
        disconnect(ta, &TagAction::toggledTag,
                   this, &TagContextMenu::onToggleTag);
    }

    QMenu::closeEvent(e);
}





























