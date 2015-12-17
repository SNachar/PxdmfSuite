/*=========================================================================

  Program:   PXDMFReader Plugin
  Module:    vtkAddZeros.cxx

  Copyright (c) GeM, Ecole Centrale Nantes.
  All rights reserved.
  Copyright: See COPYING file that comes with this distribution


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

// vtk Includes
#include "vtkAddZeros.h"
#include <vtkCellData.h>
#include <vtkPointData.h>
#include <vtkDataSet.h>
#include "vtkDemandDrivenPipeline.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkCallbackCommand.h"
#include "vtkDataArraySelection.h"
#include "vtkMultiBlockDataSet.h"
#include "vtkDataObjectTreeIterator.h"

vtkStandardNewMacro(vtkAddZeros);


vtkAddZeros::vtkAddZeros()
{
  
  this->PointDataArraySelection = vtkDataArraySelection::New();                          // delete ok
  this->CellDataArraySelection = vtkDataArraySelection::New();                           // delete ok
    // Setup the selection callback to modify this object when an array
  // selection is changed.
  this->SelectionObserver = vtkCallbackCommand::New();                                   // delete ok
  this->SelectionObserver->SetCallback(&vtkAddZeros::SelectionModifiedCallback);
  this->SelectionObserver->SetClientData(this);
  this->PointDataArraySelection->AddObserver(vtkCommand::ModifiedEvent,
                                             this->SelectionObserver);
  this->CellDataArraySelection->AddObserver(vtkCommand::ModifiedEvent,
                                            this->SelectionObserver);
  
}
//----------------------------------------------------------------------------
int vtkAddZeros::GetNumberOfCellArrays()
{
  return this->CellDataArraySelection->GetNumberOfArrays();
}
//----------------------------------------------------------------------------
int vtkAddZeros::GetNumberOfPointArrays()
{
  return this->PointDataArraySelection->GetNumberOfArrays();
}
//----------------------------------------------------------------------------
void vtkAddZeros::SelectionModifiedCallback(vtkObject*, unsigned long,
                                             void* clientdata, void*){
  static_cast<vtkAddZeros*>(clientdata)->Modified();
}
//----------------------------------------------------------------------------
vtkAddZeros::~vtkAddZeros()
{
  //delete this->Implementation;
  this->CellDataArraySelection->RemoveObserver(this->SelectionObserver);
  this->PointDataArraySelection->RemoveObserver(this->SelectionObserver);
  this->SelectionObserver->Delete();
  this->CellDataArraySelection->Delete();
  this->PointDataArraySelection->Delete();
}
//----------------------------------------------------------------------------
int vtkAddZeros::RequestData(
  vtkInformation*,
  vtkInformationVector** inputVector,
  vtkInformationVector* outputVector)
{
  // Get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  // Get the input and output objects
  vtkDataSet* input = vtkDataSet::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkDataSet * output = vtkDataSet::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));
  output->ShallowCopy(input);


  //PointData 
  for(int i =0 ; i <input->GetPointData()->GetNumberOfArrays();++i){
    std::string name = input->GetPointData()->GetArrayName(i);
    //std::cout << "GetPointArrayStatus (" << name.c_str()<< ")  " << this->GetPointArrayStatus(name.c_str()) << std::endl;
    if (this->GetPointArrayStatus(name.c_str()) ){
      vtkAbstractArray* data = input->GetPointData()->GetAbstractArray(i);
      vtkPointData* outData = output->GetPointData();  
      if (!data)  continue;
      
      outData->RemoveArray(name.c_str());
     
      vtkAbstractArray*  arr2 = data->NewInstance();
      arr2->DeepCopy(data);
      arr2->SetName((data->GetName() + std::string("_0")).c_str());
      outData->AddArray(arr2);
      
      // Preserve attribute type if applicable
      vtkDataSetAttributes* attrib = vtkDataSetAttributes::SafeDownCast(data);
      vtkDataSetAttributes* outAttrib = vtkDataSetAttributes::SafeDownCast(outData);
      if (attrib)
        {
        for (int a = 0; a < vtkDataSetAttributes::NUM_ATTRIBUTES; ++a)
          {
          if (attrib->GetAbstractAttribute(a) == data)
            {
            outAttrib->SetActiveAttribute((name+ std::string("_0")).c_str(), a);
            }
          }
        }
    }
  } 
  //CellData 
  for(int i =0 ; i <input->GetCellData()->GetNumberOfArrays();++i){
    std::string name = input->GetCellData()->GetArrayName(i);
    //std::cout << "GetCellArrayStatus (" << name.c_str()<< ")  " << this->GetCellArrayStatus(name.c_str()) << std::endl;
    if (this->GetCellArrayStatus(name.c_str()) ){
      vtkAbstractArray* data = input->GetCellData()->GetAbstractArray(i);
      vtkCellData* outData = output->GetCellData();  
      if (!data)  continue;
      
      outData->RemoveArray(name.c_str());
     
      vtkAbstractArray*  arr2 = data->NewInstance();
      arr2->DeepCopy(data);
      arr2->SetName((data->GetName() + std::string("_0")).c_str());
      outData->AddArray(arr2);
      
      // Preserve attribute type if applicable
      vtkDataSetAttributes* attrib = vtkDataSetAttributes::SafeDownCast(data);
      vtkDataSetAttributes* outAttrib = vtkDataSetAttributes::SafeDownCast(outData);
      if (attrib)
        {
        for (int a = 0; a < vtkDataSetAttributes::NUM_ATTRIBUTES; ++a)
          {
          if (attrib->GetAbstractAttribute(a) == data)
            {
            outAttrib->SetActiveAttribute((name+ std::string("_0")).c_str(), a);
            }
          }
        }
    }
  }

  return 1;
}
//----------------------------------------------------------------------------
int vtkAddZeros::ProcessRequest(
  vtkInformation* request,
  vtkInformationVector** inputVector,
  vtkInformationVector* outputVector)
{
  // create the output
  if(request->Has(vtkDemandDrivenPipeline::REQUEST_DATA_OBJECT()))
    {
    return this->RequestDataObject(request, inputVector, outputVector);
    }
  return this->Superclass::ProcessRequest(request, inputVector, outputVector);
}
//----------------------------------------------------------------------------
int vtkAddZeros::FillInputPortInformation(int port, vtkInformation* info){
  if (port == 0)
    {
    // Skip composite data sets so that executives will treat this as a simple filter
    info->Remove(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE());
    info->Append(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkDataSet");
    info->Append(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkGenericDataSet");
    info->Append(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkGraph");
    info->Append(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPistonDataObject");
    info->Append(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkTable");
    info->Set(vtkAlgorithm::INPUT_IS_OPTIONAL(), 1);
    }

  return 1;
}
//----------------------------------------------------------------------------
int vtkAddZeros::RequestDataObject(
  vtkInformation*,
  vtkInformationVector** inputVector ,
  vtkInformationVector* outputVector)
{
  vtkInformation* inInfo = inputVector[0]->GetInformationObject(0);
  if (!inInfo)
    {
    return 0;
    }
  vtkDataObject *input = inInfo->Get(vtkDataObject::DATA_OBJECT());

  if (input)
    {
    // for each output
    for(int i=0; i < this->GetNumberOfOutputPorts(); ++i)
      {
      vtkInformation* info = outputVector->GetInformationObject(i);
      vtkDataObject *output = info->Get(vtkDataObject::DATA_OBJECT());

      if (!output || !output->IsA(input->GetClassName()))
        {
        vtkDataObject* newOutput = input->NewInstance();
        info->Set(vtkDataObject::DATA_OBJECT(), newOutput);
        newOutput->Delete();
        }
      }
    return 1;
    }
  return 0;
}

int vtkAddZeros::RequestInformation(
    vtkInformation* request,
    vtkInformationVector** inputVector,
    vtkInformationVector* outputVector ) {
  
    vtkInformation* inInfo = inputVector[0]->GetInformationObject(0);
  if (!inInfo)
    {
    return 0;
    }
  vtkDataSet *input = vtkDataSet::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));

  if (input) {
    //std::cout << "reset " << std::endl;
    //PointDataArraySelection->RemoveAllArrays ();
    //CellDataArraySelection->RemoveAllArrays ();
    // for each output
    for(int i=0; i < this->GetNumberOfOutputPorts(); ++i)
      {
        for(int i =0 ; i <input->GetCellData()->GetNumberOfArrays();++i){
          CellDataArraySelection->AddArray(input->GetCellData()->GetArrayName(i));
        }
        for(int i =0 ; i <input->GetPointData()->GetNumberOfArrays();++i){
          PointDataArraySelection->AddArray(input->GetPointData()->GetArrayName(i));
        }
           
      }
    return 1;
    } else {
      vtkMultiBlockDataSet *input = vtkMultiBlockDataSet::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));
      if(input){
        vtkDataObjectTreeIterator* iter = input->NewTreeIterator();
        iter->VisitOnlyLeavesOff();
        

        for (iter->InitTraversal(); !iter->IsDoneWithTraversal() ; iter->GoToNextItem()) {
          vtkDataSet* inputNode =  vtkDataSet::SafeDownCast(input->GetDataSet(iter));
          if(inputNode){
            for(int i =0 ; i <inputNode->GetCellData()->GetNumberOfArrays();++i){
              CellDataArraySelection->AddArray(inputNode->GetCellData()->GetArrayName(i));
            }
            for(int i =0 ; i <inputNode->GetPointData()->GetNumberOfArrays();++i){
              PointDataArraySelection->AddArray(inputNode->GetPointData()->GetArrayName(i));
            }
          }
        }
        return 1;
      } 
      
    }
  return 0;
}



void vtkAddZeros::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  //os << indent << "RemoveArrays: " << (this->RemoveArrays ? "on" : "off") << endl;
  //os << indent << "UseFieldTypes: " << (this->UseFieldTypes ? "on" : "off") << endl;
}
const char* vtkAddZeros::GetPointArrayName(int index)
{
  return this->PointDataArraySelection->GetArrayName(index);
}
//----------------------------------------------------------------------------
const char* vtkAddZeros::GetCellArrayName(int index)
{
  return this->CellDataArraySelection->GetArrayName(index);
}
//----------------------------------------------------------------------------
int vtkAddZeros::GetPointArrayStatus(const char* name)
{
  return this->PointDataArraySelection->ArrayIsEnabled(name);
}
//----------------------------------------------------------------------------
void vtkAddZeros::SetPointArrayStatus(const char* name, int status)
{
  if(status)
    {
    this->PointDataArraySelection->EnableArray(name);
    }
  else
    {
    this->PointDataArraySelection->DisableArray(name);
    }
}
//----------------------------------------------------------------------------
int vtkAddZeros::GetCellArrayStatus(const char* name)
{
  return this->CellDataArraySelection->ArrayIsEnabled(name);
}
//----------------------------------------------------------------------------
void vtkAddZeros::SetCellArrayStatus(const char* name, int status)
{
  if(status)
    {
    this->CellDataArraySelection->EnableArray(name);
    }
  else
    {
    this->CellDataArraySelection->DisableArray(name);
    }
}