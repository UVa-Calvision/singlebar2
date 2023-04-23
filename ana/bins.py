#!/usr/bin/env python

from __future__ import print_function
import ROOT as r
import numpy as np
import os, sys

def makeHist(name, title, offset, length):
  return r.TH2D(name, title,
                40, offset, offset + length,
                40, 0.0, 0.01)

canvas = r.TCanvas("c", "c", 1600, 1000)
canvas.Divide(2, 1)

rFront = 217.5
rLength = 180.0

canvas.cd(1)
hist_r = makeHist("energypos_hist_r", "Rear Energy x Position deposits", rFront, rLength)

canvas.cd(2)
hist_er = r.TH1D("energyavg_hist_r", "Rear Energy Deposit", 40, rFront, rFront + rLength)


def processFile(filename):
  print("Processing file: " + filename)
  rFile = r.TFile(filename, "READ")
  rTree = rFile.Get("tree")

  for event in rTree:
    for i in range(len(event.ECAL_r_hit_energy)):
      pos = event.ECAL_r_hit_zPos[i]
      energy = event.ECAL_r_hit_energy[i]
      hist_r.Fill(pos, energy)
      hist_er.Fill(pos, energy)



for filename in sys.argv[1:]:
  processFile(filename)

canvas.cd(1)
hist_r.Draw("COLZ")

canvas.cd(2)
hist_er.Draw("B C")



print('Hit return to exit')
sys.stdout.flush() 
if sys.version_info.major==2:
    raw_input('')
else:
    input()

# save_filename = os.path.splitext(filename)[0] + ".png"
# print("Saving to: " + save_filename)
# canvas.Print(save_filename)
