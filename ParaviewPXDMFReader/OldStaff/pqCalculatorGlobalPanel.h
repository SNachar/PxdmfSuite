/*=========================================================================

  Program:   PXDMFReader Plugin
  Module:    pqCalculatorGlobalPanel.h

  Copyright (c) GeM, Ecole Centrale Nantes.
  All rights reserved.
  Copyright: See COPYING file that comes with this distribution


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME pqCalculatorGlobalPanel - 
// .SECTION Description

#ifndef _pqCalculatorGlobalPanel_h
#define _pqCalculatorGlobalPanel_h

#include "pqObjectPanel.h"

/// Panel for vtkArrayCalculator proxy
class pqCalculatorGlobalPanel : public pqObjectPanel
{
  Q_OBJECT
public:
  /// constructor
  pqCalculatorGlobalPanel(pqProxy* proxy, QWidget* p = 0);
  /// destructor
  ~pqCalculatorGlobalPanel();

  /// accept the changes made to the properties
  /// changes will be propogated down to the server manager
  virtual void accept();

  /// reset the changes made
  /// editor will query properties from the server manager
  virtual void reset();

protected slots:
  /// slot called when any calculator button is pushed
  void buttonPressed(const QString& t);
  
  void updateVariables(const QString& mode);
  void variableChosen(QAction* a);
  void disableResults(bool);
  void updateVariableNames();
protected:
  class pqInternal;
  pqInternal* Internal;
};


#endif

