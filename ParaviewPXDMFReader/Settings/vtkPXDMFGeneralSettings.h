/*=========================================================================

  Program:   PXDMFReader Plugin
  Module:    vtkPXDMFGeneralSettings.h

  Copyright (c) GeM, Ecole Centrale Nantes.
  All rights reserved.
  Copyright: See COPYING file that comes with this distribution


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __VTKGENERALSETTINGS_H
#define __VTKGENERALSETTINGS_H

#include "vtkObject.h"
#include "vtkPVClientServerCoreRenderingModule.h" //needed for exports
#include "vtkSmartPointer.h" // needed for vtkSmartPointer.

class VTK_EXPORT vtkPXDMFGeneralSettings  : public vtkObject {
public:
  static vtkPXDMFGeneralSettings* New();
 vtkTypeMacro(vtkPXDMFGeneralSettings, vtkObject);
 void PrintSelf(ostream& os, vtkIndent indent);
 
  // Description:
  // Access the singleton.
  static vtkPXDMFGeneralSettings* GetInstance();
  
  vtkSetMacro(ShowSplashScreen, bool);
  vtkGetMacro(ShowSplashScreen, bool);
  
  vtkSetMacro(UpdateOrientationAxesLabels, bool);
  vtkGetMacro(UpdateOrientationAxesLabels, bool);
  
//  vtkSetMacro(PostReconstructionMode, int);
//  vtkGetMacro(PostReconstructionMode, int);
  
  vtkSetMacro(PostReconstructionMapping, int);
  vtkGetMacro(PostReconstructionMapping, int);
  
  vtkSetMacro(PostReconstructionThreshold, int);
  vtkGetMacro(PostReconstructionThreshold, int);
           
  vtkSetMacro(PostReconstructionAnnotateFixedDims, int);
  vtkGetMacro(PostReconstructionAnnotateFixedDims, int);
  
  vtkSetMacro(GUIMaxNbModes, int);
  vtkGetMacro(GUIMaxNbModes, int);
  
  vtkSetStringMacro(GmshPath);
  vtkGetStringMacro(GmshPath);
  
  vtkSetStringMacro(LicenseNumber);
  vtkGetStringMacro(LicenseNumber);
  
protected:
  vtkPXDMFGeneralSettings();
  ~vtkPXDMFGeneralSettings();
  char * LicenseNumber;
  char* GmshPath;
  bool ShowSplashScreen;
  bool UpdateOrientationAxesLabels;
  int GUIMaxNbModes;
//  int PostReconstructionMode;
  int PostReconstructionMapping;
  int PostReconstructionThreshold;
  int PostReconstructionAnnotateFixedDims;
     /// Not Apply Any Filter                   0
    //  Mapping/Threshold/AnnotateFidexDims    1
    //  Mapping/AnnotateFidexDims              2
    //  Threshold/AnnotateFidexDims            3
private:
  vtkPXDMFGeneralSettings(const vtkPXDMFGeneralSettings&); // Not implemented
  void operator=(const vtkPXDMFGeneralSettings&); // Not implemented

  static vtkSmartPointer<vtkPXDMFGeneralSettings> Instance;
    };

#endif // VTKVGENERALSETTINGS_H
