/*=========================================================================

  Program:   PXDMFReader Plugin
  Module:    vtkfiltertest.cxx

  Copyright (c) GeM, Ecole Centrale Nantes.
  All rights reserved.
  Copyright: See COPYING file that comes with this distribution

  
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

//VTK Includes
//#include "vtkCellData.h"
//#include "vtkFloatArray.h"
//#include "vtkUnsignedCharArray.h"
//#include "vtkImageData.h"
//#include "vtkImageDataToPointSet.h"
#include "vtkInformation.h"
//#include "vtkInformationVector.h"
//#include "vtkLinearTransform.h"
#include "vtkObjectFactory.h"
//#include "vtkPointData.h"
//#include "vtkPointSet.h"
//#include "vtkRectilinearGrid.h"
//#include "vtkRectilinearGridToPointSet.h"
#include "vtkNew.h"
//#include "vtkSmartPointer.h"
//#include "vtkMath.h"

// Pluing Includes
#include "vtkfiltertest.h"



vtkStandardNewMacro(vtkfiltertest);
//vtkCxxSetObjectMacro(vtkTransformFilterWithAxis,Transform,vtkAbstractTransform);

vtkfiltertest::vtkfiltertest(){

}
//
vtkfiltertest::~vtkfiltertest(){

}
//
int vtkfiltertest::FillInputPortInformation(
	int vtkNotUsed(port),
	vtkInformation *info){

    info->Remove(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE()); 
    info->Append(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPointSet");
    info->Append(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkImageData");
    info->Append(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkRectilinearGrid");
    return 1;
}
//
int vtkfiltertest::RequestDataObject(
	vtkInformation *request, 
	vtkInformationVector **inputVector, 
	vtkInformationVector *outputVector){

//    vtkImageData *inImage = vtkImageData::GetData(inputVector[0]);
//    vtkRectilinearGrid *inRect = vtkRectilinearGrid::GetData(inputVector[0]);

//    if (inImage || inRect){
//        vtkStructuredGrid *output = vtkStructuredGrid::GetData(outputVector);
//        if (!output){
//            vtkNew<vtkStructuredGrid> newOutput;
//            outputVector->GetInformationObject(0)->Set(
//                vtkDataObject::DATA_OBJECT(), newOutput.GetPointer());
//        }
//        return 1;
//    }
//    else
//    {
        return this->Superclass::RequestDataObject(request,
                inputVector,
                outputVector);
//    }
}
//
int vtkfiltertest::RequestData(
    vtkInformation *vtkNotUsed(request),
    vtkInformationVector **inputVector,
    vtkInformationVector *outputVector){

//    vtkSmartPointer<vtkPointSet> input = vtkPointSet::GetData(inputVector[0]);
//    vtkPointSet *output = vtkPointSet::GetData(outputVector);

//    if (!input){

//        // Try converting image data.
//        vtkImageData *inImage = vtkImageData::GetData(inputVector[0]);
//        if (inImage){

//            vtkNew<vtkImageDataToPointSet> image2points;
//            image2points->SetInputData(inImage);
//            image2points->Update();
//            input = image2points->GetOutput();
//        }
//    }

//    if (!input){

//        // Try converting rectilinear grid.
//        vtkRectilinearGrid *inRect = vtkRectilinearGrid::GetData(inputVector[0]);
//        if (inRect){

//            vtkNew<vtkRectilinearGridToPointSet> rect2points;
//            rect2points->SetInputData(inRect);
//            rect2points->Update();
//            input = rect2points->GetOutput();
//        }
//    }

//    if (!input){
//        vtkErrorMacro(<< "Invalid or missing input");
//        return 0;
//    }

//    vtkPoints *inPts;
//    vtkPoints *newPts;
//    vtkDataArray *inVectors, *inCellVectors;;
//    vtkFloatArray *newVectors=NULL, *newCellVectors=NULL;
//    vtkDataArray *inNormals, *inCellNormals;
//    vtkFloatArray *newNormals=NULL, *newCellNormals=NULL;
//    vtkIdType numPts, numCells;
//    vtkPointData *pd=input->GetPointData(), *outPD=output->GetPointData();
//    vtkCellData *cd=input->GetCellData(), *outCD=output->GetCellData();

//    vtkDebugMacro(<<"Executing transform filter");

//    // First, copy the input to the output as a starting point
//    output->CopyStructure( input );

    // Check input
    //
//    if ( this->Transform == NULL ){
//        vtkErrorMacro(<<"No transform defined!");
//        return 1;
//    }

//    inPts = input->GetPoints();
//    inVectors = pd->GetVectors();
//    inNormals = pd->GetNormals();
//    inCellVectors = cd->GetVectors();
//    inCellNormals = cd->GetNormals();

//    if ( !inPts )
//    {
//        vtkErrorMacro(<<"No input data");
//        return 1;
//    }

//    numPts = inPts->GetNumberOfPoints();
//    numCells = input->GetNumberOfCells();

//    newPts = vtkPoints::New();
//    newPts->Allocate(numPts);
//    if ( inVectors )
//    {
//        newVectors = vtkFloatArray::New();
//        newVectors->SetNumberOfComponents(3);
//        newVectors->Allocate(3*numPts);
//        newVectors->SetName(inVectors->GetName());
//    }
//    if ( inNormals )
//    {
//        newNormals = vtkFloatArray::New();
//        newNormals->SetNumberOfComponents(3);
//        newNormals->Allocate(3*numPts);
//        newNormals->SetName(inNormals->GetName());
//    }

//    this->UpdateProgress (.2);
//    // Loop over all points, updating position
//    //

//    if ( inVectors || inNormals )
//    {
//        this->Transform->TransformPointsNormalsVectors(inPts,newPts,
//                inNormals,newNormals,
//                inVectors,newVectors);
//    }
//    else
//    {
//        this->Transform->TransformPoints(inPts,newPts);
//    }

//    this->UpdateProgress (.6);

//    // Can only transform cell normals/vectors if the transform
//    // is linear.
//    vtkLinearTransform* lt=vtkLinearTransform::SafeDownCast(this->Transform);
//    if (lt)
//    {
//        if ( inCellVectors )
//        {
//            newCellVectors = vtkFloatArray::New();
//            newCellVectors->SetNumberOfComponents(3);
//            newCellVectors->Allocate(3*numCells);
//            newCellVectors->SetName( inCellVectors->GetName() );
//            lt->TransformVectors(inCellVectors,newCellVectors);
//        }
//        if ( inCellNormals )
//        {
//            newCellNormals = vtkFloatArray::New();
//            newCellNormals->SetNumberOfComponents(3);
//            newCellNormals->Allocate(3*numCells);
//            newCellNormals->SetName( inCellNormals->GetName() );
//            lt->TransformNormals(inCellNormals,newCellNormals);
//        }
//    }

//    this->UpdateProgress (.8);

//    // Update ourselves and release memory
//    //
//    output->SetPoints(newPts);
//    newPts->Delete();

//    if (newNormals)
//    {
//        outPD->SetNormals(newNormals);
//        newNormals->Delete();
//        outPD->CopyNormalsOff();
//    }

//    if (newVectors)
//    { 
//        outPD->SetVectors(newVectors);
//        newVectors->Delete();
//        outPD->CopyVectorsOff();
//    }

//    if (newCellNormals)
//    {
//        outCD->SetNormals(newCellNormals);
//        newCellNormals->Delete();
//        outCD->CopyNormalsOff();
//    }

//    if (newCellVectors)
//    {
//        outCD->SetVectors(newCellVectors);
//        newCellVectors->Delete();
//        outCD->CopyVectorsOff();
//    }

//    outPD->PassData(pd);
//    outCD->PassData(cd);
    
    return 1;
}
//
//unsigned long vtkfiltertest::GetMTime(){
//    unsigned long mTime=this->MTime.GetMTime();
//    unsigned long transMTime;

//    if ( this->Transform ){
//        transMTime = this->Transform->GetMTime();
//        mTime = ( transMTime > mTime ? transMTime : mTime );
//    }
//    return mTime;
//}

void vtkfiltertest::PrintSelf(ostream& os, vtkIndent indent)
{
    this->Superclass::PrintSelf(os,indent);

    os << indent << "this filter  data: " << "\n";

}
