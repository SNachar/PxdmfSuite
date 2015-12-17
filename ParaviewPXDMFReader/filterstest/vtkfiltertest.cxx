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

#include <vector>

//VTK Includes
#include "vtkFieldData.h"
#include "vtkCellData.h"
//#include "vtkFloatArray.h"
//#include "vtkUnsignedCharArray.h"
//#include "vtkImageData.h"
//#include "vtkImageDataToPointSet.h"
#include "vtkInformation.h"
#include "vtkInformationDoubleVectorKey.h"
//#include "vtkInformationVector.h"
//#include "vtkLinearTransform.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkPointSet.h"
//#include "vtkRectilinearGrid.h"
//#include "vtkRectilinearGridToPointSet.h"
#include "vtkNew.h"
#include "vtkSmartPointer.h"
//#include "vtkMath.h"
#include "vtkDataSet.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkInformationVector.h"
#include "vtkMultiThreader.h"
#include "vtkMutexLock.h"
#include "vtkConditionVariable.h"

// Pluing Includes
#include "vtkfiltertest.h"
#include "vtkPXDMFDocumentBaseStructure.h"

#include <vtkAmoebaMinimizer.h>


vtkStandardNewMacro(vtkfiltertest);
//vtkCxxSetObjectMacro(vtkTransformFilterWithAxis,Transform,vtkAbstractTransform);

struct vtkThreadUserData {
  vtkMutexLock* Lock;
  vtkConditionVariable* Condition;
  int Done;
  //int NumberOfWorkers;

} ;




vtkfiltertest::vtkfiltertest() {
    CurrentTimeIndex = 0;
    minimizer = vtkAmoebaMinimizer::New();
    
    MaxNumberOfIterations = 100;
    NumberOfOptimizationSpaceArrays =0;
    threader = vtkMultiThreader::New();
    
    threadID = -1;
    needANewIteration =true;
    //data.NumberOfWorkers = 1;
    //threader->SetNumberOfThreads( 1 );
    //threader->SetSingleMethod( vtkRunMinimization, this );
    maincpt = 1;
    minicpt =0;
    running = 0;
    Maximize = 0;
    this->MaxNumberOfIterations=1000;
    this->ContractionRatio=0.5;
    this->ExpansionRatio=2.0;
    this->Tolerance=0.0001;
    this->ParameterTolerance=0.0001;
    
    
    
    
}
//
vtkfiltertest::~vtkfiltertest() {

}
//
int vtkfiltertest::FillInputPortInformation(
    int vtkNotUsed(port),
    vtkInformation *info) {

    info->Remove(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE());
    info->Append(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkDataObject");
    return 1;
}
//
//
int vtkfiltertest::RequestDataObject(
    vtkInformation *request,
    vtkInformationVector **inputVector,
    vtkInformationVector *outputVector) {

    //std::cout << "int RequestDataObject" << std::endl;
    vtkDataObject *inData = vtkDataObject::GetData(inputVector[0]);
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
static void  vtkFunctionToMinimize(void *arg)
{
    vtkfiltertest *THIS = static_cast<vtkfiltertest *>(arg);

    /// minimizer need a new value;
    THIS->inInfo->Remove(vtkPXDMFDocumentBaseStructure::UPDATE_FIXED_DIMENSIONS());
    std::vector<double> val;
    for (int i = 0; i < THIS->minimizer->GetNumberOfParameters(); ++i){
      std::string name  =  THIS->minimizer->GetParameterName(i);
      
      
      val.push_back(THIS->pos[name].first);
      val.push_back(THIS->pos[name].second);
      val.push_back(THIS->minimizer->GetParameterValue(name.c_str()));
    
      
    }
    THIS->inInfo->Set(vtkPXDMFDocumentBaseStructure::UPDATE_FIXED_DIMENSIONS(), &val[0], val.size());
    ++THIS->minicpt;
    //std::cout << "wainting the vtkFunctionToMinimize" << std::endl ;
    while(THIS->maincpt<=THIS->minicpt){
      
    }
    
    // pause waint for the pipeline to do a new iteration

    double r = THIS->inScalars->GetComponent(0,0);
    if(THIS->Maximize) r = -1*r ;
    THIS->minimizer->SetFunctionValue(r);
    
}
//
void*  vtkRunMinimization(void *arg)
{
    
    vtkfiltertest *THIS =static_cast<vtkfiltertest*>( static_cast<vtkMultiThreader::ThreadInfo*>(arg)->UserData);
    THIS->running=1;
    THIS->minimizer->Minimize();
    THIS->needANewIteration = 0;
    THIS->running=0;
    return NULL;
    
}
//
int vtkfiltertest::RequestData(
    vtkInformation *request,
    vtkInformationVector **inputVector,
    vtkInformationVector *outputVector) {

    vtkSmartPointer<vtkDataSet> input = vtkDataSet::GetData(inputVector[0]);


    inScalars = this->GetInputArrayToProcess(0,inputVector);

    if (  needANewIteration ) {
        if( this->CurrentTimeIndex == 0) {
          /// first time
          
          minimizer->Initialize();
//           std::cout << "GetMaxIterations " <<minimizer->GetMaxIterations() << std::endl;
//           std::cout << "GetContractionRatio " <<minimizer->GetContractionRatio()<< std::endl;
//           std::cout << "GetExpansionRatio " <<minimizer->GetExpansionRatio()<< std::endl;
//           std::cout << "GetTolerance " <<minimizer->GetTolerance()<< std::endl;
//           std::cout << "ParameterTolerance " <<minimizer->GetParameterTolerance()<< std::endl;
//           
          
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
            //std::cout << "setting  " << name << "  to val " <<input->GetFieldData()->GetArray(name.c_str())->GetComponent(0,0) << std::endl;
            //std::cout << "     interval " << data[0]  << "  " <<data[1] << std::endl;
          }
          this->minimizer->SetFunction(&vtkFunctionToMinimize,this);
          double r = this->inScalars->GetComponent(0,0);
          if(this->Maximize) r  = -1*r;
          this->minimizer->SetFunctionValue(r);
          threadID  = threader->SpawnThread(   vtkRunMinimization  ,  this   ) ;
        }
        
       
        // There is still more to do.
        request->Set(vtkStreamingDemandDrivenPipeline::CONTINUE_EXECUTING(), 1);
        request->Set(vtkDemandDrivenPipeline::REQUEST_REGENERATE_INFORMATION(), 1);
        ++CurrentTimeIndex;


    } else {

        // We are done.  Finish up.
        request->Remove(vtkStreamingDemandDrivenPipeline::CONTINUE_EXECUTING());
        this->CurrentTimeIndex = 0;
        //std::cout << "Last Data" << std::endl;
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
               
    }

    return 1;
}

void vtkfiltertest::PrintSelf(ostream& os, vtkIndent indent)
{
    this->Superclass::PrintSelf(os,indent);

    os << indent << "this filter  data: " << "\n";

}

int vtkfiltertest::RequestUpdateExtent( vtkInformation *request,
                                        vtkInformationVector **inputVector,
                                        vtkInformationVector *outputVector) {

   inInfo = inputVector[0]->GetInformationObject(0);

    
    // pause until the minimizer needs a new value
   
    if (running){
      ++maincpt;
      //std::cout << "wainting the main loop" << std::endl; ;
      while(maincpt>minicpt){
         if(!running) break;
      }
      
    }
        
    // The RequestData method will tell the pipeline executive to iterate the
    // upstream pipeline to get each time step in order.  The executive in turn
    // will call this method to get the extent request for each iteration (in this
    // case the  fixed dimension step).
    //double val[3];
    //val[0] = PXDMFDim;
    //val[1] = Dim;
    //val[2] = Pos+CurrentTimeIndex*0.05;
    //inInfo->Set(vtkPXDMFDocumentBaseStructure::UPDATE_FIXED_DIMENSIONS(), val, 3);
    if(running)
      inInfo->Set(vtkStreamingDemandDrivenPipeline::UPDATE_RESOLUTION(), 1.0);
    request->AppendUnique(vtkExecutive::KEYS_TO_COPY(), vtkPXDMFDocumentBaseStructure::UPDATE_FIXED_DIMENSIONS());
    
    //std::cout << "sending data RequestUpdateExtent " <<PXDMFDim << " " <<Dim << " " << val[2]  << " --------------------------------------------------------------"<<std::endl;

    return 1;
}
//
int vtkfiltertest::RequestInformation(vtkInformation* request,
                                      vtkInformationVector **inputVector,
                                      vtkInformationVector *outputVector ) {
    //std::cout << "int RequestInformation" << std::endl;
    vtkSmartPointer<vtkDataSet> input = vtkDataSet::GetData(inputVector[0]);

    if(input) {
        PXDMFDimsArrayNames.clear();
        int cpt = 0;
        for(int i =0; i < input->GetFieldData()->GetNumberOfArrays() ; ++i) {
            //input->GetFieldData()->GetArray(i)->GetName();
            if(input->GetFieldData()->GetAbstractArray(i)->GetInformation()->Has(vtkPXDMFDocumentBaseStructure::UNIT())) {
                //std::cout << "Adding " << input->GetFieldData()->GetAbstractArray(i)->GetName() << " to the optimization space " << std::endl;
                PXDMFDimsArrayNames[cpt] = input->GetFieldData()->GetAbstractArray(i)->GetName();
                ++cpt;
            }
        }
        NumberOfOptimizationSpaceArrays = cpt;

        if (this->ActivePXDMFDimsForOptimization.size()!=NumberOfOptimizationSpaceArrays) {
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
int vtkfiltertest::GetNumberOfOptimizationSpaceArrays() {
    return NumberOfOptimizationSpaceArrays ;
}
//----------------------------------------------------------------------------
const char* vtkfiltertest::GetOptimizationSpaceArrayName(int index) {
    return PXDMFDimsArrayNames[index].c_str();
};


int vtkfiltertest::GetOptimizationSpaceArrayStatus(const char* name) {
      for (unsigned i=0; int(i) < this->NumberOfOptimizationSpaceArrays; ++i ) {
        if (strcmp(name, GetOptimizationSpaceArrayName(i)) == 0 ) {
            return this->ActivePXDMFDimsForOptimization[i];
        }
    }
    return 0;
    
};
//
void vtkfiltertest::SetOptimizationSpaceStatus(const char* name, int status) {
    for (unsigned i=0; int(i) < this->GetNumberOfOptimizationSpaceArrays(); ++i ) {
        if (strcmp(name, GetOptimizationSpaceArrayName(i) ) == 0) {
            this->ActivePXDMFDimsForOptimization[i] = (status!=0);
        }
    }
};
void vtkfiltertest::Restart(int) {
    this->Modified();
}