from __future__ import print_function
import ROOT as r
import sys


fname = '1Mopticalphoton_1.root'


tf = r.TFile(fname)
tree = tf.Get("tree")

# Display parameters of beam
tc_beam = r.TCanvas()
tc_beam.Divide(3,2)
tc_beam.cd(1)
h_E = r.TH1F("h_E","single photon E;eV",50,1,4.5)
tree.Draw("inputMomentum[3]*1e9>>h_E")
tc_beam.cd(2)
h_lambda = r.TH1F("h_lambda","single photon lambda;nm",50,200,1200)
lambd="1239.8/(inputMomentum[3]*1e9)"  # nm
tree.Draw(lambd+">>h_lambda")
tc_beam.cd(3)
h_z = r.TH1F("h_z","Z start;mm",50,200,420)
tree.Draw("inputInitialPosition[2]>>h_z")
tc_beam.cd(4)
h_xy = r.TH2F("h_xy","X-Y start;mm;mm",20,-10,10,20,-10,10)
tree.Draw("inputInitialPosition[0]:inputInitialPosition[1]>>h_xy","","colz")
tc_beam.cd(5)
h_theta = r.TH1F("h_theta","theta start",50,0,3.3)
theta='atan2(sqrt(inputMomentum[0]*inputMomentum[0]+inputMomentum[1]*inputMomentum[1]),inputMomentum[2])'
tree.Draw(theta+">>h_theta")
tc_beam.cd(6)
h_phi = r.TH1F("h_phi","phi start",50,-3.3,3.3)
phi='atan2(inputMomentum[1],inputMomentum[0])'
tree.Draw(phi+">>h_phi")


# Display some data on detected photons
tc_ana = r.TCanvas()
tc_ana.Divide(3,2)
tc_ana.cd(1)
h_lambda_det = r.TH1F("h_lambda_det","single photon lambda,detected;nm",50,300,1000)
tree.Draw(lambd+">>h_lambda_det","SDSdetected_r_S+SDCdetected_r_S>0")
tc_ana.cd(2)
h_z_det = r.TH1F("h_z_det","starting Z position, detected;mm",50,200,420)
tree.Draw("inputInitialPosition[2]>>h_z_det","SDSdetected_r_S+SDCdetected_r_S>0")
tc_ana.cd(3)
h_z_det_v_lambda = r.TH2F("h_z_det_v_lambda","lambda vs positiondetected;nm;mm",20,300,1000,20,200,420)
tree.Draw("inputInitialPosition[2]:"+lambd+">>h_z_det_v_lambda","SDSdetected_r_S+SDCdetected_r_S>0","colz")
tc_ana.cd(4)
h_pz_det = r.TH1F("h_pz_det","detected vs Zstart, pZ>0;mm",50,200,420)
tree.Draw("inputInitialPosition[2]>>h_pz_det","SDSdetected_r_S+SDCdetected_r_S>0&inputMomentum[2]>0")
h_nz_det = r.TH1F("h_nz_det","detected vs Zstart, pZ<0;mm",50,200,420)
tree.Draw("inputInitialPosition[2]>>h_nz_det","SDSdetected_r_S+SDCdetected_r_S>0&inputMomentum[2]<0")
h_nz_det.SetLineColor(r.kRed)
h_pz_det.Draw()
h_nz_det.Draw("same")
tc_ana.cd(4).BuildLegend()


# Slow(!) python access method to take a quick looh at some shots

i=0
for event in tree:
    print("Start location = ({:.1f},{:.1f},{:.1f}) mm".format(
          event.inputInitialPosition[0],event.inputInitialPosition[1],
          event.inputInitialPosition[2]))
    print("Start P,E = ({:.1f},{:.1f},{:.1f},{:.1f}) eV".format(
          event.inputMomentum[0]*1e9,event.inputMomentum[1]*1e9,
          event.inputMomentum[2]*1e9,event.inputMomentum[3]*1e9))
    print("Detected:",event.SDSdetected_r_S+event.SDCdetected_r_S>0)

    i=i+1
    if i>=10: break

print("hit enter to end program")
# hack to work on python2 or python3
try:
    raw_input()
except:
    input()

