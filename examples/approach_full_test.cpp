/**
 * @file approach_full_test.cpp
 * @brief Test completo carta di approccio con tutte le feature
 * 
 * Genera carta di approccio per regione Orione includendo:
 * - Linee delle costellazioni
 * - Confini delle costellazioni
 * - Nomi delle costellazioni
 * - Designazioni Flamsteed/Bayer
 * - Numeri SAO
 * - Griglia coordinate
 */

#include <starmap/StarMap.h>
#include <iostream>
#include <chrono>

using namespace starmap;

int main(int argc, char* argv[]) {
    std::cout << "=== Test Carta di Approccio Completa ===\n\n";
    
    // Configurazione
    map::MapConfiguration config;
    
    // Centro: Regione Orione
    config.center = core::EquatorialCoordinates(85.0, 0.0); // RA: 5h40m, Dec: 0°
    
    // Campo di vista 20° (tipico per carta di approccio)
    config.fieldOfViewWidth = 40.0;
    config.fieldOfViewHeight = 40.0;
    
    // Dimensioni immagine
    config.imageWidth = 1920;
    config.imageHeight = 1920;
    
    // Magnitudine limite per approccio
    config.limitingMagnitude = 10.0;
    
    // Proiezione stereografica (standard astronomico)
    config.projection = map::ProjectionType::STEREOGRAPHIC;
    
    // ===== GRIGLIA =====
    config.gridStyle.enabled = true;
    config.gridStyle.raStepDegrees = 5.0;
    config.gridStyle.decStepDegrees = 5.0;
    config.gridStyle.color = 0x404040FF; // Grigio scuro
    config.gridStyle.showLabels = true;
    config.gridStyle.labelColor = 0xCCCCCCFF;
    
    // ===== STELLE =====
    config.starStyle.showNames = true;
    config.starStyle.minMagnitudeForLabel = 4.0; // Nomi per stelle luminose
    
    // **NUMERI SAO**
    config.starStyle.showSAONumbers = true;
    config.starStyle.minMagnitudeForLabel = 8.0; // SAO fino a mag 8
    
    config.starStyle.useSpectralColors = true;
    config.starStyle.minSymbolSize = 0.8f;
    config.starStyle.maxSymbolSize = 8.0f;
    
    // ===== COSTELLAZIONI =====
    config.showConstellationLines = true;
    config.showConstellationBoundaries = true;
    config.showConstellationNames = true;
    
    // ===== ASPETTO =====
    config.backgroundColor = 0x000000FF; // Nero
    config.showBorder = true;
    config.showTitle = true;
    config.title = "Carta di Approccio Orione - Test Completo";
    config.showCompass = true;
    config.showScale = true;
    
    // Orientamento standard
    config.northUp = true;
    config.eastLeft = true;
    
    // ===== GENERAZIONE CARTA =====
    std::cout << "1. Inizializzazione cataloghi...\n";
    catalog::CatalogManager catalogManager;
    
    if (catalogManager.getSAOCatalog().hasLocalDatabase()) {
        std::cout << "   ✓ Database SAO locale disponibile\n";
    } else {
        std::cout << "   ⚠ Usando query SAO online\n";
    }
    
    std::cout << "\n2. Query stelle nella regione...\n";
    auto start = std::chrono::high_resolution_clock::now();
    
    catalog::GaiaQueryParameters params;
    params.center = config.center;
    params.radiusDegrees = 25.0; // Un po' più ampio del campo visivo
    params.maxMagnitude = config.limitingMagnitude;
    
    // Arricchisci con SAO
    auto stars = catalogManager.queryStars(params, true);
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "   Trovate " << stars.size() << " stelle in " 
              << duration.count() << " ms\n";
    
    // Conta stelle con SAO
    int withSAO = 0;
    for (const auto& star : stars) {
        if (star->getSAONumber()) {
            withSAO++;
        }
    }
    std::cout << "   Stelle con SAO: " << withSAO << "/" << stars.size() 
              << " (" << (100.0 * withSAO / stars.size()) << "%)\n";
    
    std::cout << "\n3. Generazione carta...\n";
    map::ChartGenerator generator;
    
    try {
        // Genera carta completa
        start = std::chrono::high_resolution_clock::now();
        auto chart = generator.generateChart(config, stars);
        end = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        std::cout << "   Carta generata in " << duration.count() << " ms\n";
        
        // Salva
        std::string outputPath = "approccio_orione_completo.png";
        if (chart.saveAsPNG(outputPath)) {
            std::cout << "\n✓ Carta salvata: " << outputPath << "\n";
            
            // Info file
            long fileSize = chart.data.size() / 1024;
            std::cout << "  Dimensioni: " << config.imageWidth << "x" 
                      << config.imageHeight << " px\n";
            std::cout << "  Memoria: " << fileSize << " KB\n";
        } else {
            std::cerr << "\n✗ Errore nel salvare la carta\n";
            return 1;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "\n✗ Errore: " << e.what() << "\n";
        return 1;
    }
    
    // ===== STATISTICHE DETTAGLIATE =====
    std::cout << "\n4. Statistiche della carta:\n";
    std::cout << "   --------------------------------\n";
    
    // Analisi magnitudini
    double minMag = 99.0, maxMag = -99.0, sumMag = 0.0;
    std::map<int, int> magHistogram;
    
    for (const auto& star : stars) {
        double mag = star->getMagnitude();
        minMag = std::min(minMag, mag);
        maxMag = std::max(maxMag, mag);
        sumMag += mag;
        
        int magBin = static_cast<int>(mag);
        magHistogram[magBin]++;
    }
    
    std::cout << "   Magnitudine: " << std::fixed << std::setprecision(2)
              << minMag << " - " << maxMag 
              << " (media: " << (sumMag / stars.size()) << ")\n";
    
    std::cout << "\n   Distribuzione magnitudini:\n";
    for (const auto& [mag, count] : magHistogram) {
        if (mag <= 10) { // Mostra solo fino a mag 10
            std::cout << "   mag " << mag << ": " 
                      << std::string(count / 10, '*') << " (" << count << ")\n";
        }
    }
    
    // Stelle principali con SAO
    std::cout << "\n   Stelle principali con SAO:\n";
    std::vector<std::shared_ptr<core::Star>> brightStars;
    for (const auto& star : stars) {
        if (star->getMagnitude() < 4.0 && star->getSAONumber()) {
            brightStars.push_back(star);
        }
    }
    
    // Ordina per magnitudine
    std::sort(brightStars.begin(), brightStars.end(),
              [](const auto& a, const auto& b) {
                  return a->getMagnitude() < b->getMagnitude();
              });
    
    int count = 0;
    for (const auto& star : brightStars) {
        if (count++ >= 10) break; // Prime 10
        
        std::cout << "   ";
        if (!star->getName().empty()) {
            std::cout << std::setw(15) << star->getName();
        } else {
            std::cout << std::setw(15) << ("Gaia " + std::to_string(star->getGaiaId()).substr(0, 8) + "...");
        }
        
        std::cout << " | SAO " << std::setw(6) << *star->getSAONumber()
                  << " | mag " << std::fixed << std::setprecision(2) 
                  << std::setw(5) << star->getMagnitude()
                  << " | " << std::setw(7) << std::setprecision(3)
                  << star->getCoordinates().getRightAscension() << "° "
                  << std::setw(7) << star->getCoordinates().getDeclination() << "°\n";
    }
    
    std::cout << "\n=== Test completato con successo! ===\n";
    std::cout << "\nApri la carta con:\n";
    std::cout << "  open " << "approccio_orione_completo.png\n";
    std::cout << "  # oppure\n";
    std::cout << "  xdg-open " << "approccio_orione_completo.png\n";
    
    return 0;
}
