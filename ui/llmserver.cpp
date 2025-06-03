#include "llmserver.h"
#include <QCoreApplication>
#include <QDir>
#include <QThread>
#include <QDebug>

LLMServer::LLMServer(QObject* parent)
    : QObject(parent), serverProcess(nullptr) {}

LLMServer::~LLMServer() {
    stopServer();
}

void LLMServer::startServer() {
    if (serverProcess) {
        serverProcess->kill();
        serverProcess->waitForFinished();
        serverProcess->deleteLater();
    }

    serverProcess = new QProcess(this);

    QString exePath = QCoreApplication::applicationDirPath() + "/llama-server.exe";

    if (!QFile::exists(exePath)) {
        qWarning() << "❌ llama-server.exe not found!";
        return;
    }

    QDir modelsDir(QCoreApplication::applicationDirPath() + "/Models");
    QStringList modelFiles = modelsDir.entryList(QStringList() << "*.gguf", QDir::Files);

    QString modelPath;
    if (!modelFiles.isEmpty()) {
        modelPath = modelsDir.absoluteFilePath(modelFiles.first());
    } else {
        qWarning() << "❌ No .gguf model found in Models/";
        return;
    }

    QStringList args;
    args << "-m" << modelPath
         << "--port" << "8080"               // or 8000/9000 based on your client
         << "--ctx-size" << "4096"
         << "--threads" << QString::number(QThread::idealThreadCount())
         << "--gpu-layers" << "29"
         << "--verbose"; // Optional, useful for debugging

    serverProcess->setProgram(exePath);
    serverProcess->setArguments(args);

    connect(serverProcess, &QProcess::readyReadStandardOutput, this, [this]() {
        QByteArray output = serverProcess->readAllStandardOutput();
        qDebug() << QString::fromUtf8(output);
    });

    connect(serverProcess, &QProcess::readyReadStandardError, this, [this]() {
        QByteArray error = serverProcess->readAllStandardError();
        qDebug() << "[stderr] " << QString::fromUtf8(error);
    });

    serverProcess->start();
    qDebug() << "🚀 llama-server started.";
}

void LLMServer::stopServer() {
    if (serverProcess && serverProcess->state() != QProcess::NotRunning) {
        serverProcess->terminate();
        serverProcess->waitForFinished(3000);
        serverProcess->deleteLater();
        serverProcess = nullptr;
        qDebug() << "🛑 llama-server stopped.";
    }
}
