/*=========================================================================

  Program:   PXDMFReader Plugin
  Module:    vtkPXDMFDocument.cpp

  Copyright (c) GeM, Ecole Centrale Nantes.
  All rights reserved.
  Copyright: See COPYING file that comes with this distribution


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __vtkPXDMFDocument_cpp
#define __vtkPXDMFDocument_cpp

// STD Includes
#include <float.h>
#include <algorithm>   // for sort

// vtk Includes.
#include "XdmfInformation.h"
#include "vtkInformation.h"
#include "vtkObjectFactory.h"
#include "vtkDataObjectTypes.h"
#include "vtkInformationVector.h"
#include "vtkUnstructuredGrid.h"
#include "vtkStructuredGrid.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkRectilinearGrid.h"
#include "vtkIntArray.h"
#include "vtkMultiBlockDataSet.h"
#include "vtkStringArray.h"
#include "vtkFieldData.h"
#include <vtkCell.h>
#include <vtkImageData.h>
#include "vtksys/SystemTools.hxx"

// Paraveiw Includes.
#include "vtkPVInformationKeys.h"

// plugin Includes.
#include "stringhelper.h"

//////////////////////
#include "XDMF2Data.h"
// to activate the new reader comment the previous line and uncomment the next line
#include "XDMF3Data.h"
//////////////////////

#include "vtkPXDMFDocument.h"

//#define GEM_DEBUG(x) x
#define GEM_DEBUG(x) 
//
vtkStandardNewMacro(vtkPXDMFDocument);
//
vtkPXDMFDocument::vtkPXDMFDocument()  {

    this->Internal = this;
    
    ////////////////////////////////
    //this->XDMFInternal = new XDMF2Data;
    // to activate the new reader comment the previous line and uncomment the next line
    this->XDMFInternal = new XDMF3Data;
    ///////////////////////////////
    //this->SILUpdateStamp = 0;

    this->DoReconstruction = true;
    this->UseStride = false;
    this->UseInterpolation = false;
    this->ComputeDerivatives = false;
    this->Stride[0] = this->Stride[1] = this->Stride[2] = 1;

    this->DomainName = NULL;
    this->SetNumberOfOutputPorts(1);
    this->FileName = NULL;
}
//
vtkPXDMFDocument::~vtkPXDMFDocument() {
    delete this->XDMFInternal;
}

//
//vtkXdmfDocument* vtkPXDMFDocument::GetXdmfDocument() {
//    return this->XDMFInternal->XdmfDocument;
//};

//
bool vtkPXDMFDocument::CellFound() {
    return this->cellfound;
};

//
bool vtkPXDMFDocument::PointFound() {
    return this->pointfound;
};

//-----------------------------------------------------
bool vtkPXDMFDocument::PrepareDocument() {

    // Calling this method repeatedly is okay. It does work only when something
    // has changed.
    if (!this->FileName ) {
        vtkErrorMacro("File name not set");
        return false;
    }

    // First make sure the file exists.  This prevents an empty file
    // from being created on older compilers.
    if (!vtksys::SystemTools::FileExists(this->FileName)) {
        vtkErrorMacro("Error opening file " << this->FileName);
        return false;
    }

    if(!this->XDMFInternal->Init(this->FileName)){

      vtkWarningMacro(" Now we tried with the old reader (xdmf2) ");
      delete this->XDMFInternal;
      this->XDMFInternal = new XDMF2Data;
      
      if(!this->XDMFInternal->Init(this->FileName)){
         vtkErrorMacro("Failed to parse xdmf file: " << this->FileName << "  with the old reader (xdmf2) sorry I give up");
         return false;
      } 
    }

    if(!this->XDMFInternal->SetActiveDommain(this->DomainName)){
      vtkErrorMacro("Invalid domain: " << this->DomainName);
      return false;
    }
    
    if(this->XDMFInternal->NeedUpdate()){

        this->SetNumberOfPXDMFDims(this->XDMFInternal->GetNumberOfGrids());

        for (unsigned j=0; int(j)< GetNumberOfPXDMFDims(); ++j) {
            
            GridInfo data = this->XDMFInternal->GetGridData(j );
            
            Internal->NumberOfDimsInEachGrid->SetValue(j,data.dimensionality);
            
            this->Internal->NamesOfEachDimension[j].resize(data.dimensionality);
            this->Internal->UnitsOfEachDimension[j].resize(data.dimensionality);
            this->MaxOfEachDimension[j].resize(data.dimensionality);
            this->MinOfEachDimension[j].resize(data.dimensionality);
            this->PosOfEachDimension[j].resize(data.dimensionality);
    
            for(int i =0; i < data.dimensionality; ++i){
              Internal->NamesOfEachDimension[j][i] = data.coordNames[i];
              Internal->UnitsOfEachDimension[j][i] = data.coordUnits[i];
              this->MaxOfEachDimension[j][i] = data.maxs[i];
              this->MinOfEachDimension[j][i] = data.mins[i];
              this->PosOfEachDimension[j][i] = data.mins[i];
            }
        }
        int nb_attributs = this->XDMFInternal->GetNumberOfAttributes();
        this->Internal->NumberOfModesInEachPointField.clear();
        this->Internal->NumberOfModesInEachCellField.clear();
        for (unsigned k = 0; int(k) < nb_attributs; ++k) {
            AttributeInfo attributedata = this->XDMFInternal->GetAttributeData(k);
            size_t pos = attributedata.name.find_last_of("_");
            if (pos == std::string::npos) continue;
            std::string clean_name = attributedata.name.substr(0, pos);
            if (attributedata.center == XDMF_ATTRIBUTE_CENTER_NODE) {
                this->Internal->PointFieldData[clean_name] = NULL ;
                //this->Internal->NumberOfModesInEachPointField[clean_name] = 0;
                ++this->Internal->NumberOfModesInEachPointField[clean_name];
            } else {
                if (attributedata.center == XDMF_ATTRIBUTE_CENTER_CELL) {
                    this->Internal->CellFieldData[clean_name] = NULL ;
                    //this->Internal->NumberOfModesInEachCellField[clean_name] = 0;
                    ++this->Internal->NumberOfModesInEachCellField[clean_name];
                } else {
                    vtkWarningMacro("other type of attribute, not tested yet. Sorry");
                }
            }
        }
        this->UpdateInternalData();
    } else {
      return true;
    }
    
    this->ReadOutputType();
    
    if (this->Internal->VtkOutputType  != VTK_UNSTRUCTURED_GRID && this->Internal->VtkOutputType  != VTK_POLY_DATA) this->SetUseStride(1);

    return true;
}
//-----------------------------------------
int vtkPXDMFDocument::FillOutputPortInformation(int, vtkInformation *info) {
    info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkDataObject");
    return 1;
}
//----------------------------------------------------------------------------
int vtkPXDMFDocument::RequestDataObject(vtkInformationVector *outputVector) {
    GEM_DEBUG(std::cout << "In RequestDataObject " << std::endl);
    if (!this->PrepareDocument()) return 0;

    vtkDataObject* output = vtkDataSet::GetData(outputVector, 0);
    
   int outputType;
    if( this->DoReconstruction){
      outputType  = this->ReadOutputType();  
    } else {
      int cpt = 0;
      int id =0;
      for (unsigned i = 0; int(i) < GetNumberOfPXDMFDims(); ++i) {
        if (this->ActivePXDMFDimsForSpace[i]){
          ++cpt;
          id = i;
        }
      }
      if (cpt == 1)
        outputType  = this->XDMFInternal->GetGridData (id).type;
      else
        outputType  = this->XDMFInternal->GetDomainVTKDataType();
    }
    
    
    if (output && (output->GetDataObjectType() == outputType)) {
        return 1;
    }
    output = vtkDataObjectTypes::NewDataObject(outputType );                                    // delete ok 
    outputVector->GetInformationObject(0)->Set(vtkDataObject::DATA_OBJECT(), output );
    output->Delete();

    return 1;
}
//-----------------------------------------
int vtkPXDMFDocument::RequestData(vtkInformation * Info, vtkInformationVector ** inputVector,
                                  vtkInformationVector *outputVector) {

    
    if (!this->PrepareDocument()) {
        vtkErrorMacro("file not prepared in RequestData");
        return 0;
    }
    
    this->ReadOutputType();

    vtkInformation* outInfo = outputVector->GetInformationObject(0);

    vtkDataObject* output = vtkDataObject::GetData(outInfo);

    // we do not support multiple timestep requests.
    if (outInfo->Has(vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEP())) {
        this->Internal->Time = outInfo->Get(vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEP());
    }

    vtkDataObject* space_data = this->GenerateOutput(Info, inputVector, outputVector);
    output->ShallowCopy(space_data);
    space_data->Delete();
    return 1;
}
//
vtkDataSet *vtkPXDMFDocument::GetOutput(int idx) {
    return vtkDataSet::SafeDownCast(this->GetOutputDataObject(idx));
}
//
vtkDataSet *vtkPXDMFDocument::GetOutput() {
    return vtkDataSet::SafeDownCast(this->GetOutputDataObject(0));
}
//
vtkStructuredGrid *vtkPXDMFDocument::GetStructuredGridOutput() {
    return vtkStructuredGrid::SafeDownCast(this->GetOutput());
}
//
vtkUnstructuredGrid *vtkPXDMFDocument::GetUnstructuredGridOutput() {
    return vtkUnstructuredGrid::SafeDownCast(this->GetOutput());
}
//
vtkRectilinearGrid *vtkPXDMFDocument::GetRectilinearGridOutput() {
    return vtkRectilinearGrid::SafeDownCast(this->GetOutput());
}
//
int vtkPXDMFDocument::ReadOutputType() {

    //vtkXdmfDomain * domain = this->XDMFInternal->XdmfDocument->GetActiveDomain();
    XdmfInt32  nb_grid = this->XDMFInternal->GetNumberOfGrids();
    this->Internal->VtkOutputType = -1;
    for (unsigned i = 0; int(i) < nb_grid; i++) {
        
        //domain->GetVTKDataType(domain->GetGrid(i));
        int grid_type =  this->XDMFInternal->GetGridData(i).type;
        if(GetActivePXDMFDimsForSpace().size() > i)
          if (GetActivePXDMFDimsForSpace()[i] == 0) continue;
        switch (grid_type) {
        case VTK_IMAGE_DATA: {
            if (this->Internal->VtkOutputType ==-1) {
                this->Internal->VtkOutputType  = VTK_UNIFORM_GRID;
            }
            break;

        }
        case VTK_UNIFORM_GRID: {
            if (this->Internal->VtkOutputType ==-1) {
                this->Internal->VtkOutputType  = VTK_UNIFORM_GRID;
            }
            break;
        }
        case VTK_RECTILINEAR_GRID: {
            if (this->Internal->VtkOutputType ==-1 || this->Internal->VtkOutputType  == VTK_UNIFORM_GRID ) {
                this->Internal->VtkOutputType  = VTK_RECTILINEAR_GRID;
            }
            break;
        }
        case VTK_STRUCTURED_GRID: {
            if (this->Internal->VtkOutputType ==-1 || this->Internal->VtkOutputType  == VTK_UNIFORM_GRID || this->Internal->VtkOutputType  == VTK_RECTILINEAR_GRID ) {
                this->Internal->VtkOutputType  = VTK_STRUCTURED_GRID;
            }
            break;
        }
        case VTK_POLY_DATA: 
            if (this->VtkOutputType == -1 ) {
                this->VtkOutputType  = VTK_POLY_DATA;
            } else {
                this->VtkOutputType  = VTK_UNSTRUCTURED_GRID;
            }
            break;
        case VTK_UNSTRUCTURED_GRID: {
            if (this->Internal->VtkOutputType ==-1 || this->Internal->VtkOutputType == VTK_UNIFORM_GRID || this->Internal->VtkOutputType  == VTK_RECTILINEAR_GRID ||  this->Internal->VtkOutputType  == VTK_STRUCTURED_GRID ) {
                this->Internal->VtkOutputType  = VTK_UNSTRUCTURED_GRID;
            }
            break;
        }
        }
    }
    if( this->Internal->VtkOutputType == -1) 
      this->Internal->VtkOutputType  = VTK_UNSTRUCTURED_GRID;
    
    return this->Internal->VtkOutputType;
}
//
// this fuction fill the BaseStructure with everything for the reconstruction
vtkDataObject* vtkPXDMFDocument::GenerateOutput(vtkInformation * Info, vtkInformationVector ** inputVector, vtkInformationVector *outputVector) {
    if (!this->PrepareDocument()) {
        vtkErrorMacro("file not prepared in RequestData");
        return 0;
    }

    vtkInformation* outInfo = outputVector->GetInformationObject(0);



    int update_extent[6] = {0, -1, 0, -1, 0, -1};

    if (outInfo->Has(vtkStreamingDemandDrivenPipeline::UPDATE_EXTENT())) {
        outInfo->Get(vtkStreamingDemandDrivenPipeline::UPDATE_EXTENT(),update_extent);
    }

    unsigned dimcpt = 0;
    this->SetProgressText("Reading heavy data ...");
    if ( this->XDMFInternal->changeInSelectedPointCellArrays ) {
        this->XDMFInternal->changeInSelectedPointCellArrays = 0;
        GEM_DEBUG(std::cout << "Reading Heavy data" << std::endl;)
        this->Internal->PointMap.clear();
        this->Internal->PointMap.resize(GetNumberOfPXDMFDims());
        ///for every dimension allocation for every field
        int numberoffields  = this->PointFieldData.size();
        for(int i=0; i < GetNumberOfPXDMFDims(); ++i) {
            this->Internal->PointMap[i].resize(numberoffields);
            /// for every field allocation for every mode
            for(int j=0; j < numberoffields; ++j) {
                std::string name = GetPointDataName(j);
                unsigned numberOfModes = this->GetNumberOfModesOfPointData(GetPointDataName(j));
                this->Internal->PointMap[i][j].resize(numberOfModes );
                //std::fill( &this->Internal->PointMap[i][j][0], &this->Internal->PointMap[i][j][0] + numberOfModes, NULL );
                
                for(int k=0; k < (int)numberOfModes; ++k) {
                  this->PointMap[i][j][k] = NULL;
                }
                
            }
        }

        this->Internal->CellMap.clear();
        this->Internal->CellMap.resize(GetNumberOfPXDMFDims());
        ///for every dimension allocation for every field
        numberoffields  = this->CellFieldData.size();
        for(int i=0; i < GetNumberOfPXDMFDims(); ++i) {
            this->Internal->CellMap[i].resize(numberoffields);
            /// for every field allocation for every mode
            for(int j=0; j < numberoffields; ++j) {
                std::string name = GetCellDataName(j);
                unsigned numberOfModes = GetNumberOfModesOfCellData(GetCellDataName(j));
                this->Internal->CellMap[i][j].resize(numberOfModes );
                
                //std::fill( &this->Internal->CellMap[i][j][0], &this->Internal->CellMap[i][j][0] + numberOfModes, 0. );
                for(int k=0; k < (int)numberOfModes; ++k) {
                  this->CellMap[i][j][k] = NULL;
                }
              
            }
        }


        
        int stride[3] = { 1,1,1 };
        int extents[6] = { 0, -1 , 0, -1, 0, -1};
        
        for (unsigned i = 0; int(i) < GetNumberOfPXDMFDims(); ++i) {
            this->UpdateProgress(double(i) / GetNumberOfPXDMFDims());
            
            if(this->GetActivePXDMFDimsForSpace()[i] ) {
                for(unsigned j=0; int(j)< GetNumberOfDimsInEachPXDMFDim()->GetValue(i); ++j ) {
                    stride[j] = this->Stride[dimcpt];
                    extents[2*j] = update_extent[0+2*dimcpt]*this->Stride[dimcpt];
                    extents[1+2*j] = update_extent[1+2*dimcpt]*this->Stride[dimcpt];
                    ++dimcpt;
                }
            }
            
            this->AllData[i] =  this->XDMFInternal->GetVTKDataSet(i,stride,extents);
            this->cellGrids[i] = 0;
            
            if (this->AllData[i] == 0) {
                std::cout << "Convertion Error  vtkDataObject* into vtkDataSet*" << std::endl;
                exit(0);
            }
        }
    }
    vtkDataObject* space_data = NULL;
    if(this->DoReconstruction){
      space_data = GenerateSpace();
    }else {
      std::vector<std::string> names ;
      names.push_back ( std::string ( "X" ) );
      names.push_back ( std::string ( "Y" ) );
      names.push_back ( std::string ( "Z" ) );
    
      int ngrids = 0;
      int id =0;
      for (unsigned i = 0; int(i) < GetNumberOfPXDMFDims(); ++i) {
        if (this->ActivePXDMFDimsForSpace[i]){
          ++ngrids;
          id = i;
        }
      }
      if (ngrids == 1){
        
        space_data = this->AllData[id]->NewInstance();
        //space_data->DeepCopy(this->AllData[id]);
        
        
        space_data->ShallowCopy(this->AllData[id]); 
        vtkFieldData* fd = vtkFieldData::New();
        GridInfo ginfo =  this->XDMFInternal->GetGridData(id);
        for ( unsigned j = 0; int ( j ) < ginfo.dimensionality ; ++j ) {
                vtkStringArray* TitleArray = vtkStringArray::New();
                TitleArray->SetName ( ("AxisTitleFor" + names[j]).c_str() );
                TitleArray->InsertNextValue ( (ginfo.coordNames[j] + " " + ginfo.coordUnits[j] ).c_str() );
                TitleArray->GetInformation()->Set ( vtkPXDMFDocumentBaseStructure::NAME(),( ginfo.coordNames[j] ).c_str() );
                TitleArray->GetInformation()->Set ( vtkPXDMFDocumentBaseStructure::UNIT(),( ginfo.coordUnits[j] ).c_str() );
                fd->AddArray ( TitleArray );
                TitleArray->Delete();
        } 

        space_data->SetFieldData ( fd );
        fd->Delete();
      } else {
      
        vtkMultiBlockDataSet* multi_space_data = vtkMultiBlockDataSet::New();                   // delete ok
        int cpt = 0;
        for (unsigned i = 0; int(i) < GetNumberOfPXDMFDims(); ++i) {
          if (this->ActivePXDMFDimsForSpace[i])  {
                vtkDataObject* space_datat = this->AllData[i]->NewInstance();
        
        
                space_datat->ShallowCopy(this->AllData[i]); 
        
                    vtkFieldData* fd = vtkFieldData::New();
        GridInfo ginfo =  this->XDMFInternal->GetGridData(i);
        for ( unsigned j = 0; int ( j ) < ginfo.dimensionality ; ++j ) {
                vtkStringArray* TitleArray = vtkStringArray::New();
                TitleArray->SetName ( ("AxisTitleFor" + names[j]).c_str() );
                TitleArray->InsertNextValue ( (ginfo.coordNames[j] + " " + ginfo.coordUnits[j] ).c_str() );
                TitleArray->GetInformation()->Set ( vtkPXDMFDocumentBaseStructure::NAME(),( ginfo.coordNames[j] ).c_str() );
                TitleArray->GetInformation()->Set ( vtkPXDMFDocumentBaseStructure::UNIT(),( ginfo.coordUnits[j] ).c_str() );
                fd->AddArray ( TitleArray );
                TitleArray->Delete();
        } 

        space_datat->SetFieldData ( fd );
        fd->Delete();
            multi_space_data->SetBlock(cpt,space_datat ); 
            multi_space_data->GetMetaData(cpt)->Set(vtkCompositeDataSet::NAME(), this->XDMFInternal->GetGridData(i).name.c_str());
            cpt ++;
          }
        }
         space_data =  multi_space_data;
      
      }
    }

    if (!space_data) {
        vtkErrorMacro("Failed to generate data.");
        return 0;
    }

    return space_data;

}
//
int vtkPXDMFDocument::RequestInformation(vtkInformation *info, vtkInformationVector **inputVector, vtkInformationVector *outputVector) {
     GEM_DEBUG(std::cout << "In request Information" << std::endl);
    
    if (!this->PrepareDocument()) {
        vtkErrorMacro("file not prepared in RequestInformation");
        return 0;
    }

    vtkInformation* outInfo = outputVector->GetInformationObject(0);

    // * Publish time information
    if (this->haveTime) {
        unsigned time_grid=0;
        for (; time_grid < this->GetActivePXDMFDimForTime().size(); ++time_grid) {
            if (this->GetActivePXDMFDimForTime()[time_grid]) {
                break;
            }
        }

        outInfo->Set(vtkPVInformationKeys::TIME_LABEL_ANNOTATION(), (this->Internal->NamesOfEachDimension[time_grid][0]+ " " + this->Internal->UnitsOfEachDimension[time_grid][0]).c_str() );

        if (Internal->LastTimeDimension == int(time_grid) ) {
            outInfo->Set(vtkStreamingDemandDrivenPipeline::TIME_STEPS(), &this->Internal->TimeStepsVector[0], static_cast<int>(this->Internal->TimeStepsVector.size()));
            double timeRange[2];
            timeRange[0] = this->Internal->TimeStepsVector.front();
            timeRange[1] = this->Internal->TimeStepsVector.back();
            outInfo->Set(vtkStreamingDemandDrivenPipeline::TIME_RANGE(), timeRange, 2);
        } else {

            int stride[3] = { 1,1,1 };
            int extents[6] = { 0, -1 , 0, -1, 0, -1};
            if (!this->AllData[time_grid])
              this->AllData[time_grid] =  this->XDMFInternal->GetVTKDataSet(time_grid,stride,extents);

            vtkDataSet * timeGrid = this->AllData[time_grid];
            
            int nb_element = timeGrid->GetNumberOfCells();

            vtkstd::vector<double> time_steps;
            
            int NumberoOfPointsPerElement  = 0 ;
            
            if(vtkUnstructuredGrid::SafeDownCast(timeGrid)){
              switch(timeGrid->GetCell(0)->GetCellType() ){
                case(VTK_VERTEX):
                case(VTK_POLY_VERTEX):  {
                  time_steps.resize(nb_element);
                  for (unsigned i = 0; int(i) < nb_element; ++i) {
                    time_steps[i] = timeGrid->GetCell(i)->GetPoints()->GetPoint(0)[0];
                  }
                  break;
                }
                case(VTK_LINE):
                case(VTK_POLY_LINE):  {
                    /// TODO for now  we treat only 2 nodes elements
                    time_steps.resize(nb_element+1);
                    unsigned cpt =0;
                    for (unsigned i = 0; int(i) < nb_element; ++i) {
                       for (unsigned j = 0; int(j) < 2; ++j) {
                         addtime(time_steps, timeGrid->GetCell(i)->GetPoints()->GetPoint(j)[0],cpt);
                       }
                    }
                    break;
                } default :{
                  std::cout << "This kind of element is not supported for time (" << timeGrid->GetCell(0)->GetClassName() << std::endl;
                }
              }
            } else{
              vtkRectilinearGrid* rgrid = vtkRectilinearGrid::SafeDownCast(timeGrid);
              if(rgrid ){
               unsigned cpt =0;
               vtkDataArray* da =  rgrid->GetXCoordinates() ;
               
               unsigned size = da->GetNumberOfTuples() ;
               time_steps.resize(size);
               for (unsigned i =0; i < size; ++i){
                  time_steps[i] = da->GetTuple(i)[0];
               }
              } else { 
                vtkImageData * id = vtkImageData::SafeDownCast(timeGrid);
                if (id ){
                    
                  double origin = id->GetOrigin()[0];
                  double dx = id->GetSpacing()[0];
                  int dim = id->GetDimensions()[0];

                  time_steps.resize(dim);
                  unsigned cpt =0;
                  for (unsigned i = 0; i < dim; ++i) {
                    addtime(time_steps, origin + dx*i, cpt);
                  }
                } else {
                        std::cout << timeGrid->GetClassName() << " not supported for time !!" << std::endl;
                        std::cout <<  "This grid cant be used for time, sorry." << std::endl ;
                        vtkErrorMacro("This grid cant be used for time, sorry." );
                        return 0;
                }
              }
            }
            

            if (time_steps.size() > 0) {
                std::sort(time_steps.begin(),time_steps.end());
                outInfo->Set(vtkStreamingDemandDrivenPipeline::TIME_STEPS(), &time_steps[0], static_cast<int>(time_steps.size()));
                double timeRange[2];
                timeRange[0] = time_steps.front();
                timeRange[1] = time_steps.back();

                outInfo->Set(vtkStreamingDemandDrivenPipeline::TIME_RANGE(), timeRange, 2);
            }
            this->Internal->LastTimeDimension = time_grid;
            this->Internal->TimeStepsVector = time_steps;
        }
    } else {
        outInfo->Remove(vtkPVInformationKeys::TIME_LABEL_ANNOTATION());
        outInfo->Remove(vtkStreamingDemandDrivenPipeline::TIME_STEPS());
        outInfo->Remove(vtkStreamingDemandDrivenPipeline::TIME_RANGE());
    }

    if (this->Internal->VtkOutputType==VTK_UNIFORM_GRID ) {

        std::vector<int> space_grid;
        for (unsigned i = 0 ; i < this->GetActivePXDMFDimsForSpace().size(); ++i) {
            if (this->GetActivePXDMFDimsForSpace()[i]) {
                space_grid.push_back(i);
            }
        }
        int whole_extent[6] = {0,0,0,0,0,0};
        double origin[3]= {0,0,0};
        double spacing[3] = {1,1,1};

        unsigned dimcpt =0;

        for(unsigned i =0; i < space_grid.size(); ++i) {
            for(unsigned j =0; int(j) < Internal->NumberOfDimsInEachGrid->GetValue(space_grid[i]); ++j ) {
                //double Or = this->XDMFInternal->PXDMFDimensionXdmfGrids[space_grid[i]]->GetGeometry()->GetOrigin()[2-j];
                //double Or = vtkImageData::SafeDownCast(this->AllData[space_grid[i]])->GetOrigin()[j];
                double Or = XDMFInternal->GetGridData(space_grid[i]).origin[j];
                
                //double Dx = this->XDMFInternal->PXDMFDimensionXdmfGrids[space_grid[i]]->GetGeometry()->GetDxDyDz()[2-j];
                //double Dx = vtkImageData::SafeDownCast(this->AllData[space_grid[i]])->GetSpacing()[j];
                double Dx = XDMFInternal->GetGridData(space_grid[i]).spacing[j];
                
                //whole_extent[1+2*dimcpt] = this->XDMFInternal->PXDMFDimensionXdmfGrids[space_grid[i]]->GetTopology()->GetShapeDesc()->GetDimension(2-j)-1;
                //whole_extent[1+2*dimcpt] = vtkImageData::SafeDownCast(this->AllData[space_grid[i]])->GetExtent()[2*j+1]-1;
                whole_extent[1+2*dimcpt] = XDMFInternal->GetGridData(space_grid[i]).extents[j];
                origin[dimcpt] = Or;
                spacing[dimcpt] = Dx ;
                
                ++dimcpt;
            }
        }

        whole_extent[1] /= this->Stride[0];
        whole_extent[3] /= this->Stride[1];
        whole_extent[5] /= this->Stride[2];

        outInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), whole_extent, 6);

        spacing[0] *= this->Stride[0];
        spacing[1] *= this->Stride[1];
        spacing[2] *= this->Stride[2];
        outInfo->Set(vtkDataObject::ORIGIN(), origin, 3);
        outInfo->Set(vtkDataObject::SPACING(), spacing, 3);
    }
    return 1;
}
//----------------------------------------------------------------------------
void vtkPXDMFDocument::PrintSelf(ostream& os, vtkIndent indent) {
    this->Superclass::PrintSelf(os, indent);
    std::cout << "NumberOfPXDMFDims : "<< this->GetNumberOfPXDMFDims() << std::endl;
    for (unsigned i=0; int(i)< GetNumberOfPXDMFDims() ; ++i) {
        std::cout << "Number of Dims in each Grid ("<< i << ")  :" <<  this->GetNumberOfDimsInEachPXDMFDim()->GetValue(i) << std::endl;
        for (unsigned j=0; int(j)< this->GetNumberOfDimsInEachPXDMFDim()->GetValue(i); ++j) {
            std::cout << "  Name ["<<i<<"]["<<j<<"] :" <<  this->GetNamesOfDimension(i,j) << std::endl;
        }
        std::cout << "GetActivePXDMFDimsForSpace["<< i << "]  :" <<  GetActivePXDMFDimsForSpace()[i] << std::endl;
        std::cout << "GetActivePXDMFDimForTime["<< i << "]  :" <<  GetActivePXDMFDimForTime()[i] << std::endl;
    }
}
//
int vtkPXDMFDocument::ProcessRequest(vtkInformation *request, vtkInformationVector **inputVector, vtkInformationVector *outputVector) {
	
	
    if (request->Has(vtkDemandDrivenPipeline::REQUEST_DATA())){
        return this->RequestData(request, inputVector, outputVector);  
    }
    
    if (request->Has(vtkDemandDrivenPipeline::REQUEST_DATA_OBJECT())) {
        return this->RequestDataObject(outputVector);
    }

    // execute information
    if(request->Has(vtkDemandDrivenPipeline::REQUEST_INFORMATION())){	
        return this->RequestInformation(request, inputVector, outputVector);
    }
    
    if(request->Has(vtkStreamingDemandDrivenPipeline::REQUEST_UPDATE_EXTENT())) {
        vtkInformation *outInfo=outputVector->GetInformationObject(0);
        if (outInfo->Has(vtkPXDMFDocumentBaseStructure::UPDATE_FIXED_DIMENSIONS())) {
            int len = outInfo->Length(vtkPXDMFDocumentBaseStructure::UPDATE_FIXED_DIMENSIONS());
            this->Modified();
            if(len%3 == 0 ){
                double* val= outInfo->Get(vtkPXDMFDocumentBaseStructure::UPDATE_FIXED_DIMENSIONS());
                      for(int i = 0; i < len/3 ; ++i){
                                const int gridDim = val[i*3];
                                const int dim = val[i*3+1];
                                //std::cout << " Demand of UPDATE_FIXED_DIMENSIONS " << gridDim << " "<< dim << " "<< val[i*3+2] << std::endl;
                                this->SetPosOfEachDimension(gridDim,dim,val[i*3+2]);
                                
                        }
                }else{
                        std::cout << "WARNING: Wrong size of UPDATE_FIXED_DIMENSIONS = " << len << std::endl;  
                }
        }
    
    }

    
    return this->Superclass::ProcessRequest(request, inputVector, outputVector);
}
//

//----------------------------------------------------------------------------
void vtkPXDMFDocument::SetPointStatus(const char* arrayname,const int mode, const int status) {
  
    GEM_DEBUG(std::cout << "SetPointStatus(" << arrayname << " , "  << mode << "," <<  status << std::endl;)
    
    
    const int nb_modes = this->GetNumberOfModesOfPointData()[arrayname];
    
    
    bool modified= false;
    if (mode < 0) {
      if(this->XDMFInternal->GetPointArraySelection((arrayname+std::string("_0")).c_str()) != status ) {
        modified = true ;
      }
      for (int i = 0;  i < nb_modes; i++){
        this->XDMFInternal->SetPointArraySelection((arrayname+std::string("_")+to_string(i)).c_str(), status != 0);
      }
    } else {
      if(this->XDMFInternal->GetPointArraySelection((arrayname+std::string("_")+to_string(mode)).c_str()) != status ) {
        modified = true ;
      }
      this->XDMFInternal->SetPointArraySelection((arrayname+std::string("_")+to_string(mode)).c_str(), status != 0);
    }
    
    if(modified)  {
      this->XDMFInternal->changeInSelectedPointCellArrays = true ;
      this->Modified();      
    }

};
//----------------------------------------------------------------------------
void vtkPXDMFDocument::SetCellStatus(const char* arrayname,const int mode, const int status) {
    const int nb_modes = this->GetNumberOfModesOfCellData()[arrayname];
    
    bool modified= false;
    if (mode < 0) {
      if(this->XDMFInternal->GetCellArraySelection((arrayname+std::string("_0")).c_str()) != status ) {
        modified = true ;
      }
      for (int i = 0;  i < nb_modes; i++) this->XDMFInternal->SetCellArraySelection((arrayname+std::string("_")+to_string(i)).c_str(), status != 0);
    } else {
      if(this->XDMFInternal->GetCellArraySelection((arrayname+std::string("_")+to_string(mode)).c_str()) != status ) {
        modified = true ;
      }
      this->XDMFInternal->SetCellArraySelection((arrayname+std::string("_")+to_string(mode)).c_str(), status != 0);
    }
    if(modified)  {
      this->XDMFInternal->changeInSelectedPointCellArrays = true;
      this->Modified();
    }
};
//-------------------

void vtkPXDMFDocument::SetStride (int _arg1, int _arg2, int _arg3) { {
        if (this ->GetDebug() && vtkObject::GetGlobalWarningDisplay()) {
            vtkOStreamWrapper::EndlType endl;
            vtkOStreamWrapper::UseEndl(endl);
            vtkOStrStreamWrapper vtkmsg;
            vtkmsg << "Debug: In " "anonymous" ", line " << 0 << "\n" << this ->GetClassName() << " (" << this << "): " << this->GetClassName() << " (" << this << "): setting " << "Stride" " to (" << _arg1 << "," << _arg2 << "," << _arg3 << ")" << "\n\n";
            vtkOutputWindowDisplayDebugText(vtkmsg.str());
            vtkmsg.rdbuf()->freeze(0);
        }
    } ;
    if ((this->Stride [0] != _arg1)||(this->Stride [1] != _arg2)||(this->Stride [2] != _arg3)) {
        this->Stride [0] = _arg1;
        this->Stride [1] = _arg2;
        this->Stride [2] = _arg3;
        this->XDMFInternal->changeInSelectedPointCellArrays = true ;
        this->Modified();
    }
};
void vtkPXDMFDocument::SetStride (int _arg[3]) {
    this->SetStride (_arg[0], _arg[1], _arg[2]);
}

#endif //__vtkPXDMFDocument_cpp
