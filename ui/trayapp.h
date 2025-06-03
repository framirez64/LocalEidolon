#ifndef TRAYAPP_H
#define TRAYAPP_H
#include "llmserver.h"
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QWidget>

class ConversationWindow;  ;
class TrayApp : public QWidget {
  Q_OBJECT

public:
  TrayApp(QWidget *parent = nullptr);
  void showMessage(const QString &msg);

signals:
  void sendPrompt(const QString &prompt);

private slots:
  void showConversationWindow();  
  void toggleServerMode();
private:
  QSystemTrayIcon *trayIcon;
  QMenu *trayMenu;
  QAction *askAction;
  QAction *quitAction;
  ConversationWindow *conversationWindow = nullptr;  // 🔧 track your conversation window
  LLMServer *server = nullptr;
  QAction *toggleServerAction;
};

#endif // TRAYAPP_H
