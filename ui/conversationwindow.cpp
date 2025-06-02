#include "conversationwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextCursor>
#include <QCoreApplication>
#include <QRegularExpression> 
ConversationWindow::ConversationWindow(QWidget *parent)
    : QMainWindow(parent), llmProcess(nullptr) {
  setWindowTitle("Eidolon Chat");

  conversationView = new QTextEdit(this);
  conversationView->setReadOnly(true);

  inputField = new QLineEdit(this);
  sendButton = new QPushButton("Send", this);

  QWidget *central = new QWidget(this);
  QVBoxLayout *mainLayout = new QVBoxLayout(central);
  QHBoxLayout *inputLayout = new QHBoxLayout();

  inputLayout->addWidget(inputField);
  inputLayout->addWidget(sendButton);

  mainLayout->addWidget(conversationView);
  mainLayout->addLayout(inputLayout);

  setCentralWidget(central);

  connect(sendButton, &QPushButton::clicked, this, &ConversationWindow::sendPrompt);
  connect(inputField, &QLineEdit::returnPressed, this, &ConversationWindow::sendPrompt);
}

void ConversationWindow::sendPrompt() {
  QString prompt = inputField->text().trimmed();
  QString llmModelPath = QCoreApplication::applicationDirPath() + "/Models/Qwen3-1.7B-Q8_0.gguf";
  if (prompt.isEmpty()) return;

  conversationView->append(QString("<b>You:</b> %1").arg(prompt));
  inputField->clear();
  conversationView->append("<b>Eidolon:</b> ");
  if (llmProcess) {
    llmProcess->deleteLater();
  }
  llmProcess = new QProcess(this);
  isLLMReplying = true; 
  QString program = QCoreApplication::applicationDirPath() + "/llama-cli.exe";;
  QStringList arguments;
  arguments << "-m" << llmModelPath
             << "-p" << prompt
             << "-n" << "300"
             << "--gpu-layers" << "29";

  llmProcess->setProgram(program);
  llmProcess->setArguments(arguments);

  connect(llmProcess, &QProcess::readyReadStandardOutput, this, &ConversationWindow::readLLMOutput);
  llmProcess->start();
}

void ConversationWindow::readLLMOutput() {
  QByteArray chunk = llmProcess->readAllStandardOutput();
  QString text = QString::fromUtf8(chunk).trimmed();

  // ✅ Remove system tokens only at the beginning (as a single regex)
  QRegularExpression systemTokensRegex(
    R"(^\s*(user|assistant|<think>|</think>|>)+\s*)",
    QRegularExpression::CaseInsensitiveOption
  );
  text.replace(systemTokensRegex, "");

  // Append cleaned text
  conversationView->moveCursor(QTextCursor::End);
  conversationView->insertPlainText(text + " ");  // add space for smoother streaming
}
