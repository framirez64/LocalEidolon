#include "trayapp.h"
#include "conversationwindow.h"
#include <QApplication>

TrayApp::TrayApp(QWidget *parent) : QWidget(parent), conversationWindow(nullptr) {
  trayIcon = new QSystemTrayIcon(QIcon(":/tlaloc.png"), this);
  trayMenu = new QMenu();

  askAction = new QAction("Open Conversation");
  toggleServerAction = new QAction("Toggle Server Mode");  // ✅ moved here
  quitAction = new QAction("Quit");

  trayMenu->addAction(askAction);
  trayMenu->addAction(toggleServerAction);  // ✅ moved here
  trayMenu->addAction(quitAction);

  connect(askAction, &QAction::triggered, this, &TrayApp::showConversationWindow);
  connect(toggleServerAction, &QAction::triggered, this, &TrayApp::toggleServerMode);  // ✅ moved here
  connect(quitAction, &QAction::triggered, qApp, &QApplication::quit);

  trayIcon->setContextMenu(trayMenu);
  trayIcon->setToolTip("Eidolon - LLM UI");
  trayIcon->show();

  server = new LLMServer(this);  // ✅ moved here
  connect(server, &LLMServer::requestReceived, this, [this](const QString &prompt) {
    showMessage("Request: " + prompt);
  });
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
void TrayApp::toggleServerMode() {
    if (server->isRunning()) {
        server->stop();
        showMessage("Server stopped.");
    } else {
        server->start();
        showMessage("Server started.");
    }
}
