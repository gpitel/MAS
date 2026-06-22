// libMAS — emscripten/embind module for the MAS->CIAS converter (JSON-string ABI).
#include <emscripten/bind.h>
#include "MasConverter.hpp"
#include "FidelityJson.hpp"
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

static std::string mas_to_cias_json(std::string peasStr, std::string fidelityStr) {
    try {
        auto leaf = MAS::mas_to_cias(json::parse(peasStr),
                                     PEAS::fidelity_from_json(json::parse(fidelityStr)));
        return leaf.dump();
    } catch (const std::exception& e) {
        return std::string("Exception: ") + e.what();
    }
}

EMSCRIPTEN_BINDINGS(mas) {
    emscripten::function("mas_to_cias", &mas_to_cias_json);
}
