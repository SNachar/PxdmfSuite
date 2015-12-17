
#include <set>


#include "vtkApplyOnBlocksFilter.h"

#include "vtkObjectFactory.h"
#include <vtkInformation.h>
#include <vtkDemandDrivenPipeline.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkDataObjectTreeIterator.h>

//#define GEM_DEBUG(x) x
#define GEM_DEBUG(x)  

vtkStandardNewMacro(vtkApplyOnBlocksFilter);

class vtkApplyOnBlocksFilter::vtkSet : public std::set<unsigned int>{};

vtkApplyOnBlocksFilter::vtkApplyOnBlocksFilter() {
  Filter = NULL;
  this->Indices = new vtkApplyOnBlocksFilter::vtkSet();
  this->ActiveIndices = new vtkApplyOnBlocksFilter::vtkSet();
}
//
vtkApplyOnBlocksFilter::~vtkApplyOnBlocksFilter() {
  delete this->Indices;
  delete this->ActiveIndices;
  
  if(this->Filter){
    this->Filter->Delete();
  }
}
//
void vtkApplyOnBlocksFilter::AddIndex(unsigned int index){
  this->Indices->insert(index);
  this->Modified();
}
//
void vtkApplyOnBlocksFilter::RemoveIndex(unsigned int index){
  this->Indices->erase(index);
  this->Modified();
}
//
void vtkApplyOnBlocksFilter::RemoveAllIndices(){
  this->Indices->clear();
  this->Modified();
}
//
int vtkApplyOnBlocksFilter::ProcessRequest(vtkInformation* request, vtkInformationVector** inputVector, vtkInformationVector* outputVector){
  GEM_DEBUG(std::cout << "In vtkApplyOnBlocksFilter::ProcessRequest" << std::endl);
  
  if (request->Has(vtkDemandDrivenPipeline::REQUEST_INFORMATION()) && Filter){
    Filter->SetInputConnection(this->GetOutputPort()); 
    int retVal = Filter->ProcessRequest(request,inputVector,outputVector); 
    return retVal;
  }
  
  if (request->Has(vtkDemandDrivenPipeline::REQUEST_DATA())){
    int retVal = this->RequestData(request,inputVector,outputVector); 
    return retVal;
  }  
  return Superclass::ProcessRequest(request, inputVector, outputVector);
};
//
int vtkApplyOnBlocksFilter::RequestData( vtkInformation *request, vtkInformationVector **inputVector, vtkInformationVector *outputVector) {
  GEM_DEBUG(std::cout << "In vtkApplyOnBlocksFilter::RequestData" << std::endl);

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
      //this->ActiveIndices->erase(iter->GetCurrentFlatIndex());  
      //This removed the visited indices from this->ActiveIndices.
      this->CopySubTree(iter, output, input, false);
    }
  }
  iter->Delete();
  this->ActiveIndices->clear();
  
  return 1;
}
//
void vtkApplyOnBlocksFilter::CopySubTree(vtkDataObjectTreeIterator* loc, vtkMultiBlockDataSet* output, vtkMultiBlockDataSet* input, bool apply){
  
  vtkDataObject* inputNode = input->GetDataSet(loc);
  if (!inputNode->IsA("vtkCompositeDataSet")){
    GEM_DEBUG(std::cout << " apply " << (apply?"Yes":"No")  << std::endl);
    vtkDataObject* clone = inputNode->NewInstance();
    if(apply){
      vtkDataObject* cloneinput  = inputNode->NewInstance();
      cloneinput->ShallowCopy(inputNode);
      this->Filter->SetInputDataObject (cloneinput);
      this->Filter->Update();
      
      vtkDataObject* clone = this->Filter->GetOutputDataObject(0)->NewInstance();
      clone->ShallowCopy(this->Filter->GetOutputDataObject(0));
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
//
unsigned long vtkApplyOnBlocksFilter::GetMTime(){
  unsigned long mTime = this->Superclass::GetMTime();
  if (this->Filter)
    mTime = std::max(mTime,this->Filter->GetMTime());
  return mTime;
};
//
void vtkApplyOnBlocksFilter::PrintSelf(ostream& os, vtkIndent indent){
  this->Superclass::PrintSelf(os,indent);
  os << indent << " filter : " << "\n";
  if(this->Filter) this->Filter->PrintSelf(os, indent.GetNextIndent());
}
//
void vtkApplyOnBlocksFilter::SetFilter (vtkAlgorithm * _arg){
     GEM_DEBUG(std::cout << "In vtkApplyOnBlocksFilter::SetFilter" << std::endl);
      
      if (this->Filter != _arg ) { 
        vtkAlgorithm * tempSGMacroVar = this->Filter ; 
        this->Filter = _arg ; 
        if (this->Filter != NULL ) { 
          this->Filter ->Register(this);  
          
        } 
        if (tempSGMacroVar != NULL ) { 
          tempSGMacroVar->UnRegister(this); 
        } 
        this->Modified();
        this->Filter->SetInputDataObject (NULL);
      } 
  }