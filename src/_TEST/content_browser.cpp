#include "content_browser.h"

#include <QHBoxLayout>
#include <QFileDialog>

namespace _TEST {

ContentBrowser::ContentBrowser(QWidget* parent)
    : QWidget(parent)
    , browse_button_(0)
    , file_edit_(0)
    , last_url_()
{
    initWidgets();
    initLayout();
}

ContentBrowser::~ContentBrowser()
{}

const QUrl &ContentBrowser::getLastUrl() const
{
    return last_url_;
}

void ContentBrowser::browseClicked(bool)
{
    QUrl url = QFileDialog::getOpenFileUrl(
        this,
        "Open Audio",
        QUrl(),
        tr("Audio files (*.mp3 *.wav *.wma *.ogg)")
    );

    if(!url.isEmpty() && url.isValid()) {
        last_url_ = url;
        file_edit_->setText(last_url_.fileName());
        emit urlChanged(last_url_);
    }
}

void ContentBrowser::initWidgets()
{
    browse_button_ = new QPushButton("...", this);

    file_edit_ = new QLineEdit(this);
    file_edit_->setEnabled(false);

    connect(browse_button_, SIGNAL(clicked(bool)),
            this, SLOT(browseClicked(bool)));
}

void ContentBrowser::initLayout()
{
    QHBoxLayout* layout = new QHBoxLayout;

    layout->addWidget(browse_button_, 0);
    layout->addWidget(file_edit_, 2);

    setLayout(layout);
}

} // namespace _TEST
