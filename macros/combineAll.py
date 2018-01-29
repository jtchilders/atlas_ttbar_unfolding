#!/usr/bin/python

# Author : Peter Berta
# Date   : 30.3.2012

import os, sys

topPath = os.getenv("TOPPATH")
os.chdir(topPath)

#TagName = [ 'pretag', 'tagged']
TagName = ['tagged']
Spectrums= ['Pt']
#Spectrums= ['Pt','Mtt','Ptt','leptonicPt','hadronicPt']
Ljets = [ 'el' ]
#Ljets = [ 'el', 'mu' ]
scriptname='_runCombineAll.sh'
runscript=open(scriptname, 'w')
  
for tagname in  TagName:
  for ljets in Ljets:
    for spectrum in Spectrums:
      cmd = 'root -l -b -q \'' + topPath + 'macros/combineAll/combineAll.C+("' + spectrum + '", "' + tagname + '", "' + ljets + '")\''
      print cmd
      runscript.write(cmd + '\n')

runscript.close()
os.system('chmod +x ' + scriptname)
os.system('./' + scriptname)

