/*=========================================================================

  Program:   PXDMFReader Plugin
  Module:    vtkPXdmfWriter3.cxx

  Copyright (c) GeM, Ecole Centrale Nantes.
  All rights reserved.
  Copyright: See COPYING file that comes with this distribution


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// PXDMFReader Pluing Includes
#include "vtkPXDMFWriter3.h"
#include "stringhelper.h"

//VTK Includes
#include "vtkDataObject.h"
#include "vtkDirectedGraph.h"
#include "vtkImageData.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkMultiBlockDataSet.h"
#include "vtkObjectFactory.h"
#include "vtkPointSet.h"
#include "vtkRectilinearGrid.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkStructuredGrid.h"
#include "vtkXdmf3DataSet.h"
#include "vtkFieldData.h"
#include "vtkStringArray.h"


#include "XdmfDomain.hpp"
#include "XdmfGridCollection.hpp"
#include "XdmfGridCollectionType.hpp"
#include "XdmfHeavyDataWriter.hpp"
#include "XdmfWriter.hpp"
#include "XdmfInformation.hpp"
#include "XdmfRegularGrid.hpp"
#include "XdmfRectilinearGrid.hpp"
#include "XdmfCurvilinearGrid.hpp"
#include "XdmfUnstructuredGrid.hpp"


#include <stack>
//STD Includes

#include "limits.h"

vtkStandardNewMacro(vtkPXDMFWriter3);


//
void vtkPXDMFWriter3::SetASCII(bool ascii){
  if(ascii){
    this->SetLightDataLimit(INT_MAX);
    //std::cout << "UINT_MAX " << INT_MAX << std::endl;
  } else {
    this->SetLightDataLimit(100);
  }
};
//

//=============================================================================
class vtkPXDMFWriter3::Internals {
public:
  Internals()
  {
  }
  ~Internals() {};
  void Init(const char *filename)
  {
    this->Domain = XdmfDomain::New();
    this->Writer = XdmfWriter::New(filename);
    this->Writer->setLightDataLimit(0);
    this->Writer->getHeavyDataWriter()->setReleaseData(true);
    this->NumberOfTimeSteps = 1;
    this->CurrentTimeIndex = 0;
    this->DestinationGroups.push(this->Domain);
    this->Destination = this->DestinationGroups.top();

  }
  void SwitchToTemporal()
  {
    shared_ptr<XdmfGridCollection> dest = XdmfGridCollection::New();
    dest->setType(XdmfGridCollectionType::Temporal());
    this->DestinationGroups.push(dest);
    this->Destination = this->DestinationGroups.top();
    this->Domain->insert(dest);
  }
  void WriteDataObject(vtkDataObject *dataSet, bool hasTime, double time,
    const char* name = 0)
  {
    if (!dataSet)
      {
      return;
      }
    bool needToWriteInfo = true;
    
    shared_ptr<XdmfGrid>  grid;
    
    
    switch (dataSet->GetDataObjectType())
      {
      case VTK_MULTIBLOCK_DATA_SET:
        {
        shared_ptr<XdmfGridCollection> group = XdmfGridCollection::New();
        this->Destination->insert(group);
        this->DestinationGroups.push(group);
        this->Destination = this->DestinationGroups.top();
        vtkMultiBlockDataSet *mbds = vtkMultiBlockDataSet::SafeDownCast(dataSet);
        for (unsigned int i = 0; i< mbds->GetNumberOfBlocks(); i++)
          {
          vtkDataObject *next = mbds->GetBlock(i);
          const char* blockName = mbds->GetMetaData(i)->Get(vtkCompositeDataSet::NAME());
          this->WriteDataObject(next, hasTime, time, blockName);
          }
        this->DestinationGroups.pop();
        this->Destination = this->DestinationGroups.top();
        needToWriteInfo = false;
        break;
        }
      case VTK_STRUCTURED_POINTS:
      case VTK_IMAGE_DATA:
      case VTK_UNIFORM_GRID:
        {
        vtkXdmf3DataSet::VTKToXdmf(
          vtkImageData::SafeDownCast(dataSet),
          this->Destination.get(),
          hasTime, time, name);
          grid = this->Destination->getRegularGrid(this->Destination->getNumberRegularGrids()-1);
        break;
        }
      case VTK_RECTILINEAR_GRID:
        {
        vtkXdmf3DataSet::VTKToXdmf(
          vtkRectilinearGrid::SafeDownCast(dataSet),
          this->Destination.get(),
          hasTime, time, name);
          grid = this->Destination->getRectilinearGrid(this->Destination->getNumberRectilinearGrids()-1);
        break;
        }
      case VTK_STRUCTURED_GRID:
        {
        vtkXdmf3DataSet::VTKToXdmf(
          vtkStructuredGrid::SafeDownCast(dataSet),
          this->Destination.get(),
          hasTime, time, name);
        grid = this->Destination->getCurvilinearGrid(this->Destination->getNumberCurvilinearGrids()-1);
        break;
        }
      case VTK_POLY_DATA:
      case VTK_UNSTRUCTURED_GRID:
        {
        vtkXdmf3DataSet::VTKToXdmf(
          vtkPointSet::SafeDownCast(dataSet),
          this->Destination.get(),
          hasTime, time, name);
          grid = this->Destination->getUnstructuredGrid(this->Destination->getNumberUnstructuredGrids()-1);
        break;
        }
      //case VTK_GRAPH:
      case VTK_DIRECTED_GRAPH:
      //case VTK_UNDIRECTED_GRAPH:
        {
        vtkXdmf3DataSet::VTKToXdmf(
          vtkDirectedGraph::SafeDownCast(dataSet),
          this->Destination.get(),
          hasTime, time, name);
        break;
        }
      default:
        {
        }
      }
      
    if(needToWriteInfo){
      
      ++this->Current_grid_cpt;
      grid->setName("PGD"+to_string(Current_grid_cpt));
      std::vector<std::string> names ;
      names.push_back(std::string("X"));
      names.push_back(std::string("Y"));
      names.push_back(std::string("Z"));
      
      vtkFieldData* inFD = dataSet->GetFieldData();
      int nbdims = 2;
      for(; nbdims >= 0 ; --nbdims){
        if(inFD->GetAbstractArray(("AxisTitleFor"+names[nbdims]).c_str())){
          //std::cout << "---" << nbdims << std::string(vtkStringArray::SafeDownCast(inFD->GetAbstractArray(("AxisTitleFor"+names[nbdims]).c_str()))->GetValue(0)) << std::endl;
          if( vtkStringArray::SafeDownCast(inFD->GetAbstractArray(("AxisTitleFor"+names[nbdims]).c_str()))->GetValue(0).size()) 
            break;
        }
      }
      nbdims++;
      
      shared_ptr<XdmfInformation>  info = XdmfInformation::New("Dims",to_string(nbdims)); 
      // TODO insert but where
      grid->insert(info);
      for(int i = 0; i < nbdims ; i++){
        
        shared_ptr<XdmfInformation> name = XdmfInformation::New("Dim"+to_string(i),"");
        shared_ptr<XdmfInformation> unit = XdmfInformation::New("Unit"+to_string(i),"" );
  
        if(inFD->GetAbstractArray(("AxisTitleFor"+names[i]).c_str())){
          std::string title = vtkStringArray::SafeDownCast(inFD->GetAbstractArray(("AxisTitleFor"+names[i]).c_str()))->GetValue(0);
          std::size_t pso = title.rfind(" ");
      
      
          if(pso > title.size() ) pso = title.size();
            name->setValue(title.substr(0,pso).c_str()); 
            //std::cout << title.substr(0,pso).c_str()<< std::endl;
            if(pso+1 < title.size()){
                //std::cout << title.substr(pso+1,title.size()).c_str()<< std::endl;
              unit->setValue(title.substr(pso+1,title.size()).c_str());    
            } else {
              unit->setValue("");    
            }
        } else {
          name->setValue(names[i].c_str());
          unit->setValue("");    
        }
        grid->insert(name);  
        grid->insert(unit);  
      }
    }
    
    this->Domain->accept(this->Writer);
  }
  boost::shared_ptr<XdmfDomain> Domain;
  boost::shared_ptr<XdmfDomain> Destination;
  boost::shared_ptr<XdmfWriter> Writer;

  std::stack<boost::shared_ptr<XdmfDomain> > DestinationGroups;

  int NumberOfTimeSteps;
  int CurrentTimeIndex;
  int Current_grid_cpt;
};

//==============================================================================

//----------------------------------------------------------------------------
vtkPXDMFWriter3::vtkPXDMFWriter3()
{
  this->FileName = NULL;
  this->LightDataLimit = 100;
  this->WriteAllTimeSteps = false;
  this->Internal = new Internals();
  this->SetNumberOfOutputPorts(0);
}

//----------------------------------------------------------------------------
vtkPXDMFWriter3::~vtkPXDMFWriter3()
{
  this->SetFileName(NULL);
}

//----------------------------------------------------------------------------
void vtkPXDMFWriter3::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "FileName: " <<
    (this->FileName ? this->FileName : "(none)") << endl;
  os << indent << "LightDataLimit: " <<
    this->LightDataLimit << endl;
  os << indent << "WriteAllTimeSteps: " <<
    (this->WriteAllTimeSteps?"ON":"OFF") << endl;
}

//------------------------------------------------------------------------------
void vtkPXDMFWriter3::SetInputData(vtkDataObject *input)
{
  this->SetInputDataInternal(0,input);
}

//------------------------------------------------------------------------------
int vtkPXDMFWriter3::Write()
{
  // Make sure we have input.
  if (this->GetNumberOfInputConnections(0) < 1)
    {
    vtkErrorMacro("No input provided!");
    return 0;
    }

  // always write even if the data hasn't changed
  this->Modified();

  if (!this->Internal)
  {
    this->Internal = new Internals();
  }
  this->Internal->Init(this->FileName);

  this->Update();

  //this->Internal->Domain->accept(this->Internal->Writer);

  delete this->Internal;
  this->Internal = NULL;

  return 1;
}

//----------------------------------------------------------------------------
int vtkPXDMFWriter3::RequestInformation(
  vtkInformation* vtkNotUsed(request),
  vtkInformationVector** inputVector,
  vtkInformationVector* vtkNotUsed(outputVector))
{
  // Does the input have timesteps?
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  if ( inInfo->Has(vtkStreamingDemandDrivenPipeline::TIME_STEPS()) )
    {
    this->Internal->NumberOfTimeSteps =
      inInfo->Length( vtkStreamingDemandDrivenPipeline::TIME_STEPS() );
    }
  else
    {
    this->Internal->NumberOfTimeSteps = 1;
    }
  //cerr << "WRITER NUM TS = " << this->Internal->NumberOfTimeSteps << endl;

  return 1;
}

//----------------------------------------------------------------------------
int vtkPXDMFWriter3::RequestUpdateExtent(
  vtkInformation* vtkNotUsed(request),
  vtkInformationVector** inputVector,
  vtkInformationVector* vtkNotUsed(outputVector))
{
  double *inTimes = inputVector[0]->GetInformationObject(0)->Get(
      vtkStreamingDemandDrivenPipeline::TIME_STEPS());
  if (inTimes && this->WriteAllTimeSteps)
    {
    //TODO:? Add a user ivar to specify a particular time,
    //which is different from current time. Can do it by updating
    //to a particular time then writing without writealltimesteps,
    //but that is annoying.
    double timeReq = inTimes[this->Internal->CurrentTimeIndex];
    inputVector[0]->GetInformationObject(0)->Set(
        vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEP(),
        timeReq);
    }

  return 1;
}

//----------------------------------------------------------------------------
int vtkPXDMFWriter3::RequestData(
  vtkInformation* request,
  vtkInformationVector** inputVector,
  vtkInformationVector* vtkNotUsed(outputVector))
{
  if (!this->Internal->Domain)
    {
    //call Write() instead of RD() directly. Write() does setup first before it calls RD().
    return 1;
    }

  this->Internal->Writer->setLightDataLimit(this->LightDataLimit);

  if (this->Internal->CurrentTimeIndex == 0 &&
      this->WriteAllTimeSteps &&
      this->Internal->NumberOfTimeSteps > 1)
    {
    // Tell the pipeline to start looping.
    this->Internal->SwitchToTemporal();
    request->Set(vtkStreamingDemandDrivenPipeline::CONTINUE_EXECUTING(), 1);
    }

  vtkInformation* inInfo = inputVector[0]->GetInformationObject(0);
  vtkDataObject* input = inInfo->Get(vtkDataObject::DATA_OBJECT());
  vtkInformation *inDataInfo = input->GetInformation();
  double dataT = 0;
  bool hasTime = false;
  if (inDataInfo->Has(vtkDataObject::DATA_TIME_STEP()))
    {
    dataT = input->GetInformation()->Get(vtkDataObject::DATA_TIME_STEP());
    hasTime = true;
    }
  this->Internal->Current_grid_cpt = 0;
  this->Internal->WriteDataObject(input, hasTime, dataT);

  this->Internal->CurrentTimeIndex++;
  if (this->Internal->CurrentTimeIndex >= this->Internal->NumberOfTimeSteps &&
      this->WriteAllTimeSteps)
    {
    // Tell the pipeline to stop looping.
    request->Remove(vtkStreamingDemandDrivenPipeline::CONTINUE_EXECUTING());
    this->Internal->CurrentTimeIndex = 0;
    }

  return 1;
}



