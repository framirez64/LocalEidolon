#include "conversationwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextCursor>
#include <QCoreApplication>
#include <QRegularExpression> 
#include <QDir>
#include <QThread>
#include <QFont>

ConversationWindow::ConversationWindow(QWidget *parent)
    : QMainWindow(parent), llmProcess(nullptr) {
    setWindowTitle("Eidolon Chat");
    resize(800, 600);

    conversationView = new QTextEdit(this);
    conversationView->setReadOnly(true);

    inputField = new QLineEdit(this);
    sendButton = new QPushButton("Send", this);
    
    // Mode indicator
    modeLabel = new QLabel("Mode: CLI", this);
    QFont font = modeLabel->font();
    font.setBold(true);
    modeLabel->setFont(font);
    modeLabel->setStyleSheet("QLabel { color: #2196F3; padding: 5px; }");

    QWidget *central = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(central);
    QHBoxLayout *inputLayout = new QHBoxLayout();
    QHBoxLayout *topLayout = new QHBoxLayout();

    topLayout->addWidget(modeLabel);
    topLayout->addStretch();

    inputLayout->addWidget(inputField);
    inputLayout->addWidget(sendButton);

    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(conversationView);
    mainLayout->addLayout(inputLayout);

    setCentralWidget(central);

    connect(sendButton, &QPushButton::clicked, this, &ConversationWindow::sendPrompt);
    connect(inputField, &QLineEdit::returnPressed, this, &ConversationWindow::sendPrompt);
    
    updateModeDisplay();
}

void ConversationWindow::setServerMode(bool enabled) {
    isServerMode = enabled;
    updateModeDisplay();
}

void ConversationWindow::updateModeDisplay() {
    if (isServerMode) {
        modeLabel->setText("Mode: Server (http://localhost:8080)");
        modeLabel->setStyleSheet("QLabel { color: #4CAF50; padding: 5px; }");
        inputField->setPlaceholderText("Server mode - use web interface for chat");
        inputField->setEnabled(false);
        sendButton->setEnabled(false);
    } else {
        modeLabel->setText("Mode: CLI");
        modeLabel->setStyleSheet("QLabel { color: #2196F3; padding: 5px; }");
        inputField->setPlaceholderText("Type your message here...");
        inputField->setEnabled(true);
        sendButton->setEnabled(true);
    }
}

void ConversationWindow::sendPrompt() {
    if (isServerMode) {
        conversationView->append("<i>Server mode is active. Please use the web interface at http://localhost:8080</i>");
        return;
    }

    QString prompt = inputField->text().trimmed();
    if (prompt.isEmpty()) return;

    conversationView->append(QString("<b>You:</b> %1").arg(prompt));
    inputField->clear();
    conversationView->append("<b>Eidolon:</b> ");
    
    if (llmProcess) {
        llmProcess->deleteLater();
    }
    llmProcess = new QProcess(this);
    isLLMReplying = true; 

    QDir modelsDir(QCoreApplication::applicationDirPath() + "/Models");
    QStringList modelFiles = modelsDir.entryList(QStringList() << "*.gguf", QDir::Files);

    QString modelPath;
    if (!modelFiles.isEmpty()) {
        modelPath = modelsDir.absoluteFilePath(modelFiles.first());
    } else {
        conversationView->append("<i>No .gguf model found in the Models/ folder!</i>");
        return;
    }
    
    QString program = QCoreApplication::applicationDirPath() + "/llama-cli.exe";
    QStringList arguments;
    arguments << "-m" << modelPath
              << "-p" << prompt
              << "-n" << "300"
              << "--threads" << QString::number(QThread::idealThreadCount())
              << "--gpu-layers" << "29";

    llmProcess->setProgram(program);
    llmProcess->setArguments(arguments);

    connect(llmProcess, &QProcess::readyReadStandardOutput, this, &ConversationWindow::readLLMOutput);
    connect(llmProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            [this](int exitCode, QProcess::ExitStatus exitStatus) {
                isLLMReplying = false;
                conversationView->append("\n");
            });
    
    llmProcess->start();
}

void ConversationWindow::readLLMOutput() {
    QByteArray chunk = llmProcess->readAllStandardOutput();
    QString text = QString::fromUtf8(chunk).trimmed();

    // Remove system tokens only at the beginning
    QRegularExpression systemTokensRegex(
        R"(^\s*(user|assistant|<think>|</think>|>)+\s*)",
        QRegularExpression::CaseInsensitiveOption
    );
    text.replace(systemTokensRegex, "");

    // Append cleaned text
    conversationView->moveCursor(QTextCursor::End);
    conversationView->insertPlainText(text + " ");
}