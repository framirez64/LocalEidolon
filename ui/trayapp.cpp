#include "trayapp.h"
#include "conversationwindow.h"
#include <QApplication>

TrayApp::TrayApp(QWidget *parent) : QWidget(parent), conversationWindow(nullptr) {
  trayIcon = new QSystemTrayIcon(QIcon(":/tlaloc.png"), this);
  trayMenu = new QMenu();

  askAction = new QAction("Open Conversation");
  quitAction = new QAction("Quit");

  trayMenu->addAction(askAction);
  trayMenu->addAction(quitAction);

  connect(askAction, &QAction::triggered, this, &TrayApp::showConversationWindow);
  connect(quitAction, &QAction::triggered, qApp, &QApplication::quit);

  trayIcon->setContextMenu(trayMenu);
  trayIcon->setToolTip("Eidolon - LLM UI");
  trayIcon->show();
}

void TrayApp::showMessage(const QString &msg) {
  trayIcon->showMessage("Eidolon", msg, QSystemTrayIcon::Information, 3000);
}

void TrayApp::showConversationWindow() {
  if (!conversationWindow) {
    conversationWindow = new ConversationWindow();
  }
  conversationWindow->show();
  conversationWindow->raise();
  conversationWindow->activateWindow();
}
