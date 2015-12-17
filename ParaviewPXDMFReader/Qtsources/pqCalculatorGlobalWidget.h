/*=========================================================================

  Program:   PXDMFReader Plugin
  Module:    pqCalculatorGlobalWidget.h

  Copyright (c) GeM, Ecole Centrale Nantes.
  All rights reserved.
  Copyright: See COPYING file that comes with this distribution


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __pqCalculatorGlobalWidget_h
#define __pqCalculatorGlobalWidget_h

// ParaView Includes
#include "pqPropertyWidget.h"

/// pqCalculatorGlobalWidget is a property-widget that can shows a calculator-like
/// UI for the property. It is designed to be used with vtkPVArrayCalculator (or
/// similar) filters. It allows users to enter expressions to compute derived
/// quantities. To determine the list of input arrays available.
///
/// CAVEATS: Currently, this widget expects two additional properties on the
/// proxy: "Input", that provides the input and "AttributeMode", which
/// corresponds to the chosen attribute type. This code can be revised to use
/// RequiredProperties on the domain to make it reuseable, if needed.
class VTK_EXPORT pqCalculatorGlobalWidget : public pqPropertyWidget
{
  Q_OBJECT
  typedef pqPropertyWidget Superclass;
public:
  pqCalculatorGlobalWidget(vtkSMProxy* proxy, vtkSMProperty* property, QWidget* parent=0);
  virtual ~pqCalculatorGlobalWidget();

protected slots:
  /// called when the user selects a variable from the scalars/vectors menus.
  void variableChosen(QAction* action);
  
  /// called when user clicks one of the function buttons
  void buttonPressed(const QString&);

  /// updates the variables in the menus.
  void updateVariableNames();
  void updateVariables(const QString& mode);

private:
  Q_DISABLE_COPY(pqCalculatorGlobalWidget);

  class pqInternals;
  pqInternals* Internals;
};

#endif
