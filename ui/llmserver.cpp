#include "llmserver.h"
#include <QCoreApplication>
#include <QDir>
#include <QThread>
#include <QDebug>
#include <QStandardPaths>

LLMServer::LLMServer(QObject* parent)
    : QObject(parent), serverProcess(nullptr) {}

LLMServer::~LLMServer() {
    stopServer();
}

void LLMServer::startServer() {
    if (isRunning()) {
        qDebug() << "Server already running";
        return;
    }

    if (serverProcess) {
        serverProcess->deleteLater();
    }

    serverProcess = new QProcess(this);

    QString exePath = QCoreApplication::applicationDirPath() + "/llama-server.exe";

    if (!QFile::exists(exePath)) {
        QString error = "llama-server.exe not found at: " + exePath;
        qWarning() << "❌" << error;
        emit serverError(error);
        return;
    }

    QString modelPath = findModelPath();
    if (modelPath.isEmpty()) {
        QString error = "No .gguf model found in Models/ folder";
        qWarning() << "❌" << error;
        emit serverError(error);
        return;
    }

    QStringList args;
    args << "-m" << modelPath
         << "--port" << "8080"
         << "--host" << "0.0.0.0"  // Allow connections from localhost
         << "--ctx-size" << "4096"
         << "--threads" << QString::number(QThread::idealThreadCount())
         << "--gpu-layers" << "29"
         << "--verbose";

    serverProcess->setProgram(exePath);
    serverProcess->setArguments(args);

    // Connect signals
    connect(serverProcess, &QProcess::readyReadStandardOutput, this, [this]() {
        QByteArray output = serverProcess->readAllStandardOutput();
        qDebug() << "[stdout]" << QString::fromUtf8(output);
    });

    connect(serverProcess, &QProcess::readyReadStandardError, this, [this]() {
        QByteArray error = serverProcess->readAllStandardError();
        qDebug() << "[stderr]" << QString::fromUtf8(error);
    });

    connect(serverProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &LLMServer::onServerFinished);

    connect(serverProcess, &QProcess::errorOccurred,
            this, &LLMServer::onServerError);

    qDebug() << "🚀 Starting llama-server with model:" << modelPath;
    serverProcess->start();

    if (serverProcess->waitForStarted(5000)) {
        qDebug() << "✅ llama-server started successfully";
        emit serverStarted();
    } else {
        qWarning() << "❌ Failed to start llama-server";
        emit serverError("Failed to start server process");
    }
}

void LLMServer::stopServer() {
    if (!serverProcess || serverProcess->state() == QProcess::NotRunning) {
        return;
    }

    qDebug() << "🛑 Stopping llama-server...";
    serverProcess->terminate();
    
    if (!serverProcess->waitForFinished(5000)) {
        qWarning() << "Force killing llama-server";
        serverProcess->kill();
        serverProcess->waitForFinished(2000);
    }

    serverProcess->deleteLater();
    serverProcess = nullptr;
    qDebug() << "✅ llama-server stopped";
    emit serverStopped();
}

bool LLMServer::isRunning() const {
    return serverProcess && serverProcess->state() == QProcess::Running;
}

QString LLMServer::getServerUrl() const {
    return "http://localhost:8080";
}

QString LLMServer::findModelPath() {
    QDir modelsDir(QCoreApplication::applicationDirPath() + "/Models");
    QStringList modelFiles = modelsDir.entryList(QStringList() << "*.gguf", QDir::Files);

    if (!modelFiles.isEmpty()) {
        return modelsDir.absoluteFilePath(modelFiles.first());
    }
    return QString();
}

void LLMServer::onServerFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    qDebug() << "Server finished with exit code:" << exitCode;
    if (exitStatus == QProcess::CrashExit) {
        emit serverError("Server crashed with exit code: " + QString::number(exitCode));
    }
    emit serverStopped();
}

void LLMServer::onServerError(QProcess::ProcessError error) {
    QString errorString;
    switch (error) {
        case QProcess::FailedToStart:
            errorString = "Failed to start server";
            break;
        case QProcess::Crashed:
            errorString = "Server crashed";
            break;
        case QProcess::Timedout:
            errorString = "Server timed out";
            break;
        default:
            errorString = "Unknown server error";
    }
    qWarning() << "❌ Server error:" << errorString;
    emit serverError(errorString);
}