#ifndef MISC_CHAR_INPUT_DIALOG_H
#define MISC_CHAR_INPUT_DIALOG_H

#include <QDialog>
#include <QMouseEvent>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

namespace Misc {

class CharInputDialog : public QDialog
{
    Q_OBJECT
public:
    explicit CharInputDialog(QWidget *parent = 0);

    void setDialogMessage(const QString&);
    const QString getDialogMessage() const;

    const QChar& getChar() const;
    void setChar(const QChar&);

signals:

public slots:

private slots:
    void onOk();

private:
    void keyPressEvent(QKeyEvent* e);
    void initWidgets();
    void initLayout();

    QChar selected_char_;
    QLabel* label_;
    QLineEdit* edit_;
    QPushButton* ok_;
    QPushButton* cancel_;
};

} // namespace Misc

#endif // MISC_CHAR_INPUT_DIALOG_H
