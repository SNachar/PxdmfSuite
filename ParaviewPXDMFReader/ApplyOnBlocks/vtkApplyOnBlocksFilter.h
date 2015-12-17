/*=========================================================================

  Program:   PXDMFReader Plugin
  Module:    ApplyOnBlocksFilter.h

  Copyright (c) GeM, Ecole Centrale Nantes.
  All rights reserved.
  Copyright: See COPYING file that comes with this distribution


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME ApplyOnBlocksFilter - meta-fiter to apply a filter to only selected blocks
// .SECTION Description
// ApplyOnBlocksFilter can be used to apply a filter only to selected blocks of a multi-block dataset. 
// The remaining block are left untouched
//

#ifndef __applyonblocksfilter_h
#define __applyonblocksfilter_h

//#include <map>
//#include <vector>
//#include <utility>

#include "iostream"

#include "vtkFiltersGeneralModule.h" // For export macro
//#include "vtkPointSetAlgorithm.h"
#include "vtkMultiBlockDataSetAlgorithm.h"

//class vtkAlgorithmOutput;
class vtkDataObjectTreeIterator;
//class vtkDataSet;

/// public vtkPointSetAlgorithm
class VTK_EXPORT vtkApplyOnBlocksFilter : public vtkMultiBlockDataSetAlgorithm {
public:
  static vtkApplyOnBlocksFilter* New();
  vtkTypeMacro(vtkApplyOnBlocksFilter, vtkMultiBlockDataSetAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  virtual int ProcessRequest(vtkInformation*,
                             vtkInformationVector**,
                             vtkInformationVector*);
  
  int RequestData( vtkInformation *request, vtkInformationVector **inputVector, vtkInformationVector *outputVector);

  void AddIndex(unsigned int index);
  void RemoveIndex(unsigned int index);
  void RemoveAllIndices();
  unsigned long GetMTime();
  
  // Description:
  // Set/get the internal reader.
  virtual void SetFilter (vtkAlgorithm * _arg);
  vtkGetObjectMacro(Filter, vtkAlgorithm);
  
protected:
  vtkApplyOnBlocksFilter();
  ~vtkApplyOnBlocksFilter();
  
  void CopySubTree(vtkDataObjectTreeIterator* loc, vtkMultiBlockDataSet* output, vtkMultiBlockDataSet* input, bool apply);
    
  class vtkSet;
  vtkSet *Indices;
  vtkSet *ActiveIndices;
  vtkAlgorithm* Filter;
};

#endif
