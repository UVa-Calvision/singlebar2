#ifndef SD_Base_h
#define SD_Base_h

#include "PerProcessRecord.hh"

/*
 * SD_Base implements methods that all detectors might find useful.
 */
template <typename Impl>
class SD_Base : public G4VSensitiveDetector
{
public:

  SD_Base(const G4String& name)
    : G4VSensitiveDetector(name)
  {}

protected:

  // PerProcess create helpers for branch variables. Don't use a templated function
  // because (template class)::(template method) calls get messy. 
  //
  // Impl::HistogramName gives a unique name to the histogram
  // Impl::BranchName gives a unique name to branch variables

  PerProcess<TH1F> createHistogram(const std::string& name, const std::string& title,
                                  int nBuckets, double low, double high) {
    return PerProcess<TH1F>::createHistogram(Impl::HistogramName, name, title, nBuckets, low, high);
  }

  PerProcess<int> createInt(const std::string& name) {
    return PerProcess<int>::createBranch(Impl::BranchName, name);
  }

  PerProcess<float> createFloat(const std::string& name) {
    return PerProcess<float>::createBranch(Impl::BranchName, name);
  }

  PerProcess<std::vector<float>> createVector(const std::string& name) {
    return PerProcess<std::vector<float>>::createBranch(Impl::BranchName, name);
  }
};

#endif
