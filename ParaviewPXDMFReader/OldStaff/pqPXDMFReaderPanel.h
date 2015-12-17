/*=========================================================================

  Program:   PXDMFReader Plugin
  Module:    pqPXDMFReaderPanel.h

  Copyright (c) GeM, Ecole Centrale Nantes.
  All rights reserved.
  Copyright: See COPYING file that comes with this distribution


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME pqPXDMFReaderPanel
// .SECTION Description

#ifndef _pqPXDMFReaderPanel_h
#define _pqPXDMFReaderPanel_h


// Paraview Includes.
#include "pqNamedObjectPanel.h"

class pqDoubleRangeWidget;
class pqFixedDimensionControl;
class QTreeWidgetItem;
class QDoubleValidator;
class MyStride;
class QGroupBox;
class QVBoxLayout;

class pqPXDMFReaderPanel : public pqNamedObjectPanel{
  Q_OBJECT
public:
  /// constructor
  pqPXDMFReaderPanel(pqProxy* proxy, QWidget* p = NULL);
  /// destructor
  ~pqPXDMFReaderPanel();
  virtual void updateInformationAndDomains();

protected slots:
  void SpaceDimsChanged(QTreeWidgetItem *, int);
  void FixedPXDMFDimsChangedSlot();
//  void FixedPXDMFDimsChangedFromServerSlot();
  void TimeDimsChanged(QTreeWidgetItem *, int);
  void SetStride();
  void SetUseinterpolation(bool state);
  void SetComputeDerivatives(bool state);
  //void SetContinuousUpdate(bool state);
  void toggleDetach();
  void refresh();
  void configPanel();

  
protected:
  void FixedPXDMFDimsChanged(bool fromserver);
  pqFixedDimensionControl* AddFixedDimension(const unsigned PXDMFDims, const unsigned dim, const QString &text, const QString &unit, const double &min, const double &max, const int &internalpos);
  QGroupBox* FixedDimensionsBox;
  QVBoxLayout *fixlayout;
  void DeleteFixedDimension(unsigned,unsigned);
  class pqUI;
  pqUI* UI;
private: 
  //pqPropertyLinks Links;
  std::vector<pqFixedDimensionControl*>  FixedDimensions;
  int NumberOfPXDMFDims;
  int ininit; 
  std::vector<int> NumberOfDimensionsPerPXDMFDim;
  std::vector<std::vector<std::string> > NamesOfEachPXDMFDimension;
  std::vector<std::vector<std::string> > UnitsOfEachPXDMFDimension;
  std::vector<std::vector<double> > MinsOfEachPXDMFDimension;
  std::vector<std::vector<double> > MaxsOfEachPXDMFDimension;
  void UpdateExtraDims();
  MyStride * mystride;
  void AddStride();
  void DeleteStride();
  class pqInternals;
  pqInternals* Internals;
};
//

#endif

