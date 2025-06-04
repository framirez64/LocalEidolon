#include "conversationwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextCursor>
#include <QCoreApplication>
#include <QRegularExpression> 
#include <QDir>
#include <QThread>
#include <QFont>
#include <QScrollBar>

ConversationWindow::ConversationWindow(const LlamaSettings& settings, QWidget *parent)
    : QMainWindow(parent), llmProcess(nullptr), currentSettings(settings) {
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
    
    // Settings indicator
    settingsLabel = new QLabel(this);
    QFont smallFont = settingsLabel->font();
    smallFont.setPointSize(smallFont.pointSize() - 1);
    settingsLabel->setFont(smallFont);
    settingsLabel->setStyleSheet("QLabel { color: #666; }");

    QWidget *central = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(central);
    QHBoxLayout *inputLayout = new QHBoxLayout();
    QHBoxLayout *topLayout = new QHBoxLayout();
    QVBoxLayout *statusLayout = new QVBoxLayout();

    statusLayout->addWidget(modeLabel);
    statusLayout->addWidget(settingsLabel);
    statusLayout->addStretch();
    
    topLayout->addLayout(statusLayout);
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
    updateSettingsDisplay();
}

void ConversationWindow::updateSettings(const LlamaSettings& settings) {
    currentSettings = settings;
    updateSettingsDisplay();
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

void ConversationWindow::updateSettingsDisplay() {
    QString settingsText = QString("Tokens: %1 | Temp: %2 | Thinking: %3")
        .arg(currentSettings.maxTokens)
        .arg(currentSettings.temperature, 0, 'f', 2)
        .arg(currentSettings.thinkingMode ? "ON" : "OFF");
    
    if (!currentSettings.systemPrompt.isEmpty()) {
        settingsText += " | System Prompt: Active";
    }
    
    settingsLabel->setText(settingsText);
}

void ConversationWindow::sendPrompt() {
    if (isServerMode) {
        conversationView->append("<i>Server mode is active. Please use the web interface at http://localhost:8080</i>");
        return;
    }

    QString prompt = inputField->text().trimmed();
    if (prompt.isEmpty()) return;

    // Add system prompt if provided
    QString fullPrompt = prompt;
    if (!currentSettings.systemPrompt.isEmpty()) {
        fullPrompt = currentSettings.systemPrompt + "\n\n" + prompt;
    }

    conversationView->append(QString("<b>You:</b> %1").arg(prompt));
    inputField->clear();
    
    if (currentSettings.thinkingMode) {
        conversationView->append("<b>Eidolon (thinking):</b> <i>Processing...</i>");
    }
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
    QStringList arguments = buildCLIArguments(fullPrompt, modelPath);

    llmProcess->setProgram(program);
    llmProcess->setArguments(arguments);

    connect(llmProcess, &QProcess::readyReadStandardOutput, this, &ConversationWindow::readLLMOutput);
    connect(llmProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &ConversationWindow::onProcessFinished);
    
    qDebug() << "🚀 Starting llama-cli with arguments:" << arguments.join(" ");
    llmProcess->start();
}

QStringList ConversationWindow::buildCLIArguments(const QString& prompt, const QString& modelPath) {
    QStringList arguments;
    
    // Basic arguments
    arguments << "-m" << modelPath
              << "-p" << prompt;
    
    // Generation settings from our config
    arguments << "-n" << QString::number(currentSettings.maxTokens);
    arguments << "--temp" << QString::number(currentSettings.temperature, 'f', 2);
    arguments << "--top-k" << QString::number(currentSettings.topK);
    arguments << "--top-p" << QString::number(currentSettings.topP, 'f', 2);
    
    // Context size
    arguments << "--ctx-size" << QString::number(currentSettings.contextSize);
    
    // Performance settings
    int threads = currentSettings.threads;
    if (threads <= 0) {
        threads = QThread::idealThreadCount();
    }
    arguments << "--threads" << QString::number(threads);
    arguments << "--gpu-layers" << QString::number(currentSettings.gpuLayers);
    
    // Thinking mode support (some llama-cli versions support this)
    if (currentSettings.thinkingMode) {
        arguments << "--thinking";  // If supported
    }
    
    // Interactive mode settings
    if (currentSettings.streamResponse) {
        arguments << "--interactive-first";
    }
    
    return arguments;
}

void ConversationWindow::readLLMOutput() {
    if (!llmProcess) return;
    
    QByteArray chunk = llmProcess->readAllStandardOutput();
    QString text = QString::fromUtf8(chunk);

    // Handle thinking mode output differently
    if (currentSettings.thinkingMode) {
        // Look for thinking delimiters (this depends on your model's format)
        static QRegularExpression thinkingStart(R"(<think>|<thinking>)", QRegularExpression::CaseInsensitiveOption);
        static QRegularExpression thinkingEnd(R"(</think>|</thinking>)", QRegularExpression::CaseInsensitiveOption);
        
        if (text.contains(thinkingStart)) {
            conversationView->append("<i style='color: #888;'>[Thinking process]</i>");
        }
        if (text.contains(thinkingEnd)) {
            conversationView->append("<i style='color: #888;'>[End thinking]</i>");
        }
    }

    // Remove system tokens only at the beginning
    QRegularExpression systemTokensRegex(
        R"(^\s*(user|assistant|<think>|</think>|<thinking>|</thinking>|>)+\s*)",
        QRegularExpression::CaseInsensitiveOption
    );
    text.replace(systemTokensRegex, "");

    // Append cleaned text
    if (!text.isEmpty()) {
        conversationView->moveCursor(QTextCursor::End);
        conversationView->insertPlainText(text);
        
        // Auto-scroll to bottom
        QScrollBar *scrollBar = conversationView->verticalScrollBar();
        scrollBar->setValue(scrollBar->maximum());
    }
}

void ConversationWindow::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    isLLMReplying = false;
    conversationView->append("\n");
    
    if (exitStatus == QProcess::CrashExit) {
        conversationView->append("<i style='color: red;'>⚠️ Process crashed with exit code: " + QString::number(exitCode) + "</i>");
    } else if (exitCode != 0) {
        conversationView->append("<i style='color: orange;'>⚠️ Process finished with exit code: " + QString::number(exitCode) + "</i>");
    }
    
    // Clean up
    if (llmProcess) {
        llmProcess->deleteLater();
        llmProcess = nullptr;
    }
}