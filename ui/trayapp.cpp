#include "trayapp.h"
#include "conversationwindow.h"
#include "settingsdialog.h"
#include <QApplication>
#include <QDesktopServices>
#include <QUrl>
#include <QMessageBox>
#include <QStandardPaths>

TrayApp::TrayApp(QWidget *parent) : QWidget(parent), conversationWindow(nullptr) {
    // Initialize settings
    appSettings = new QSettings("Eidolon", "LLMApp", this);
    loadSettings();
    
    // Initialize server with current settings
    server = new LLMServer(this);
    server->updateSettings(currentSettings);
    
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
    settingsAction = new QAction("Settings...");
    quitAction = new QAction("Quit");

    // Add actions to menu
    trayMenu->addAction(conversationAction);
    trayMenu->addSeparator();
    trayMenu->addAction(toggleServerAction);
    trayMenu->addAction(webInterfaceAction);
    trayMenu->addSeparator();
    trayMenu->addAction(settingsAction);
    trayMenu->addSeparator();
    trayMenu->addAction(quitAction);

    // Connect actions
    connect(conversationAction, &QAction::triggered, this, &TrayApp::showConversationWindow);
    connect(toggleServerAction, &QAction::triggered, this, &TrayApp::toggleServerMode);
    connect(webInterfaceAction, &QAction::triggered, this, &TrayApp::openWebInterface);
    connect(settingsAction, &QAction::triggered, this, &TrayApp::showSettingsDialog);
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
        conversationWindow = new ConversationWindow(currentSettings);
        conversationWindow->setServerMode(server->isRunning());
    }
    conversationWindow->show();
    conversationWindow->raise();
    conversationWindow->activateWindow();
}

void TrayApp::showSettingsDialog() {
    SettingsDialog dialog(currentSettings, this);
    if (dialog.exec() == QDialog::Accepted) {
        currentSettings = dialog.getSettings();
        saveSettings();
        
        // Update server with new settings
        server->updateSettings(currentSettings);
        
        // Update conversation window if open
        if (conversationWindow) {
            conversationWindow->updateSettings(currentSettings);
        }
        
        showMessage("Settings updated successfully!");
        
        // If server is running, suggest restart for some settings
        if (server->isRunning()) {
            QMessageBox::information(this, "Settings Updated", 
                "Some settings (like context size and GPU layers) will take effect "
                "when you restart the server.");
        }
    }
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

void TrayApp::loadSettings() {
    // Load generation settings
    currentSettings.maxTokens = appSettings->value("generation/maxTokens", 300).toInt();
    currentSettings.thinkingMode = appSettings->value("generation/thinkingMode", false).toBool();
    currentSettings.temperature = appSettings->value("generation/temperature", 0.7).toDouble();
    currentSettings.topK = appSettings->value("generation/topK", 40).toInt();
    currentSettings.topP = appSettings->value("generation/topP", 0.9).toDouble();
    currentSettings.streamResponse = appSettings->value("generation/streamResponse", true).toBool();
    currentSettings.systemPrompt = appSettings->value("generation/systemPrompt", "").toString();
    
    // Load model settings
    currentSettings.contextSize = appSettings->value("model/contextSize", 4096).toInt();
    currentSettings.gpuLayers = appSettings->value("model/gpuLayers", 29).toInt();
    currentSettings.threads = appSettings->value("model/threads", -1).toInt();
}

void TrayApp::saveSettings() {
    // Save generation settings
    appSettings->setValue("generation/maxTokens", currentSettings.maxTokens);
    appSettings->setValue("generation/thinkingMode", currentSettings.thinkingMode);
    appSettings->setValue("generation/temperature", currentSettings.temperature);
    appSettings->setValue("generation/topK", currentSettings.topK);
    appSettings->setValue("generation/topP", currentSettings.topP);
    appSettings->setValue("generation/streamResponse", currentSettings.streamResponse);
    appSettings->setValue("generation/systemPrompt", currentSettings.systemPrompt);
    
    // Save model settings
    appSettings->setValue("model/contextSize", currentSettings.contextSize);
    appSettings->setValue("model/gpuLayers", currentSettings.gpuLayers);
    appSettings->setValue("model/threads", currentSettings.threads);
    
    appSettings->sync();
}