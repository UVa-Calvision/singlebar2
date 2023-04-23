#ifndef CreateTree_H
#define CreateTree_H 1

#include <iostream>
#include <vector>
#include "TString.h"
#include <map>
#include <unordered_map>
#include <memory>

#include "TParameter.h"
#include "TH2F.h"
#include "TH3F.h"
#include "TFile.h"
#include "TTree.h"
#include "TNtuple.h"

#include "G4ThreeVector.hh"

// to do: clean up unused values!!

class CreateTree
{
public:
  CreateTree(const TString name, TFile* file);

  TTree *GetTree() const { return ftree; };
  TString GetName() const { return fname; };
   
  int Fill();
  bool Write();
  void Clear();

  static CreateTree *Instance() { return fInstance; };
  static CreateTree *fInstance;

  template <typename T>
  void WriteObject(const T& value, const std::string& name) {
    std::cout << "Writing parameter " << name << ": " << value << "\n";
    TParameter<T> v(name.c_str(), value);
    fFile->cd();
    fFile->WriteTObject(&v);
  }

  // template <typename T>
  // void WriteObject(const T& value, const std::string& name) {
  //   const T temp = value;
  //   WriteObject<T>(&temp, name);
  // }

  // ----- Histograms

  // Creates a histogram in the root file and returns its handle
  TH1F* createHistogram(const std::string& name, const std::string& title, int nBuckets, double low, double high);

  // Finds the histogram handle for the given name
  TH1F* lookupHistogram(const std::string& name);

  // ----- Tree branches

  // The createBranch methods create a branch in the tree for the provided name, and
  // allocate memory which the tree will store whenever Fill() is called. The templates
  // are deleted so only listed specializations can be written to the tree.
  
  template <typename T>
  T* createBranch(const std::string& name) = delete;

  template <typename T>
  T& lookupBranch(const std::string& name) = delete;


  template <typename T, unsigned int N>
  std::vector<float>* createBranch(const std::string& name) = delete;

  template <typename T, unsigned int N>
  std::vector<float>& lookupBranch(const std::string& name) = delete;


private:
  TTree* ftree;
  TFile* fFile;
  TString fname;

  std::map<std::string, TH1F*> Histograms;

  // Need unique_ptrs so that memory locations aren't changed when map resizes.
  std::map<std::string, std::unique_ptr<float>> TreeFloats;
  std::map<std::string, std::unique_ptr<int>> TreeInts;
  std::map<std::string, std::unique_ptr<std::vector<int>>> TreeIntVectors;
  std::map<std::string, std::unique_ptr<std::vector<float>>> TreeFloatVectors;

  // Helper function for creating branches of ints, floats, doubles, etc.
  template <typename T>
  T* createNumericBranch(const std::string& name,
    std::map<std::string, std::unique_ptr<T>>& ptrTree,
    const std::string& rootTypeName) {

    auto [iter, success] = ptrTree.emplace(name, nullptr);
    
    if (success) {
      std::unique_ptr<T>& value = std::get<1>(*iter);
      value = std::make_unique<T>(T{});
      GetTree()->Branch(name.c_str(), value.get(), (name + "/" + rootTypeName).c_str());
      return value.get();
    } else {
      std::cerr << rootTypeName << " " << name << " already exists in the tree!\n";
      return nullptr;
    }
  }

  // Helper function for creating arrays of ints, floats, doubles, etc.
  template <typename T, unsigned int N>
  std::vector<T>* createVectorBranch(const std::string& name,
      std::map<std::string, std::unique_ptr<std::vector<T>>>& ptrTree,
      const std::string& typeName) {

    auto [iter, success] = ptrTree.emplace(name, nullptr);
    if (success) {
      std::unique_ptr<std::vector<T>>& value = std::get<1>(*iter);
      value = std::make_unique<std::vector<T>>(N, T{});
      GetTree()->Branch(name.c_str(), typeName.c_str(), value.get());
      return value.get();
    } else {
      std::cerr << typeName << " " << name << " already exists in the tree!\n";
      return nullptr;
    }
  }
};

// ----- createBranch specializations

template <> int* CreateTree::createBranch(const std::string&);
template <> float* CreateTree::createBranch(const std::string&);
template <> std::vector<int>* CreateTree::createBranch<std::vector<int>>(const std::string&);
template <> std::vector<float>* CreateTree::createBranch<std::vector<float>>(const std::string&);
template <> std::vector<float>* CreateTree::createBranch<float, 3>(const std::string&);
template <> std::vector<float>* CreateTree::createBranch<float, 4>(const std::string&);

template <> int& CreateTree::lookupBranch(const std::string&);
template <> float& CreateTree::lookupBranch(const std::string&);
template <> std::vector<int>& CreateTree::lookupBranch<std::vector<int>>(const std::string&);
template <> std::vector<float>& CreateTree::lookupBranch<std::vector<float>>(const std::string&);
template <> std::vector<float>& CreateTree::lookupBranch<float, 3>(const std::string&);
template <> std::vector<float>& CreateTree::lookupBranch<float, 4>(const std::string&);

// ----- Utility methods for wrangling G4 types into the tree

// Writes values of v into the 3-vector a
void convertThreeVector(std::vector<float>& a, const G4ThreeVector& v);

// Writes values of v into the first three components of a, and then writes t
// into the fourth component of a
void convertFourVector(std::vector<float>& a, const G4ThreeVector& v, G4double t);

#endif
