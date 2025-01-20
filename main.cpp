#include "AppBodeDiagramm.h"
#include <QtWidgets/QApplication>
#include "Orchestrator.h"

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    // Instantiate the Orchestrator
    Orchestrator* orchestrator = new Orchestrator();

    // Instantiate the main window (AppBodeDiagramm) with the orchestrator reference
    AppBodeDiagramm* GUI = new AppBodeDiagramm(*orchestrator);
    GUI->show();

    // Set the GUI reference in the Orchestrator
    orchestrator->setGUIRef(*GUI);

    // Initial Creating the Plots so they are not empty.
    orchestrator->updateRecognizedFunction();

    return a.exec();
}
