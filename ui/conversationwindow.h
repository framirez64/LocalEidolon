#ifndef CONVERSATIONWINDOW_H
#define CONVERSATIONWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QProcess>
#include <QLabel>
#include "llamasettings.h"

class ConversationWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit ConversationWindow(const LlamaSettings& settings, QWidget *parent = nullptr);

public slots:
    void setServerMode(bool enabled);
    void updateSettings(const LlamaSettings& settings);

private slots:
    void sendPrompt();
    void readLLMOutput();
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);

private:
    QTextEdit *conversationView;
    QLineEdit *inputField;
    QPushButton *sendButton;
    QLabel *modeLabel;
    QLabel *settingsLabel;
    QProcess *llmProcess = nullptr;
    QString outputBuffer;
    bool isLLMReplying = false;
    bool isServerMode = false;
    LlamaSettings currentSettings;
    
    void updateModeDisplay();
    void updateSettingsDisplay();
    QStringList buildCLIArguments(const QString& prompt, const QString& modelPath);
};

#endif // CONVERSATIONWINDOW_H