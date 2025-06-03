#ifndef LLMSERVER_H
#define LLMSERVER_H

#include <QObject>
#include <thread>
#include <atomic>

class LLMServer : public QObject {
    Q_OBJECT
public:
    explicit LLMServer(QObject *parent = nullptr);
    ~LLMServer();

    void start();
    void stop();
    bool isRunning() const;

signals:
    void requestReceived(const QString &prompt);

private:
    std::thread serverThread;
    std::atomic<bool> running;
};

#endif // LLMSERVER_H
