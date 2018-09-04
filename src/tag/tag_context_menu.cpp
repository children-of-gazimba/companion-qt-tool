#include "tag_context_menu.h"

#include <QDebug>

TagContextMenu::TagContextMenu(DB::Handler *db_handler, QWidget *parent)
    : QMenu(tr("Assign Tags"),parent)
    , selected_records_()
    , tag_actions_()
    , sf_per_tag_()
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
    connect(ta, &TagAction::toggledTag,
            this, &TagContextMenu::onToggleTag);
    tag_actions_.insert(tag,ta);
    this->addAction(ta);
}

void TagContextMenu::deleteTagAction(DB::TagRecord *tag)
{
    TagAction *t = tag_actions_[tag];
    tag_actions_.remove(tag);
    t->deleteLater();
}

void TagContextMenu::updateSelectedRecords(QList<DB::SoundFileRecord *> selected_records)
{
    sf_per_tag_.clear();

    selected_records_ = selected_records;
    QList<DB::TagRecord*> tags = tag_model_->getTags();

    // check associations between soundfiles and tags
    foreach(auto tag, tags){
        sf_per_tag_[tag] = QList<DB::SoundFileRecord*>();
    }
    foreach (auto sf, selected_records_){
        foreach (auto tag, db_handler_->getTagRecordsBySoundFileId(sf->id)){
            sf_per_tag_[tag].append(sf);
        }
    }
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
//        qDebug() << "sf:" << sf->name;
        if(db_handler_->getSoundFileTableModel()->getSoundFileById(sf->id) == nullptr){
            continue;
        }
        if (state == Qt::CheckState::Checked && !sf_per_tag_[tag].contains(sf)){
            db_handler_->addSoundFileTag(sf->id, tag->id);
            sf_per_tag_[tag].append(sf);
        } else if(state == Qt::CheckState::Unchecked && sf_per_tag_[tag].contains(sf))  {
            db_handler_->deleteSoundFileTag(sf->id, tag->id);
            sf_per_tag_[tag].removeOne(sf);
        }
    }
}

void TagContextMenu::onAboutToShow()
{
    QList<DB::TagRecord*> tags = tag_model_->getTags();
    // check which tag checkboxes need to be checked
    QMap<DB::TagRecord*, Qt::CheckState> tag_states;
    foreach(auto t, sf_per_tag_.keys()){
        if (sf_per_tag_[t].length() == selected_records_.length()){
            tag_states.insert(t, Qt::CheckState::Checked);
        } else if (sf_per_tag_[t].length() == 0){
            tag_states.insert(t, Qt::CheckState::Unchecked);
        } else{
            tag_states.insert(t, Qt::CheckState::PartiallyChecked);
        }
    }

    // create tag actions
    foreach (DB::TagRecord* tag, tags) {
        addTagAction(tag, tag_states[tag]);
    }
}


void TagContextMenu::init()
{
    QList<DB::TagRecord*> tags = tag_model_->getTags();

    connect(this, &QMenu::aboutToShow,
            this, &TagContextMenu::onAboutToShow);
}

void TagContextMenu::closeEvent(QCloseEvent* e)
{
    foreach(auto t, tag_actions_.keys()){
        deleteTagAction(t);
    }

    QMenu::closeEvent(e);
}





























