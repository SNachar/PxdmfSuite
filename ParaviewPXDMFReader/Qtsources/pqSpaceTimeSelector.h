/*=========================================================================

  Program:   PXDMFReader Plugin
  Module:    pqSpaceTimeSelector.h

  Copyright (c) GeM, Ecole Centrale Nantes.
  All rights reserved.
  Copyright: See COPYING file that comes with this distribution


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/  
// .NAME pqSpaceTimeSelector 
// .SECTION Description

// Qt Includes.
#include "pqPropertyWidget.h"

class QTabWidget;
class pqFixedDimensionControl;
class QGroupBox;
class QVBoxLayout;
class QTreeWidgetItem;

class pqSpaceTimeSelector: public pqPropertyWidget { 
  Q_OBJECT
  typedef pqPropertyWidget Superclass;
public:
  pqSpaceTimeSelector(vtkSMProxy *smproxy, vtkSMProperty *smproperty, QWidget *parentObject=0) ;
  virtual ~pqSpaceTimeSelector();
  virtual void apply();
private:
  Q_DISABLE_COPY(pqSpaceTimeSelector);
  int NumberOfPXDMFDims;
  int ininit; 
  std::vector<int> NumberOfDimensionsPerPXDMFDim;
  std::vector<std::vector<std::string> > NamesOfEachPXDMFDimension;
  std::vector<std::vector<std::string> > UnitsOfEachPXDMFDimension;
  std::vector<std::vector<double> > MinsOfEachPXDMFDimension;
  std::vector<std::vector<double> > MaxsOfEachPXDMFDimension;
  std::vector<pqFixedDimensionControl*>  FixedDimensions;
  QGroupBox* FixedDimensionsBox;
  QVBoxLayout *fixlayout;
  void UpdateExtraDims();
  pqFixedDimensionControl* AddFixedDimension(const unsigned PXDMFDims, const unsigned dim, const QString &text, const QString &unit, const double &min, const double &max, const int &internalpos);
  void DeleteFixedDimension(unsigned PGDdimToDelete, unsigned dimToDelete);

  void FixedPXDMFDimsChanged(bool fromserver = 0 );
  class pqInternals;
  pqInternals* Internals;
  class pqUI;
  pqUI* UI;

protected slots:
  void SpaceDimsChanged(QTreeWidgetItem *, int);
  void FixedPXDMFDimsChangedSlot(); 
  void TimeDimsChanged(QTreeWidgetItem *, int);
  void toggleDetach();
  void CleanVisualizationSpaceStatus();
signals :
  void SpaceTimeDimsChanded();
public slots:
  virtual void FixedPXDMFDimsChangedFromServerSlot();
};

  