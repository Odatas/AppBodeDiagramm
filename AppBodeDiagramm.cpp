#include "ui_AppBodeDiagramm.h"
#include "AppBodeDiagramm.h"
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLineEdit>
#include <QLabel>
#include <QWidget>
#include "Orchestrator.h"
#include <QPushButton>
#include <QFileDialog>

#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QLogValueAxis>
#include <cmath>

AppBodeDiagramm::AppBodeDiagramm(Orchestrator& orchestrator, QWidget* parent)
    : QMainWindow(parent), orchestratorRef(orchestrator), magnitudeChartView(nullptr)
{
    // Create top-left widgets
    numeratorTextBox = new QLineEdit(this);
    denominatorTextBox = new QLineEdit(this);
    recognizedFunctionTextNominator = new QLabel("1", this);
    recognizedFunctionTextDivider = new QLabel("-", this);
    recognizedFunctionTextDenominator = new QLabel("1", this);
    exportButton = new QPushButton("Export Bode Diagrams", this);

    // Top-left layout for existing widgets
    QVBoxLayout* topLeftLayout = new QVBoxLayout();
    topLeftLayout->addWidget(new QLabel("Polynomial Transfer function:", this));
    topLeftLayout->addWidget(new QLabel("Numerator coefficients", this));
    topLeftLayout->addWidget(numeratorTextBox);
    topLeftLayout->addWidget(new QLabel("Denominator coefficients", this));
    topLeftLayout->addWidget(denominatorTextBox);
    topLeftLayout->addWidget(new QLabel("Recognized Transfer Function:", this));
    topLeftLayout->addWidget(recognizedFunctionTextNominator);
    topLeftLayout->addWidget(recognizedFunctionTextDivider);
    topLeftLayout->addWidget(recognizedFunctionTextDenominator);
    topLeftLayout->addWidget(exportButton); 

    // Create bottom-left widgets
    infoTextWidget = new QLabel("If value can not be exactly calculated you will see an interval instead of a single value.", this);
    amplitudeMarginLabel = new QLabel("Gain Margin: -", this);
    phaseMarginLabel = new QLabel("Phase Margin: -", this);
    phaseCrossoverFrequencyLabel = new QLabel("Phase Crossover Frequency: -", this);
    gainCrossoverFrequencyLabel = new QLabel("Gain Crossover Frequency: -", this);

    // Bottom-left layout for widgets
    QVBoxLayout* stabilityLayout = new QVBoxLayout();
    stabilityLayout->addWidget(amplitudeMarginLabel);
    stabilityLayout->addWidget(phaseMarginLabel);
    stabilityLayout->addWidget(phaseCrossoverFrequencyLabel);
    stabilityLayout->addWidget(gainCrossoverFrequencyLabel);
    stabilityLayout->addWidget(infoTextWidget);

    QWidget* topLeftWidget = new QWidget(this);
    topLeftWidget->setLayout(topLeftLayout);

    // Bottom-left sector placeholder
    bottomLeftWidget = new QWidget(this);
    bottomLeftWidget->setStyleSheet("background-color: lightyellow;");  // Visible placeholder
    bottomLeftWidget->setLayout(stabilityLayout);

    // Top-right sector: Frequency response plot placeholder
    frequencyResponsePlot = new QWidget(this);

    // Bottom-right sector: Phase response plot placeholder
    phaseResponsePlot = new QWidget(this);

    // Main layout with grid (2x2 layout)
    QGridLayout* mainLayout = new QGridLayout();
    mainLayout->addWidget(topLeftWidget, 0, 0);  // Top-left
    mainLayout->addWidget(bottomLeftWidget, 1, 0);  // Bottom-left 
    mainLayout->addWidget(frequencyResponsePlot, 0, 1);  // Top-right
    mainLayout->addWidget(phaseResponsePlot, 1, 1);  // Bottom-right

    // Set equal stretching for both rows and columns
    mainLayout->setRowStretch(0, 1);  // Top row takes half the height
    mainLayout->setRowStretch(1, 1);  // Bottom row takes half the height
    mainLayout->setColumnStretch(0, 1);  // Left column takes half the width
    mainLayout->setColumnStretch(1, 1);  // Right column takes half the width

    // Set the layout to the central widget
    QWidget* centralWidget = new QWidget(this);
    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);

    // Set the window to start maximized
    this->showMaximized();

    // Connect signals to update recognized function when text changes
    connect(numeratorTextBox, &QLineEdit::textChanged, this, [this]() {
        orchestratorRef.updateRecognizedFunction();
        });

    connect(denominatorTextBox, &QLineEdit::textChanged, this, [this]() {
        orchestratorRef.updateRecognizedFunction();
        });

    // Export Picture
    connect(exportButton, &QPushButton::clicked, this, &AppBodeDiagramm::ExportBodeDiagrams);

}

std::string AppBodeDiagramm::GetNumeratorBoxValue()
{
    QString qstr = numeratorTextBox->text();
    if (qstr.isEmpty()) {
        return "1";
    }
    return qstr.toStdString();
}

std::string AppBodeDiagramm::GetDenominatorBoxValue()
{
    QString qstr = denominatorTextBox->text();
    if (qstr.isEmpty()) {
        return "1";
    }
    return qstr.toStdString();
}

void AppBodeDiagramm::SetRecognizedFunctionNominator(const std::string& recognizedNumerator)
{
    QString qstr = QString::fromStdString(recognizedNumerator);
    recognizedFunctionTextNominator->setText(qstr);
}

void AppBodeDiagramm::SetRecognizedFunctionDenominator(const std::string& recognizedDenominator)
{
    QString qstr = QString::fromStdString(recognizedDenominator);
    recognizedFunctionTextDenominator->setText(qstr);
}

void AppBodeDiagramm::SetDivider(const std::string& dividor)
{
    QString qstr = QString::fromStdString(dividor);
    recognizedFunctionTextDivider->setText(qstr);
}

void AppBodeDiagramm::CreateMagnitudePlot(const std::vector<double>& frequencies, const std::vector<double>& magnitudes)
{

    // Create a new QLineSeries to represent the magnitude plot
    auto series = new QLineSeries();
    for (size_t i = 0; i < frequencies.size(); ++i) {
        if (frequencies[i] <= 0) {
            continue; // Skip invalid frequency values for log10
        }
        series->append(frequencies[i], magnitudes[i]);

    }

    // Create a new chart and add the series to it
    auto chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Magnitude Plot");

    auto logAxisX = new QLogValueAxis();
    logAxisX->setTitleText("Frequency (log10 scale)");
    logAxisX->setBase(10);  // Base 10 for logarithmic scale
    logAxisX->setLabelFormat("%g");
    logAxisX->setMinorTickCount(9); 
    chart->addAxis(logAxisX, Qt::AlignBottom);
    series->attachAxis(logAxisX);

    auto axisY = new QValueAxis();
    axisY->setTitleText("Magnitude (dB)");
    axisY->setLabelFormat("%.1f");
    auto minMagnitude = *std::min_element(magnitudes.begin(), magnitudes.end());
    auto maxMagnitude = *std::max_element(magnitudes.begin(), magnitudes.end());

    // Add space to make sure data is always visible.
    axisY->setRange(minMagnitude - 20, maxMagnitude + 20);

    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    // Create a new chart view to display the chart
    if (magnitudeChartView) {
        delete magnitudeChartView;
    }
    magnitudeChartView = new QChartView(chart);
    magnitudeChartView->setRenderHint(QPainter::Antialiasing);

    // Replace the existing frequency response plot widget with the new chart view
    auto layout = new QVBoxLayout();
    layout->addWidget(magnitudeChartView);
    delete frequencyResponsePlot->layout();
    frequencyResponsePlot->setLayout(layout);
}

void AppBodeDiagramm::CreatePhasePlot(const std::vector<double>& frequencies, const std::vector<double>& phases)
{
    // Create a new QLineSeries to represent the phase plot
    auto series = new QLineSeries();
    for (size_t i = 0; i < frequencies.size(); ++i) {
        if (frequencies[i] <= 0) {
            continue; // Skip invalid frequency values for log10
        }
        series->append(frequencies[i], phases[i]);
    }

    // Create a new chart and add the series to it
    auto chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Phase Plot");

    auto logAxisX = new QLogValueAxis();
    logAxisX->setTitleText("Frequency (log10 scale)");
    logAxisX->setBase(10);  // Base 10 for logarithmic scale
    logAxisX->setLabelFormat("%g");  
    logAxisX->setMinorTickCount(9);
    chart->addAxis(logAxisX, Qt::AlignBottom);
    series->attachAxis(logAxisX);

    auto axisY = new QValueAxis();
    axisY->setTitleText("Phase (degrees)");
    axisY->setLabelFormat("%.1f");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);
    auto minPhase = *std::min_element(phases.begin(), phases.end());
    auto maxPhase = *std::max_element(phases.begin(), phases.end());
    axisY->setRange(minPhase - 20, maxPhase + 20);

    // Create a new chart view to display the chart
    auto phaseChartView = new QChartView(chart);
    phaseChartView->setRenderHint(QPainter::Antialiasing);

    // Replace the existing phase response plot widget with the new chart view
    auto layout = new QVBoxLayout();
    layout->addWidget(phaseChartView);
    delete phaseResponsePlot->layout();
    phaseResponsePlot->setLayout(layout);
}

void AppBodeDiagramm::UpdateAmplitudeMargin(const std::string& value) {
    amplitudeMarginLabel->setText(QString::fromStdString("Amplitude Margin (AM): " + value));
}

void AppBodeDiagramm::UpdatePhaseMargin(const std::string& value) {
    phaseMarginLabel->setText(QString::fromStdString("Phase Margin (PM): " + value));
}

void AppBodeDiagramm::UpdatePhaseCrossoverFrequency(const std::string& value) {
    phaseCrossoverFrequencyLabel->setText(QString::fromStdString("Phase Crossover Frequency (PCF): " + value));
}

void AppBodeDiagramm::UpdateGainCrossoverFrequency(const std::string& value) {
    gainCrossoverFrequencyLabel->setText(QString::fromStdString("Gain Crossover Frequency (GCF): " + value));
}

void AppBodeDiagramm::ExportBodeDiagrams()
{
    QString filePath = QFileDialog::getSaveFileName(this, "Save Bode Diagrams", "", "PNG Files (*.png);;All Files (*)");

    if (filePath.isEmpty()) {
        return; // Cancel if no filename was specified.
    }
    
    // Amplitude diagram
    if (magnitudeChartView) {
        // Create picture of current view
        QPixmap magnitudePixmap = magnitudeChartView->grab(); 
        magnitudePixmap.save(filePath + "-amplitude.png");
    }

    // Phase-diagram
    if (phaseResponsePlot && phaseResponsePlot->layout()) {
        QChartView* phaseChartView = dynamic_cast<QChartView*>(phaseResponsePlot->layout()->itemAt(0)->widget());
        if (phaseChartView) {
            QPixmap phasePixmap = phaseChartView->grab();
            phasePixmap.save(filePath + "-phase.png");
        }
    }
}


