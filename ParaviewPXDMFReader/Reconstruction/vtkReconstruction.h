/*=========================================================================

  Program:   PXDMFReader Plugin
  Module:    vtkReconstruction.h

  Copyright (c) GeM, Ecole Centrale Nantes.
  All rights reserved.
  Copyright: See COPYING file that comes with this distribution


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkReconstruction
// .SECTION Description

#ifndef __vtkReconstruction_h
#define __vtkReconstruction_h

// Pluing Includes
#include "vtkPXDMFDocumentBaseStructure.h"

class vtkMultiBlockDataSet;
class vtkStringArray;

class VTK_EXPORT vtkReconstruction : public vtkPXDMFDocumentBaseStructure //vtkTableAlgorithm
{
public:
    static vtkReconstruction* New();
    vtkTypeMacro(vtkReconstruction, vtkPXDMFDocumentBaseStructure);
    void PrintSelf(ostream& os, vtkIndent indent);

    int GetNumberOfVisualizationTimeArrays() {
        return this->GetNumberOfPXDMFDims();
    };

    virtual int FillInputPortInformation(int port, vtkInformation* info);
    virtual int FillOutputPortInformation(int port, vtkInformation* info);
    virtual int RequestData(vtkInformation* request,
                            vtkInformationVector** inputVector,
                            vtkInformationVector* outputVector);
    virtual int RequestDataObject(vtkInformation *request,
                                  vtkInformationVector **inputVector,
                                  vtkInformationVector *outputVector);
    int ProcessRequest(  vtkInformation* request,  vtkInformationVector** inputVector,  vtkInformationVector* outputVector);
    virtual int RequestInformation(
        vtkInformation* request,
        vtkInformationVector** inputVector,
        vtkInformationVector* outputVector );

    vtkDataObject* GenerateOutput(vtkInformation * Info, vtkInformationVector ** inputVector, vtkInformationVector *outputVector);
    void  CalculateMaxMin(const unsigned j);
    void PrepareDocumentInput(vtkInformation* request,
                              vtkInformationVector** inputVector,
                              vtkInformationVector* outputVector);
    vtkDoubleArray* GetPXDMFDimsMinRangeDataInfo();
    vtkStringArray* GetPXDMFDimsNamesDataInfo();
    vtkStringArray* GetPXDMFDimsUnitsDataInfo();
    vtkDoubleArray* GetPXDMFDimsMaxRangeDataInfo();
    vtkDoubleArray* GetPXDMFDimsPosDataInfo();
    const char* GetVisualizationTimeArrayName(int index);
    int GetVisualizationTimeArrayStatus(const char* name);
    void SetVisualizationTimeStatus(const char* name, int status);
    void SetPXDMFDimsPosDataInfo(int id , double value);
    
    vtkSetMacro(ContinuousUpdate, bool);
    vtkGetMacro(ContinuousUpdate, bool);
    vtkBooleanMacro(ContinuousUpdate, bool);
    bool ContinuousUpdate;
    
protected:
    vtkReconstruction();
    ~vtkReconstruction();

private:
    vtkReconstruction(const vtkReconstruction&); // Not implemented
    void operator=(const vtkReconstruction&); // Not implemented
    
    vtkSmartPointer<vtkStringArray> vtkPXDMFDimsNamesData;
    vtkSmartPointer<vtkStringArray> vtkPXDMFDimsUnitsData;
    vtkSmartPointer<vtkDoubleArray> vtkPXDMFDimsMinRangeData;
    vtkSmartPointer<vtkDoubleArray> vtkPXDMFDimsMaxRangeData;
    vtkSmartPointer<vtkDoubleArray> vtkPXDMFDimsPosData;
};

#endif

