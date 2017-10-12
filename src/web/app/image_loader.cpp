#include "image_loader.h"

#include <QHBoxLayout>
#include <QFileDialog>

namespace Web {
namespace App {

ImageLoader::ImageLoader(QWidget *parent)
    : QWidget(parent)
    , line_edit_(0)
    , button_(0)
{
    initWidgets();
    initLayout();
}

const QString ImageLoader::loadImage()
{
    QString file_name = QFileDialog::getOpenFileName(this, tr("Open Image"), "",
                                                     "Images (*.jpg, *.png)");

    if(file_name.size() > 0) {
        line_edit_->setText(file_name);
        emit newImageLoaded(file_name);
        return file_name;
    }

    return QString();
}

const QString ImageLoader::getLastLoadedImage() const
{
    return line_edit_->text();
}

void ImageLoader::onButtonClicked()
{
    loadImage();
}

void ImageLoader::initWidgets()
{
    button_ = new QPushButton(tr("open"), this);
    line_edit_ = new QLineEdit(tr("No file opened yet"), this);
    line_edit_->setReadOnly(true);

    connect(button_, SIGNAL(clicked()),
            this, SLOT(onButtonClicked()));
}

void ImageLoader::initLayout()
{
    QHBoxLayout* layout = new QHBoxLayout;

    layout->addWidget(line_edit_, 10);
    layout->addWidget(button_, 0);

    setLayout(layout);
}

} // namespace App
} // namespace Web
