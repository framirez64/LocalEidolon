#ifndef TRAYAPP_H
#define TRAYAPP_H

#include "llmserver.h"
#include "llamasettings.h"
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QWidget>
#include <QSettings>

class ConversationWindow;

class TrayApp : public QWidget {
    Q_OBJECT

public:
    TrayApp(QWidget *parent = nullptr);
    void showMessage(const QString &msg);
    const LlamaSettings& getCurrentSettings() const { return currentSettings; }

public slots:
    void toggleServerMode();

private slots:
    void showConversationWindow();
    void openWebInterface();
    void showSettingsDialog();
    void onServerStarted();
    void onServerStopped();
    void onServerError(const QString& error);

private:
    QSystemTrayIcon *trayIcon;
    QMenu *trayMenu;
    QAction *conversationAction;
    QAction *toggleServerAction;
    QAction *webInterfaceAction;
    QAction *settingsAction;
    QAction *quitAction;
    
    ConversationWindow *conversationWindow = nullptr;
    LLMServer *server = nullptr;
    LlamaSettings currentSettings;
    QSettings *appSettings;
    
    void updateMenus();
    void loadSettings();
    void saveSettings();
};

#endif // TRAYAPP_H