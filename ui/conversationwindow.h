#ifndef CONVERSATIONWINDOW_H
#define CONVERSATIONWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QProcess>

class ConversationWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit ConversationWindow(QWidget *parent = nullptr);

private slots:
  void sendPrompt();
  void readLLMOutput();

private:
  QTextEdit *conversationView;
  QLineEdit *inputField;
  QPushButton *sendButton;
  QProcess *llmProcess = nullptr;
  QString outputBuffer;
  bool isLLMReplying = false;
};

#endif // CONVERSATIONWINDOW_H
