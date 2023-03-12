#ifndef SD_Base_h
#define SD_Base_h

#include <array>
#include <string>
#include <string_view>
#include <optional>

#include "G4VSensitiveDetector.hh"
#include "CreateTree.hh"

enum class ProcessType {
  Scin,
  Ceren,
};

std::optional<ProcessType> readProcess(const G4String& processName);
unsigned int processToInt(ProcessType type);
std::array<ProcessType, 2> processTypes();


class G4Step;
class GHCofThisEvent;

template <typename T>
  struct PerProcess {
    PerProcess() : values({nullptr, nullptr}) {}

    constexpr T& operator[](ProcessType t) {
      return *(values[processToInt(t)]);
    }

    void set(ProcessType t, T* v) {
      values[processToInt(t)] = v;
    }

  private:
    std::array<T*, 2> values;
};

template <typename Impl>
class SD_Base : public G4VSensitiveDetector
{
public:

  SD_Base(const G4String& name)
    : G4VSensitiveDetector(name)
  {}

protected:

  PerProcess<TH1F> createHistogram(const std::string& name, const std::string& title,
                                  int nBuckets, double low, double high) {
    return createPerProcess<TH1F>([&](ProcessType t) {
        return CreateTree::Instance()->createHistogram(Impl::HistogramName(name, t), title, nBuckets, low, high);
      });
  }

  PerProcess<int> createInt(const std::string& name) {
    return createPerProcess<int>([&](ProcessType t) {
        return CreateTree::Instance()->createInt(Impl::BranchName(name, t));
      });
  }

  PerProcess<float> createFloat(const std::string& name) {
    return createPerProcess<float>([&](ProcessType t) {
        return CreateTree::Instance()->createFloat(Impl::BranchName(name, t));
      });
  }
  
  PerProcess<std::vector<float>> createVector(const std::string& name) {
    return createPerProcess<std::vector<float>>([&](ProcessType t) {
        return CreateTree::Instance()->createVector(Impl::BranchName(name, t));
      });
  }

private:
  template <typename T, typename CreateFunc>
  PerProcess<T> createPerProcess(const CreateFunc& f) {
    PerProcess<T> vals;
    for (ProcessType t : processTypes()) {
      vals.set(t, f(t));
    }
    return vals;
  }
};

#endif
