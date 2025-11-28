#ifndef STARMAP_GAIA_CLIENT_H
#define STARMAP_GAIA_CLIENT_H

#include "starmap/core/CelestialObject.h"
#include "starmap/core/Coordinates.h"
#include <vector>
#include <memory>
#include <string>

namespace starmap {
namespace catalog {

/**
 * @brief Parametri per query al catalogo GAIA
 */
struct GaiaQueryParameters {
    core::EquatorialCoordinates center;
    double radiusDegrees = 1.0;
    double maxMagnitude = 15.0;
    int maxResults = 10000;
};

/**
 * @brief Client per catalogo Gaia usando IOC_GaiaLib UnifiedGaiaCatalog
 * 
 * Utilizza il catalogo multifile V2 per performance ottimali:
 * - Cone search 0.5°: ~0.001 ms
 * - Cone search 5°: ~13 ms
 * - Query per nome: <1 ms (451 stelle IAU ufficiali)
 */
class GaiaClient {
public:
    GaiaClient();
    ~GaiaClient();

    /**
     * @brief Query a cono per regione del cielo
     * @param params Parametri della query (centro, raggio, magnitudine max)
     * @return Lista di stelle trovate
     */
    std::vector<std::shared_ptr<core::Star>> queryRegion(
        const GaiaQueryParameters& params);

    /**
     * @brief Query per Gaia source_id
     * @param gaiaId Il source_id Gaia DR3
     * @return Stella trovata o nullptr
     */
    std::shared_ptr<core::Star> queryById(long long gaiaId);

    /**
     * @brief Query per nome stella (IAU, Bayer, Flamsteed, HD, HIP)
     * @param name Nome della stella (es. "Sirius", "α CMa", "HD 48915")
     * @return Stella trovata o nullptr
     * 
     * Supporta 451 stelle IAU ufficiali con cross-matching automatico:
     * - Nomi IAU: "Sirius", "Vega", "Polaris", "Betelgeuse"
     * - Bayer: "α CMa", "α Lyr", "α UMi"
     * - HD: "HD 48915", "HD 172167"
     * - HIP: "HIP 32349", "HIP 91262"
     */
    std::shared_ptr<core::Star> queryByName(const std::string& name);

    /**
     * @brief Verifica se il catalogo è disponibile
     * @return true se inizializzato correttamente
     */
    bool isAvailable() const;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl_;
};

} // namespace catalog
} // namespace starmap

#endif // STARMAP_GAIA_CLIENT_H
