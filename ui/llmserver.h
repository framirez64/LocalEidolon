#ifndef LLMSERVER_H
#define LLMSERVER_H

#include <QObject>
#include <QProcess>
#include "llamasettings.h"

class LLMServer : public QObject {
    Q_OBJECT

public:
    explicit LLMServer(QObject* parent = nullptr);
    ~LLMServer();

    void startServer();
    void stopServer();
    bool isRunning() const;
    QString getServerUrl() const;
    void updateSettings(const LlamaSettings& settings);

signals:
    void serverStarted();
    void serverStopped();
    void serverError(const QString& error);

private slots:
    void onServerFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onServerError(QProcess::ProcessError error);

private:
    QProcess* serverProcess;
    LlamaSettings currentSettings;
    QString findModelPath();
    QStringList buildServerArguments();
};

#endif // LLMSERVER_H