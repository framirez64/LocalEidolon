#ifndef LLMSERVER_H
#define LLMSERVER_H

#include <QObject>
#include <QProcess>

class LLMServer : public QObject {
    Q_OBJECT

public:
    explicit LLMServer(QObject* parent = nullptr);
    ~LLMServer();

    bool isRunning() const { return serverProcess && serverProcess->state() != QProcess::NotRunning; }

    void startServer();  // 🟢 Match the cpp file
    void stopServer();   // 🟢 Match the cpp file

signals:
    void requestReceived(const QString& prompt);

private:
    QProcess* serverProcess;
};

#endif // LLMSERVER_H
