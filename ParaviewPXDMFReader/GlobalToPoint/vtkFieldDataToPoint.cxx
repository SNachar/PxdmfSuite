/*=========================================================================

  Program:   PXDMFReader Plugin
  Module:    vtkFieldDataToPoint.cxx

  Copyright (c) GeM, Ecole Centrale Nantes.
  All rights reserved.
  Copyright: See COPYING file that comes with this distribution


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkFieldDataToPoint.h"

//VTK Includes
#include "vtkObjectFactory.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkVertex.h"
#include "vtkFieldData.h"
#include "vtkDoubleArray.h"
#include "vtkPointData.h"
#include "vtkUnstructuredGrid.h"
#include "vtkProbeFilter.h"
#include "vtkNew.h"

vtkStandardNewMacro(vtkFieldDataToPoint);
//----------------------------------------------------------------------------
vtkFieldDataToPoint::vtkFieldDataToPoint()
{
    this->XColumn = 0;
    this->YColumn = 0;
    this->ZColumn = 0;
    this->XColumnIndex = -1;
    this->YColumnIndex = -1;
    this->ZColumnIndex = -1;
    this->XComponent = 0;
    this->YComponent = 0;
    this->ZComponent = 0;
    this->Create2DPoints = 0;
    this->PreserveCoordinateAsDataArrays = false;
    this->ExtractFields = true;
    this->SetNumberOfOutputPorts(1);
}

//----------------------------------------------------------------------------
vtkFieldDataToPoint::~vtkFieldDataToPoint()
{
    this->SetXColumn(0);
    this->SetYColumn(0);
    this->SetZColumn(0);
}

//----------------------------------------------------------------------------
int vtkFieldDataToPoint::FillInputPortInformation(
    int vtkNotUsed(port), vtkInformation* info) {
    info->Remove(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE());
    info->Append(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPointSet");
    info->Append(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkImageData");
    info->Append(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkRectilinearGrid");
    return 1;

}
//----------------------------------------------------------------------------
int vtkFieldDataToPoint::RequestDataObject(vtkInformation *request,
        vtkInformationVector **inputVector,
        vtkInformationVector *outputVector) {



    vtkUnstructuredGrid *output = vtkUnstructuredGrid::GetData(outputVector);
    if (!output)  {
        vtkNew<vtkUnstructuredGrid> newOutput;
        outputVector->GetInformationObject(0)->Set(
            vtkDataObject::DATA_OBJECT(), newOutput.GetPointer());
    }

    return 1;
}
//----------------------------------------------------------------------------
int vtkFieldDataToPoint::RequestData(vtkInformation* vtkNotUsed(request),
                                     vtkInformationVector** inputVector, vtkInformationVector* outputVector)
{

    vtkDataObject* input = vtkDataObject::GetData(inputVector[0]);

    vtkFieldData* inputData = input? input->GetFieldData(): 0;

    vtkUnstructuredGrid* output = vtkUnstructuredGrid::GetData(outputVector);
    if (  output== 0) {
        std::cerr <<  " Output is empty " ;
        return  0;

    }

    if (  inputData->GetNumberOfArrays()== 0)
    {
        return 1;
    }

    vtkDataArray* xarray = NULL;
    vtkDataArray* yarray = NULL;
    vtkDataArray* zarray = NULL;


    if(this->XColumn && this->YColumn)
    {
        xarray = vtkDataArray::SafeDownCast(
                     inputData->GetArray(this->XColumn)
                 );
        yarray = vtkDataArray::SafeDownCast(
                     inputData->GetArray(this->YColumn)
                 );
        zarray = vtkDataArray::SafeDownCast(
                     inputData->GetArray(this->ZColumn)
                 );
    }
    else if(this->XColumnIndex >= 0)
    {
        xarray = vtkDataArray::SafeDownCast(
                     inputData->GetArray(this->XColumnIndex)
                 );
        yarray = vtkDataArray::SafeDownCast(
                     inputData->GetArray(this->YColumnIndex)
                 );
        zarray = vtkDataArray::SafeDownCast(
                     inputData->GetArray(this->ZColumnIndex)
                 );
    }

    // zarray is optional
    if(this->Create2DPoints)
    {
        if (!xarray || !yarray)
        {
            vtkErrorMacro("Failed to locate  the columns to use for the point"
                          " coordinates");
            return 0;
        }
    }
    else
    {
        if (!xarray || !yarray || !zarray)
        {
            vtkErrorMacro("Failed to locate  the columns to use for the point"
                          " coordinates");
            return 0;
        }
    }

    vtkPoints* newPoints = vtkPoints::New();                                      // delete ok

    if (xarray == yarray && yarray == zarray &&
            this->XComponent == 0 &&
            this->YComponent == 1 &&
            this->ZComponent == 2 &&
            xarray->GetNumberOfComponents() == 3)
    {
        newPoints->SetData(xarray);
    }
    else
    {
        // Ideally we determine the smallest data type that can contain the values
        // in all the 3 arrays. For now I am just going with doubles.
        vtkDoubleArray* newData =  vtkDoubleArray::New();                                       // delete ok
        newData->SetNumberOfComponents(3);
        newData->SetNumberOfTuples(1);
        vtkIdType numtuples = newData->GetNumberOfTuples();
        if(this->Create2DPoints)
        {
            for (vtkIdType cc=0; cc < numtuples; cc++)
            {
                newData->SetComponent(cc, 0, xarray->GetComponent(cc, this->XComponent));
                newData->SetComponent(cc, 1, yarray->GetComponent(cc, this->YComponent));
                newData->SetComponent(cc, 2, 0.0);
            }
        }
        else
        {
            for (vtkIdType cc=0; cc < numtuples; cc++)
            {
                newData->SetComponent(cc, 0, xarray->GetComponent(cc, this->XComponent));
                newData->SetComponent(cc, 1, yarray->GetComponent(cc, this->YComponent));
                newData->SetComponent(cc, 2, zarray->GetComponent(cc, this->ZComponent));
            }
        }
        newPoints->SetData(newData);
        newData->Delete();
    }

    output->SetPoints(newPoints);
    newPoints->Delete();

    // Now create a poly-vertex cell will all the points.
    vtkIdType numPts = newPoints->GetNumberOfPoints();
    vtkIdType *ptIds = new vtkIdType[numPts];
    for (vtkIdType cc=0; cc < numPts; cc++)
    {
        ptIds[cc] = cc;
    }
    output->Allocate(1);
    output->InsertNextCell(VTK_VERTEX, numPts, ptIds);
    delete [] ptIds;

    // Add all other columns as point data.

    for (int cc=0; cc <  inputData->GetNumberOfArrays(); cc++)
    {
        vtkAbstractArray* arr = inputData->GetArray(cc);
        if(this->PreserveCoordinateAsDataArrays)
        {
            output->GetPointData()->AddArray(arr);
        }
        else if (arr != xarray && arr != yarray && arr != zarray)
        {
            output->GetPointData()->AddArray(arr);
        }
    }

    if(ExtractFields) {
        vtkProbeFilter* probe = vtkProbeFilter::New();
        probe->SetInputData(output);
        probe->SetSourceData(input);
        probe->Update();

        int maskindex;
        if(probe->GetOutput()->GetPointData()->GetArray("vtkValidPointMask", maskindex )->GetTuple1(0) != 0 ) {
            for(int i=0; i < probe->GetOutput()->GetPointData()->GetNumberOfArrays(); ++i ) {
                if(maskindex ==  i ) continue;
                output->GetPointData()->AddArray(probe->GetOutput()->GetPointData()->GetArray(i));
            }
        };
        probe->Delete();                                // delete ok


    }

    return 1;
}

//----------------------------------------------------------------------------
void vtkFieldDataToPoint::PrintSelf(ostream& os, vtkIndent indent)
{
    this->Superclass::PrintSelf(os, indent);
    os << indent << "XColumn: "
       << (this->XColumn? this->XColumn : "(none)") << endl;
    os << indent << "XComponent: " << this->XComponent << endl;
    os << indent << "XColumnIndex: " << this->XColumnIndex << endl;
    os << indent << "YColumn: "
       << (this->YColumn? this->YColumn : "(none)") << endl;
    os << indent << "YComponent: " << this->YComponent << endl;
    os << indent << "YColumnIndex: " << this->YColumnIndex << endl;
    os << indent << "ZColumn: "
       << (this->ZColumn? this->ZColumn : "(none)") << endl;
    os << indent << "ZComponent: " << this->ZComponent << endl;
    os << indent << "ZColumnIndex: " << this->ZColumnIndex << endl;
    os << indent << "Create2DPoints: " << (this->Create2DPoints ? "true" : "false") << endl;
    os << indent << "PreserveCoordinateAsDataArrays: "
       << (this->PreserveCoordinateAsDataArrays ? "true" : "false") << endl;
}


