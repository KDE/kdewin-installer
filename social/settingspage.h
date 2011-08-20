#ifndef SETTINGSPAGE_H
#define SETTINGSPAGE_H

#include <QWidget>

namespace Ui {
    class SettingsPage;
}

class SettingsPage : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsPage(QWidget *parent = 0);
    ~SettingsPage();
public slots:
    void setCurrentRoot(QString);
signals:
    void setRoot(QString);

private:
    Ui::SettingsPage *ui;
private slots:
    void browseButtonClicked();
    void saveButtonClicked();
};

#endif // SETTINGSPAGE_H
