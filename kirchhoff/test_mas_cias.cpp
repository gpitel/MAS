// Minimal dependency-free unit tests for mas_to_cias (ideal coupled-inductor leaf).
#include "MasConverter.hpp"
#include "Fidelity.hpp"
#include <nlohmann/json.hpp>
#include <iostream>

using nlohmann::json;
using PEAS::Fidelity;

#include <catch2/catch_test_macros.hpp>
#define CHECK_MSG(cond, ...) do { INFO(__VA_ARGS__); CHECK(cond); } while (0)

static double atom_lp(const json& leaf) {
    return leaf.at("components").at(0).at("data").at("inputs").at("designRequirements")
               .at("magnetizingInductance").at("nominal").get<double>();
}
static const json& atom_ratios(const json& leaf) {
    return leaf.at("components").at(0).at("data").at("inputs").at("designRequirements").at("turnsRatios");
}

TEST_CASE("MAS mas_to_cias", "[mas]") {
    // ideal flyback transformer: Lp = 1mH, one secondary, n = 4
    json doc = json::parse(R"({
        "magnetic": {},
        "inputs": { "designRequirements": {
            "magnetizingInductance": { "nominal": 1e-3 },
            "turnsRatios": [ { "nominal": 4.0 } ] } }
    })");
    json leaf = MAS::mas_to_cias(doc, Fidelity(Fidelity::Origin::REQUIREMENTS));

    // 2-winding transformer -> 4 winding terminals, ONE magnetic component (coupling stays inside it)
    CHECK_MSG(leaf.at("ports").size() == 4, "ideal: 4 winding terminals (1 pri + 1 sec)");
    CHECK_MSG(leaf.at("ports").at(0).at("name") == "primary_start", "ideal: port[0]=primary_start");
    CHECK_MSG(leaf.at("ports").at(2).at("name") == "secondary1_start", "ideal: port[2]=secondary1_start");
    CHECK_MSG(leaf.at("components").size() == 1, "ideal: ONE magnetic component (not L+L+K edges)");
    CHECK_MSG(leaf.at("components").at(0).at("name") == "T", "ideal: component T");
    CHECK_MSG(atom_lp(leaf) == 1e-3, "ideal: Lp from magnetizingInductance (1mH)");
    CHECK_MSG((atom_ratios(leaf).size() == 1 && atom_ratios(leaf).at(0).at("nominal") == 4.0),
          "ideal: turnsRatios carried (n=4)");
    CHECK_MSG(leaf.at("connections").size() == 4, "ideal: 4 winding nets");

    // two-secondary transformer -> 6 terminals
    json doc2 = json::parse(R"({
        "magnetic": {},
        "inputs": { "designRequirements": {
            "magnetizingInductance": { "minimum": 0.9e-3, "maximum": 1.1e-3 },
            "turnsRatios": [ { "nominal": 4.0 }, { "nominal": 2.0 } ] } }
    })");
    json leaf2 = MAS::mas_to_cias(doc2, Fidelity(Fidelity::Origin::REQUIREMENTS));
    CHECK_MSG(leaf2.at("ports").size() == 6, "two-secondary: 6 terminals");
    CHECK_MSG(atom_lp(leaf2) == 1e-3, "two-secondary: Lp resolves (min+max)/2 = 1mH");

    // DATASHEET / MKF_MODEL must throw (Phase 4)
    bool d1 = false, d2 = false;
    try { MAS::mas_to_cias(doc, Fidelity(Fidelity::Origin::DATASHEET)); } catch (const std::exception&) { d1 = true; }
    try { MAS::mas_to_cias(doc, Fidelity(Fidelity::Origin::MKF_MODEL)); } catch (const std::exception&) { d2 = true; }
    CHECK_MSG(d1, "DATASHEET origin throws (Phase 4 TODO)");
    CHECK_MSG(d2, "MKF_MODEL origin throws (Phase 4 TODO)");

}
