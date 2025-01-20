#ifndef ORCHESTRATOR_H
#define ORCHESTRATOR_H

#include <string>
#include <vector>

class AppBodeDiagramm;

class Orchestrator {
public:
    // Sets the reference to the GUI frame
    void setGUIRef(AppBodeDiagramm& frameRef);

    // Updates the recognized function based on GUI input
    void updateRecognizedFunction();

    // Creates a transfer function from the input string
    std::string CreateTransferFunction(const std::string& input, std::vector<double>& coefficients);

    // Creates a divider line for display purposes
    std::string CreateDividerLength(const std::string& numeratorValue, const std::string& denominatorValue);

private:
    AppBodeDiagramm* GUIRef = nullptr;
};

#endif // ORCHESTRATOR_H
