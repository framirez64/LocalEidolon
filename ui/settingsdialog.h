#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QSpinBox>
#include <QCheckBox>
#include <QSlider>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QTabWidget>
#include <QTextEdit>
#include "llamasettings.h"

class SettingsDialog : public QDialog {
    Q_OBJECT

public:
    explicit SettingsDialog(const LlamaSettings& settings, QWidget *parent = nullptr);
    LlamaSettings getSettings() const;

private slots:
    void resetToDefaults();
    void onTemperatureSliderChanged(int value);

private:
    // Generation settings
    QSpinBox *maxTokensSpinBox;
    QCheckBox *thinkingModeCheckBox;
    QSlider *temperatureSlider;
    QLabel *temperatureLabel;
    QSpinBox *topKSpinBox;
    QSpinBox *topPSpinBox;
    QCheckBox *streamResponseCheckBox;
    
    // Model settings
    QSpinBox *contextSizeSpinBox;
    QSpinBox *gpuLayersSpinBox;
    QComboBox *threadsComboBox;
    
    // System prompt
    QTextEdit *systemPromptEdit;
    
    QPushButton *resetButton;
    QPushButton *okButton;
    QPushButton *cancelButton;
    
    void setupUI();
    void loadSettings(const LlamaSettings& settings);
};

#endif // SETTINGSDIALOG_H