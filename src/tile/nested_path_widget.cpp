#include "nested_path_widget.h"

#include <QHBoxLayout>

#include "resources/lib.h"

NestedPathWidget::NestedPathWidget(QWidget *parent)
    : QWidget(parent)
    , back_button_(nullptr)
    , path_label_(nullptr)
{
    initWidgets();
    initLayout();
}

void NestedPathWidget::setPathText(const QStack<QGraphicsScene *> &scene_stack, const QMap<QGraphicsScene *, QString> scene_names)
{
    QString path = "";
    int i = 0;
    foreach(auto s, scene_stack) {
        if(i != 0)
            path += " > ";
        if(i == scene_stack.size() - 1)
            path += "<b>";
        path += scene_names[s];
        if(i == scene_stack.size() - 1)
            path += "</b>";
        ++i;
    }
    path_label_->setText(path);
    adjustSize();
}

const QString NestedPathWidget::getPathText() const
{
    return path_label_->text();
}

void NestedPathWidget::initWidgets()
{
    back_button_ = new QPushButton(this);
    back_button_->setFixedSize(50,50);
    back_button_->setMask(QRegion(back_button_->geometry(), QRegion::Ellipse));
    back_button_->setIcon(QIcon(*Resources::Lib::PX_BACK_BUTTON));
    QSize button_size(back_button_->geometry().size());
    button_size.setHeight(button_size.height()*0.66);
    button_size.setWidth(button_size.height());
    back_button_->setIconSize(button_size);
    back_button_->setStyleSheet("QPushButton {"
                                "   border: none; "
                                "   opacity: 0.8; "
                                "}"
                                ""
                                "QPushButton:pressed {"
                                "   background-color: rgba(50, 152, 253, 40);"
                                "}"
                                "");
    connect(back_button_, &QPushButton::clicked,
            this, &NestedPathWidget::backButtonClicked);

    path_label_ = new QLabel(this);
}

void NestedPathWidget::initLayout()
{
    QHBoxLayout* layout = new QHBoxLayout;
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(back_button_);
    layout->addWidget(path_label_);
    setLayout(layout);
}
