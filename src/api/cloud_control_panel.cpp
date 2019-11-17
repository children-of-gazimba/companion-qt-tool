#include "cloud_control_panel.h"

#include <QVBoxLayout>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>
#include <QHeaderView>
#include <QMessageBox>

#include "resources/lib.h"

CloudControlPanel::CloudControlPanel(QWidget *parent)
    : QWidget(parent)
    , name_edit_(new QLineEdit(this))
    , url_edit_(new QLineEdit(this))
    , token_edit_(new QLineEdit(this))
    , button_submit_(new QPushButton(tr("submit"), this))
    , button_delete_(new QPushButton(tr("delete"), this))
    , table_view_(new QTableView(this))
{
    initWidgets();
    initLayout();

    setMinimumSize(640,480);
    setWindowTitle("Cloud Control Panel");
    setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint);
}

void CloudControlPanel::populateApiConfigForEdit(const QModelIndex &index)
{
    QString name = Resources::Lib::API_CONFIG_MODEL->getName(index);
    QString url = "";
    QString token = "";
    if(name.size() > 0) {
        auto cfg = (*Resources::Lib::API_CONFIG_MODEL)[name];
        url = cfg.server_url.toString();
        token = cfg.access_token;
    }
    name_edit_->setText(name);
    url_edit_->setText(url);
    token_edit_->setText(token);
}

void CloudControlPanel::submitEdit()
{
    if(name_edit_->text().size() == 0 || url_edit_->text().size() == 0 || token_edit_->text().size() == 0)
        return;

    QUrl url(url_edit_->text());
    if(!url.isValid()) {
        QMessageBox::critical(nullptr, tr("Invalid Url"),
                              tr("The server url you have set is invalid."));
        return;
    }

    if(Resources::Lib::API_CONFIG_MODEL->hasApiConfig(name_edit_->text())) {
        auto res = QMessageBox::question(
            nullptr, tr("Update Configuration"),
            "You are about to update the configuration for an existing Connection "
            "'"+name_edit_->text()+"'. Do you wish to continue?"
        );
        if(res == QMessageBox::No)
            return;
    }

    ApiConfig cfg;
    cfg.server_url = url;
    cfg.access_token = token_edit_->text();

    Resources::Lib::API_CONFIG_MODEL->setApiConfig(name_edit_->text(), cfg);
    applyDeleteEnabled(name_edit_->text());
}

void CloudControlPanel::deleteEdit()
{
    auto res = QMessageBox::question(
        nullptr, tr("Delete Configuration"),
        "You are about to delete the configuration "
        "'"+name_edit_->text()+"'. Do you wish to continue?"
    );
    if(res == QMessageBox::No)
        return;
    Resources::Lib::API_CONFIG_MODEL->removeApiConfig(name_edit_->text());
    url_edit_->setText("");
    token_edit_->setText("");
    name_edit_->setText("");
}

void CloudControlPanel::applyDeleteEnabled(const QString &name)
{
    button_delete_->setEnabled(Resources::Lib::API_CONFIG_MODEL->hasApiConfig(name));
}

void CloudControlPanel::initWidgets()
{
    button_delete_->setEnabled(false);

    name_edit_->setPlaceholderText(tr("Name"));
    url_edit_->setPlaceholderText(tr("Url"));
    token_edit_->setPlaceholderText(tr("Access Token"));

    table_view_->setModel(Resources::Lib::API_CONFIG_MODEL);
    table_view_->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    table_view_->horizontalHeader()->setStretchLastSection(true);
    table_view_->setSelectionBehavior(QAbstractItemView::SelectRows);

    connect(table_view_, &QTableView::doubleClicked,
            this, &CloudControlPanel::populateApiConfigForEdit);
    connect(button_submit_, &QPushButton::clicked,
            this, &CloudControlPanel::submitEdit);
    connect(button_delete_, &QPushButton::clicked,
            this, &CloudControlPanel::deleteEdit);
    connect(name_edit_, &QLineEdit::textChanged,
            this, &CloudControlPanel::applyDeleteEnabled);
}

void CloudControlPanel::initLayout()
{
    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(table_view_);
    layout->addWidget(name_edit_);
    layout->addWidget(url_edit_);
    layout->addWidget(token_edit_);
    layout->addWidget(button_submit_);
    layout->addWidget(button_delete_);
    setLayout(layout);
}
