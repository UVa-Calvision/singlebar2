#include "CreateTree.hh"
#include <algorithm>
#include <iostream>
#include <type_traits>

using namespace std;

CreateTree *CreateTree::fInstance = NULL;

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

CreateTree::CreateTree(const TString name, TFile* file)
  : fFile(file)
{
  if (fInstance)
  {
    return;
  }

  this->fInstance = this;
  this->fname = name;
  this->ftree = new TTree(name, name);

  this->Clear();
}

int CreateTree::Fill()
{
  return this->GetTree()->Fill();
}

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

bool CreateTree::Write()
{
  fFile->cd();
  ftree->Write();

  for (auto&& [_, h]: Histograms) {
    h->Write();
  }

  return true;
}

void CreateTree::Clear()
{
  for (auto&& [_, f] : TreeFloats) {
    *f = 0.;
  }

  for (auto&& [_, i] : TreeInts) {
    *i = 0;
  }

  for (auto&& [_, v] : TreeFloatVectors) {
    for (auto& x : *v) {
      x = 0.;
    }
  }
}

TH1F* CreateTree::createHistogram(
  const std::string& name, const std::string& title,
  int nBuckets, double low, double high) {

  auto [iter, success] = Histograms.emplace(name, nullptr);

  // If the key doesn't exist, then we create the histogram
  if (success) {
    TH1F*& value = std::get<1>(*iter);
    value = new TH1F(name.c_str(), title.c_str(), nBuckets, low, high);
    return value;
  } else {
    // If the key doesn't exist, write an error
    std::cerr << "Histogram " << name << " already exists!\n";
    return nullptr;
  }
}

TH1F* CreateTree::lookupHistogram(const std::string& name) {
  return Histograms[name];
}

template <>
float* CreateTree::createBranch(const std::string& name) {
  return createNumericBranch(name, TreeFloats, "F");
}

template <>
float& CreateTree::lookupBranch(const std::string& name) {
  return *(TreeFloats[name]);
}

template <>
int* CreateTree::createBranch(const std::string& name) {
  return createNumericBranch(name, TreeInts, "I");
}

template <>
int& CreateTree::lookupBranch(const std::string& name) {
  return *(TreeInts[name]);
}

template <>
std::vector<int>* CreateTree::createBranch<std::vector<int>>(const std::string& name) {
  return createVectorBranch<int, 0>(name, TreeIntVectors, "vector<int>");
}

template <>
std::vector<int>& CreateTree::lookupBranch<std::vector<int>>(const std::string& name) {
  return *(TreeIntVectors[name]);
}

template <>
std::vector<float>* CreateTree::createBranch<std::vector<float>>(const std::string& name) {
  return createVectorBranch<float, 0>(name, TreeFloatVectors, "vector<float>");
}

template <>
std::vector<float>& CreateTree::lookupBranch<std::vector<float>>(const std::string& name) {
  return *(TreeFloatVectors[name]);
}

template <>
std::vector<float>* CreateTree::createBranch<float, 3>(const std::string& name) {
  return createVectorBranch<float, 3>(name, TreeFloatVectors, "vector<float>");
}

template <>
std::vector<float>& CreateTree::lookupBranch<float, 3>(const std::string& name) {
  return *(TreeFloatVectors[name]);
}

template <>
std::vector<float>* CreateTree::createBranch<float, 4>(const std::string& name) {
  return createVectorBranch<float, 4>(name, TreeFloatVectors, "vector<float>");
}

template <>
std::vector<float>& CreateTree::lookupBranch<float, 4>(const std::string& name) {
  return *(TreeFloatVectors[name]);
}

// ------ Helper for converting G4Vectors to std::vectors for the tree

void convertThreeVector(std::vector<float>& a, const G4ThreeVector& v) {
  a[0] = v[0];
  a[1] = v[1];
  a[2] = v[2];
}

void convertFourVector(std::vector<float>& a, const G4ThreeVector& v, G4double t) {
  a[0] = v[0];
  a[1] = v[1];
  a[2] = v[2];
  a[3] = t;
}
