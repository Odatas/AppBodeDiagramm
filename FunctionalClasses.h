#ifndef FUNCTIONALCLASSES_H
#define FUNCTIONALCLASSES_H

#include <vector>
#include <complex>
#include <string>

// TransferFunction class
class TransferFunction {
private:
    std::vector<double> numerator;
    std::vector<double> denominator;

public:
    TransferFunction(const std::vector<double>& num, const std::vector<double>& den);
    std::vector<std::complex<double>> calculateFrequencyResponse(const std::vector<double>& frequencies) const;
    const std::vector<double>& getNumerator() const;
    const std::vector<double>& getDenominator() const;
};

// FrequencyResponse class
class FrequencyResponse {
private:
    std::vector<double> frequencies;
    std::vector<double> magnitudes;
    std::vector<double> phases;

public:
    FrequencyResponse(const std::vector<double>& freqs);
    void compute(const TransferFunction& transferFunction);
    const std::vector<double>& getMagnitudes() const;
    const std::vector<double>& getPhases() const;
    const std::vector<double>& getFrequencies() const;
};

class StabilityAnalyzer {
private:
    std::string amplitudeMargin;
    std::string phaseMargin;
    std::string phaseCrossoverFrequency;   // Frequency where phase = -180 degrees
    std::string gainCrossoverFrequency;    // Frequency where gain = 0 dB

public:
    StabilityAnalyzer();

    // Analyse-Methode
    void analyze(const TransferFunction& transferFunction, const FrequencyResponse& frequencyResponse);

    // Getter für die Analysewerte
    std::string getAmplitudeMargin() const;
    std::string getPhaseMargin() const;
    std::string getPhaseCrossoverFrequency() const;
    std::string getGainCrossoverFrequency() const;
};

#endif // FUNCTIONALCLASSES_H
