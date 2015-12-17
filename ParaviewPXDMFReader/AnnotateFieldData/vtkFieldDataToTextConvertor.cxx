/*=========================================================================

  Program:   PXDMFReader Plugin
  Module:    vtkFieldDataToTextConvertor.cxx

  Copyright (c) GeM, Ecole Centrale Nantes.
  All rights reserved.
  Copyright: See COPYING file that comes with this distribution


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

//vtk Includes
#include "vtkFieldDataToTextConvertor.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkStringArray.h"
#include "vtkTable.h"
#include "vtkFieldData.h"
#include "vtkDataArray.h"

//plugin include
#include "vtkPXDMFDocumentBaseStructure.h"

vtkStandardNewMacro(vtkFieldDataToTextConvertor);
//----------------------------------------------------------------------------
vtkFieldDataToTextConvertor::vtkFieldDataToTextConvertor() {
    this->Format = 0;
    this->SetFormat("%f %s");
}
//----------------------------------------------------------------------------
vtkFieldDataToTextConvertor::~vtkFieldDataToTextConvertor() {
    this->SetFormat(0);
}

//----------------------------------------------------------------------------
int vtkFieldDataToTextConvertor::FillInputPortInformation(
    int vtkNotUsed(port), vtkInformation* info){
    info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkDataObject");
    info->Set(vtkAlgorithm::INPUT_IS_OPTIONAL(), 1);
    return 1;
}
//----------------------------------------------------------------------------
int vtkFieldDataToTextConvertor::RequestInformation(
    vtkInformation *request,
    vtkInformationVector **inputVector,
    vtkInformationVector *outputVector){
  
    if (!this->Superclass::RequestInformation(
                request, inputVector, outputVector))
    {
        return 0;
    }
    double timeRange[2];
    timeRange[0] = VTK_DOUBLE_MIN;
    timeRange[1] = VTK_DOUBLE_MAX;

    vtkInformation* outInfo = outputVector->GetInformationObject(0);
    outInfo->Set(vtkStreamingDemandDrivenPipeline::TIME_RANGE(), timeRange, 2);
    return 1;
}

//----------------------------------------------------------------------------
int vtkFieldDataToTextConvertor::RequestData(
    vtkInformation* vtkNotUsed(request),
    vtkInformationVector** inputVector,
    vtkInformationVector* outputVector){
  
    vtkDataObject* input = vtkDataObject::GetData(inputVector[0]);
    vtkTable* output = vtkTable::GetData(outputVector);

    char *buffer = new char[strlen(this->Format)+1024];
    strcpy(buffer, "Nothing");

    vtkFieldData* inputData = input? input->GetFieldData(): 0;

    vtkStringArray* data = vtkStringArray::New();                                               // delete ok.
    data->SetName("Text");
    data->SetNumberOfComponents(1);
    bool nothing =  true;
    vtkInformationStringKey *key =  vtkPXDMFDocumentBaseStructure::UNIT();
    if(inputData && this->Format) {
        strcpy(buffer, "");
        for(int i=0; i < inputData->GetNumberOfArrays(); ++i) {
            vtkDataArray *thearray = inputData->GetArray(i);
            if(thearray ) {
                vtkInformation* info = thearray->GetInformation();  // to get the units

                if(!info->Has(key)) continue;

                int numComponents = thearray->GetNumberOfComponents();
                if(numComponents == 1) {
                    sprintf(buffer+strlen(buffer), "%s: ", thearray->GetName());
                    for(int j=0; j<numComponents; j++) {
                        if(j) {
                            sprintf(buffer+strlen(buffer), "%s", " ,");
                        }
                        if(info->Has(key)) {
                            sprintf(buffer+strlen(buffer), this->Format, thearray->GetTuple(0)[j], info->Get( key));
                        } else {
                            sprintf(buffer+strlen(buffer), this->Format, thearray->GetTuple(0)[j]);
                        }
                    }
                }
                sprintf(buffer+strlen(buffer), "%s", " \n");
                nothing = false;
            }
        }
    } 
    
    if(nothing) {
        sprintf(buffer+strlen(buffer), "%s", "(no field data)\n");
    }
    data->InsertNextValue(buffer);
    output->AddColumn(data);

    data->Delete();

    delete[] buffer;
    return 1;
}

//----------------------------------------------------------------------------
void vtkFieldDataToTextConvertor::PrintSelf(ostream& os, vtkIndent indent){
    this->Superclass::PrintSelf(os, indent);
    os << indent << "Format: "
       << (this->Format? this->Format : "(none)") << endl;
}


