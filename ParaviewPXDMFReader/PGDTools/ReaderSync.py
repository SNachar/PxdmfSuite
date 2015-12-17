#=========================================================================
#
#  Program:   PXDMFReader Plugin
#  Module:    ReaderSync/py
#
#  Copyright (c) GeM, Ecole Centrale Nantes.
#  All rights reserved.
#  Copyright: See COPYING file that comes with this distribution
#
#
#     This software is distributed WITHOUT ANY WARRANTY; without even
#     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
#     PURPOSE.  See the above copyright notice for more information.
#
#=========================================================================

from paraview.simple import *
import paraview.vtk

class ReaderSync():
  """ Class to synchronize PXDMF Readers
    import ReaderSync
    Pxdmfsync = ReaderSync.ReaderSync()
    Pxdmfsync.SetFixedDimension('Amp', 1.2)
    Pxdmfsync.SetFixedDimensionPer('DX', 0.2)
  """
  def __init__(self):
    self.UpdateSources()
    self.printstate = 0;
  
  def UpdateSources(self):
    """Update the internal list of PXDMF Readers"""
    self.mins = {}
    self.maxs = {}
    self.sources = GetSources()
    self.nbSources = len(self.sources)
    self.PXDMFReaders = []
    for i in range(self.nbSources):
      mysource = self.sources[self.sources.keys()[i]]
      if(isinstance(mysource, paraview.servermanager.sources.PXDMFReader )):
        self.PXDMFReaders.append(mysource)
        rmins = mysource.GetPropertyValue('PXDMFDimsMinRangeDataInfo')
        rmaxs = mysource.GetPropertyValue('PXDMFDimsMaxRangeDataInfo')
        names = mysource.GetPropertyValue('PXDMFDimsNameDataInfo')
        nbdims = len(names)
        for dim in range(nbdims):
          if self.mins.has_key(names[dim]):
            self.mins[names[dim]] = min(self.mins[names[dim]],rmins[dim])
            self.maxs[names[dim]] = min(self.maxs[names[dim]],rmaxs[dim])
          else:
            self.mins[names[dim]] = rmins[dim]
            self.maxs[names[dim]] = rmaxs[dim]

  def SetFixedDimension(self, name, value):
    """Set the value of a fixed coordinate """
    if not self.mins.has_key(name):
      return
    
    for mysource in self.PXDMFReaders:
      fixeddims = mysource.GetProperty('FixedDimensions')
      names = mysource.GetPropertyValue('PXDMFDimsNameDataInfo')
      for dim in range(len(names)):
        if names[dim] == name:
          fixeddims[dim] = value
          
  def SetFixedDimensionPer(self, name, value):
    """Set the value of a fixed coordinate using percentage. value must be between 0 and 1 """
    if not self.mins.has_key(name):
      return
    
    for mysource in self.PXDMFReaders:
      fixeddims = mysource.GetProperty('FixedDimensions')
      names = mysource.GetPropertyValue('PXDMFDimsNameDataInfo')
      for dim in range(len(names)):
        if names[dim] == name:
          fixeddims[dim] = value*(self.maxs[name]-self.mins[name])+self.mins[name]

  def SetFixedDimensionIndexPer(self, index, value):
    """Set the value of a fixed coordinate using percentage. value must be between 0 and 1 """
    if len(self.mins)<= index:
       return
    
    name = self.mins.items()[index][0]
    
    for mysource in self.PXDMFReaders:
      fixeddims = mysource.GetProperty('FixedDimensions')
      names = mysource.GetPropertyValue('PXDMFDimsNameDataInfo')
      for dim in range(len(names)):
        if names[dim] == name:
          fixeddims[dim] = value*(self.maxs[name]-self.mins[name])+self.mins[name]          
          
  def UpdatePipeline(self):
    """Call UpdatePipeline() on each register PXDMFReader"""
    for reader in self.PXDMFReaders:
      reader.UpdatePipeline()
      
  def GetState(self):
    """To recover the state off all the coordinate in a text form. must set the printstate variable to 1 first 
    >>> Pxdmfsync.printstate = 1
    >>> Pxdmfsync.GetState()
    'STATE;:Y:0.0:1.0:X:0.0:3.0;FIN'
    """
    
    if self.printstate:
      self.UpdateSources();
      keys = self.maxs.keys()
      res = "STATE;"
      for key in keys :
        res += ":"+key +":" + str(self.mins[key])+ ":" +  str(self.maxs[key]) 
      res += ";FIN\n"
      self.printstate = 0;
      return res
    return ""

def pvrotate(x,y):
  """ 
  Point of view rotation in degree
  note: Render() must be called to redraw the scene
  """
  renView = GetRenderView()
  transform = paraview.vtk.vtkTransform()
  camera = GetActiveCamera()
  renderer = renView.GetRenderer()
  scale = paraview.vtk.vtkMath.Norm(camera.GetPosition())
  if scale <= 0.0:
    scale = paraview.vtk.vtkMath.Norm(camera.GetFocalPoint())
    if scale <= 0.0:
      scale = 1.0
      
  FPoint = camera.GetFocalPoint()
  FPoint0 = FPoint[0]
  FPoint1 = FPoint[1]
  FPoint2 = FPoint[2]
  camera.SetFocalPoint(FPoint0/scale,FPoint1/scale,FPoint2/scale)
  
  PPoint = camera.GetPosition()
  PPoint0 = PPoint[0]
  PPoint1 = PPoint[1]
  PPoint2 = PPoint[2]
  camera.SetPosition(PPoint0/scale,PPoint1/scale,PPoint2/scale)
  
  center = renView.CenterOfRotation
  Center0 = center[0]
  Center1 = center[1]
  Center2 = center[2]

  renderer.SetWorldPoint(Center0,Center1,Center2,1.0)
  renderer.WorldToDisplay()
    
  v2 = [0,0,0]
  transform.Identity()
  transform.Translate(Center0/scale,Center1/scale,Center2/scale)
  
  camera.OrthogonalizeViewUp()
  viewUp = camera.GetViewUp()
  viewUp0 = viewUp[0]
  viewUp1 = viewUp[1]
  viewUp2 = viewUp[2]
  size = renderer.GetSize()
  transform.RotateWXYZ(360.0 * x / size[0], viewUp0, viewUp1, viewUp2)
  
  paraview.vtk.vtkMath.Cross(camera.GetDirectionOfProjection(), viewUp, v2)
  transform.RotateWXYZ(-360.0 * y / size[1], v2[0], v2[1], v2[2])
  transform.Translate(-Center0/scale,-Center1/scale,-Center2/scale)
  
  camera.ApplyTransform(transform)
  camera.OrthogonalizeViewUp()
  
  FPoint = camera.GetFocalPoint()
  FPoint0 = FPoint[0]
  FPoint1 = FPoint[1]
  FPoint2 = FPoint[2]
  camera.SetFocalPoint(FPoint0*scale,FPoint1*scale,FPoint2*scale)
  
  PPoint = camera.GetPosition()
  PPoint0 = PPoint[0]
  PPoint1 = PPoint[1]
  PPoint2 = PPoint[2]
  camera.SetPosition(PPoint0*scale,PPoint1*scale,PPoint2*scale)
  
  renderer.ResetCameraClippingRange()
def pvzoom(a):
  """ 
  Zoom : negative number get closer, positive get further away
  note: Render() must be called to redraw the scene
  """
  RenderView1 = GetRenderView();
  Fp = RenderView1.CameraFocalPoint;
  Cp = RenderView1.CameraPosition;
  Cvu = RenderView1.CameraViewUp;
  Ld = [Fp[0]-Cp[0],Fp[1]-Cp[1],Fp[2]-Cp[2]];
  RenderView1.CameraPosition[0] = RenderView1.CameraPosition[0]-Ld[0]*a*0.2
  RenderView1.CameraPosition[1] = RenderView1.CameraPosition[1]-Ld[1]*a*0.2
  RenderView1.CameraPosition[2] = RenderView1.CameraPosition[2]-Ld[2]*a*0.2
  GetRenderView().GetRenderer().ResetCameraClippingRange()

def pvdolly(a):
  """ 
  Zoom : positive number get closser, negative get further away
  note: Render() must be called to redraw the scene
  """
  camera = GetActiveCamera()
  renView = GetRenderView()
  renderer = renView.GetRenderer()
  FPoint = camera.GetFocalPoint()
  FPoint0 = FPoint[0]
  FPoint1 = FPoint[1]
  FPoint2 = FPoint[2]

  PPoint = camera.GetPosition()
  PPoint0 = PPoint[0]
  PPoint1 = PPoint[1]
  PPoint2 = PPoint[2]
  
  Norm = camera.GetDirectionOfProjection()
  Norm0 = Norm[0]
  Norm1 = Norm[1]
  Norm2 = Norm[2]
  
  size = renderer.GetSize()
  rng = GetRenderView().CameraClippingRange
  ZoomScale = 1.5 * rng[1] / size[1]
  k = a * ZoomScale
  
  temp = k * Norm0
  PPoint0 += temp
  FPoint0 += temp
  
  temp = k * Norm1
  PPoint1 += temp
  FPoint1 += temp
  
  temp = k * Norm2
  PPoint2 += temp
  FPoint2 += temp
  
  camera.SetFocalPoint(FPoint0, FPoint1, FPoint2)
  camera.SetPosition(PPoint0, PPoint1, PPoint2)
  renderer.ResetCameraClippingRange()

def pvpan(x,y):
  """ 
  Pan : xpan, ypan
  note: Render() must be called to redraw the scene
  """
  renView = GetRenderView()
  camera = GetActiveCamera()
  (FPoint0,FPoint1,FPoint2) = camera.GetFocalPoint()
  (PPoint0,PPoint1,PPoint2) = camera.GetPosition()
  
  renderer = renView.GetRenderer()
  renderer.SetWorldPoint(FPoint0, FPoint1, FPoint2, 1.0)
  renderer.WorldToDisplay()
  DPoint = renderer.GetDisplayPoint()
  focalDepth = DPoint[2]
  
  (centerX,centerY) = renView.GetRenderWindow().GetSize()
  
  APoint0 = centerX/2.0 + x
  APoint1 = centerY/2.0 + y

  renderer.SetDisplayPoint(APoint0, APoint1, focalDepth)
  renderer.DisplayToWorld()
  (RPoint0,RPoint1,RPoint2,RPoint3) = renderer.GetWorldPoint()
  
  if RPoint3 != 0.0:
      RPoint0 = RPoint0/RPoint3
      RPoint1 = RPoint1/RPoint3
      RPoint2 = RPoint2/RPoint3

  camera.SetFocalPoint( (FPoint0-RPoint0)/2.0 + FPoint0,
                        (FPoint1-RPoint1)/2.0 + FPoint1,
                        (FPoint2-RPoint2)/2.0 + FPoint2)
  camera.SetPosition( (FPoint0-RPoint0)/2.0 + PPoint0,
                      (FPoint1-RPoint1)/2.0 + PPoint1,
                      (FPoint2-RPoint2)/2.0 + PPoint2)

  renderer.ResetCameraClippingRange()

def pvroll(x):
  """ 
  roll : rotation in degree
  note: Render() must be called to redraw the scene
  """
  renView = GetRenderView()
  camera = GetActiveCamera()
  renderer = renView.GetRenderer()
  FPoint = camera.GetFocalPoint()
  FPoint0 = FPoint[0]
  FPoint1 = FPoint[1]
  FPoint2 = FPoint[2]
  
  PPoint = camera.GetPosition()
  PPoint0 = PPoint[0]
  PPoint1 = PPoint[1]
  PPoint2 = PPoint[2]

  Axis0 = FPoint0 - PPoint0
  Axis1 = FPoint1 - PPoint1
  Axis2 = FPoint2 - PPoint2

  center = renView.CenterOfRotation
  Center0 = center[0]
  Center1 = center[1]
  Center2 = center[2]
  renderer.SetWorldPoint(Center0,Center1,Center2,1.0)
  renderer.WorldToDisplay()
  
  transform = paraview.vtk.vtkTransform()
  transform.Identity()
  transform.Translate(Center0,Center1,Center2)
  transform.RotateWXYZ(x, Axis0, Axis1, Axis2)
  transform.Translate(-Center0,-Center1,-Center2)

  camera.ApplyTransform(transform)
  camera.OrthogonalizeViewUp()

  renderer.ResetCameraClippingRange()
