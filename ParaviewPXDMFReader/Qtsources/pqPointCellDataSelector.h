/*=========================================================================

  Program:   PXDMFReader Plugin
  Module:    pqPointCellDataSelector.h

  Copyright (c) GeM, Ecole Centrale Nantes.
  All rights reserved.
  Copyright: See COPYING file that comes with this distribution


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/  
// .NAME pqPointCellDataSelector 
// .SECTION Description

// Qt Includes.

#include "pqPropertyWidget.h"

class QTabWidget;
class pqFixedDimensionControl;
class QGroupBox;
class QVBoxLayout;
class QTreeWidgetItem;

class pqPointCellDataSelector: public pqPropertyWidget { 

  Q_OBJECT 
  typedef pqPropertyWidget Superclass;
public:
  pqPointCellDataSelector(vtkSMProxy *smproxy, vtkSMProperty *smproperty, QWidget *parentObject=0) ;
  virtual ~pqPointCellDataSelector();
  virtual void apply();
  pqPropertyLinks& Plinks();
  class pqUI;
  pqUI* UI;
protected slots:
  void PointDataChanged(QTreeWidgetItem *, int);
  void CellDataChanged(QTreeWidgetItem *, int);
private:
  Q_DISABLE_COPY(pqPointCellDataSelector)
  
  
};

  
