#!/usr/bin/env python

from __future__ import print_function
import ROOT as r
import numpy as np
import os, sys

def makeGraph(ys, offset, length):
  xs = np.arange(offset, offset + length, length / float(ys.shape[0]))
  return r.TGraph(len(ys), xs, ys)

f = r.TFile("test.root")
tree = f.tree

f_frontOffset = 0.
f_crystalLength = 0.
f_energy = np.array([])

r_energy = np.array([])
r_frontOffset = 0.
r_crystalLength = 0.

target_index = 3
i = 0
for event in tree:
  if i == target_index:
    print("Found bin {}".format(i))
    print("Front:")
    f_energy = np.array(event.ECAL_f_scin_bin_energy).flatten('C').astype('float')
    print(f_energy.shape[0])
    f_frontOffset = event.ECAL_f_frontOffset
    print(f_frontOffset)
    f_crystalLength = event.ECAL_f_crystalLength
    print(f_crystalLength)

    print("Rear:")
    r_energy = np.array(event.ECAL_r_scin_bin_energy).flatten('C').astype('float')
    print(r_energy.shape[0])
    r_frontOffset = event.ECAL_r_frontOffset
    print(r_frontOffset)
    r_crystalLength = event.ECAL_r_crystalLength
    print(r_crystalLength)

    # total_energy = sum(event.ECAL_f_scin_bin_energy)
    # print("Total energy: %f" % total_energy)
    # print("Deposited energy: %f" % event.ECAL_f_depositedEnergy)
    # print("Deposited ionizing energy: %f" % event.ECAL_f_depositedIonEnergy)
  i += 1

r_frontOffset -= f_frontOffset
f_frontOffset -= f_frontOffset

tc = r.TCanvas()
mg = r.TMultiGraph("mg", "Total Scintillation Energy Per Bin;z [mm];E [GeV]")

g_f = makeGraph(f_energy, f_frontOffset, f_crystalLength)
g_r = makeGraph(r_energy, r_frontOffset, r_crystalLength)

mg.Add(g_f)
mg.Add(g_r)
mg.Draw("AL")

tc.Update()

print('Hit return to exit')
sys.stdout.flush() 
if sys.version_info.major==2:
    raw_input('')
else:
    input()
