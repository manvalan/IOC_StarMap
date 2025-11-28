#include "starmap/catalog/GaiaClient.h"
#include <ioc_gaialib/unified_gaia_catalog.h>
#include <ioc_gaialib/types.h>
#include <cmath>
#include <cstdlib>

namespace starmap {
namespace catalog {

class GaiaClient::Impl {
public:
    Impl() {
        std::string home = getenv("HOME");
        // Configurazione corretta per UnifiedGaiaCatalog
        std::string config = R"({
            "catalog_type": "multifile_v2",
            "multifile_directory": ")" + home + R"(/.catalog/gaia_mag18_v2_multifile",
            "max_cached_chunks": 100,
            "log_level": "info"
        })";
        
        available_ = ioc::gaia::UnifiedGaiaCatalog::initialize(config);
    }
    
    ~Impl() {
        ioc::gaia::UnifiedGaiaCatalog::shutdown();
    }
    
    bool available_ = false;
};

GaiaClient::GaiaClient() : pImpl_(std::make_unique<Impl>()) {}
GaiaClient::~GaiaClient() = default;

bool GaiaClient::isAvailable() const {
    return pImpl_->available_;
}

std::vector<std::shared_ptr<core::Star>> GaiaClient::queryRegion(
    const GaiaQueryParameters& params) {
    
    std::vector<std::shared_ptr<core::Star>> stars;
    
    if (!pImpl_->available_) return stars;
    
    auto& catalog = ioc::gaia::UnifiedGaiaCatalog::getInstance();
    
    // Usa QueryParams (API corretta da types.h)
    ioc::gaia::QueryParams qp;
    qp.ra_center = params.center.getRightAscension();
    qp.dec_center = params.center.getDeclination();
    qp.radius = params.radiusDegrees;
    qp.max_magnitude = params.maxMagnitude;
    
    auto gaiaStars = catalog.queryCone(qp);
    
    for (const auto& gs : gaiaStars) {
        auto star = std::make_shared<core::Star>();
        star->setGaiaId(gs.source_id);
        star->setCoordinates(core::EquatorialCoordinates(gs.ra, gs.dec));
        star->setMagnitude(gs.phot_g_mean_mag);
        if (gs.parallax > 0) star->setParallax(gs.parallax);
        star->setProperMotionRA(gs.pmra);
        star->setProperMotionDec(gs.pmdec);
        double bpRp = gs.getBpRpColor();
        if (!std::isnan(bpRp)) star->setColorIndex(bpRp);
        
        // Imposta il nome IAU se disponibile (usa getDesignation())
        std::string designation = gs.getDesignation();
        if (!designation.empty()) {
            star->setName(designation);
        }
        
        stars.push_back(star);
    }
    
    if (params.maxResults > 0 && stars.size() > static_cast<size_t>(params.maxResults)) {
        stars.resize(params.maxResults);
    }
    
    return stars;
}

std::shared_ptr<core::Star> GaiaClient::queryById(long long gaiaId) {
    if (!pImpl_->available_) return nullptr;
    
    auto& catalog = ioc::gaia::UnifiedGaiaCatalog::getInstance();
    auto result = catalog.queryBySourceId(static_cast<uint64_t>(gaiaId));
    
    if (result.has_value()) {
        const auto& gs = result.value();
        auto star = std::make_shared<core::Star>();
        star->setGaiaId(gs.source_id);
        star->setCoordinates(core::EquatorialCoordinates(gs.ra, gs.dec));
        star->setMagnitude(gs.phot_g_mean_mag);
        if (gs.parallax > 0) star->setParallax(gs.parallax);
        std::string designation = gs.getDesignation();
        if (!designation.empty()) star->setName(designation);
        return star;
    }
    
    return nullptr;
}

std::shared_ptr<core::Star> GaiaClient::queryByName(const std::string& name) {
    if (!pImpl_->available_) return nullptr;
    
    auto& catalog = ioc::gaia::UnifiedGaiaCatalog::getInstance();
    auto result = catalog.queryByName(name);
    
    if (result.has_value()) {
        const auto& gs = result.value();
        auto star = std::make_shared<core::Star>();
        star->setGaiaId(gs.source_id);
        star->setCoordinates(core::EquatorialCoordinates(gs.ra, gs.dec));
        star->setMagnitude(gs.phot_g_mean_mag);
        if (gs.parallax > 0) star->setParallax(gs.parallax);
        star->setProperMotionRA(gs.pmra);
        star->setProperMotionDec(gs.pmdec);
        // Usa la designazione dal catalogo o il nome cercato
        std::string designation = gs.getDesignation();
        star->setName(!designation.empty() ? designation : name);
        return star;
    }
    
    return nullptr;
}

} // namespace catalog
} // namespace starmap
