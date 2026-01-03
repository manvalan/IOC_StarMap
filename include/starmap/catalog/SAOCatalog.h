#ifndef STARMAP_SAO_CATALOG_H
#define STARMAP_SAO_CATALOG_H

#include "starmap/core/CelestialObject.h"
#include "GaiaSAODatabase.h"
#include <memory>
#include <string>
#include <optional>

namespace starmap {
namespace catalog {

/**
 * @brief Gestione del catalogo SAO (Smithsonian Astrophysical Observatory)
 * 
 * Il catalogo SAO contiene circa 259,000 stelle con magnitudine < 9.
 * Fornisce cross-reference con altri cataloghi e numeri SAO storicamente importanti.
 */
class SAOCatalog {
public:
    /**
     * @brief Costruttore con path opzionale al database locale
     * @param localDbPath Path al database Gaia-SAO locale (default: "gaia_sao_xmatch.db")
     */
    explicit SAOCatalog(const std::string& localDbPath = "gaia_sao_xmatch.db");
    ~SAOCatalog();

    /**
     * @brief Cerca il numero SAO per coordinate
     * @param coords Coordinate equatoriali J2000
     * @param searchRadiusArcsec Raggio di ricerca in arcosecondi (default 10")
     * @return Numero SAO se trovato
     */
    std::optional<int> findSAOByCoordinates(
        const core::EquatorialCoordinates& coords,
        double searchRadiusArcsec = 10.0);

    /**
     * @brief Cerca stella per numero SAO
     * @param saoNumber Numero SAO
     * @return Stella con dati SAO se trovata
     */
    std::shared_ptr<core::Star> findBySAONumber(int saoNumber);

    /**
     * @brief Query online al servizio SIMBAD per cross-match SAO
     * @param gaiaId ID sorgente GAIA
     * @return Numero SAO se disponibile
     */
    std::optional<int> querySIMBADForSAO(long long gaiaId);

    /**
     * @brief Cross-match tra coordinate e catalogo SAO via VizieR
     * @param coords Coordinate equatoriali
     * @param radiusArcsec Raggio di ricerca
     * @return Numero SAO se trovato
     */
    std::optional<int> crossMatchVizieR(
        const core::EquatorialCoordinates& coords,
        double radiusArcsec = 10.0);

    /**
     * @brief Carica cache locale del catalogo SAO (se disponibile)
     * @param catalogPath Path al file del catalogo SAO
     * @return true se caricato con successo
     */
    bool loadLocalCatalog(const std::string& catalogPath);

    /**
     * @brief Arricchisce una stella GAIA con il numero SAO
     * @param star Puntatore a stella da arricchire
     * @return true se numero SAO trovato e aggiunto
     */
    bool enrichWithSAO(std::shared_ptr<core::Star> star);

    /**
     * @brief Verifica se database locale è disponibile
     * @return true se database locale può essere usato
     */
    bool hasLocalDatabase() const;

    /**
     * @brief Ottieni statistiche del database locale
     * @return Stringa con statistiche o messaggio errore
     */
    std::string getDatabaseStatistics() const;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl_;
    std::unique_ptr<GaiaSAODatabase> localDatabase_;
};

/**
 * @brief Struttura per entry del catalogo SAO
 */
struct SAOEntry {
    int saoNumber;
    core::EquatorialCoordinates coordinates;
    double magnitude;
    std::string spectralType;
    std::string name; // Nome proprio se disponibile
};

} // namespace catalog
} // namespace starmap

#endif // STARMAP_SAO_CATALOG_H
