
import sys

print sys.argv

try: paraview.simple
except: from paraview.simple import *
paraview.simple._DisableFirstRenderCameraReset()


if(len(sys.argv) == 1) :
  sys.exit('I need the root path of the paraview to load the plugin')

ParaviewLibDir = sys.argv[1];
paraview.servermanager.LoadPlugin(ParaviewLibDir + '/lib/libPXDMFReader.so',True)

Disk1 = Disk()
Disk1.CircumferentialResolution = 230
Disk1.RadialResolution = 139

RenderView1 = GetRenderView()
RenderView1.CameraViewUp = [0.0, 1.0, 0.0]
RenderView1.CameraPosition = [0.0, 0.0, 10000.0]
RenderView1.InteractionMode = '2D'

DataRepresentation2 = Show()
DataRepresentation2.ScaleFactor = 0.2
DataRepresentation2.EdgeColor = [0.0, 0.0, 0.5000076295109483]

TransformWithAxis1 = paraview.servermanager.filters.TransformWithAxis( Transform="Transform", Input=Disk1 )
TransformWithAxis1.TitleX = 'XX m'
TransformWithAxis1.TitleY = 'YY m'
TransformWithAxis1.TitleZ = ''
TransformWithAxis1.Transform.Scale = [1.0, 1.0, 0.0]

Line1 = Line()

RenderView1.CameraPosition = [0.0, 0.0, 5.464101615137755]
RenderView1.CameraClippingRange = [5.409460598986378, 5.546063139364821]
RenderView1.CameraParallelScale = 1.4142135623730951

active_objects.source.SMProxy.InvokeEvent('UserEvent', 'ShowWidget')


RenderView1.CameraClippingRange = [5.361827618527939, 5.6060232479318515]

DataRepresentation3 = Show()
DataRepresentation3.ScaleFactor = 0.1
DataRepresentation3.SelectionPointFieldDataArrayName = 'Texture Coordinates'
DataRepresentation3.EdgeColor = [0.0, 0.0, 0.5000076295109483]

Triangulate1 = Triangulate(Input=Line1)

TransformWithAxis2 = paraview.servermanager.filters.TransformWithAxis( Transform="Transform" , Input=Triangulate1)
TransformWithAxis2.TitleX = 'X'
TransformWithAxis2.TitleY = ''
TransformWithAxis2.TitleZ = ''
TransformWithAxis2.Transform.Scale = [1.0, 1.0, 0.0]



RenderView1.CameraClippingRange = [5.409460598986378, 5.546063139364821]

DataRepresentation4 = Show()
DataRepresentation4.ScaleFactor = 0.1
DataRepresentation4.SelectionPointFieldDataArrayName = 'Texture Coordinates'
DataRepresentation4.EdgeColor = [0.0, 0.0, 0.5000076295109483]

GroupDatasets1 = GroupDatasets( Input=[  TransformWithAxis1, TransformWithAxis2] )

DataRepresentation3.Visibility = 0

DataRepresentation5 = Show()
DataRepresentation5.ScaleFactor = 0.2
DataRepresentation5.SelectionPointFieldDataArrayName = 'Texture Coordinates'
DataRepresentation5.EdgeColor = [0.0, 0.0, 0.5000076295109483]

DataRepresentation2.Visibility = 0

DataRepresentation4.Visibility = 0

Render()
 
# Specifying the source explicitly
paraview.servermanager.writers.PXDMFWriter3()
writer= CreateWriter("./test.pxdmf", GroupDatasets1)
writer.UpdatePipeline()

#writer= CreateWriter("./test.pxdmf", GroupDatasets1)
#writer.UpdatePipeline()
