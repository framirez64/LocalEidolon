#include "llmserver.h"
#include "CivetServer.h"

#include <QDebug>
#include <QString>

#define DOCUMENT_ROOT "."
#define PORT "9001"

LLMServer::LLMServer(QObject* parent)
    : QObject(parent), server(nullptr), wsHandler(nullptr)
{
    // Do not start immediately—defer to start()
}

LLMServer::~LLMServer() {
    stop();
}

void LLMServer::start() {
    if (server) {
        qDebug() << "[LLMServer] Already running.";
        return;
    }

    const char* options[] = {
        "document_root", DOCUMENT_ROOT,
        "listening_ports", PORT,
        nullptr
    };

    try {
        server = new CivetServer(options);
        wsHandler = new WebSocketHandlerImpl(this);
        server->addWebSocketHandler("/ws", wsHandler);
        qDebug() << "[LLMServer] WebSocket server started on port" << PORT;
    } catch (const std::exception& e) {
        qCritical() << "[LLMServer] Failed to start server:" << e.what();
        stop();
    }
}

void LLMServer::stop() {
    if (wsHandler) {
        delete wsHandler;
        wsHandler = nullptr;
    }
    if (server) {
        delete server;
        server = nullptr;
        qDebug() << "[LLMServer] Server stopped.";
    }
}

bool LLMServer::isRunning() const {
    return server != nullptr;
}
