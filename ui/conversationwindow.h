#ifndef CONVERSATIONWINDOW_H
#define CONVERSATIONWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QProcess>
#include <QLabel>

class ConversationWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit ConversationWindow(QWidget *parent = nullptr);

public slots:
    void setServerMode(bool enabled);

private slots:
    void sendPrompt();
    void readLLMOutput();

private:
    QTextEdit *conversationView;
    QLineEdit *inputField;
    QPushButton *sendButton;
    QLabel *modeLabel;
    QProcess *llmProcess = nullptr;
    QString outputBuffer;
    bool isLLMReplying = false;
    bool isServerMode = false;
    
    void updateModeDisplay();
};

#endif // CONVERSATIONWINDOW_H