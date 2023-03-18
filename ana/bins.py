#!/usr/bin/env python

from __future__ import print_function
import ROOT as r
import os, sys

f = r.TFile("test.root")
tree = f.tree

target_index = 3
i = 0
for event in tree:
  if i == target_index:
    total_energy = sum(event.ECAL_f_scin_bin_energy)
    print("Total energy: %f" % total_energy)
    print("Deposited energy: %f" % event.ECAL_f_depositedEnergy)
    print("Deposited ionizing energy: %f" % event.ECAL_f_depositedIonEnergy)

  i += 1
