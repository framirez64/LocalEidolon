#ifndef LLMSERVER_H
#define LLMSERVER_H

#include <QObject>
#include <QString>
#include <CivetServer.h>
#include <QDebug>

class LLMServer : public QObject {
    Q_OBJECT

public:
    explicit LLMServer(QObject* parent = nullptr);
    ~LLMServer();

    bool isRunning() const;
    void start();
    void stop();

signals:
    void requestReceived(const QString& prompt);

private:
    CivetServer* server = nullptr;

    class WebSocketHandlerImpl : public CivetWebSocketHandler {
    public:
        explicit WebSocketHandlerImpl(LLMServer* parent) : parent(parent) {}

        bool handleConnection(CivetServer*, const struct mg_connection*) override {
            qDebug() << "[WebSocket] Client connected.";
            return true;
        }

        void handleReadyState(CivetServer*, struct mg_connection*) override {
            qDebug() << "[WebSocket] Ready to send/receive messages.";
        }

        bool handleData(CivetServer*, struct mg_connection* conn, int, char* data, size_t dataLen) override {
            QString message = QString::fromUtf8(data, int(dataLen));
            qDebug() << "[WebSocket] Received message:" << message;

            emit parent->requestReceived(message);

            std::string reply = "Echo: " + message.toStdString();
            mg_websocket_write(conn, MG_WEBSOCKET_OPCODE_TEXT, reply.c_str(), reply.length());

            return true;
        }

        void handleClose(CivetServer*, const struct mg_connection*) override {
            qDebug() << "[WebSocket] Client disconnected.";
        }

    private:
        LLMServer* parent;
    };

    WebSocketHandlerImpl* wsHandler = nullptr;
};

#endif // LLMSERVER_H
