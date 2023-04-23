#!/usr/bin/env python

from __future__ import print_function
import ROOT as r
import numpy as np
import os, sys

canvas = r.TCanvas("c", "c", 1600, 1000)
canvas.Divide(2,1)

filename = sys.argv[1]
print("Reading file: " + str(filename))
rFile = r.TFile(filename)

canvas.cd(1)
hist_createdPhotons = rFile.h_phot_produce_pos_ECAL_r_Scin
hist_createdPhotons.Draw()

hist_detectedPhotons = rFile.h_phot_creationPos_SiPMS_Scin
hist_detectedPhotons.Add(rFile.h_phot_creationPos_SiPMC_Scin)
hist_detectedPhotons.Draw("SAME")

leg = r.TLegend()
leg.AddEntry(hist_createdPhotons, "Created Photons")
leg.AddEntry(hist_detectedPhotons, "Detected Photons")
leg.Draw()

canvas.cd(2)
hist_efficiency = hist_detectedPhotons.Clone()
hist_efficiency.Divide(hist_createdPhotons)
hist_efficiency.SetTitle("Detection Efficiency;Position [mm]")
hist_efficiency.Draw()

# print('Hit return to exit')
# sys.stdout.flush() 
# if sys.version_info.major==2:
#     raw_input('')
# else:
#     input()

save_filename = "efficiency.png"
print("Saving to: " + save_filename)
canvas.Print(save_filename)
