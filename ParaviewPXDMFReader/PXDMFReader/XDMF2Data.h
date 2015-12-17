/*=========================================================================

  Program:   PXDMFReader Plugin
  Module:    XDMF2Data.h

  Copyright (c) GeM, Ecole Centrale Nantes.
  All rights reserved.
  Copyright: See COPYING file that comes with this distribution


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef XDMF2DATA_H
#define XDMF2DATA_H

// Paraview/VTK Includes.
#include "vtkSmartPointer.h"

#define vtkstd std
class vtkXdmfDocument;
class vtkXdmfDomain;

namespace xdmf2 {
  class XdmfGrid;  
};

// plugin Includes 
#include "FileDataAbstract.h"

//
class XDMF2Data : public FileDataAbstract {
public:
  XDMF2Data();
  ~XDMF2Data();
  bool  Init(char* filename);
  //
  bool SetActiveDommain(char* domainName);
  //
  bool NeedUpdate();
  //
  int GetNumberOfGrids();
  GridInfo GetGridData(int j );
  //
  int GetNumberOfAttributes();
  AttributeInfo GetAttributeData(int j );
  
  virtual vtkDataSet* GetVTKDataSet(int i,int* stride, int* extents);
  //
  virtual void ForceUpdate();
  
  virtual int GetNumberOfPointArrays();
  virtual void SetPointArraySelection(const char* ,bool);
  virtual bool GetPointArraySelection(const char * name);
  virtual const char* GetPointArrayName(int& index);
  
  virtual int GetNumberOfCellArrays();
  virtual void SetCellArraySelection(const char*,bool);
  virtual bool GetCellArraySelection(const char * name);
  virtual const char* GetCellArrayName(int& index);
  
/// Domain (XMDF) used in the previouse recosntruction
  
  void CalculateMaxMin(const unsigned j,GridInfo& data);
  virtual int GetDomainVTKDataType();
  
private:
  vtkXdmfDocument* XdmfDocument;                                 /// The underlying XDMF document 
  vtkstd::vector<xdmf2::XdmfGrid*> PXDMFDimensionXdmfGrids;               /// The grids for each PXDMF dimensions
  std::vector<vtkSmartPointer<vtkDataSet> > vtkGrids;               /// The grids for each PXDMF dimensions
  typedef xdmf2::XdmfGrid XdmfGrid;
  vtkXdmfDomain * PreviouseDomain;   
};
//
#endif // XDMF2DATA_H
