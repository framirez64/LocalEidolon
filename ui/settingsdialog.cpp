#include "settingsdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QTextEdit>
#include <QDialogButtonBox>
#include <QThread>
#include <QTabWidget>

SettingsDialog::SettingsDialog(const LlamaSettings& settings, QWidget *parent)
    : QDialog(parent) {
    setWindowTitle("Eidolon Settings");
    setModal(true);
    resize(500, 600);
    
    setupUI();
    loadSettings(settings);
}

void SettingsDialog::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // Create tab widget
    QTabWidget *tabWidget = new QTabWidget(this);
    
    // Generation Tab
    QWidget *generationTab = new QWidget();
    QVBoxLayout *genLayout = new QVBoxLayout(generationTab);
    
    // Generation Settings Group
    QGroupBox *genGroup = new QGroupBox("Generation Settings");
    QGridLayout *genGrid = new QGridLayout(genGroup);
    
    // Max Tokens
    genGrid->addWidget(new QLabel("Max Tokens:"), 0, 0);
    maxTokensSpinBox = new QSpinBox();
    maxTokensSpinBox->setRange(1, 8192);
    maxTokensSpinBox->setSingleStep(50);
    maxTokensSpinBox->setToolTip("Maximum number of tokens to generate");
    genGrid->addWidget(maxTokensSpinBox, 0, 1);
    
    // Thinking Mode
    thinkingModeCheckBox = new QCheckBox("Enable Thinking Mode");
    thinkingModeCheckBox->setToolTip("Allow the model to show its reasoning process");
    genGrid->addWidget(thinkingModeCheckBox, 1, 0, 1, 2);
    
    // Stream Response
    streamResponseCheckBox = new QCheckBox("Stream Response");
    streamResponseCheckBox->setToolTip("Show response as it's generated (real-time)");
    genGrid->addWidget(streamResponseCheckBox, 2, 0, 1, 2);
    
    // Temperature
    genGrid->addWidget(new QLabel("Temperature:"), 3, 0);
    temperatureSlider = new QSlider(Qt::Horizontal);
    temperatureSlider->setRange(1, 200); // 0.01 to 2.0
    temperatureSlider->setValue(70); // 0.7 default
    temperatureLabel = new QLabel("0.70");
    temperatureLabel->setMinimumWidth(40);
    
    QHBoxLayout *tempLayout = new QHBoxLayout();
    tempLayout->addWidget(temperatureSlider);
    tempLayout->addWidget(temperatureLabel);
    genGrid->addLayout(tempLayout, 3, 1);
    
    connect(temperatureSlider, &QSlider::valueChanged, 
            this, &SettingsDialog::onTemperatureSliderChanged);
    
    // Top-K
    genGrid->addWidget(new QLabel("Top-K:"), 4, 0);
    topKSpinBox = new QSpinBox();
    topKSpinBox->setRange(1, 100);
    topKSpinBox->setToolTip("Limit vocabulary to top K most likely tokens");
    genGrid->addWidget(topKSpinBox, 4, 1);
    
    // Top-P
    genGrid->addWidget(new QLabel("Top-P:"), 5, 0);
    topPSpinBox = new QSpinBox();
    topPSpinBox->setRange(1, 100);
    topPSpinBox->setSuffix("%");
    topPSpinBox->setToolTip("Nucleus sampling: consider tokens with cumulative probability up to P%");
    genGrid->addWidget(topPSpinBox, 5, 1);
    
    genLayout->addWidget(genGroup);
    
    // System Prompt Group
    QGroupBox *promptGroup = new QGroupBox("System Prompt");
    QVBoxLayout *promptLayout = new QVBoxLayout(promptGroup);
    
    systemPromptEdit = new QTextEdit();
    systemPromptEdit->setMaximumHeight(100);
    systemPromptEdit->setPlaceholderText("Enter system prompt (optional)...");
    promptLayout->addWidget(systemPromptEdit);
    
    genLayout->addWidget(promptGroup);
    genLayout->addStretch();
    
    tabWidget->addTab(generationTab, "Generation");
    
    // Model Tab
    QWidget *modelTab = new QWidget();
    QVBoxLayout *modelLayout = new QVBoxLayout(modelTab);
    
    QGroupBox *modelGroup = new QGroupBox("Model Settings");
    QGridLayout *modelGrid = new QGridLayout(modelGroup);
    
    // Context Size
    modelGrid->addWidget(new QLabel("Context Size:"), 0, 0);
    contextSizeSpinBox = new QSpinBox();
    contextSizeSpinBox->setRange(512, 32768);
    contextSizeSpinBox->setSingleStep(512);
    contextSizeSpinBox->setToolTip("Maximum context window size");
    modelGrid->addWidget(contextSizeSpinBox, 0, 1);
    
    // GPU Layers
    modelGrid->addWidget(new QLabel("GPU Layers:"), 1, 0);
    gpuLayersSpinBox = new QSpinBox();
    gpuLayersSpinBox->setRange(0, 100);
    gpuLayersSpinBox->setToolTip("Number of layers to offload to GPU (0 = CPU only)");
    modelGrid->addWidget(gpuLayersSpinBox, 1, 1);
    
    // Threads
    modelGrid->addWidget(new QLabel("CPU Threads:"), 2, 0);
    threadsComboBox = new QComboBox();
    threadsComboBox->addItem("Auto-detect", -1);
    for (int i = 1; i <= QThread::idealThreadCount() * 2; ++i) {
        threadsComboBox->addItem(QString::number(i), i);
    }
    threadsComboBox->setToolTip("Number of CPU threads to use");
    modelGrid->addWidget(threadsComboBox, 2, 1);
    
    modelLayout->addWidget(modelGroup);
    modelLayout->addStretch();
    
    tabWidget->addTab(modelTab, "Model");
    
    mainLayout->addWidget(tabWidget);
    
    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    
    resetButton = new QPushButton("Reset to Defaults");
    connect(resetButton, &QPushButton::clicked, this, &SettingsDialog::resetToDefaults);
    
    QDialogButtonBox *buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    
    buttonLayout->addWidget(resetButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(buttonBox);
    
    mainLayout->addLayout(buttonLayout);
}

void SettingsDialog::loadSettings(const LlamaSettings& settings) {
    maxTokensSpinBox->setValue(settings.maxTokens);
    thinkingModeCheckBox->setChecked(settings.thinkingMode);
    streamResponseCheckBox->setChecked(settings.streamResponse);
    temperatureSlider->setValue(static_cast<int>(settings.temperature * 100));
    topKSpinBox->setValue(settings.topK);
    topPSpinBox->setValue(static_cast<int>(settings.topP * 100));
    contextSizeSpinBox->setValue(settings.contextSize);
    gpuLayersSpinBox->setValue(settings.gpuLayers);
    systemPromptEdit->setPlainText(settings.systemPrompt);
    
    // Set threads combo box
    int threadIndex = threadsComboBox->findData(settings.threads);
    if (threadIndex >= 0) {
        threadsComboBox->setCurrentIndex(threadIndex);
    }
    
    onTemperatureSliderChanged(temperatureSlider->value());
}

LlamaSettings SettingsDialog::getSettings() const {
    LlamaSettings settings;
    settings.maxTokens = maxTokensSpinBox->value();
    settings.thinkingMode = thinkingModeCheckBox->isChecked();
    settings.streamResponse = streamResponseCheckBox->isChecked();
    settings.temperature = temperatureSlider->value() / 100.0;
    settings.topK = topKSpinBox->value();
    settings.topP = topPSpinBox->value() / 100.0;
    settings.contextSize = contextSizeSpinBox->value();
    settings.gpuLayers = gpuLayersSpinBox->value();
    settings.threads = threadsComboBox->currentData().toInt();
    settings.systemPrompt = systemPromptEdit->toPlainText();
    
    return settings;
}

void SettingsDialog::resetToDefaults() {
    LlamaSettings defaults;
    loadSettings(defaults);
}

void SettingsDialog::onTemperatureSliderChanged(int value) {
    double temp = value / 100.0;
    temperatureLabel->setText(QString::number(temp, 'f', 2));
}