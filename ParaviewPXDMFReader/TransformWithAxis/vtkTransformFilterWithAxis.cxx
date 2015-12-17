/*=========================================================================

  Program:   PXDMFReader Plugin
  Module:    vtkTransformFilterWithAxis.cxx

  Copyright (c) GeM, Ecole Centrale Nantes.
  All rights reserved.
  Copyright: See COPYING file that comes with this distribution

  
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

//VTK Includes
#include "vtkCellData.h"
#include "vtkFloatArray.h"
#include "vtkUnsignedCharArray.h"
#include "vtkImageData.h"
#include "vtkImageDataToPointSet.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkLinearTransform.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkPointSet.h"
#include "vtkRectilinearGrid.h"
#include "vtkRectilinearGridToPointSet.h"
#include "vtkNew.h"
#include "vtkSmartPointer.h"
#include "vtkMath.h"
#include "vtkStringArray.h"

// Pluing Includes
#include "vtkTransformFilterWithAxis.h"
#include "vtkPXDMFDocumentBaseStructure.h"

vtkStandardNewMacro(vtkTransformFilterWithAxis);
vtkCxxSetObjectMacro(vtkTransformFilterWithAxis,Transform,vtkAbstractTransform);

vtkTransformFilterWithAxis::vtkTransformFilterWithAxis()
{
    this->Transform = NULL;
    this->MoveRotateScaleAxis = 1;
	TitleNameX = NULL;
    UnitNameX = NULL;
    TitleNameY = NULL;
    UnitNameY = NULL;
    TitleNameZ = NULL;
    UnitNameZ = NULL;
    
}
//
vtkTransformFilterWithAxis::~vtkTransformFilterWithAxis()
{
    this->SetTransform(NULL);

	this->SetTitleNameX(NULL);
    this->SetUnitNameX(NULL);
    this->SetTitleNameY(NULL);
    this->SetUnitNameY(NULL);
    this->SetTitleNameZ(NULL);
    this->SetUnitNameZ(NULL);
}

int vtkTransformFilterWithAxis::FillInputPortInformation(int vtkNotUsed(port),
        vtkInformation *info)
{
    info->Remove(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE()); 
    info->Append(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPointSet");
    info->Append(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkImageData");
    info->Append(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkRectilinearGrid");
    return 1;
}

int vtkTransformFilterWithAxis::RequestDataObject(vtkInformation *request,
        vtkInformationVector **inputVector,
        vtkInformationVector *outputVector)
{
    vtkImageData *inImage = vtkImageData::GetData(inputVector[0]);
    vtkRectilinearGrid *inRect = vtkRectilinearGrid::GetData(inputVector[0]);

    if (inImage || inRect)
    {
        vtkStructuredGrid *output = vtkStructuredGrid::GetData(outputVector);
        if (!output)
        {
            vtkNew<vtkStructuredGrid> newOutput;
            outputVector->GetInformationObject(0)->Set(
                vtkDataObject::DATA_OBJECT(), newOutput.GetPointer());
        }
        return 1;
    }
    else
    {
        return this->Superclass::RequestDataObject(request,
                inputVector,
                outputVector);
    }
}

int vtkTransformFilterWithAxis::RequestData(
    vtkInformation *vtkNotUsed(request),
    vtkInformationVector **inputVector,
    vtkInformationVector *outputVector)
{
    vtkSmartPointer<vtkPointSet> input = vtkPointSet::GetData(inputVector[0]);
    vtkPointSet *output = vtkPointSet::GetData(outputVector);

    if (!input)
    {
        // Try converting image data.
        vtkImageData *inImage = vtkImageData::GetData(inputVector[0]);
        if (inImage)
        {
            vtkNew<vtkImageDataToPointSet> image2points;
            image2points->SetInputData(inImage);
            image2points->Update();
            input = image2points->GetOutput();
        }
    }

    if (!input)
    {
        // Try converting rectilinear grid.
        vtkRectilinearGrid *inRect = vtkRectilinearGrid::GetData(inputVector[0]);
        if (inRect)
        {
            vtkNew<vtkRectilinearGridToPointSet> rect2points;
            rect2points->SetInputData(inRect);
            rect2points->Update();
            input = rect2points->GetOutput();
        }
    }

    if (!input)
    {
        vtkErrorMacro(<< "Invalid or missing input");
        return 0;
    }

    vtkPoints *inPts;
    vtkPoints *newPts;
    vtkDataArray *inVectors, *inCellVectors;;
    vtkFloatArray *newVectors=NULL, *newCellVectors=NULL;
    vtkDataArray *inNormals, *inCellNormals;
    vtkFloatArray *newNormals=NULL, *newCellNormals=NULL;
    vtkIdType numPts, numCells;
    vtkPointData *pd=input->GetPointData(), *outPD=output->GetPointData();
    vtkCellData *cd=input->GetCellData(), *outCD=output->GetCellData();

    vtkDebugMacro(<<"Executing transform filter");

    // First, copy the input to the output as a starting point
    output->CopyStructure( input );

    // Check input
    //
    if ( this->Transform == NULL )
    {
        vtkErrorMacro(<<"No transform defined!");
        return 1;
    }

    inPts = input->GetPoints();
    inVectors = pd->GetVectors();
    inNormals = pd->GetNormals();
    inCellVectors = cd->GetVectors();
    inCellNormals = cd->GetNormals();

    if ( !inPts )
    {
        vtkErrorMacro(<<"No input data");
        return 1;
    }

    numPts = inPts->GetNumberOfPoints();
    numCells = input->GetNumberOfCells();

    newPts = vtkPoints::New();
    newPts->Allocate(numPts);
    if ( inVectors )
    {
        newVectors = vtkFloatArray::New();
        newVectors->SetNumberOfComponents(3);
        newVectors->Allocate(3*numPts);
        newVectors->SetName(inVectors->GetName());
    }
    if ( inNormals )
    {
        newNormals = vtkFloatArray::New();
        newNormals->SetNumberOfComponents(3);
        newNormals->Allocate(3*numPts);
        newNormals->SetName(inNormals->GetName());
    }

    this->UpdateProgress (.2);
    // Loop over all points, updating position
    //

    if ( inVectors || inNormals )
    {
        this->Transform->TransformPointsNormalsVectors(inPts,newPts,
                inNormals,newNormals,
                inVectors,newVectors);
    }
    else
    {
        this->Transform->TransformPoints(inPts,newPts);
    }

    this->UpdateProgress (.6);

    // Can only transform cell normals/vectors if the transform
    // is linear.
    vtkLinearTransform* lt=vtkLinearTransform::SafeDownCast(this->Transform);
    if (lt)
    {
        if ( inCellVectors )
        {
            newCellVectors = vtkFloatArray::New();
            newCellVectors->SetNumberOfComponents(3);
            newCellVectors->Allocate(3*numCells);
            newCellVectors->SetName( inCellVectors->GetName() );
            lt->TransformVectors(inCellVectors,newCellVectors);
        }
        if ( inCellNormals )
        {
            newCellNormals = vtkFloatArray::New();
            newCellNormals->SetNumberOfComponents(3);
            newCellNormals->Allocate(3*numCells);
            newCellNormals->SetName( inCellNormals->GetName() );
            lt->TransformNormals(inCellNormals,newCellNormals);
        }
    }

    this->UpdateProgress (.8);

    // Update ourselves and release memory
    //
    output->SetPoints(newPts);
    newPts->Delete();

    if (newNormals)
    {
        outPD->SetNormals(newNormals);
        newNormals->Delete();
        outPD->CopyNormalsOff();
    }

    if (newVectors)
    {
        outPD->SetVectors(newVectors);
        newVectors->Delete();
        outPD->CopyVectorsOff();
    }

    if (newCellNormals)
    {
        outCD->SetNormals(newCellNormals);
        newCellNormals->Delete();
        outCD->CopyNormalsOff();
    }

    if (newCellVectors)
    {
        outCD->SetVectors(newCellVectors);
        newCellVectors->Delete();
        outCD->CopyVectorsOff();
    }

    outPD->PassData(pd);
    outCD->PassData(cd);

    
    
    
    vtkFieldData* inFD = input->GetFieldData();
    if (inFD){
        vtkFieldData* outFD = output->GetFieldData();
        if (!outFD){
            outFD = vtkFieldData::New();
            output->SetFieldData(outFD);
            outFD->Delete();
        }
        outFD->PassData(inFD);
        std::vector<std::string> names ;
        names.push_back(std::string("X"));
        names.push_back(std::string("Y"));
        names.push_back(std::string("Z"));
            
        for(int i = 0; i < 3;  ++i){
          
          if( i ==0 && strlen(this->TitleNameX) == 0 ) continue;
          if( i ==1 && strlen(this->TitleNameY) == 0 ) continue;
          if( i ==2 && strlen(this->TitleNameZ) == 0 ) continue;

          
          vtkStringArray* newtitle = vtkStringArray::New();  
          newtitle->SetNumberOfComponents(1);
          newtitle->SetName( ("AxisTitleFor"+names[i]).c_str() );
          char uString[120];

         switch(i){
            case(0):strcpy(uString, TitleNameX);break;
            case(1):strcpy(uString, TitleNameY);break;
            case(2):strcpy(uString, TitleNameZ);break;
          }
          
          //strcpy(uString, (TitleNames[i]).c_str());
          
          newtitle->Resize(1);
          newtitle->InsertValue(0,uString);
          
          
          switch(i){
            case(0):{
              if( strlen(this->UnitNameX) == 0 ) break;
              newtitle->GetInformation()->Set(
                    vtkPXDMFDocumentBaseStructure::UNIT(),
                    UnitNameX);
              break;
            }
            case(1):{
              if( strlen(this->UnitNameY) == 0 ) break;
              newtitle->GetInformation()->Set(
                    vtkPXDMFDocumentBaseStructure::UNIT(),
                    UnitNameY);
              break;}
            case(2):{
               if( strlen(this->UnitNameZ) == 0 ) break;
              newtitle->GetInformation()->Set(
                    vtkPXDMFDocumentBaseStructure::UNIT(),
                    UnitNameZ);
              break;}
          }
          
          
          outFD->AddArray(newtitle);
        }
        
        
        
        
        

        /// transformation of the axis;
        if(this->MoveRotateScaleAxis && lt) {
            int index;  // just a temporal value


            std::vector<double> norms;
            norms.resize(3);
            double uvw[3][3] ; 	/// the old axis
            double upvpwp[3][3] ;	/// the new one

            /// the compute the new axis
            for(unsigned i = 0; i < 3; ++i) {
                vtkFloatArray* newFieldVectors = vtkFloatArray::New();
                newFieldVectors->SetNumberOfComponents(3);
                newFieldVectors->SetName( ("AxisBaseFor"+names[i]).c_str() );

		vtkDataArray* data = inFD->GetArray(("AxisBaseFor"+names[i]).c_str(),index);
                if( data ) {
		    /// in the case we have a  AxisBaseFor... already
                    for(int  k =0; k < 3 ; k++) {
                        uvw[i][k] = data->GetTuple(0)[k];
                    }
                } else {
                    /// we create the axis 
                    for(unsigned j = 0; j < 3; ++j) {
                        uvw[i][j] = 0;
                    }
                    uvw[i][i] = 1.;
                }
		
		/// we apply the tranformation for the axis
                lt->TransformNormal(uvw[i],upvpwp[i]);
		
		/// in the case is a 2D domain, we generate the third axis with a cross product
                if(i == 2) {
                    if(vtkMath::Norm(upvpwp[i]) == 0) {
                        vtkMath::Cross(upvpwp[0],upvpwp[1],upvpwp[2]);
                    }
                }

                newFieldVectors->InsertNextTuple(upvpwp[i]);
                outFD->AddArray(newFieldVectors);
                newFieldVectors->Delete();
            }

            double Pmin[3];
            double Ppmin[3];
            double Pmax[3];
            double Ppmax[3];
            
            ///1) Compute Pmin and Pmax as follow
            ///Pmin = U.OrientedBounds[0] + V.OrientedBouds[2] + W.OrientedBouds[4]
            vtkFloatArray* data = dynamic_cast<vtkFloatArray*>( inFD->GetArray("OrientedBoundingBox",index));
            if( data ) {

                for(unsigned j = 0; j < 3; ++j) {
                    Pmin[j] =  uvw[0][j]*data->GetPointer(0)[0]
                               + uvw[1][j]*data->GetPointer(0)[2]
                               + uvw[2][j]*data->GetPointer(0)[4];

                    Pmax[j] =  uvw[0][j]*data->GetPointer(0)[1]
                               + uvw[1][j]*data->GetPointer(0)[3]
                               + uvw[2][j]*data->GetPointer(0)[5];
                }
            } else {

                for(unsigned j = 0; j < 3; ++j) {
                    Pmin[j] =  uvw[0][j]*input->GetPoints()->GetBounds()[0]
                               + uvw[1][j]*input->GetPoints()->GetBounds()[2]
                               + uvw[2][j]*input->GetPoints()->GetBounds()[4];

                    Pmax[j] =  uvw[0][j]*input->GetPoints()->GetBounds()[1]
                               + uvw[1][j]*input->GetPoints()->GetBounds()[3]
                               + uvw[2][j]*input->GetPoints()->GetBounds()[5];
                }

            }

            ///2) Apply your transform (Rotation, Scale, Translation) to those 2 points
            lt->TransformPoint(Pmin,Ppmin);
            lt->TransformPoint(Pmax,Ppmax);

    
            ///4) resolve the equations
            double A[3][3];

            for(unsigned j=0; j < 3 ; j++) {
                for(unsigned k=0; k < 3 ; k++) {
                    A[j][k] = upvpwp[k][j];
                }
            }

            double y1[3];
            double y2[3];

            vtkMath::LinearSolve3x3(A,Ppmin, y1 );

            for(unsigned j=0; j < 3 ; j++) {
                for(unsigned k=0; k < 3 ; k++) {
                    A[j][k] = upvpwp[k][j];
                }
            }

            vtkMath::LinearSolve3x3(A,Ppmax, y2 );

            vtkFloatArray* newFieldVectors = vtkFloatArray::New();
            newFieldVectors->SetNumberOfComponents(6);
            for(unsigned j = 0; j < 3; j++) {
                newFieldVectors->InsertNextValue(y1[j]);
                newFieldVectors->InsertNextValue(y2[j]);
            }


            newFieldVectors->SetName( "OrientedBoundingBox" );
            outFD->AddArray(newFieldVectors);
            newFieldVectors->Delete();

            // "LabelRangeActiveFlag"  //
            {
                vtkUnsignedCharArray* newFieldVectors = vtkUnsignedCharArray::New();
                newFieldVectors->SetNumberOfComponents(1);
                newFieldVectors->SetName( "LabelRangeActiveFlag" );
                newFieldVectors->InsertNextValue(this->LabelRange);
                newFieldVectors->InsertNextValue(this->LabelRange);
                newFieldVectors->InsertNextValue(this->LabelRange);
                outFD->AddArray(newFieldVectors);
                newFieldVectors->Delete();
            }
            if(this->LabelRange) {
                // "LabelRangeForXZY"  //
                for(unsigned i = 0; i < 3; ++i) {
                    if(!inFD->GetArray(("LabelRangeFor"+names[i]).c_str(),index)) {
                        vtkFloatArray* newFieldVectors = vtkFloatArray::New();
                        newFieldVectors->SetNumberOfComponents(2);
                        newFieldVectors->SetName( ("LabelRangeFor"+names[i]).c_str() );
                        newFieldVectors->InsertNextValue(input->GetPoints()->GetBounds()[i*2]);
                        newFieldVectors->InsertNextValue(input->GetPoints()->GetBounds()[1+i*2]);
                        outFD->AddArray(newFieldVectors);
                        newFieldVectors->Delete();
                    }
                }

                // "LabelRangeForXZY"  //
                for(unsigned i = 0; i < 3; ++i) {
                    if(!inFD->GetArray(("LinearTransformFor"+names[i]).c_str(),index)) {
                        vtkFloatArray* newFieldVectors = vtkFloatArray::New();
                        newFieldVectors->SetNumberOfComponents(2);
                        newFieldVectors->SetName( ("LinearTransformFor"+names[i]).c_str() );
                        newFieldVectors->InsertNextValue(input->GetPoints()->GetBounds()[1+i*2] - input->GetPoints()->GetBounds()[i*2]);
                        newFieldVectors->InsertNextValue((input->GetPoints()->GetBounds()[1+i*2] + input->GetPoints()->GetBounds()[i*2])/2);
                        outFD->AddArray(newFieldVectors);
                        newFieldVectors->Delete();
                    }
                }
            }

        }

    }

    return 1;
}

unsigned long vtkTransformFilterWithAxis::GetMTime()
{
    unsigned long mTime=this->MTime.GetMTime();
    unsigned long transMTime;

    if ( this->Transform )
    {
        transMTime = this->Transform->GetMTime();
        mTime = ( transMTime > mTime ? transMTime : mTime );
    }

    return mTime;
}

void vtkTransformFilterWithAxis::PrintSelf(ostream& os, vtkIndent indent)
{
    this->Superclass::PrintSelf(os,indent);

    os << indent << "Transform: " << this->Transform << "\n";
}
