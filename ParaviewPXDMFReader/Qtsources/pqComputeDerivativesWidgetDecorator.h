/*=========================================================================

  Program:   PXDMFReader Plugin
  Module:    pqComputeDerivativesWidgetDecorator.h

  Copyright (c) GeM, Ecole Centrale Nantes.
  All rights reserved.
  Copyright: See COPYING file that comes with this distribution


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME pqComputeDerivativesWidgetDecorator 
// .SECTION Description

#ifndef __pqComputeDerivativesWidgetDecorator_h
#define __pqComputeDerivativesWidgetDecorator_h

// Vtk Includes 
#include "vtkWeakPointer.h"

// ParaView Includes
#include "pqPropertyWidgetDecorator.h"

class vtkObject;

class pqComputeDerivativesWidgetDecorator : public pqPropertyWidgetDecorator
{
  Q_OBJECT
  typedef pqPropertyWidgetDecorator Superclass;
public:
  pqComputeDerivativesWidgetDecorator(
    vtkPVXMLElement* config, pqPropertyWidget* parentObject);
  
  virtual ~pqComputeDerivativesWidgetDecorator();

  /// Overridden to hide the widget when ShrinkFactor < 0.1
  virtual bool canShowWidget(bool show_advanced) const;

private:
  Q_DISABLE_COPY(pqComputeDerivativesWidgetDecorator)

  vtkWeakPointer<vtkObject> ObservedObject;
  unsigned long ObserverId;
};

#endif