#include "llmserver.h"
#include "App.h"
#include <iostream>

LLMServer::LLMServer(QObject *parent) : QObject(parent), running(false) {}

LLMServer::~LLMServer() {
    stop();
}

void LLMServer::start() {
    if (running) return;

    running = true;
    serverThread = std::thread([this]() {
        struct PerSocketData {};
        uWS::App().ws<PerSocketData>("/*", {
            .open = [](auto* ws) {
                std::cout << "Client connected\n";
            },
            .message = [this](auto* ws, std::string_view message, uWS::OpCode opCode) {
                QString prompt = QString::fromStdString(std::string(message));
                emit requestReceived(prompt);
                ws->send("Received: " + std::string(message), opCode);
            }
        }).listen(9001, [](auto* token) {
            if (token) {
                std::cout << "LLM server listening on port 9001\n";
            }
        }).run();
    });
}

void LLMServer::stop() {
    if (!running) return;
    running = false;
    if (serverThread.joinable()) {
        serverThread.detach(); // not ideal, but uWebSockets has no stop API
    }
}

bool LLMServer::isRunning() const {
    return running;
}
