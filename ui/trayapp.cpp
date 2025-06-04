#include "trayapp.h"
#include "conversationwindow.h"
#include <QApplication>
#include <QDesktopServices>
#include <QUrl>
#include <QMessageBox>

TrayApp::TrayApp(QWidget *parent) : QWidget(parent), conversationWindow(nullptr) {
    // Initialize server first
    server = new LLMServer(this);
    
    // Connect server signals
    connect(server, &LLMServer::serverStarted, this, &TrayApp::onServerStarted);
    connect(server, &LLMServer::serverStopped, this, &TrayApp::onServerStopped);
    connect(server, &LLMServer::serverError, this, &TrayApp::onServerError);

    // Create tray icon and menu
    trayIcon = new QSystemTrayIcon(QIcon(":/tlaloc.png"), this);
    trayMenu = new QMenu();

    conversationAction = new QAction("Open Chat Window");
    toggleServerAction = new QAction("Start Server Mode");
    webInterfaceAction = new QAction("Open Web Interface");
    quitAction = new QAction("Quit");

    // Add actions to menu
    trayMenu->addAction(conversationAction);
    trayMenu->addSeparator();
    trayMenu->addAction(toggleServerAction);
    trayMenu->addAction(webInterfaceAction);
    trayMenu->addSeparator();
    trayMenu->addAction(quitAction);

    // Connect actions
    connect(conversationAction, &QAction::triggered, this, &TrayApp::showConversationWindow);
    connect(toggleServerAction, &QAction::triggered, this, &TrayApp::toggleServerMode);
    connect(webInterfaceAction, &QAction::triggered, this, &TrayApp::openWebInterface);
    connect(quitAction, &QAction::triggered, qApp, &QApplication::quit);

    trayIcon->setContextMenu(trayMenu);
    trayIcon->setToolTip("Eidolon - LLM UI (CLI Mode)");
    trayIcon->show();

    updateMenus();
}

void TrayApp::showMessage(const QString &msg) {
    trayIcon->showMessage("Eidolon", msg, QSystemTrayIcon::Information, 3000);
}

void TrayApp::showConversationWindow() {
    if (!conversationWindow) {
        conversationWindow = new ConversationWindow();
        conversationWindow->setServerMode(server->isRunning());
    }
    conversationWindow->show();
    conversationWindow->raise();
    conversationWindow->activateWindow();
}

void TrayApp::toggleServerMode() {
    if (server->isRunning()) {
        server->stopServer();
    } else {
        server->startServer();
    }
}

void TrayApp::openWebInterface() {
    if (server->isRunning()) {
        QDesktopServices::openUrl(QUrl(server->getServerUrl()));
    } else {
        showMessage("Server is not running. Start server mode first.");
    }
}

void TrayApp::onServerStarted() {
    showMessage("Server started at " + server->getServerUrl());
    if (conversationWindow) {
        conversationWindow->setServerMode(true);
    }
    updateMenus();
}

void TrayApp::onServerStopped() {
    showMessage("Server stopped. CLI mode active.");
    if (conversationWindow) {
        conversationWindow->setServerMode(false);
    }
    updateMenus();
}

void TrayApp::onServerError(const QString& error) {
    showMessage("Server Error: " + error);
    QMessageBox::warning(nullptr, "Eidolon - Server Error", 
                        "Failed to start server:\n" + error + 
                        "\n\nCheck that llama-server.exe and model files are present.");
}

void TrayApp::updateMenus() {
    bool serverRunning = server->isRunning();
    
    if (serverRunning) {
        toggleServerAction->setText("Stop Server Mode");
        trayIcon->setToolTip("Eidolon - LLM UI (Server Mode: " + server->getServerUrl() + ")");
        webInterfaceAction->setEnabled(true);
    } else {
        toggleServerAction->setText("Start Server Mode");
        trayIcon->setToolTip("Eidolon - LLM UI (CLI Mode)");
        webInterfaceAction->setEnabled(false);
    }
}