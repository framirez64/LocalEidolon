#ifndef LLAMASETTINGS_H
#define LLAMASETTINGS_H

#include <QString>

struct LlamaSettings {
    int maxTokens = 300;
    bool thinkingMode = false;
    double temperature = 0.7;
    int topK = 40;
    double topP = 0.9;
    int contextSize = 4096;
    int gpuLayers = 29;
    int threads = -1; // -1 means auto-detect
    QString systemPrompt = "";
    bool streamResponse = true;
};

#endif // LLAMASETTINGS_H