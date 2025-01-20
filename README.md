# AppBodeDiagramm

AppBodeDiagramm ist eine Qt-basierte Anwendung, die Bode-Diagramme (Amplituden- und Phasengänge) für benutzerdefinierte Übertragungsfunktionen erstellt und visualisiert. Die Anwendung umfasst eine GUI zur Eingabe der Übertragungsfunktion, sowie eine Echtzeitanalyse von Stabilitätsparametern wie Phasen- und Verstärkungsmargen.

## Features

- **GUI** zur Eingabe von Zähler- und Nennerkoeffizienten einer Übertragungsfunktion.
- Automatische Berechnung und Anzeige der erkannten Übertragungsfunktion.
- Visualisierung der Amplituden- und Phasengänge als logarithmische Diagramme.
- Anzeige von Stabilitätsparametern:
  - Verstärkungsmarge (Gain Margin)
  - Phasenmarge (Phase Margin)
  - Frequenz des Phasenkreuzpunkts (Phase Crossover Frequency)
  - Frequenz des Verstärkungskreuzpunkts (Gain Crossover Frequency)
- Export der Bode-Diagramme als PNG-Dateien.

## Installation

### Voraussetzungen

- Qt Framework (Version 5.15 oder höher)
- QtCharts-Modul
- C++17-Unterstützung

### Build-Schritte

1. Stelle sicher, dass die Qt-Entwicklungsumgebung installiert ist.
2. Klone dieses Repository:

   ```bash
   git clone https://github.com/dein-username/AppBodeDiagramm.git
   cd AppBodeDiagramm
   ```

3. Öffne das Projekt in Qt Creator oder einer anderen unterstützten IDE.
4. Führe das Projekt aus oder baue die ausführbare Datei über die IDE.

## Verwendung

1. Starte die Anwendung.
2. Gib die Zähler- und Nennerkoeffizienten der gewünschten Übertragungsfunktion in die entsprechenden Textfelder ein.
3. Die berechneten Amplituden- und Phasengänge werden automatisch im Diagramm angezeigt.
4. Analysiere die Stabilitätsparameter, die im unteren Bereich der GUI angezeigt werden.
5. Optional: Exportiere die Diagramme über die Schaltfläche "Export Bode Diagrams".

## Code-Struktur

- **`main.cpp`**: Einstiegspunkt der Anwendung.
- **`AppBodeDiagramm`**: GUI-Komponente der Anwendung, die für die Benutzerinteraktion und Diagrammvisualisierung verantwortlich ist.
- **`Orchestrator`**: Vermittlerklasse zwischen GUI und den funktionalen Klassen.
- **`FunctionalClasses`**: Implementierung von:
  - `TransferFunction`: Verarbeitung von Übertragungsfunktionen.
  - `FrequencyResponse`: Berechnung der Frequenzantwort.
  - `StabilityAnalyzer`: Analyse von Stabilitätsparametern.
  - 
## Beitrag leisten

Beiträge sind willkommen! Erstelle bitte einen Pull-Request oder melde ein Problem über den Issue-Tracker.
