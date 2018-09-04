#ifndef TAG_ACTION_H
#define TAG_ACTION_H

#include <QWidgetAction>
#include <QCheckBox>

#include <db/table_records.h>


class TagAction : public QWidgetAction
{
    Q_OBJECT
public:
    explicit TagAction(DB::TagRecord* tag, QWidget *parent = nullptr);
    virtual ~TagAction();

    void setCheckState(Qt::CheckState state);
    DB::TagRecord const& getTag() const;

signals:
    void toggledTag(DB::TagRecord* tag, Qt::CheckState state);

public slots:


private:
    void init();

    DB::TagRecord *tag_;
    QCheckBox *check_box_;
};

#endif // TAG_ACTION_H
