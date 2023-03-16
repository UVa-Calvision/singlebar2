# Single Bar

GEANT4-based simulation of a single bar or small matrix for DualReadout calorimeter studies.

Note: this code is based on the GEANT4 version 9 releases.  The optical photon tracing and scintiallor (material) description APIs are modified in the version 10 releases.  This this code  will require several code updates for version 10 compatibility.

## Requirements
The only requirement to run this code out of the box is to have a working installation of [cvmfs](https://cernvm.cern.ch/fs/).  

## Set up

The following works on the UVa NAS1 server:
```bash
source g4env.sh
cmake -DGeant4_DIR=/cvmfs/sft.cern.ch/lcg/releases/Geant4/10.05.p01-76df0/x86_64-centos7-gcc8-opt/lib64/Geant4-10.5.1
cmake --build .
or
make
```

For debug builds add -DCMAKE_BUILD_TYPE=Debug to original cmake command

Note: each time you open a new terminal, you will need to source the g4env.sh script to set up you environment.  The first cmake command is only needed the fist time the code is built.  For subsequent changes it is only necessary to type 'make'.

## View the geometry / run interactively

```bash
# Example: usage with GUI
./CV_Testbeam -c template.cfg -u Qt  
# -u <GUI>
```
## Run the events defined in run.mac

```bash
# Example: usage
./CV_Testbeam -c template.cfg -m run.mac -o test
# -c <config file>
# -m <macro file>
# -o <output file[.root]>
```
* The default run.mac simulates ten 100 GeV muons aimed arouns the axis of the crystals module.
* Use run1Mopticalphoton.mac to generate 1e6 optical photons randomly within the rear crystal.  Note that the particle source setting assumes the detauls cryatsl size and positioning in template.cfg.  When using the optical photon particle souece, the proper setting for the optical tracing flags is:
```
switchOnScintillation = 0
propagateScintillation = 1
switchOnCerenkov = 0
propagateCerenkov = 0
```
example run command:
``./CV_Testbeam -c templateOP.cfg -m run1Mopticalphoton.mac -o 1Mopticalphoton_1.root`

```

## Use the runner script

```bash
python runner.py -b <beam> -G <E in GeV> of -M <E in MeV> -n <events> -o<outputfile>
use standard GEANT particle names:  [e-,pi+,mu-,gamma,etc]
example: python runner.py -bpi+ -G10 -n10 -opi_test

usage: runner.py [-h] [-G GEV] [-M MEV] [-n NEVENT] [-b BEAM] [-d OUTDIR]
                 [-o OUTPUT] [-f] [-g SIPMGAPMATERIAL] [-w WRAPPERMATERIAL]
                 [-p]

GEANT Runner Script

optional arguments:
  -h, --help            show this help message and exit
  -G GEV, --GeV GEV     Particle energy in GeV
  -M MEV, --MeV MEV     Particle energy in MeV
  -n NEVENT, --nevent NEVENT
                        Number of events to run
  -b BEAM, --beam BEAM  Beam particle type (eg, pi-, mu-, e-)
  -d OUTDIR, --outdir OUTDIR
                        Output directory name [.]
  -o OUTPUT, --output OUTPUT
                        Output file name[.root]
  -f, --force           Force overwrite
  -g SIPMGAPMATERIAL, --SiPMgapMaterial SIPMGAPMATERIAL
                        Material between xtal face and SiPM ([1 air], 2
                        optical grease, 5 silcone)
  -w WRAPPERMATERIAL, --WrapperMaterial WRAPPERMATERIAL
                        Crystal wapper material (17 Epoxy, [18 Al])
  -a ANGLE, --angle ANGLE
                        Angle of bar [0]
  -y YSHIFT, --yshift YSHIFT
                        y-shift of bar [0]
  -p, --idealPolished   Use ideal polished surfaces
  -C CRYSTALTYPE, --CrystalType CRYSTALTYPE
                        Crystal type [14 PWO], 18 BGO
  -t, --tee             tee output to logfile and terminal

```

## Bulk running
use the generate_temp.sh in `./template`
```bash
cd template 
source generate_temp.sh
```


## Root file

__Histograms__

lambda of photons produced: (f)ront/(rear) crystal, Cherenkov/Scintillation light
- h_phot_lambda_ECAL_f_produce_Ceren
- h_phot_lambda_ECAL_f_produce_Scin
- h_phot_lambda_ECAL_r_produce_Ceren
- h_phot_lambda_ECAL_r_produce_Scin

time of photon production (ns)
- h_phot_time_ECAL_f_produce_Ceren
- h_phot_time_ECAL_f_produce_Scin
- h_phot_time_ECAL_r_produce_Ceren
- h_phot_time_ECAL_r_produce_Scin

time of photon detected: SiPM(F) in front SiPM(C/S) in rear, Cherenkov/Scintillation light
- h_phot_time_SiPMF_Ceren
- h_phot_time_SiPMF_Scin
- h_phot_time_SiPMC_Ceren
- h_phot_time_SiPMC_Scin
- h_phot_time_SiPMS_Ceren
- h_phot_time_SiPMS_Scin

photons collected in front sipm
- h_phot_lambda_SiPMF_f_Ceren
- h_phot_lambda_SiPMF_f_Scin

photons collected in rear sipms
- h_phot_lambda_SiPMC_r_Ceren
- h_phot_lambda_SiPMC_r_Scin 
- h_phot_lambda_SiPMS_r_Ceren
- h_phot_lambda_SiPMS_r_Scin

__Branches in tree__ (partial list)

beam parameters
- Branch("inputMomentum", "vector<float>", &inputMomentum)  3-momentum, MeV
- Branch("primaryID", &this->primaryID, "primaryID/I") beam particle ID

energy deposited in MeV, total and by ionization only
- Branch("depositedEnergyECAL_f", &this->depositedEnergyECAL_f, "depositedEnergyECAL_f/F") 
- Branch("depositedEnergyECAL_r", &this->depositedEnergyECAL_r, "depositedEnergyECAL_r/F") 
- Branch("depositedIonEnergyTotal", &this->depositedIonEnergyTotal, "depositedIonEnergyTotal/F")
- Branch("depositedIonEnergyECAL_f", &this->depositedIonEnergyECAL_f, "depositedIonEnergyECAL_f/F")
- Branch("depositedIonEnergyECAL_r", &this->depositedIonEnergyECAL_r, "depositedIonEnergyECAL_r/F")
  
count of Scint and Cherenkov photons produced in crystal bars
- Branch("ECAL_f_total_S", &this->ECAL_f_total_S, "ECAL_f_total_S/I")
- Branch("ECAL_f_total_C", &this->ECAL_f_total_C, "ECAL_f_total_C/I")
- Branch("ECAL_r_total_S", &this->ECAL_r_total_S, "ECAL_r_total_S/I")
- Branch("ECAL_r_total_C", &this->ECAL_r_total_C, "ECAL_r_total_C/I")  

count of photons exiting front and rear crystals
- Branch("ECAL_f_exit_S", &this->ECAL_f_exit_S, "ECAL_f_exit_S/I")
- Branch("ECAL_f_exit_C", &this->ECAL_f_exit_C, "ECAL_f_exit_C/I")
- Branch("ECAL_r_exit_S", &this->ECAL_r_exit_S, "ECAL_r_exit_S/I")
- Branch("ECAL_r_exit_C", &this->ECAL_r_exit_C, "ECAL_r_exit_C/I")

count of photons detected in three SiPM detectors
- Branch("SDFdetected_f_S", &this->SDFdetected_f_S, "SDFdetected_f_S/I")
- Branch("SDFdetected_f_C", &this->SDFdetected_f_C, "SDFdetected_f_C/I")
- Branch("SDCdetected_r_S", &this->SDCdetected_r_S, "SDCdetected_r_S/I")
- Branch("SDCdetected_r_C", &this->SDCdetected_r_C, "SDCdetected_r_C/I")
- Branch("SDSdetected_r_S", &this->SDSdetected_r_S, "SDSdetected_r_S/I")
- Branch("SDSdetected_r_C", &this->SDSdetected_r_C, "SDSdetected_r_C/I")


## GEANT4 Resources
- [GEANT4 Homepage](https://geant4.web.cern.ch/)
- [GEANT4 Tutorials/Schools](https://geant4.web.cern.ch/past-events)
  - Example [beginner's course](https://indico.cern.ch/event/865808/timetable/)


## Template to access and build the examples
```
ls $G4EXAMPLES
cd /tmp; mkdir geant-test; cd geant-test
# build a specific example, eg
cmake $G4EXAMPLES/basic/B5
make
./exampleB5
```

## How to fix your git
To remove local changes and reset to the remote
```
git fetch origin
git reset --hard origin/main
```

