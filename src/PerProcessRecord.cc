#include "PerProcessRecord.hh"

std::optional<ProcessType> readProcess(const G4String& processName) {
  if (processName == "Cerenkov") {
    return ProcessType::Ceren;
  } else if (processName == "Scintillation") {
    return ProcessType::Scin;
  } else {
    return std::nullopt;
  }
}

const std::array<ProcessType, 2> processTypes = {ProcessType::Scin, ProcessType::Ceren};

std::string processNameShort(ProcessType t) {
  switch (t) {
    case ProcessType::Ceren: return "Ceren";
    case ProcessType::Scin: return "Scin";
    default: return "Unknown";
  }
}

std::string processNameInitial(ProcessType t) {
  switch (t) {
    case ProcessType::Ceren: return "C";
    case ProcessType::Scin: return "S";
    default: return "Unknown";
  }
}
