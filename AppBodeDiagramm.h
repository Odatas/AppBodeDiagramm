#pragma once

#include <QtWidgets/QMainWindow>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QWidget>
#include <QtCharts/QChartView>
#include <vector>

class Orchestrator;

class AppBodeDiagramm : public QMainWindow
{
    Q_OBJECT

public:
    AppBodeDiagramm(Orchestrator& orchestrator, QWidget* parent = nullptr);

    std::string GetNumeratorBoxValue();
    std::string GetDenominatorBoxValue();

    void SetRecognizedFunctionNominator(const std::string& recognizedNumerator);
    void SetRecognizedFunctionDenominator(const std::string& recognizedDenominator);
    void SetDivider(const std::string& dividor);
    void CreateMagnitudePlot(const std::vector<double>& frequencies, const std::vector<double>& magnitudes);
    void CreatePhasePlot(const std::vector<double>& frequencies, const std::vector<double>& phases);
    void ExportBodeDiagrams();

    // Methods to update Stability Analysis values
    void UpdateAmplitudeMargin(const std::string& value);
    void UpdatePhaseMargin(const std::string& value);
    void UpdatePhaseCrossoverFrequency(const std::string& value);
    void UpdateGainCrossoverFrequency(const std::string& value);


private:
    // Widgets for the top-left sector
    QLineEdit* numeratorTextBox;
    QLineEdit* denominatorTextBox;
    QLabel* recognizedFunctionTextNominator;
    QLabel* recognizedFunctionTextDivider;
    QLabel* recognizedFunctionTextDenominator;
    QPushButton* exportButton;

    // Placeholder for bottom-left sector (add widgets later)
    QWidget* bottomLeftWidget;

    // Widgets for plots (top-right and bottom-right sectors)
    QWidget* frequencyResponsePlot;
    QWidget* phaseResponsePlot;
    QChartView* magnitudeChartView;

    QGridLayout* mainLayout;

    Orchestrator& orchestratorRef;

    // Widgets for Stability Analysis Display
    QLabel* infoTextWidget;
    QLabel* amplitudeMarginLabel;
    QLabel* phaseMarginLabel;
    QLabel* phaseCrossoverFrequencyLabel;
    QLabel* gainCrossoverFrequencyLabel;

   
};
