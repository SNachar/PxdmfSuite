/*=========================================================================

  Program:   PXDMFReader Plugin
  Module:    vtkAddZeros.h

  Copyright (c) GeM, Ecole Centrale Nantes.
  All rights reserved.
  Copyright: See COPYING file that comes with this distribution


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkAddZeros - Passes a subset of arrays to the output with a _0 appended to the name
//
// .SECTION Description
// This filter preserves all the topology of the input, but only some arrays
// will have a '_0' appended to the names.
//
// Arrays with special attributes (scalars, pedigree ids, etc.) will retain those
// attributes in the output.

class vtkCallbackCommand;
class vtkDataArraySelection;

#ifndef __vtkAddZeros_h
#define __vtkAddZeros_h

#include "vtkFiltersGeneralModule.h" // For export macro
#include "vtkDataObjectAlgorithm.h"

class VTK_EXPORT vtkAddZeros : public vtkDataObjectAlgorithm
{
public:
  static vtkAddZeros* New();
  vtkTypeMacro(vtkAddZeros,vtkDataObjectAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Adds "_0" to the name of each array to pass through.
  
  // Description:
  // This is required to capture REQUEST_DATA_OBJECT requests.
  virtual int ProcessRequest(vtkInformation* request,
                             vtkInformationVector** inputVector,
                             vtkInformationVector* outputVector);

  // Description:
  // Get the number of point or cell arrays available in the input.
  int GetNumberOfPointArrays();
  int GetNumberOfCellArrays();

  // Description:
  // Get the name of the point or cell array with the given index in
  // the input.
  const char* GetPointArrayName(int index);
  const char* GetCellArrayName(int index);
  
    // Description:
  // Get/Set whether the point or cell array with the given name is to
  // be treated.
  int GetPointArrayStatus(const char* name);
  int GetCellArrayStatus(const char* name);
  //int GetFieldArrayStatus(const char* name);
  void SetPointArrayStatus(const char* name, int status);
  void SetCellArrayStatus(const char* name, int status);
  //void SetFieldArrayStatus(const char* name, int status);
  
    // Callback registered with the SelectionObserver.
  static void SelectionModifiedCallback(vtkObject* caller, unsigned long eid,
                                        void* clientdata, void* calldata);
  
protected:
  vtkAddZeros();
  ~vtkAddZeros();

  // Description:
  // Override to limit types of supported input types to non-composite
  // datasets
  virtual int FillInputPortInformation(int port, vtkInformation* info);

  // Description:
  // Creates the same output type as the input type.
  virtual int RequestDataObject(vtkInformation* request,
                                vtkInformationVector** inputVector,
                                vtkInformationVector* outputVector);

  int RequestData(vtkInformation*, vtkInformationVector**, vtkInformationVector*);
  int RequestInformation(vtkInformation* request, vtkInformationVector** inputVector, vtkInformationVector* outputVector );

private:
  vtkAddZeros(const vtkAddZeros&); // Not implemented
  void operator=(const vtkAddZeros&);   // Not implemented
  
    // The observer to modify this object when the array selections are
  // modified.
  vtkCallbackCommand* SelectionObserver;
    // The array selections.
  vtkDataArraySelection* PointDataArraySelection;
  vtkDataArraySelection* CellDataArraySelection;
  
};

#endif

