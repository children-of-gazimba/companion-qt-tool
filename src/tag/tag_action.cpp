#include "tag_action.h"

TagAction::TagAction(DB::TagRecord* tag, QWidget *parent)
    : QWidgetAction (parent)
    , tag_(tag)
    , check_box_(nullptr)
{
    init();

}

TagAction::~TagAction()
{

}

void TagAction::setCheckState(Qt::CheckState state)
{
    check_box_->setCheckState(state);
}

const DB::TagRecord &TagAction::getTag() const
{
    return *tag_;
}


void TagAction::init()
{
    check_box_ = new QCheckBox(tag_->name, this->parentWidget());
    check_box_->setChecked(false);
    // check_box_->setTristate(true);
    setDefaultWidget(check_box_);

    connect(check_box_, &QCheckBox::stateChanged,
            this, [=](int state){

        emit toggledTag(tag_, (Qt::CheckState) state);
    });
}
