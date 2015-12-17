/*=========================================================================

  Program:   PXDMFReader Plugin
  Module:    XDMF3Data.h

  Copyright (c) GeM, Ecole Centrale Nantes.
  All rights reserved.
  Copyright: See COPYING file that comes with this distribution


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef XDMF3DATA_H
#define XDMF3DATA_H

// Paraview/VTK Includes.
class XdmfDomain;
class XdmfReader;
class XdmfGrid;
class XdmfGridCollection;

#include "XdmfGrid.hpp"
#include "vtkSmartPointer.h"

// plugin Includes 
#include "FileDataAbstract.h"

//
class vtkXdmf3ArraySelection;

class XDMF3Data: public FileDataAbstract {
public:
  //
  XDMF3Data();
  ~XDMF3Data();
  //
  bool Init(char* filename);
  bool SetActiveDommain(char* domainName);
  virtual bool NeedUpdate();
  int GetNumberOfGrids();
  
  virtual vtkDataSet* GetVTKDataSet(int i,int* stride, int* extents);
  virtual void ForceUpdate();
  virtual int GetDomainVTKDataType();
  void  CalculateMaxMin(shared_ptr <XdmfGrid >& grid, GridInfo& data);
    
  virtual GridInfo GetGridData(int j );
  //
  virtual int GetNumberOfAttributes();
  virtual AttributeInfo GetAttributeData(int j );

  virtual int GetNumberOfPointArrays();
  virtual void SetPointArraySelection(const char* ,bool);
  virtual bool GetPointArraySelection(const char * name);
  virtual const char* GetPointArrayName(int& index);

  virtual int GetNumberOfCellArrays();
  virtual void SetCellArraySelection(const char*,bool);
  virtual bool GetCellArraySelection(const char * name);
  virtual const char* GetCellArrayName(int& index);
  void XdmfDocument ( unsigned int i );
  
private:
  void PoputateGridData ( int j ,GridInfo& data, shared_ptr<XdmfGrid> grid );
  int GetNumberOfGridsRecursive(boost::shared_ptr<XdmfGridCollection> group);
  shared_ptr <XdmfGrid > GetGridRecursive (shared_ptr <XdmfGridCollection > fgrid, int& type, int& j ) ;

  shared_ptr<XdmfReader> xdmfReader;
  shared_ptr<XdmfDomain > Domain;
  std::vector<shared_ptr<XdmfGrid> > xdmfGrids;               /// The grids for each PXDMF dimensions
  std::vector<vtkSmartPointer<vtkDataSet> > vtkGrids;               /// The grids for each PXDMF dimensions
  std::vector<GridInfo> gridsInfo;
  //typedef xdmf3::XdmfGrid XdmfGrid;
  
  vtkXdmf3ArraySelection* FieldArrays;
  vtkXdmf3ArraySelection* CellArrays;
  vtkXdmf3ArraySelection* PointArrays;
};
//
#endif // XDMF3DATA_H

