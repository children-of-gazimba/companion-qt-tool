#include "tag_view.h"

#include <QVBoxLayout>
#include <QHeaderView>

TagView::TagView(QWidget *parent)
    : QWidget(parent)
    , tag_view_(nullptr)
    , add_button_(nullptr)
    , tag_name_edit_(nullptr)
    , model_(nullptr)
{
    initWidgets();
    initLayout();
}

void TagView::setTagModel(DB::Model::TagTableModel *model)
{
    model_ = model;
    tag_view_->setModel(model_);
}

void TagView::verifyTag()
{
    if(model_ == nullptr)
        return;
    if(model_->getTagByName(tag_name_edit_->text()) != nullptr) {
        add_button_->setEnabled(false);
    }
    else {
        add_button_->setEnabled(true);
    }
}

void TagView::onAddTagClicked()
{
    if(model_ == nullptr)
        return;
    model_->addTagRecord(tag_name_edit_->text());
    verifyTag();
}

void TagView::initWidgets()
{
    tag_name_edit_ = new QLineEdit(this);
    tag_name_edit_->setPlaceholderText(tr("Tag Name"));
    connect(tag_name_edit_, &QLineEdit::editingFinished,
            this, &TagView::verifyTag);

    add_button_ = new QPushButton(tr("Add"), this);
    add_button_->setEnabled(false);
    connect(add_button_, &QPushButton::clicked,
            this, &TagView::onAddTagClicked);

    tag_view_ = new QTableView(this);
    tag_view_->horizontalHeader()->setStretchLastSection(true);
    tag_view_->verticalHeader()->hide();
}

void TagView::initLayout()
{
    auto layout = new QVBoxLayout;

    auto add_layout = new QHBoxLayout;
    add_layout->addWidget(tag_name_edit_, 10);
    add_layout->addWidget(add_button_, -1);
    add_layout->setContentsMargins(0,0,0,0);

    layout->addLayout(add_layout);
    layout->addWidget(tag_view_);

    setLayout(layout);
}
