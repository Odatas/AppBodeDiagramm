#include "Orchestrator.h"
#include "AppBodeDiagramm.h"
#include "FunctionalClasses.h"
#include <sstream> 
#include <string> 
#include <vector> 
#include <algorithm>
#include <cmath> 
#include <regex>

std::vector<std::string> splitString(const std::string& input, char delimiter) {
    std::vector<std::string> tokens;
    std::istringstream stream(input);
    std::string token;

    while (std::getline(stream, token, delimiter)) {
        tokens.push_back(token);
    }

    return tokens;
}

// Helper function to replace all occurrences of a character in a string
static void replaceAllChars(std::string& s, char oldChar, char newChar)
{
    std::replace(s.begin(), s.end(), oldChar, newChar);
}

std::string Orchestrator::CreateTransferFunction(const std::string& input, std::vector<double>& coefficients)
{

    coefficients.clear();

    // A temporary vector to store all valid parsed values
    std::vector<double> parsedValues;

    // Regex to match numbers (with optional sign) in the input string:
    // Explanation:
    //   ([+-]?)          -> optional sign (+ or -)
    //   \s*              -> any amount of whitespace
    //   (\d+(?:[.,]\d+)?) -> one or more digits, optionally followed by a decimal
    //                        separator (comma or dot) and more digits
    //
    std::regex re(R"(([+-]?)\s*(\d+(?:[.,]\d+)?))");

    // Use regex iterators to find all matches in the input
    auto begin = std::sregex_iterator(input.begin(), input.end(), re);
    auto end = std::sregex_iterator();

    for (auto it = begin; it != end; ++it)
    {
        std::smatch match = *it;
        // match[1] = sign string ("+" or "-"), match[2] = numeric part (e.g., "3.4" or "3,4")
        std::string signStr = match[1];
        std::string numberStr = match[2];

        // Build the full numeric string including the sign
        std::string fullNumberStr = signStr + numberStr;

        // Replace commas with dots so std::stod can parse it
        replaceAllChars(fullNumberStr, ',', '.');

        try
        {
            // Convert to double (can throw invalid_argument or out_of_range)
            double value = std::stod(fullNumberStr);
            parsedValues.push_back(value);
        }
        catch (const std::invalid_argument&)
        {
            // Ignore invalid tokens
        }
        catch (const std::out_of_range&)
        {
            // Ignore tokens that are out of double's range
        }
    }

    // Build the polynomial string from the parsed values
    std::string result;

    // The highest exponent is (number_of_values - 1)
    int power = static_cast<int>(parsedValues.size()) - 1;

    for (size_t i = 0; i < parsedValues.size(); ++i)
    {
        double value = parsedValues[i];
        // Store the value in the coefficients vector
        coefficients.push_back(value);

        // Handle sign in the output string
        if (!result.empty())
        {
            if (value < 0.0)
                result += " - ";
            else
                result += " + ";
        }
        else
        {
            // For the very first term, if it's negative, just prepend "-"
            if (value < 0.0)
                result += "-";
        }

        // Use the absolute value here because we've already handled the sign
        double absValue = std::fabs(value);
        result += std::to_string(absValue);

        // Append the exponent if needed
        if (power > 1)
        {
            // Example: S^3
            result += "S^" + std::to_string(power);
        }
        else if (power == 1)
        {
            // Example: S
            result += "S";
        }

        // Decrement exponent for the next term
        power--;
    }

    return result;
}

std::string Orchestrator::CreateDividerLength(const std::string& numeratorValue, const std::string& denominatorValue) {
    size_t maxLength = std::max(numeratorValue.length(), denominatorValue.length());
    return std::string(maxLength, '-');
}

void Orchestrator::setGUIRef(AppBodeDiagramm& frameRef) {
    GUIRef = &frameRef;
}

void Orchestrator::updateRecognizedFunction() {
    // Get values from gui
    std::string numerator = GUIRef->GetNumeratorBoxValue();
    std::string denominator = GUIRef->GetDenominatorBoxValue();
    
    // Creat transfer function 
    std::vector<double> numeratorCoefficients;
    std::vector<double> denominatorCoefficients;

    std::string recognizedNumerator = CreateTransferFunction(numerator, numeratorCoefficients);
    std::string recognizedDenominator = CreateTransferFunction(denominator, denominatorCoefficients);

    // Set recognized transfer function to gui
    GUIRef->SetRecognizedFunctionNominator(recognizedNumerator);
    GUIRef->SetRecognizedFunctionDenominator(recognizedDenominator);

    // Expand divider to cover whole function
    std::string divider = CreateDividerLength(recognizedNumerator, recognizedDenominator);
    GUIRef->SetDivider(divider);

    // Calculate Transferfunction values.
    TransferFunction transferFunction(numeratorCoefficients, denominatorCoefficients);

    std::vector<double> frequencies;
    int numPoints = 10000;
    double start = 0.01;
    double end = 1000000;

    // Generate 'numPoints' frequency values equally spaced on a log scale between 'start' and 'end'.
    for (int i = 0; i < numPoints; ++i) {
        double exponent = log10(start) + i * (log10(end) - log10(start)) / (numPoints - 1);
        frequencies.push_back(pow(10, exponent));
    }

    // DO Analysis
    FrequencyResponse frequencyResponse(frequencies);
    frequencyResponse.compute(transferFunction);

    StabilityAnalyzer stabilityAnalyzer;
    stabilityAnalyzer.analyze(transferFunction, frequencyResponse);

    // Fill gui elements
    GUIRef->CreateMagnitudePlot(frequencyResponse.getFrequencies(), frequencyResponse.getMagnitudes());
    GUIRef->CreatePhasePlot(frequencyResponse.getFrequencies(), frequencyResponse.getPhases());
    GUIRef->UpdateAmplitudeMargin(stabilityAnalyzer.getAmplitudeMargin());
    GUIRef->UpdatePhaseMargin(stabilityAnalyzer.getPhaseMargin());
    GUIRef->UpdatePhaseCrossoverFrequency(stabilityAnalyzer.getPhaseCrossoverFrequency());
    GUIRef->UpdateGainCrossoverFrequency(stabilityAnalyzer.getGainCrossoverFrequency());

}
