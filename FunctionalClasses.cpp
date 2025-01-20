#include "FunctionalClasses.h"
#include <cmath>
#include <iostream>

// TransferFunction class implementation
TransferFunction::TransferFunction(const std::vector<double>& num, const std::vector<double>& den)
    : numerator(num), denominator(den) {}

std::vector<std::complex<double>> TransferFunction::calculateFrequencyResponse(const std::vector<double>& frequencies) const {
    std::vector<std::complex<double>> response;

    for (double omega : frequencies) {
        std::complex<double> s(0, omega);  // s = jω
        std::complex<double> num(0, 0);
        std::complex<double> den(0, 0);

        for (size_t i = 0; i < numerator.size(); ++i) {
            num += numerator[i] * std::pow(s, numerator.size() - 1 - i);
        }

        for (size_t i = 0; i < denominator.size(); ++i) {
            den += denominator[i] * std::pow(s, denominator.size() - 1 - i);
        }

        response.push_back(num / den);
    }

    return response;
}

const std::vector<double>& TransferFunction::getNumerator() const {
    return numerator;
}

const std::vector<double>& TransferFunction::getDenominator() const {
    return denominator;
}

// FrequencyResponse class implementation
FrequencyResponse::FrequencyResponse(const std::vector<double>& freqs)
    : frequencies(freqs) {}

void FrequencyResponse::compute(const TransferFunction& transferFunction) {
    std::vector<std::complex<double>> response = transferFunction.calculateFrequencyResponse(frequencies);

    magnitudes.clear();
    phases.clear();

    double previous_phase = 0.0; // Startwert für die vorherige Phase

    for (const auto& r : response) {
        // Calculation of magnitude in dB
        magnitudes.push_back(20 * std::log10(std::abs(r)));

        // Calculate phase and convert into degree
        double phase = std::arg(r) * 180 / 3.141592;

        // Phase Unwrapping to cure anomalies
        if (!phases.empty()) {
            double phase_diff = phase - previous_phase;
            if (phase_diff > 180.0) {
                phase -= 360.0; 
            }
            else if (phase_diff < -180.0) {
                phase += 360.0;
            }
        }

        phases.push_back(phase);
        previous_phase = phase;
    }
}


const std::vector<double>& FrequencyResponse::getMagnitudes() const {
    return magnitudes;
}

const std::vector<double>& FrequencyResponse::getPhases() const {
    return phases;
}

const std::vector<double>& FrequencyResponse::getFrequencies() const {
    return frequencies;
}


StabilityAnalyzer::StabilityAnalyzer()
    : amplitudeMargin("-"), phaseMargin("-"),
    phaseCrossoverFrequency("-"), gainCrossoverFrequency("-") {}


void StabilityAnalyzer::analyze(const TransferFunction& transferFunction, const FrequencyResponse& frequencyResponse)
{
    const auto& magnitudes = frequencyResponse.getMagnitudes();
    const auto& phases = frequencyResponse.getPhases();
    const auto& frequencies = frequencyResponse.getFrequencies();

    // Check for empty lists
    if (magnitudes.empty() || phases.empty() || frequencies.empty()) {
        return;
    }

    bool gainCrossoverFound = false;
    bool phaseCrossoverFound = false;


    double prevMagnitude = magnitudes[0];
    double prevPhase = phases[0];
    double prevFrequency = frequencies[0];

    // Schleife durch alle Datenpunkte
    for (size_t i = 1; i < frequencies.size(); ++i)
    {
        // ============================
        // 1) Gain Crossover Check (0-db-frequency)
        // ============================
        // a) Exact value found
        if (!gainCrossoverFound && magnitudes[i] == 0.0)
        {
            // Phase Margin = 180° + Phase at this point.
            phaseMargin = std::to_string(180.0 + phases[i]);
            gainCrossoverFrequency = std::to_string(frequencies[i]);
            gainCrossoverFound = true;
        }

        // b) 0-dB was crossed between two points (sign change after multiplication)
        if (!gainCrossoverFound && (prevMagnitude * magnitudes[i] < 0.0))
        {
            // Two values when the value is found between two points.
            // We could also do an interpolation but it would introduce a small inaccuracy
            std::string phaseMarginPrev = std::to_string(180.0 + prevPhase);
            std::string phaseMarginNow = std::to_string(180.0 + phases[i]);

            phaseMargin = phaseMarginPrev + " to " + phaseMarginNow;

            // Frequency where 0db is crossed
            gainCrossoverFrequency = std::to_string(prevFrequency) + " to " + std::to_string(frequencies[i]);
            gainCrossoverFound = true;
        }

        // ============================
        // 2) Phase Crossover Check (-180° frequency)
        // ============================
        // a) Exact value found
        if (!phaseCrossoverFound && phases[i] == -180.0)
        {
            // Gain Margin (amplitudeMargin) is magnitude at this point
            amplitudeMargin = std::to_string(magnitudes[i]);
            phaseCrossoverFrequency = std::to_string(frequencies[i]);
            phaseCrossoverFound = true;
        }

        // b) -180° was crosed between two points
        if (!phaseCrossoverFound &&
            ((prevPhase < -180.0 && phases[i] > -180.0) ||
                (prevPhase > -180.0 && phases[i] < -180.0)))
        {
            std::string gainMarginPrev = std::to_string(prevMagnitude);
            std::string gainMarginNow = std::to_string(magnitudes[i]);

            // Two values when the value is found between two points.
            // Again we could also do an interpolation but it would introduce a small inaccuracy.
            amplitudeMargin = gainMarginPrev + " to " + gainMarginNow;
            phaseCrossoverFrequency = std::to_string(prevFrequency) + " to " + std::to_string(frequencies[i]);
            phaseCrossoverFound = true;
        }

        prevMagnitude = magnitudes[i];
        prevPhase = phases[i];
        prevFrequency = frequencies[i];
    }
}


std::string StabilityAnalyzer::getAmplitudeMargin() const {
    return amplitudeMargin;
}

std::string StabilityAnalyzer::getPhaseMargin() const {
    return phaseMargin;
}

std::string StabilityAnalyzer::getPhaseCrossoverFrequency() const {
    return phaseCrossoverFrequency;
}

std::string StabilityAnalyzer::getGainCrossoverFrequency() const {
    return gainCrossoverFrequency;
}



