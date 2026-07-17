#pragma once

// MasConverter — "generate a CIAS element (leaf) from a MAS magnetic part".
//
// MAS is the only family with THREE origins:
//   REQUIREMENTS (ideal): the coupled-inductor model from inputs.designRequirements
//       (magnetizingInductance + turnsRatios). Implemented here.
//   DATASHEET           : from datasheetInfo per-winding inductance/turns — Phase 4.
//   MKF_MODEL           : full physical model via MKF export_magnetic_as_subcircuit — Phase 4.
//
// Per the CIAS rule, a transformer is ONE multi-winding component (coupling lives INSIDE its MAS
// model, never as a CIAS edge). So the leaf has a single magnetic component "T" whose winding
// terminals are the pins primary_start/primary_end/secondary{i}_start/secondary{i}_end. The CIAS
// emitter (P2) expands that one component into L (per winding) + K (coupling) when emitting SPICE.

#include <nlohmann/json.hpp>
#include <string>
#include "Fidelity.hpp"

namespace MAS {

nlohmann::json mas_to_cias(const nlohmann::json& peas,
                           const PEAS::Fidelity& fidelity,
                           const std::string& name = "transformer");

} // namespace MAS
