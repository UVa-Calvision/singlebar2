#!/usr/bin/env python

from __future__ import print_function
import ROOT as r
import numpy as np
import os, sys

canvas = r.TCanvas("c", "c", 1600, 1000)
canvas.Divide(2, 2)

canvas.cd(1)
# hist_Birks = r.TGraph("birks", "Scintillation Yield Per Ionizing Energy Deposit", 

canvas.cd(2)
hist_delay = r.TH1D("delay", "Scintillation Delay Time;[ns]", 500, 0., 50.)

canvas.cd(3)
hist_perEvent = r.TH1D("perevent", "Scintillation Photons Per Event", 20, 0, 1000000)

nCerenPhotons = 0
nScinPhotons = 0
histCerenPhotons = 0
histScinPhotons = 0

hit_pos = r.std.vector("float")(0)
hit_energy = r.std.vector("float")(0)
hit_scins = r.std.vector("float")(0)
hit_detect = r.std.vector("float")(0)

for filename in sys.argv[1:]:
  #filename = sys.argv[1]
  print("Reading file: " + str(filename))
  rFile = r.TFile(filename)
  hist_delay.Add(rFile.h_phot_delay_time_ECAL_r_Scin)

  rTree = rFile.Get("tree")

  for event in rTree:
    nCerenPhotons += sum(event.ECAL_r_hit_photon_C)
    nScinPhotons += sum(event.ECAL_r_hit_photon_S)

    hist_perEvent.Fill(sum(event.ECAL_r_hit_photon_S))

    for i in range(len(event.ECAL_r_hit_energy)):
      if (event.ECAL_r_hit_energy[i] * 1000.0 < 1.5):
        hit_energy.push_back(event.ECAL_r_hit_energy[i] * 1000.0)
        hit_scins.push_back(event.ECAL_r_hit_photon_S[i])
        hit_detect.push_back(event.ECAL_r_hit_detected_S[i])
        hit_pos.push_back(event.ECAL_r_hit_zPos[i])

  histCerenPhotons += rFile.Get("h_phot_produce_time_ECAL_r_Ceren").Integral()
  histScinPhotons += rFile.Get("h_phot_produce_time_ECAL_r_Scin").Integral()

print("Total hit ceren photons: %d" % nCerenPhotons)
print("Total histogram ceren photons: %d" % histCerenPhotons)
print("Difference: %d" % (nCerenPhotons - histCerenPhotons))
print("\n")
print("Total hit scin photons: %d" % nScinPhotons)
print("Total histogram scin photons: %d" % histScinPhotons)
print("Difference: %d" % (nScinPhotons - histScinPhotons))

canvas.cd(1)
hist_Birks = r.TGraph(hit_energy.size(), hit_energy.data(), hit_scins.data())
f1 = r.TF1("fbirks", "[0] * x", 300.0)
hist_Birks.Fit(f1)
f1 = hist_Birks.GetFunction("fbirks")
f1.SetLineWidth(1)
hist_Birks.SetTitle("Scintillation Yield Per Ionizing Energy Deposit;Hit Energy Deposit [MeV];# Scintillation Photons")
hist_Birks.Draw("AP")

leg = r.TLegend(0.3, 0.7, 0.65, 0.8)
leg.AddEntry(hist_Birks, "slope = " + str(f1.GetParameter(0)))
leg.Draw()

canvas.cd(2)
#hist_delay = rFile.h_phot_delay_time_ECAL_r_Scin
f2 = r.TF1("fdelay", "[0] * exp(-x/5.0) + [1] * exp(-x/15.0)", hist_delay.Integral(), hist_delay.Integral())
hist_delay.Fit(f2)
f2 = hist_delay.GetFunction("fdelay")
f2.SetLineWidth(1)
hist_delay.Draw()

leg2 = r.TLegend(0.2, 0.7, 0.6, 0.8)
leg2.AddEntry(f2, "A fast = " + str(f2.GetParameter(0)))
leg2.AddEntry(f2, "A slow = " + str(f2.GetParameter(1)))
leg2.Draw()

canvas.cd(3)
hist_perEvent.Draw()

# canvas.cd(4)
# hist_detected = r.TGraph(hit_pos.size(), hit_pos.data(), hit_detect.data())
# hist_detected.SetTitle("blah;Z Position [mm];# Photons")
# hist_detected.Draw("AP")

print('Hit return to exit')
sys.stdout.flush() 
if sys.version_info.major==2:
    raw_input('')
else:
    input()

# save_filename = "scintillation_yield.png"
# print("Saving to: " + save_filename)
# canvas.Print(save_filename)
