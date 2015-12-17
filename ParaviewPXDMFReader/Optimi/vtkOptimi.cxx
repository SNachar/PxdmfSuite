/*=========================================================================

  Program:   PXDMFReader Plugin
  Module:    vtkOptimi.cxx

  Copyright (c) GeM, Ecole Centrale Nantes.
  All rights reserved.
  Copyright: See COPYING file that comes with this distribution


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

// Std Includes 
#include <vector>

//VTK Includes
#include "vtkFieldData.h"
#include "vtkCellData.h"
#include "vtkInformation.h"
#include "vtkInformationDoubleVectorKey.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkPointSet.h"
#include "vtkNew.h"
#include "vtkSmartPointer.h"
#include "vtkDataSet.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkInformationVector.h"
#include "vtkMultiThreader.h"
#include "vtkMutexLock.h"
#include "vtkConditionVariable.h"
#include "vtkAmoebaMinimizer.h"

// Pluing Includes
#include "vtkOptimi.h"
#include "vtkPXDMFDocumentBaseStructure.h"


vtkStandardNewMacro(vtkOptimi);

class vtkThreadUserData {
public:
  vtkMutexLock* Lock;
  vtkConditionVariable* Condition;
  int Done;
} ;

//
vtkOptimi::vtkOptimi() {
    CurrentTimeIndex = 0;
    minimizer = vtkAmoebaMinimizer::New();              // delete ok
    
    MaxNumberOfIterations = 100;
    NumberOfOptimizationSpaceArrays =0;
    threader = vtkMultiThreader::New();                // delete ok
    
    threadID = -1;
    needANewIteration =true;

    maincpt = 1;
    minicpt =0;
    running = 0;
    Maximize = 0;
    this->MaxNumberOfIterations=1000;
    this->ContractionRatio=0.5;
    this->ExpansionRatio=2.0;
    this->Tolerance=0.0001;
    this->ParameterTolerance=0.0001;
    this->PrintValues = 0;
    
    this->myMutex1 = new vtkSimpleMutexLock();
    this->myMutex2 = new vtkSimpleMutexLock();
    this->myMutex3 = new vtkSimpleMutexLock();
    this->myMutex4 = new vtkSimpleMutexLock();

    main_0 = true;
    opti_0 = true;   
}
//
vtkOptimi::~vtkOptimi() {
    this->minimizer->Delete();
    this->myMutex1->Delete();
    this->myMutex2->Delete();
    this->myMutex3->Delete();
    this->myMutex4->Delete();
    this->threader->Delete();
}
//
int vtkOptimi::FillInputPortInformation(
    int vtkNotUsed(port),
    vtkInformation *info) {

    info->Remove(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE());
    info->Append(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkDataObject");
    return 1;
}
//
//
int vtkOptimi::RequestDataObject(
    vtkInformation *request,
    vtkInformationVector **inputVector,
    vtkInformationVector *outputVector) {

    return this->Superclass::RequestDataObject(request,
            inputVector,
            outputVector);
}
//
//
void vtkOptimi::barrier(const bool& main){
 if(main){ 
     if(main_0){
	this->myMutex1->Unlock();
        this->myMutex3->Lock();

	this->myMutex2->Unlock();
        this->myMutex4->Lock();
        main_0 = false;
     } else {
        this->myMutex3->Unlock();
        this->myMutex1->Lock();

        this->myMutex4->Unlock();
	this->myMutex2->Lock();
	main_0 = true;

     }
 } else{
     if(opti_0){
        this->myMutex3->Unlock();
        this->myMutex1->Lock();

        this->myMutex4->Unlock();
	this->myMutex2->Lock();
	opti_0 = false;
     } else {
	this->myMutex1->Unlock();
        this->myMutex3->Lock();

	this->myMutex2->Unlock();
        this->myMutex4->Lock();
	opti_0 = true;
     }
 }

}
//

//
static void  vtkFunctionToMinimize(void *arg){


    vtkOptimi *THIS = static_cast<vtkOptimi *>(arg);

    THIS->barrier(0);
    /// minimizer need a new value;
    THIS->inInfo->Remove(vtkPXDMFDocumentBaseStructure::UPDATE_FIXED_DIMENSIONS());
    std::vector<double> val;
    for (int i = 0; i < THIS->minimizer->GetNumberOfParameters(); ++i){
      std::string name  =  THIS->minimizer->GetParameterName(i);
      const double data = THIS->minimizer->GetParameterValue(name.c_str());
      
      val.push_back(THIS->pos[name].first);
      val.push_back(THIS->pos[name].second);
      val.push_back(data);
      if(THIS->PrintValues){
	  std::cout << "(" << name << "=" << data << ")";
      }
      
    }
    THIS->inInfo->Set(vtkPXDMFDocumentBaseStructure::UPDATE_FIXED_DIMENSIONS(), &val[0], val.size());
    THIS->barrier(0);

    THIS->barrier(0);
    double r = THIS->inScalars->GetComponent(0,0);
    if(THIS->Maximize) r = -1*r ;
    if(THIS->PrintValues){
       std::cout << " = " << r << std::endl;
    }

    THIS->minimizer->SetFunctionValue(r);
    THIS->barrier(0);

}
//
void*  vtkRunMinimization(void *arg)
{
    vtkOptimi *THIS =static_cast<vtkOptimi*>( static_cast<vtkMultiThreader::ThreadInfo*>(arg)->UserData);

    THIS->myMutex3->Lock();//---------------------------------------------------------------------------------------------------------
    THIS->myMutex4->Lock();//---------------------------------------------------------------------------------------------------------

    THIS->running = 1;
    THIS->minimizer->Minimize();
    THIS->needANewIteration = 0;
    THIS->running=0;
    THIS->myMutex3->Unlock();
    THIS->myMutex4->Unlock();

    return NULL;
    
}
int vtkOptimi::RequestData(
    vtkInformation *request,
    vtkInformationVector **inputVector,
    vtkInformationVector *outputVector) {

    vtkSmartPointer<vtkDataSet> input = vtkDataSet::GetData(inputVector[0]);


    inScalars = this->GetInputArrayToProcess(0,inputVector);

    if (  needANewIteration ) {
        
        if( this->CurrentTimeIndex == 0) {
          /// first time
          
          minimizer->Initialize();
          minimizer->SetMaxIterations(this->MaxNumberOfIterations);
          minimizer->SetContractionRatio(this->ContractionRatio);
          minimizer->SetExpansionRatio(this->ExpansionRatio);
          minimizer->SetTolerance(this->Tolerance);
          minimizer->SetParameterTolerance(this->ParameterTolerance);

          
          for(int i =0; i < this->NumberOfOptimizationSpaceArrays; ++i) {
            if(!this->ActivePXDMFDimsForOptimization[i]) continue;
            std::string name = this->PXDMFDimsArrayNames[i];

            vtkInformation* info =  input->GetFieldData()->GetArray(name.c_str())->GetInformation();
            double* data = info->Get(vtkPXDMFDocumentBaseStructure::BOUND_MIN_MAX_PXDMFDim_Dim());
            pos[name] = std::pair<int,int>(data[2],data[3]);
            
            minimizer->SetParameterValue(name.c_str(),input->GetFieldData()->GetArray(name.c_str())->GetComponent(0,0));
            minimizer->SetParameterScale(name.c_str(),(data[1]-data[0])/100);
          }

          this->minimizer->SetFunction(&vtkFunctionToMinimize,this);
          double r = this->inScalars->GetComponent(0,0);

          if(this->Maximize) r  = -1*r;

          this->minimizer->SetFunctionValue(r);
          myMutex1->Lock();//---------------------------------------------------------------------------------------------------------
          myMutex2->Lock();//---------------------------------------------------------------------------------------------------------
 
          threadID  = threader->SpawnThread( (vtkThreadFunctionType)(vtkRunMinimization ) ,  this   ) ;
          while(running == 0){
          }


        } else {
            this->barrier(1);
	        this->barrier(1);
	    }
        
       
        // There is still more to do.
        request->Set(vtkStreamingDemandDrivenPipeline::CONTINUE_EXECUTING(), 1);
        // TODO this line was commented to be able to compile the plugin
        //request->Set(vtkDemandDrivenPipeline::REQUEST_REGENERATE_INFORMATION(), 1);
        ++CurrentTimeIndex;

        std::cout << "// There is still more to do." << std::endl;
        
    } else {

        // We are done.  Finish up.
        std::cout << "// We are done.  Finish up." << std::endl;
        request->Remove(vtkStreamingDemandDrivenPipeline::CONTINUE_EXECUTING());
        this->CurrentTimeIndex = 0;
        needANewIteration =1;
        threader->TerminateThread(threadID);    
        
        
        vtkDataSet *output = vtkDataSet::GetData(outputVector);
        output->CopyStructure(input);
        vtkPointData *pd=input->GetPointData(), *outPD=output->GetPointData();
        vtkCellData  *cd=input->GetCellData(), *outCD=output->GetCellData();
        vtkFieldData *fd=input->GetFieldData(), *outFD=output->GetFieldData();

        outPD->PassData(pd);
        outCD->PassData(cd);
        outFD->PassData(fd);
        myMutex1->Unlock();
        myMutex2->Unlock();
    }
    return 1;
}

void vtkOptimi::PrintSelf(ostream& os, vtkIndent indent)
{
    this->Superclass::PrintSelf(os,indent);
    os << indent << "this filter  data: " << "\n";
}

int vtkOptimi::RequestUpdateExtent( vtkInformation *request,
                                        vtkInformationVector **inputVector,
                                            vtkInformationVector *outputVector) {
   std::cout << "+" << std::endl;
   inInfo = inputVector[0]->GetInformationObject(0);
   
    // pause until the minimizer needs a new value
    if(this->running){
       this->barrier(1);
	// putting data from the opti thread
       this->barrier(1);
     if(needANewIteration){
        //inInfo->Set(vtkStreamingDemandDrivenPipeline::UPDATE_RESOLUTION(), 1.0);
        //inInfo->Set(vtkStreamingDemandDrivenPipeline::REQUEST_UPDATE_EXTENT());
        //inInfo->Set(vtkStreamingDemandDrivenPipeline::REQUEST_TIME_DEPENDENT_INFORMATION());
        inInfo->Set(vtkStreamingDemandDrivenPipeline::REQUEST_DATA());
        //inInfo->Set(vtkStreamingDemandDrivenPipeline::REQUEST_RESOLUTION_PROPAGATE());
        int update_extent[6] = {-1, -2, 0, -1, 0, -1};
        inInfo->Set(vtkStreamingDemandDrivenPipeline::UPDATE_EXTENT(),update_extent,6);
        inInfo->Set(vtkStreamingDemandDrivenPipeline::CONTINUE_EXECUTING(), 1);
        
     }
    }

    request->AppendUnique(vtkExecutive::KEYS_TO_COPY(), vtkPXDMFDocumentBaseStructure::UPDATE_FIXED_DIMENSIONS());
    
    return 1;
}
//
int vtkOptimi::RequestInformation(vtkInformation* request,
                                      vtkInformationVector **inputVector,
                                      vtkInformationVector *outputVector ) {
    vtkSmartPointer<vtkDataSet> input = vtkDataSet::GetData(inputVector[0]);

    if(input) {
        PXDMFDimsArrayNames.clear();
        int cpt = 0;
        for(int i =0; i < input->GetFieldData()->GetNumberOfArrays() ; ++i) {
            if(input->GetFieldData()->GetAbstractArray(i)->GetInformation()->Has(vtkPXDMFDocumentBaseStructure::BOUND_MIN_MAX_PXDMFDim_Dim())) {
                PXDMFDimsArrayNames[cpt] = input->GetFieldData()->GetAbstractArray(i)->GetName();
                ++cpt;
            }
        }
        NumberOfOptimizationSpaceArrays = cpt;

        if ((int) this->ActivePXDMFDimsForOptimization.size() != NumberOfOptimizationSpaceArrays) {
            this->ActivePXDMFDimsForOptimization.resize(NumberOfOptimizationSpaceArrays);
            for(int i=0; i < NumberOfOptimizationSpaceArrays; ++i) {
                this->ActivePXDMFDimsForOptimization[i] =0;
            }
        }
    } else {
        std::cout << "input empty"  << std::endl;
        return 0;
    }
    return 1;
}
//
int vtkOptimi::GetNumberOfOptimizationSpaceArrays() {
    return NumberOfOptimizationSpaceArrays ;
}
//----------------------------------------------------------------------------
const char* vtkOptimi::GetOptimizationSpaceArrayName(int index) {
    return PXDMFDimsArrayNames[index].c_str();
};

//----------------------------------------------------------------------------
int vtkOptimi::GetOptimizationSpaceArrayStatus(const char* name) {
      for (unsigned i=0; int(i) < this->NumberOfOptimizationSpaceArrays; ++i ) {
        if (strcmp(name, GetOptimizationSpaceArrayName(i)) == 0 ) {
            return this->ActivePXDMFDimsForOptimization[i];
        }
    }
    return 0;
    
};

//----------------------------------------------------------------------------
void vtkOptimi::SetOptimizationSpaceStatus(const char* name, int status) {
    for (unsigned i=0; int(i) < this->GetNumberOfOptimizationSpaceArrays(); ++i ) {
        if (strcmp(name, GetOptimizationSpaceArrayName(i) ) == 0) {
            this->ActivePXDMFDimsForOptimization[i] = (status!=0);
        }
    }
};

//----------------------------------------------------------------------------
void vtkOptimi::Restart(int) {
    this->Modified();
}
