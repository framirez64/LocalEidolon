#ifndef TRAYAPP_H
#define TRAYAPP_H

#include "llmserver.h"
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QWidget>

class ConversationWindow;

class TrayApp : public QWidget {
    Q_OBJECT

public:
    TrayApp(QWidget *parent = nullptr);
    void showMessage(const QString &msg);

public slots:
    void toggleServerMode();

private slots:
    void showConversationWindow();
    void openWebInterface();
    void onServerStarted();
    void onServerStopped();
    void onServerError(const QString& error);

private:
    QSystemTrayIcon *trayIcon;
    QMenu *trayMenu;
    QAction *conversationAction;
    QAction *toggleServerAction;
    QAction *webInterfaceAction;
    QAction *quitAction;
    
    ConversationWindow *conversationWindow = nullptr;
    LLMServer *server = nullptr;
    
    void updateMenus();
};

#endif // TRAYAPP_H