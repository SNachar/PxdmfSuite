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

#include "pqComputeDerivativesWidgetDecorator.h"

// Vtk Includes 
#include "vtkCommand.h"
#include "vtkSMProperty.h"
#include "vtkSMProxy.h"
#include "vtkSMUncheckedPropertyHelper.h"

// ParaView Includes
#include "pqCoreUtilities.h"
#include "pqPropertyWidget.h"

//-----------------------------------------------------------------------------
pqComputeDerivativesWidgetDecorator::pqComputeDerivativesWidgetDecorator(
    vtkPVXMLElement* config, pqPropertyWidget* parentObject)
  : Superclass(config, parentObject)
{
  vtkSMProxy* proxy = parentObject->proxy();
  vtkSMProperty* prop = proxy? proxy->GetProperty("UseInterpolation") : NULL;
  if (!prop)
    {
    qDebug("Could not locate property named 'UseInterpolation'. "
      "pqComputeDerivativesWidgetDecorator will have no effect.");
    return;
    }

  this->ObservedObject = prop;
  this->ObserverId = pqCoreUtilities::connect(
    prop, vtkCommand::UncheckedPropertyModifiedEvent,
    this, SIGNAL(visibilityChanged()));
}

//-----------------------------------------------------------------------------
pqComputeDerivativesWidgetDecorator::~pqComputeDerivativesWidgetDecorator()
{
  if (this->ObservedObject && this->ObserverId)
    {
    this->ObservedObject->RemoveObserver(this->ObserverId);
    }
}

//-----------------------------------------------------------------------------
bool pqComputeDerivativesWidgetDecorator::canShowWidget(bool show_advanced) const
{
  pqPropertyWidget* parentObject = this->parentWidget();
  vtkSMProxy* proxy = parentObject->proxy();
  vtkSMProperty* prop = proxy? proxy->GetProperty("UseInterpolation") : NULL;
  if (prop)
    {
    double value = vtkSMPropertyHelper(prop).GetAsInt();
    if (value == 0)
      {
      return false;
      }
    }

  return this->Superclass::canShowWidget(show_advanced);
}
