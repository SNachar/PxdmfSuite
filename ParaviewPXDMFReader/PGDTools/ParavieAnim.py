from paraview.simple import *
import ReaderSync 
from math import *
import time

#execfile('/home/fbordeu/projects/PGD/ParaviewPXDMFReader/Python/ReaderSync.py')

sync = ReaderSync.ReaderSync()
nbdims = len(sync.maxs.keys())	
offset = 2*pi/nbdims
for i in range(200):
	for j in range(nbdims):
		key = sync.maxs.keys()[j]
		mi = sync.mins[key]
		ma = sync.maxs[key]	
		sync.SetFixedDimension(key,(sin(j*offset+(20./(20+nbdims/2-j))*i/10.)/2+0.5)*(ma-mi)+mi)
	Render()
	
	time.sleep(.01)
	#name = str(i)
	#WriteImage("/tmp/videoparaview/image"+(5-len(name))*"0" + name+".png")
