#include "interactive_image_shape_widget.h"

#include <QGroupBox>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QDebug>

#include "resources/lib.h"

InteractiveImageShapeWidget::InteractiveImageShapeWidget(QWidget *parent)
    : QWidget(parent)
    , shape_(0)
    , title_label_(0)
    , name_label_(0)
    , name_edit_(0)
    , tracker_label_(0)
    , tracker_picker_(0)
    , delete_button_(0)
    , save_button_(0)
    , collapse_button_(0)
    , link_select_(0)
    , grab_select_(0)
    , collapsible_widgets_()
{
    initWidgets();
    initLayout();
    hideCollapsibleWidgets();
}

InteractiveImageShapeWidget::InteractiveImageShapeWidget(InteractiveImageShape *shape, QWidget *parent)
    : QWidget(parent)
    , shape_(shape)
    , title_label_(0)
    , name_label_(0)
    , name_edit_(0)
    , tracker_label_(0)
    , tracker_picker_(0)
    , delete_button_(0)
    , save_button_(0)
    , collapse_button_(0)
    , link_select_(0)
    , grab_select_(0)
    , collapsible_widgets_()
{
    initWidgets();
    initLayout();
    updateUI();
    hideCollapsibleWidgets();
    connect(shape_, &InteractiveImageShape::destroyed,
            this, &InteractiveImageShapeWidget::deleteLater);
}

InteractiveImageShapeWidget::~InteractiveImageShapeWidget()
{}

void InteractiveImageShapeWidget::setShape(InteractiveImageShape *shape)
{
    if(shape_) {
        disconnect(shape_, &InteractiveImageShape::destroyed,
                   this, &InteractiveImageShapeWidget::deleteLater);
    }
    shape_ = shape;
    connect(shape_, &InteractiveImageShape::destroyed,
            this, &InteractiveImageShapeWidget::deleteLater);
    updateUI();
}

InteractiveImageShape *InteractiveImageShapeWidget::getShape() const
{
    return shape_;
}

void InteractiveImageShapeWidget::onSave()
{
    /*if(shape_->getName().compare(name_edit_->text()) != 0)
        shape_->setName(name_edit_->text());

    shape_->setTrackableName(tracker_picker_->currentText());
    if(shape_->getTrackableName().size() == 0) {
        shape_->removeManipulate();
    }
    else {
        foreach(auto tracker, Resources::Lib::TRACKER_MODEL->getTrackers()) {
            if(shape_->getTrackableName().compare(tracker->getName()) == 0) {
                if(link_select_->isChecked()) {
                    qDebug().nospace() << Q_FUNC_INFO << " @ line " << __LINE__;
                    qDebug() << "  > " << "linking";
                    tracker->link(shape_, Tracker::REL_POSITION);
                    tracker->link(shape_, Tracker::ROTATION);
                }
                else if(grab_select_->isChecked()) {
                    qDebug().nospace() << Q_FUNC_INFO << " @ line " << __LINE__;
                    qDebug() << "  > " << "grabbing";
                    tracker->grab(shape_, Tracker::REL_POSITION);
                    tracker->grab(shape_, Tracker::ROTATION);
                }
            }
        }
    }
    */

    title_label_->setText(name_edit_->text());
    save_button_->setEnabled(false);
    updateUI();
}

void InteractiveImageShapeWidget::onDelete()
{
    QMessageBox b;
    b.setText(tr("You are about to delete a shape named '") + title_label_->text() + tr("'."));
    b.setInformativeText(tr("Do you wish to proceed?"));
    b.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    b.setDefaultButton(QMessageBox::No);
    b.setWindowTitle(tr("Delete Shape"));
    if(b.exec() == QMessageBox::Yes) {
        shape_->deleteLater();
    }
}

void InteractiveImageShapeWidget::onCollapseTriggered()
{
    if(collapse_button_->text().compare("show more") == 0) {
        collapse_button_->setText(tr("show less"));
        showCollapsibleWidgets();
    }
    else {
        collapse_button_->setText(tr("show more"));
        hideCollapsibleWidgets();
    }
}

void InteractiveImageShapeWidget::hideCollapsibleWidgets()
{
    foreach(auto w, collapsible_widgets_)
        w->hide();
}

void InteractiveImageShapeWidget::showCollapsibleWidgets()
{
    foreach(auto w, collapsible_widgets_)
        w->show();
}

void InteractiveImageShapeWidget::contentsModifiedEvent()
{
    bool tracking_enabled = tracker_picker_->currentText().size() > 0;
    link_select_->setEnabled(tracking_enabled);
    grab_select_->setEnabled(tracking_enabled);
    if(save_button_->isEnabled())
        return;
    save_button_->setEnabled(true);
    title_label_->setText(title_label_->text()+"*");
}

void InteractiveImageShapeWidget::updateUI()
{
    if(shape_) {
        /*if(shape_->getName().size() > 0)
            title_label_->setText(shape_->getName());
        else
            title_label_->setText(tr("UNNAMED TOKEN"));
        name_edit_->setText(shape_->getName());
        tracker_picker_->setCurrentTracker(shape_->getTrackableName());*/
    }
    else {
        title_label_->setText(tr("UNNAMED TOKEN"));
        name_edit_->setText("");
        tracker_picker_->setCurrentIndex(0);
    }
}

void InteractiveImageShapeWidget::initWidgets()
{
    title_label_ = new QLabel(tr("Unnamed Shape"), this);
    QFont f(title_label_->font());
    f.setPointSize(f.pointSize()*1.5);
    title_label_->setFont(f);

    name_label_ = new QLabel(tr("Name"), this);
    name_edit_ = new QLineEdit(this);
    name_edit_->setPlaceholderText(tr("<name here>"));
    connect(name_edit_, &QLineEdit::textChanged,
            this, [=](){contentsModifiedEvent();});

    tracker_label_ = new QLabel(tr("Tracking Source"), this);

    tracker_picker_ = new TrackerPicker(this);
    connect(tracker_picker_, &TrackerPicker::currentTextChanged,
            this, [=](const QString&){contentsModifiedEvent();});

    delete_button_ = new QPushButton(tr("delete"), this);
    connect(delete_button_, &QPushButton::clicked,
            this, &InteractiveImageShapeWidget::onDelete);

    save_button_ = new QPushButton(tr("save"), this);
    save_button_->setEnabled(false);
    connect(save_button_, &QPushButton::clicked,
            this, &InteractiveImageShapeWidget::onSave);

    collapse_button_ = new QPushButton(tr("show more"), this);
    connect(collapse_button_, &QPushButton::clicked,
            this, &InteractiveImageShapeWidget::onCollapseTriggered);

    link_select_ = new QRadioButton(tr("link"), this);
    connect(link_select_, &QRadioButton::clicked,
            this, [=](){contentsModifiedEvent();});
    link_select_->setEnabled(false);

    grab_select_ = new QRadioButton(tr("grab"), this);
    connect(grab_select_, &QRadioButton::clicked,
            this, [=](){contentsModifiedEvent();});
    grab_select_->setEnabled(false);

    collapsible_widgets_.append(name_label_);
    collapsible_widgets_.append(name_edit_);
    collapsible_widgets_.append(tracker_picker_);
    collapsible_widgets_.append(tracker_label_);
    collapsible_widgets_.append(link_select_);
    collapsible_widgets_.append(grab_select_);
}

void InteractiveImageShapeWidget::initLayout()
{
    QVBoxLayout* layout = new QVBoxLayout;
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);

    QVBoxLayout* group_layout = new QVBoxLayout;
    QGroupBox* group = new QGroupBox(this);
    group->setLayout(group_layout);
    layout->addWidget(group,10);
    layout->addWidget(collapse_button_,-1);

    setLayout(layout);
    /*setMinimumHeight(120);
    setMaximumHeight(minimumHeight());*/
    QSizePolicy p(sizePolicy());
    p.setVerticalPolicy(QSizePolicy::Fixed);
    setSizePolicy(p);

    QHBoxLayout* header_layout = new QHBoxLayout;
    header_layout->addWidget(title_label_);
    header_layout->addStretch(4);
    header_layout->addWidget(save_button_, -1);
    header_layout->addWidget(delete_button_, -1);

    QHBoxLayout* name_layout = new QHBoxLayout;
    name_layout->addWidget(name_label_,1);
    name_layout->addWidget(name_edit_,3);

    QHBoxLayout* tracking_src_layout = new QHBoxLayout;
    tracking_src_layout->addWidget(tracker_label_,1);
    tracking_src_layout->addWidget(tracker_picker_,3);

    QGroupBox* tracking_box = new QGroupBox(tr("Tracking"), this);
    collapsible_widgets_.append(tracking_box);

    QVBoxLayout* tracking_box_layout = new QVBoxLayout;
    tracking_box->setLayout(tracking_box_layout);
    QHBoxLayout* tracking_mani_layout = new QHBoxLayout;
    tracking_mani_layout->addWidget(link_select_);
    tracking_mani_layout->addWidget(grab_select_);
    link_select_->setChecked(true);

    tracking_box_layout->addLayout(tracking_src_layout);
    tracking_box_layout->addLayout(tracking_mani_layout);

    group_layout->addLayout(header_layout, -1);
    group_layout->addLayout(name_layout);
    group_layout->addWidget(tracking_box);
}
