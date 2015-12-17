
# to run in linux
#/Big/fbordeu/builds/ParaView_dist/bin/pvpython

# to use de time.sleep funtionc
import time
from math import *


# import paraview 
from paraview.simple import *

# import the PXDMFReader Plugin
paraview.servermanager.LoadPlugin('/Big/fbordeu/builds/PXDMFReader_bin/libPXDMFReader.so',True)
paraview.servermanager.LoadPlugin('/Big/fbordeu/builds/AnnotateFieldData/libAnnotateFieldData.so',True)

# Read the file
serpentin_pxdmf =  servermanager.sources.PXDMFReader(FileName='/media/USB/serpentin.pxdmf' )

# Get the Available Dims in the 
Dims=  serpentin_pxdmf.GetProperty('VisualizationSpaceStatus').GetAvailable()
for d in range(len(Dims)):
	print Dims[d]

# Set The Space and Time Dimensions, and FixedDimensions
serpentin_pxdmf.GetProperty('VisualizationSpaceStatus').SetData(Dims[0]) 
serpentin_pxdmf.GetProperty('VisualizationTimeStatus').SetData(Dims[2])
serpentin_pxdmf.GetProperty('FixedDimensions').SetData([0.0, 0.0, -0.3, 0.006000000000000005, -0.15, 0.09000000000000002])

# Use Interpolation
serpentin_pxdmf.GetProperty('UseInterpolation').SetData(1)

# To Generate the Output
serpentin_pxdmf.UpdatePipeline()

# check of Ouput data
print 'Number of cells  : ' + str(serpentin_pxdmf.GetDataInformation().GetNumberOfCells())
print 'Number of Points : ' + str(serpentin_pxdmf.GetDataInformation().GetNumberOfPoints())

# We need the render view to change the time
RenderView1 = GetRenderView()


# different visualtisations

#shrinkFilter = Shrink(serpentin_pxdmf)
#DataRepresentation1 = Show(shrinkFilter)

####

Calculator1 = Calculator(serpentin_pxdmf)
Calculator1.AttributeMode = 'point_data'
Calculator1.Function = 'coords+mapping*jHat'
Calculator1.CoordinateResults = 1
DataRepresentation1 = Show(Calculator1)

####

#DataRepresentation1 = Show(serpentin_pxdmf)

####

# to plot the fixed dimensions
annotatedata = AnnotateFieldData(serpentin_pxdmf)
Show(annotatedata)


# to Set up the animation
AnimationScene1 = GetAnimationScene()
AnimationScene1.PlayMode = 'Snap To TimeSteps'
AnimationScene1.StartTime = -0.30000000
AnimationScene1.EndTime = 0.30000000


# to plot the mapping
#a1_mapping_PVLookupTable = GetLookupTableForArray( "mapping", 1, NanColor=[0.247059, 0.0, 0.0], RGBPoints=[-0.3, 0.231373, 0.298039, 0.752941, 0.3, 0.705882, 0.0156863, 0.14902], VectorMode='Magnitude', ColorSpace='Diverging', ScalarRangeInitialized=1.0 )
#a1_mapping_PiecewiseFunction = CreatePiecewiseFunction( Points=[0.0, 0.0, 0.5, 0.0, 1.0, 1.0, 0.5, 0.0] )
#DataRepresentation1.ScalarOpacityFunction = a1_mapping_PiecewiseFunction
#DataRepresentation1.ColorArrayName = 'mapping'
#DataRepresentation1.LookupTable = a1_mapping_PVLookupTable	

#to plot the temperature
a1_T_PVLookupTable = GetLookupTableForArray( "T", 1, NanColor=[0.247059, 0.0, 0.0], RGBPoints=[0, 0.231373, 0.298039, 0.752941, 0.131, 0.705882, 0.0156863, 0.14902], VectorMode='Magnitude', ColorSpace='Diverging', ScalarRangeInitialized=1.0 )
a1_T_PiecewiseFunction = CreatePiecewiseFunction( Points=[0.0, 0.0, 0.5, 0.0, 1.0, 1.0, 0.5, 0.0] )
DataRepresentation1.ScalarOpacityFunction = a1_T_PiecewiseFunction
DataRepresentation1.ColorArrayName = 'T'
DataRepresentation1.LookupTable = a1_T_PVLookupTable	

# plot the solution with respect to time
for i in range(21):
	val = (i - 10 )*.03
	print val
	RenderView1.ViewTime = val

	Render()
	time.sleep(.1)


#Take the time out and use fixed dimensions for the animation
serpentin_pxdmf.GetProperty('VisualizationTimeStatus').SetData('')
dpi = pi/20
print time.time()


for i in range(200):
	serpentin_pxdmf.GetProperty('FixedDimensions').SetData([0.0, 0.0, sin(dpi*i*3)*0.3, sin(dpi*i*2-2)*0.3, sin(dpi*i*2.5+1)*0.3, cos(dpi*i*4-3)*0.3])
	Render()
	time.sleep(.01)
	name = str(i)
	WriteImage("/tmp/videoparaview/image"+(5-len(name))*"0" + name+".png")
print time.time()

mencoder "mf://*.png" -of rawvideo -mpegopts format=mpeg1:tsaf:muxrate=2000 -o output.mpg  -oac lavc -lavcopts acodec=mp2:abitrate=224 -ovc lavc  -lavcopts vcodec=mpeg2video:vbitrate=1152:keyint=15:mbd=2:aspect=1

