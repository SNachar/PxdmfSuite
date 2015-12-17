/*=========================================================================

  Program:   PXDMFReader Plugin
  Module:    vtkApplyOnBlocks.cxx

  Copyright (c) GeM, Ecole Centrale Nantes.
  All rights reserved.
  Copyright: See COPYING file that comes with this distribution


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

//Std Includes
#include <vector>
#include <set>

//VTK Includes
#include "vtkObjectFactory.h"
#include "vtkDataSet.h"
#include "vtkInformationVector.h"
#include <vtkInformation.h>
#include "vtkAlgorithmOutput.h"
#include "vtkAlgorithm.h"
#include <vtkDataObjectAlgorithm.h>
#include "vtkMultiBlockDataSet.h"
#include "vtkDataObjectTreeIterator.h"

// Plugin Includes
#include "vtkApplyOnBlocks.h"

#define GEM_DEBUG ///
//#define GEM_DEBUG 

vtkStandardNewMacro(vtkApplyOnBlocks);

class vtkApplyOnBlocks::vtkSet : public std::set<unsigned int>{};

vtkApplyOnBlocks::vtkApplyOnBlocks() {
  AlgoToApply = NULL;
  tmpinput = NULL; 
  this->Indices = new vtkApplyOnBlocks::vtkSet();
  this->ActiveIndices = new vtkApplyOnBlocks::vtkSet();
}
//
vtkApplyOnBlocks::~vtkApplyOnBlocks() {
  delete this->Indices;
  delete this->ActiveIndices;
}
//
void vtkApplyOnBlocks::AddIndex(unsigned int index){
  this->Indices->insert(index);
  this->Modified();
}
//
void vtkApplyOnBlocks::RemoveIndex(unsigned int index){
  this->Indices->erase(index);
  this->Modified();
}
//
void vtkApplyOnBlocks::RemoveAllIndices(){
  this->Indices->clear();
  this->Modified();
}
//
void vtkApplyOnBlocks::SetAlgoToApply (vtkAlgorithmOutput* _arg) { 
  GEM_DEBUG(std::cout << "In vtkApplyOnBlocks::SetalgoToApply" << std::endl);
  
  if (this->AlgoToApplyOutput != _arg) { 
    this->AlgoToApplyOutput = _arg;
    AlgoToApply   = this->AlgoToApplyOutput->GetProducer()->GetInputAlgorithm (0,0);
    //this->ClearFilters ();
    //this->RegisterFilter(algoToApply);
    //this->Superclass::SetActiveFilter(0);
    //std::cout << "tmpinput" << tmpinput << std::endl;
    //std::cout << "this->algoToApply " <<this->algoToApply << std::endl;
    //std::cout << "this->algoToApply->GetProducer() " <<this->algoToApply->GetProducer() << std::endl;
    //std::cout << "this->algoToApply->GetProducer()->GetInputAlgorithm (0,0) " << this->algoToApply->GetProducer()->GetInputAlgorithm (0,0)<< std::endl;
    //this->algoToApply->GetProducer()->GetInputAlgorithm (0,0)->Print(std::cout) ;
    //td::cout << "vtkAlgorithm::SafeDownCast(this->algoToApply->GetProducer()->GetInputAlgorithm (0,0)) " << vtkAlgorithm::SafeDownCast(this->algoToApply->GetProducer()->GetInputAlgorithm (0,0))<< std::endl;
    //std::cout << " " << << std::endl;
    //std::cout << " " << << std::endl;
    ///vtkAlgorithm::SafeDownCast(algoToApplythis->algoToApply->GetProducer()->GetInputAlgorithm (0,0))->SetInputData(0,tmpinput);
     AlgoToApply->SetInputDataObject (tmpinput);
     this->Modified(); 
   } 
} 
//
int vtkApplyOnBlocks::RequestInformation(vtkInformation *info, vtkInformationVector **inputVector, vtkInformationVector *outputVector) {
  GEM_DEBUG(std::cout << "In vtkApplyOnBlocks::RequestInformation" << std::endl);
  
  vtkInformation* outInfo = outputVector->GetInformationObject(0);
   
  if ( vtkCompositeDataSet::GetData ( inputVector[0], 0 ) ) {
    //AlgoToApply->SetInputDataObject (vtkCompositeDataSet::GetData ( inputVector[0], 0 ));
    AlgoToApply->SetInputConnection(this->GetOutputPort()); 
//      vtkCompositeDataSet *input = vtkCompositeDataSet::GetData ( inputVector[0], 0 );
//      vtkCompositeDataIterator* iter = input->NewIterator();
//      for ( iter->InitTraversal(); !iter->IsDoneWithTraversal(); iter->GoToNextItem() ) {
//        tmpinput = vtkDataSet::SafeDownCast ( iter->GetCurrentDataObject() );
//        if(this->algoToApply)
//          std::cout << "seting the data to the filter" << std::endl;
//          std::cout << "tmpinput" << tmpinput << std::endl;
//          //vtkDataObjectAlgorithm::SafeDownCast(this->algoToApply->GetProducer())->SetInputData(0,tmpinput);
//          //->SetInputData(0,tmpinput);
//           algoToApply->SetInputDataObject (tmpinput);
// 
//        break;
//      }
     
   }
   return 1;
}
//
int vtkApplyOnBlocks::RequestData( vtkInformation *request, vtkInformationVector **inputVector, vtkInformationVector *outputVector) {
  GEM_DEBUG(std::cout << "In vtkApplyOnBlocks::RequestData" << std::endl);

  vtkMultiBlockDataSet *input = vtkMultiBlockDataSet::GetData(inputVector[0], 0);
  vtkMultiBlockDataSet *output = vtkMultiBlockDataSet::GetData(outputVector, 0);

  output->CopyStructure(input);

  (*this->ActiveIndices) = (*this->Indices);

  // Copy selected blocks over to the output.
  vtkDataObjectTreeIterator* iter = input->NewTreeIterator();
  iter->VisitOnlyLeavesOff();

  for (iter->InitTraversal(); !iter->IsDoneWithTraversal() ; iter->GoToNextItem()) {
    if (this->ActiveIndices->find(iter->GetCurrentFlatIndex()) != this->ActiveIndices->end()) {
      this->ActiveIndices->erase(iter->GetCurrentFlatIndex());
      // This removed the visited indices from this->ActiveIndices.
      this->CopySubTree(iter, output, input, true);
    } else {
      this->ActiveIndices->erase(iter->GetCurrentFlatIndex());  
      //This removed the visited indices from this->ActiveIndices.
      this->CopySubTree(iter, output, input, false);
    }
  }
  iter->Delete();
  this->ActiveIndices->clear();
  
  return 1;
}
//
void vtkApplyOnBlocks::CopySubTree(vtkDataObjectTreeIterator* loc, vtkMultiBlockDataSet* output, vtkMultiBlockDataSet* input, bool apply){
  
  vtkDataObject* inputNode = input->GetDataSet(loc);
  if (!inputNode->IsA("vtkCompositeDataSet")){
    GEM_DEBUG(std::cout << " apply " << (apply?"Yes":"No")  << std::endl);
    vtkDataObject* clone = inputNode->NewInstance();
    if(apply){
      vtkDataObject* cloneinput  = inputNode->NewInstance();
      cloneinput->ShallowCopy(inputNode);
      AlgoToApply->SetInputDataObject (cloneinput);
      AlgoToApply->Update();
      
      vtkDataObject* clone = AlgoToApply->GetOutputDataObject(0)->NewInstance();
      clone->ShallowCopy(AlgoToApply->GetOutputDataObject(0));
      output->SetDataSet(loc, clone);
      clone->Delete();
    } else {
      vtkDataObject* clone = inputNode->NewInstance();
      clone->ShallowCopy(inputNode);
      output->SetDataSet(loc, clone);
      clone->Delete();
    }
  } else {
    vtkCompositeDataSet* cinput = vtkCompositeDataSet::SafeDownCast(inputNode);
    vtkCompositeDataSet* coutput = vtkCompositeDataSet::SafeDownCast(output->GetDataSet(loc));
    vtkCompositeDataIterator* iter = cinput->NewIterator();
    vtkDataObjectTreeIterator* treeIter = vtkDataObjectTreeIterator::SafeDownCast(iter);
    if(treeIter) {
      treeIter->VisitOnlyLeavesOff();
    }
    for (iter->InitTraversal(); !iter->IsDoneWithTraversal(); iter->GoToNextItem()){
      vtkDataObject* curNode = iter->GetCurrentDataObject();
      vtkDataObject* clone = curNode->NewInstance();
      clone->ShallowCopy(curNode);
      coutput->SetDataSet(iter, clone);
      clone->Delete();
      this->ActiveIndices->erase(loc->GetCurrentFlatIndex() + iter->GetCurrentFlatIndex());
    }
    iter->Delete();
  }
}
//
void vtkApplyOnBlocks::PrintSelf(ostream& os, vtkIndent indent){
  this->Superclass::PrintSelf(os,indent);
  //os << indent << " : " << "\n";
}
//
int vtkApplyOnBlocks::ProcessRequest(vtkInformation *request, vtkInformationVector **inputVector, vtkInformationVector *outputVector){
  GEM_DEBUG(std::cout << "In vtkApplyOnBlocks::ProcessRequest" << std::endl);
  
  //if (request->Has(vtkDemandDrivenPipeline::REQUEST_DATA())){
  //     return this->RequestData(request, inputVector, outputVector);  
  //}
  request->Print(std::cout);
  return this->Superclass::ProcessRequest(request, inputVector, outputVector);
};
//
unsigned long vtkApplyOnBlocks::GetMTime(){
  unsigned long mTime = this->Superclass::GetMTime();
  mTime = std::max(mTime,AlgoToApply->GetMTime());
  return mTime;
};