#ifndef SETTINGSPAGE_H
#define SETTINGSPAGE_H

#include <QDialog>

#include "ui_settingspage.h"
#include "globalconfig.h"
#include "settings.h"

class SettingsPage;

class SettingsSubPage : public QObject
{
    Q_OBJECT 

public:
    SettingsSubPage(Ui::SettingsDialog &_ui) 
        : s(Settings::getInstance()), ui(_ui) {}
    virtual QWidget *widget() = 0;
    virtual void reset() = 0;
    virtual void accept() = 0;
    virtual void reject() = 0;
    virtual bool isComplete() = 0;
protected:
    Settings &s;
    Ui::SettingsDialog &ui;
signals:
    void completeStateChanged();
};

class SettingsDownloadPage : public SettingsSubPage
{
    Q_OBJECT 

public: 
    SettingsDownloadPage(Ui::SettingsDialog &ui)
        : SettingsSubPage(ui) {}
    virtual QWidget *widget();
    virtual void reset();
    virtual void accept();
    virtual void reject();
    virtual bool isComplete();

protected:
};

class SettingsMirrorPage : public SettingsSubPage
{
    Q_OBJECT 

public: 
    SettingsMirrorPage(Ui::SettingsDialog &ui)
        : SettingsSubPage(ui) {}
    virtual QWidget *widget();
    virtual void reset();
    virtual void accept();
    virtual void reject();
    virtual bool isComplete();
private slots:
    void mirrorChanged(int mirrorNumber);
};

class SettingsInstallPage : public SettingsSubPage
{
public: 
    SettingsInstallPage(Ui::SettingsDialog &ui) 
        : SettingsSubPage(ui) {}
    virtual QWidget *widget();
    virtual void reset();
    virtual void accept();
    virtual void reject();
    virtual bool isComplete();
};

class SettingsProxyPage : public SettingsSubPage
{
public: 
    SettingsProxyPage(Ui::SettingsDialog &ui) 
        : SettingsSubPage(ui) {}
    virtual QWidget *widget();
    virtual void reset();
    virtual void accept();
    virtual void reject();
    virtual bool isComplete();
};

class SettingsPage : public QDialog
{
    Q_OBJECT

public:
    SettingsPage(QWidget *parent = 0);
    void init();
    QT_DEPRECATED void setGlobalConfig(GlobalConfig *globalConfig) 
    { 
    }
    SettingsDownloadPage *downloadPage() { return &m_downloadPage; }
    SettingsInstallPage *installPage() { return &m_installPage; }
    SettingsProxyPage *proxyPage() {return &m_proxyPage; }
    SettingsMirrorPage *mirrorPage() {return &m_mirrorPage; }

protected:
    SettingsDownloadPage m_downloadPage;
    SettingsInstallPage m_installPage;
    SettingsProxyPage m_proxyPage;
    SettingsMirrorPage m_mirrorPage;

private slots: 
    void accept();
    void reject();
    void rootPathSelectClicked();
    void tempPathSelectClicked();
    void addNewMirrorClicked();
    void switchProxyFields(bool checked);
	void rootPathEditChanged();
     
private:
    Ui::SettingsDialog ui;
    Settings &s;
};
 

#endif
