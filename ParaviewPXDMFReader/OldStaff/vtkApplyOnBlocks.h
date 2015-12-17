/*=========================================================================

  Program:   PXDMFReader Plugin
  Module:    ApplyOnBlocks.h

  Copyright (c) GeM, Ecole Centrale Nantes.
  All rights reserved.
  Copyright: See COPYING file that comes with this distribution


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME ApplyOnBlocks - Apply a filter to only selected blocks
// .SECTION Description
// ApplyOnBlocks can be used to apply a filter only to selected blocks of a multi-block dataset. 
// The remaining block are lest untouched
//
// .SECTION See Also
// vtk vtk vtk

#ifndef __applyonblocks_h
#define __applyonblocks_h

#include <map>
#include <vector>
#include <utility>

#include "vtkFiltersGeneralModule.h" // For export macro
#include "vtkPointSetAlgorithm.h"
#include "vtkMultiBlockDataSetAlgorithm.h"

class vtkAlgorithmOutput;
class vtkDataObjectTreeIterator;
class vtkDataSet;

/// public vtkPointSetAlgorithm
class VTK_EXPORT vtkApplyOnBlocks : public vtkMultiBlockDataSetAlgorithm {
public:
    static vtkApplyOnBlocks *New();
    vtkTypeMacro(vtkApplyOnBlocks,vtkMultiBlockDataSetAlgorithm);
    void PrintSelf(ostream& os, vtkIndent indent);

    void SetAlgoToApply (vtkAlgorithmOutput* _arg); 
    vtkGetObjectMacro(AlgoToApplyOutput, vtkAlgorithmOutput);
    
    void AddIndex(unsigned int index);
    void RemoveIndex(unsigned int index);
    void RemoveAllIndices();
    unsigned long GetMTime();
protected:

    vtkApplyOnBlocks();
    ~vtkApplyOnBlocks();
    
    virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
    virtual int RequestInformation(vtkInformation *info, vtkInformationVector **inputVector, vtkInformationVector *outputVector);
    
    int ProcessRequest(vtkInformation *request, vtkInformationVector **inputVector, vtkInformationVector *outputVector);
    
    void CopySubTree(vtkDataObjectTreeIterator* loc, vtkMultiBlockDataSet* output, vtkMultiBlockDataSet* input, bool apply);

private:
    vtkApplyOnBlocks(const vtkApplyOnBlocks&);  // Not implemented.
    void operator=(const vtkApplyOnBlocks&);  // Not implemented.
    
    class vtkSet;
    vtkSet *Indices;
    vtkSet *ActiveIndices;
    vtkAlgorithmOutput *AlgoToApplyOutput;
    vtkAlgorithm *AlgoToApply;
    vtkDataSet* tmpinput ;
};

#endif
