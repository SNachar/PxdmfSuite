/*=========================================================================

  Program:   PXDMFReader Plugin
  Module:    vtkPXDMFGeneralSettings.cpp

  Copyright (c) GeM, Ecole Centrale Nantes.
  All rights reserved.
  Copyright: See COPYING file that comes with this distribution


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkPXDMFGeneralSettings.h"

// std Includes
#include <cassert>

// Vtk Includes
#include "vtkObjectFactory.h"

vtkSmartPointer<vtkPXDMFGeneralSettings> vtkPXDMFGeneralSettings::Instance;

vtkInstantiatorNewMacro(vtkPXDMFGeneralSettings);
//----------------------------------------------------------------------------
vtkPXDMFGeneralSettings* vtkPXDMFGeneralSettings::New()
{
  vtkPXDMFGeneralSettings* instance = vtkPXDMFGeneralSettings::GetInstance();
  assert(instance);
  instance->Register(NULL);
  return instance;
}
//----------------------------------------------------------------------------
vtkPXDMFGeneralSettings::vtkPXDMFGeneralSettings()
  : ShowSplashScreen(true),
  PostReconstructionMapping(0),
  PostReconstructionThreshold(0),
  PostReconstructionAnnotateFixedDims(0),
  GUIMaxNbModes(100),
  GmshPath(0),
  LicenseNumber(0),
  UpdateOrientationAxesLabels(true)
{
  
}
//----------------------------------------------------------------------------
vtkPXDMFGeneralSettings::~vtkPXDMFGeneralSettings()
{
  this->SetLicenseNumber(0);
  this->SetGmshPath(0);
}
//----------------------------------------------------------------------------
vtkPXDMFGeneralSettings* vtkPXDMFGeneralSettings::GetInstance()
{
  if (!vtkPXDMFGeneralSettings::Instance)
    {
    vtkPXDMFGeneralSettings* instance = new vtkPXDMFGeneralSettings();
    vtkObjectFactory::ConstructInstance(instance->GetClassName());
    vtkPXDMFGeneralSettings::Instance.TakeReference(instance);
    }
  return vtkPXDMFGeneralSettings::Instance;
}
//----------------------------------------------------------------------------
void vtkPXDMFGeneralSettings::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
