#include "starmap/catalog/SAOCatalog.h"
#include "starmap/catalog/GaiaSAODatabase.h"
#include "starmap/utils/HttpClient.h"
#include <sstream>
#include <cmath>
#include <map>
#include <iostream>

namespace starmap {
namespace catalog {

// URL del servizio VizieR per query al catalogo SAO
const std::string VIZIER_SAO_URL = "https://vizier.cds.unistra.fr/viz-bin/votable";
const std::string SIMBAD_TAP_URL = "https://simbad.cds.unistra.fr/simbad/sim-tap/sync";

class SAOCatalog::Impl {
public:
    Impl() {}
    
    utils::HttpClient httpClient_;
    std::map<int, SAOEntry> localCache_; // Cache locale per performance
};

SAOCatalog::SAOCatalog(const std::string& localDbPath) 
    : pImpl_(std::make_unique<Impl>())
    , localDatabase_(std::make_unique<GaiaSAODatabase>(localDbPath)) {
    
    if (localDatabase_->isAvailable()) {
        std::cout << "Gaia-SAO local database loaded successfully" << std::endl;
        std::cout << localDatabase_->getStatistics() << std::endl;
    } else {
        std::cout << "Warning: Gaia-SAO local database not available. Using online queries only." << std::endl;
    }
}

SAOCatalog::~SAOCatalog() = default;

std::optional<int> SAOCatalog::findSAOByCoordinates(
    const core::EquatorialCoordinates& coords,
    double searchRadiusArcsec) {
    
    return crossMatchVizieR(coords, searchRadiusArcsec);
}

std::shared_ptr<core::Star> SAOCatalog::findBySAONumber(int saoNumber) {
    // Controlla cache locale
    auto it = pImpl_->localCache_.find(saoNumber);
    if (it != pImpl_->localCache_.end()) {
        auto star = std::make_shared<core::Star>();
        star->setSAONumber(it->second.saoNumber);
        star->setCoordinates(it->second.coordinates);
        star->setMagnitude(it->second.magnitude);
        star->setSpectralType(it->second.spectralType);
        star->setName(it->second.name);
        return star;
    }
    
    // Query VizieR per numero SAO specifico
    std::ostringstream query;
    query << VIZIER_SAO_URL << "?-source=I/131A/sao&-out.max=1&SAO=" << saoNumber;
    
    try {
        std::string response = pImpl_->httpClient_.get(query.str());
        
        // Parser semplificato - in produzione usare libreria XML
        if (response.find("<TR>") != std::string::npos) {
            auto star = std::make_shared<core::Star>();
            star->setSAONumber(saoNumber);
            
            // Estrai dati dalla risposta VOTable
            // Questo è un parser molto semplificato
            size_t raPos = response.find("_RAJ2000");
            size_t decPos = response.find("_DEJ2000");
            size_t magPos = response.find("Vmag");
            
            // TODO: Implementare parser completo
            
            return star;
        }
    } catch (const std::exception&) {
        // Errore nella query
    }
    
    return nullptr;
}

std::optional<int> SAOCatalog::querySIMBADForSAO(long long gaiaId) {
    // Query SIMBAD per cross-reference
    std::ostringstream adql;
    adql << "SELECT ident.id FROM ident JOIN ids ON ident.oidref = ids.oidref "
         << "WHERE ids.id = 'Gaia DR3 " << gaiaId << "' "
         << "AND ident.id LIKE 'SAO %'";
    
    std::ostringstream requestUrl;
    requestUrl << SIMBAD_TAP_URL << "?REQUEST=doQuery&LANG=ADQL&FORMAT=votable&QUERY=";
    
    // URL-encode
    std::string encodedQuery;
    for (char c : adql.str()) {
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            encodedQuery += c;
        } else if (c == ' ') {
            encodedQuery += '+';
        } else {
            char hex[4];
            snprintf(hex, sizeof(hex), "%%%02X", (unsigned char)c);
            encodedQuery += hex;
        }
    }
    
    requestUrl << encodedQuery;
    
    try {
        pImpl_->httpClient_.setTimeout(30);
        std::string response = pImpl_->httpClient_.get(requestUrl.str());
        
        // Cerca pattern "SAO NNNN"
        size_t pos = response.find("SAO ");
        if (pos != std::string::npos) {
            pos += 4;
            std::string numStr;
            while (pos < response.length() && isdigit(response[pos])) {
                numStr += response[pos++];
            }
            if (!numStr.empty()) {
                return std::stoi(numStr);
            }
        }
    } catch (const std::exception&) {
        // Errore nella query
    }
    
    return std::nullopt;
}

std::optional<int> SAOCatalog::crossMatchVizieR(
    const core::EquatorialCoordinates& coords,
    double radiusArcsec) {
    
    // Query VizieR con ricerca conica
    std::ostringstream query;
    query << VIZIER_SAO_URL 
          << "?-source=I/131A/sao"
          << "&-c=" << coords.getRightAscension() 
          << "+" << coords.getDeclination()
          << "&-c.rs=" << (radiusArcsec / 3600.0) // converti in gradi
          << "&-out.max=1"
          << "&-out=SAO,_RAJ2000,_DEJ2000,Vmag";
    
    try {
        std::string response = pImpl_->httpClient_.get(query.str());
        
        // Parser semplificato per estrarre numero SAO
        size_t saoPos = response.find("<TD>");
        if (saoPos != std::string::npos) {
            saoPos += 4;
            size_t endPos = response.find("</TD>", saoPos);
            if (endPos != std::string::npos) {
                std::string saoStr = response.substr(saoPos, endPos - saoPos);
                try {
                    // Rimuovi spazi
                    saoStr.erase(remove_if(saoStr.begin(), saoStr.end(), ::isspace), 
                                saoStr.end());
                    if (!saoStr.empty() && isdigit(saoStr[0])) {
                        return std::stoi(saoStr);
                    }
                } catch (...) {}
            }
        }
    } catch (const std::exception&) {
        // Errore nella query
    }
    
    return std::nullopt;
}

bool SAOCatalog::loadLocalCatalog(const std::string& catalogPath) {
    // TODO: Implementare caricamento da file locale
    // Il catalogo SAO può essere scaricato da VizieR o altri fonti
    // Formato tipico: CSV o FITS
    
    // Per ora ritorna false
    return false;
}

bool SAOCatalog::enrichWithSAO(std::shared_ptr<core::Star> star) {
    if (!star) return false;
    
    // Se già ha un numero SAO, non fare nulla
    if (star->getSAONumber().has_value()) {
        return true;
    }
    
    // PRIORITÀ 1: Prova con database locale usando Gaia ID
    if (localDatabase_->isAvailable() && star->getGaiaId() > 0) {
        auto sao = localDatabase_->findSAOByGaiaId(star->getGaiaId());
        if (sao.has_value()) {
            star->setSAONumber(sao.value());
            return true;
        }
    }
    
    // PRIORITÀ 2: Prova con database locale usando coordinate
    if (localDatabase_->isAvailable()) {
        auto sao = localDatabase_->findSAOByCoordinates(star->getCoordinates(), 5.0);
        if (sao.has_value()) {
            star->setSAONumber(sao.value());
            return true;
        }
    }
    
    // FALLBACK 3: Query online SIMBAD se disponibile Gaia ID
    if (star->getGaiaId() > 0) {
        auto sao = querySIMBADForSAO(star->getGaiaId());
        if (sao.has_value()) {
            star->setSAONumber(sao.value());
            return true;
        }
    }
    
    // FALLBACK 4: Query online VizieR con coordinate
    auto sao = crossMatchVizieR(star->getCoordinates(), 5.0);
    if (sao.has_value()) {
        star->setSAONumber(sao.value());
        return true;
    }
    
    return false;
}

bool SAOCatalog::hasLocalDatabase() const {
    return localDatabase_ && localDatabase_->isAvailable();
}

std::string SAOCatalog::getDatabaseStatistics() const {
    if (!localDatabase_) {
        return "Local database not initialized";
    }
    return localDatabase_->getStatistics();
}

} // namespace catalog
} // namespace starmap
