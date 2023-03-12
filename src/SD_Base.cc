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

unsigned int processToInt(ProcessType type) {
  for (unsigned int i = 0; i < processTypes().size(); i++) {
    if (type == processTypes()[i]) {
      return i;
    }
  }
  return -1;
}

std::array<ProcessType, 2> processTypes() {
  return {ProcessType::Ceren, ProcessType::Scin};
}
