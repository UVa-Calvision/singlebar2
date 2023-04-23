#!/usr/bin/env python

###! /usr/bin/env python

# note python2 is assumed b/c we're using CMS releases
from __future__ import print_function
import fileinput
import argparse
import sys
import subprocess


# helper function to replace parameter strings in our cfg files
def update_param(file,tp):
    #print(file)
    for line in fileinput.input(file, inplace=True):
        # inside this loop the STDOUT will be redirected to the file
        # the comma after each print statement is needed to avoid 
        # double line breaks
        # eg print line.replace("100", "helloworld"),
        if line.startswith(tp[0]):
            print(tp[0],tp[1])
        else: print (line.strip()),
            
# using template cfg files, make files for this run
import tempfile, shutil, os


def check_env():
    try:  
        os.environ["G4INSTALL"]
    except KeyError: 
        print("You must source g4env.sh before running GEANT")
        sys.exit(1)


if __name__ == '__main__':
    check_env()
    parser = argparse.ArgumentParser(description='GEANT Runner Script')
    parser.add_argument("-G", "--GeV", default=-1, type=float,
                        help="Particle energy in GeV"
    )
    parser.add_argument("-M", "--MeV", default=-1, type=float,
                        help="Particle energy in MeV"
    )
    parser.add_argument("-n", "--nevent", type=int, default=100,
                        help="Number of events to run [100]"
    )
    parser.add_argument("-b", "--beam", default="e-",
                        help="Beam particle type (eg, pi-, mu-, e-)"
    )
    parser.add_argument("-d", "--outdir", default=".",
                        help="Output directory name [.]"
    )
    parser.add_argument("-o", "--output", default="test",
                        help="Output file name[.root]"
    )
    parser.add_argument("-f", "--force", default=None,
                        help="Force overwrite",
                        action="store_true"
    )
    parser.add_argument("-g", "--SiPMgapMaterial", default="1",
                        help="Material between xtal face and SiPM ([1 air], 2 optical grease, 5 silcone)"
    )
    # to do: fix material 17
    parser.add_argument("-w", "--WrapperMaterial", default="18",
                        help="Crystal wapper material (17 Epoxy, [18 Al])"  
    )
    parser.add_argument("-a", "--angle", default="0",
                        help="Angle of bar [0]"  
    )
    parser.add_argument("-y", "--yshift", default="0",
                        help="y-shift of bar [0]"  
    )
    parser.add_argument("-p", "--idealPolished", default=None,
                        help="Use ideal polished surfaces",
                        action="store_true"
    )
    parser.add_argument("-C", "--CrystalType", default="14",
                        help="Crystal type [14 PWO], 18 BGO"
    )
    parser.add_argument("-t", "--tee", default=False,
                        help="tee output to logfile and terminal",
                        action="store_true"
    )
    parser.add_argument("-T", "--template", default="template.cfg",
                        help="template.cfg file to use"
    )

    
    args = parser.parse_args()
    
    if args.GeV>0: 
        E=args.GeV
        Eunit="GeV"
    elif args.MeV>0:
        E=args.MeV
        Eunit="MeV"
    else:
        print("Invalid particle energy, use -G or -M flag")
        sys.exit(1)
    beam=args.beam
    outfile=args.output
    if outfile.endswith(".root"): outfile=outfile[:-5]
    if os.path.exists(outfile+".root"):
        if args.force:
            print("Replacing",outfile+".root")
        else:
            print(outfile+".root exists, exiting...")
            sys.exit(1)

    if not args.outdir==".":
        if not os.path.exists(args.outdir):
            os.makedirs(args.outdir)
        

    print("Simulating",args.beam,"beam at",E,Eunit)
    print("Output:",outfile+".root");
    
    # define the cfg and logfile names
    run_mac = args.outdir+"/"+outfile+'.runmac'
    shutil.copy2('run.mac', run_mac)
    temp_cfg = args.outdir+"/"+outfile+'.cfg'
    shutil.copy2(args.template,temp_cfg)
    outfile=args.outdir+"/"+outfile
    logfile=outfile+".log"

    print('generating config files',run_mac,temp_cfg)
    update_param(run_mac,['/run/beamOn',args.nevent])
    update_param(run_mac,['/gps/energy',str(E)+" "+Eunit])
    update_param(run_mac,['/gps/particle',args.beam])
    update_param(temp_cfg,['gap_material',' = '+args.SiPMgapMaterial])
    update_param(temp_cfg,['wrap_material',' = '+args.WrapperMaterial])
    update_param(temp_cfg,['ecal_material',' = '+args.CrystalType])
    update_param(temp_cfg,['ecal_incline',' = '+args.angle])
    update_param(temp_cfg,['ecal_yshift',' = '+args.yshift])
    if args.idealPolished:
        update_param(temp_cfg,['ecal_surface = 0'])
    
    EXE=os.getcwd()+'/CV_Testbeam'
    
    command = 'time ' + EXE + ' -m ' + run_mac + ' -c ' + temp_cfg + ' -o ' + outfile
    if args.tee: command = command + '| tee '  + logfile
    else: command = command + ' > ' + logfile
    print(command,command.split())
    # use subprocess.run for python3
    # subprocess.call(command.split(), shell=True)
    subprocess.call(command, shell=True)

# todo: consider adding conditionals for python 2 and 3 compatibility
