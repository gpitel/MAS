// PyMAS — pybind11 module exposing the MAS->CIAS converter.
#include <pybind11/pybind11.h>
#include <pybind11_json/pybind11_json.hpp>
#include "MasConverter.hpp"
#include "FidelityJson.hpp"

namespace py = pybind11;
using json = nlohmann::json;

PYBIND11_MODULE(PyMAS, m) {
    m.doc() = "MAS (magnetic) -> CIAS leaf converter";
    m.def("mas_to_cias",
          [](const json& peas, const json& fidelity, const std::string& name) {
              return MAS::mas_to_cias(peas, PEAS::fidelity_from_json(fidelity), name);
          },
          py::arg("peas"), py::arg("fidelity"), py::arg("name") = "transformer",
          "Convert a MAS magnetic PEAS document to a CIAS leaf (ideal coupled-inductor; "
          "DATASHEET/MKF_MODEL origins are Phase 4).");
}
