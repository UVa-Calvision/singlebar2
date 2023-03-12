#include "SD_Base.hh"

std::optional<ProcessType> readProcess(const G4String& processName) {
  if (processName == "Cerenkov") {
    return ProcessType::Ceren;
  } else if (processName == "Scintilation") {
    return ProcessType::Scin;
  } else {
    return std::nullopt;
  }
}

const std::array<ProcessType, 2> processTypes = {ProcessType::Scin, ProcessType::Ceren};

