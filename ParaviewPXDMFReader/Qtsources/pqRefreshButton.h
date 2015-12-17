/*=========================================================================

  Program:   PXDMFReader Plugin
  Module:    pqRefreshButton.h

  Copyright (c) GeM, Ecole Centrale Nantes.
  All rights reserved.
  Copyright: See COPYING file that comes with this distribution


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME pqRefreshButton 
// .SECTION Description

#ifndef __pqRefreshButton_h
#define __pqRefreshButton_h

#include "pqCommandButtonPropertyWidget.h"

class pqRefreshButton : public pqPropertyWidget
{
  Q_OBJECT

public:
  explicit pqRefreshButton(vtkSMProxy *proxy,
                                         vtkSMProperty *property,
                                         QWidget *parent = 0);
  ~pqRefreshButton();

private slots:
  void buttonClicked();

private:
  vtkSMProperty* Property;
};

#endif