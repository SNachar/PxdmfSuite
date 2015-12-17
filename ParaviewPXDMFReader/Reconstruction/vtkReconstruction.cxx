/*=========================================================================

  Program:   PXDMFReader Plugin
  Module:    vtkReconstruction.cpp

  Copyright (c) GeM, Ecole Centrale Nantes.
  All rights reserved.
  Copyright: See COPYING file that comes with this distribution


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
//std includes
#include <algorithm>

//VTK Includes
#include <vtkCellData.h>
#include "vtkDoubleArray.h"
#include "vtkIntArray.h"
#include "vtkCell.h"
#include <vtkPointData.h>
#include "vtkDataObjectTypes.h"
#include "vtkInformationVector.h"
#include "vtkInformation.h"
#include <vtkStringArray.h>
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkCompositeDataIterator.h"
#include "vtkReconstruction.h"
#include "vtkMultiBlockDataSet.h"
#include "vtkDataSet.h"
#include "vtkImageData.h"
#include "vtkRectilinearGrid.h"


// Paraview Includes.
#include "vtkPVInformationKeys.h"

// Pluing Includes
#include "stringhelper.h"

vtkReconstruction* vtkReconstruction::New() {
    return new vtkReconstruction();
    };
//
vtkReconstruction::vtkReconstruction() {

    this->SetNumberOfOutputPorts ( 1 );
    this->SetNumberOfInputPorts ( 1 );

    this->vtkPXDMFDimsNamesData = vtkSmartPointer<vtkStringArray>::New();
    this->vtkPXDMFDimsNamesData->Initialize();
    this->vtkPXDMFDimsNamesData->SetNumberOfValues ( 0 );

    this->vtkPXDMFDimsUnitsData = vtkSmartPointer<vtkStringArray>::New();
    this->vtkPXDMFDimsUnitsData->Initialize();
    this->vtkPXDMFDimsUnitsData->SetNumberOfValues ( 0 );

    vtkPXDMFDimsMinRangeData = vtkSmartPointer<vtkDoubleArray> ::New();
    vtkPXDMFDimsMinRangeData->Initialize();
    vtkPXDMFDimsMaxRangeData =vtkSmartPointer<vtkDoubleArray> ::New();
    vtkPXDMFDimsMaxRangeData->Initialize();

    vtkPXDMFDimsPosData = vtkSmartPointer<vtkDoubleArray> ::New();
    vtkPXDMFDimsPosData->Initialize();

    };
//
vtkReconstruction::~vtkReconstruction() {
    };
//
//----------------------------------------------------------------------------
vtkStringArray* vtkReconstruction::GetPXDMFDimsNamesDataInfo() {
    if ( this->vtkPXDMFDimsNamesData->GetNumberOfValues() ==0 ) {
        for ( unsigned i = 0; int ( i ) < this->GetNumberOfPXDMFDims(); i++ )
            for ( unsigned j = 0; int ( j ) < this->GetNumberOfDimsInEachPXDMFDim()->GetValue ( i ); j++ )
                vtkPXDMFDimsNamesData->InsertNextValue ( this->GetNamesOfDimension ( i,j ) );
        }
    return vtkPXDMFDimsNamesData;
    };
//
vtkStringArray* vtkReconstruction::GetPXDMFDimsUnitsDataInfo() {
    if ( this->vtkPXDMFDimsUnitsData->GetNumberOfValues() ==0 ) {
        for ( unsigned i = 0; int ( i ) < this->GetNumberOfPXDMFDims(); i++ )
            for ( unsigned j = 0; int ( j ) < this->GetNumberOfDimsInEachPXDMFDim()->GetValue ( i ); j++ )
                vtkPXDMFDimsUnitsData->InsertNextValue ( this->GetUnitsOfDimension ( i,j ) );
        }
    return vtkPXDMFDimsUnitsData;
    };
//
void vtkReconstruction::PrintSelf ( ostream& os, vtkIndent indent ) {
    this->Superclass::PrintSelf ( os, indent );
    };
//
int vtkReconstruction::FillInputPortInformation ( int vtkNotUsed ( port ), vtkInformation* info ) {
    info->Remove ( vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE() );
    info->Append ( vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkDataObjectTree" );
    info->Append ( vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkGenericDataSet" );
    info->Append ( vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkDataObject" );
    info ->Set ( vtkAlgorithm::INPUT_IS_REPEATABLE(), 1 );

    return 1;
    };

//
int vtkReconstruction::FillOutputPortInformation ( int vtkNotUsed ( port ), vtkInformation *info ) {
    info->Set ( vtkDataObject::DATA_TYPE_NAME(), "vtkDataObject" );
    return 1;

    }
//
vtkDoubleArray* vtkReconstruction::GetPXDMFDimsMaxRangeDataInfo() {
    unsigned cpt =0;

    if ( vtkPXDMFDimsMaxRangeData->GetSize() == 0 ) {

        for ( unsigned i = 0 ; int ( i ) < this->GetNumberOfPXDMFDims(); ++i ) {
            cpt += this->GetNumberOfDimsInEachPXDMFDim()->GetValue ( i );
            }
        vtkPXDMFDimsMaxRangeData->SetNumberOfComponents ( 1 );
        vtkPXDMFDimsMaxRangeData->SetNumberOfValues ( cpt );
        }
    cpt=0;

    for ( unsigned i = 0 ; int ( i ) < this->GetNumberOfPXDMFDims(); ++i ) {
        for ( unsigned j = 0 ; int ( j ) < this->GetNumberOfDimsInEachPXDMFDim()->GetValue ( i ); ++j ) {
            vtkPXDMFDimsMaxRangeData->SetValue ( cpt, MaxOfEachDimension[i][j] );
            ++cpt;
            }
        }
    return vtkPXDMFDimsMaxRangeData;
    };
//
void vtkReconstruction::PrepareDocumentInput ( vtkInformation* ,
        vtkInformationVector** inputVector,
        vtkInformationVector* ) {

    int numInputs = inputVector[0]->GetNumberOfInformationObjects();

    if ( numInputs == 1 && vtkCompositeDataSet::GetData ( inputVector[0], 0 ) ) {

        vtkCompositeDataSet *input = vtkCompositeDataSet::GetData ( inputVector[0], 0 );
        int cpt =0;
        vtkCompositeDataIterator* iter = input->NewIterator();
        for ( iter->InitTraversal(); !iter->IsDoneWithTraversal(); iter->GoToNextItem() ) {
            ++cpt;
            };
        SetNumberOfPXDMFDims ( cpt );

        cpt = 0;
        for ( iter->InitTraversal(); !iter->IsDoneWithTraversal(); iter->GoToNextItem(),++cpt ) {
            vtkDataSet* input = vtkDataSet::SafeDownCast ( iter->GetCurrentDataObject() );;
            if ( AllData[cpt] != input ) {
                AllData[cpt] = input;
                this->forceMeshUpdate = true;
                this->Modified();
                }
            if ( !AllData[cpt] ) {
                cout << "Error class not supported " <<endl;
                }
            }
        iter->Delete();
        }
    else {
        SetNumberOfPXDMFDims ( numInputs );
        for ( int idx = 0; idx < numInputs; ++idx ) {
            vtkDataSet* input = 0;
            vtkInformation* inInfo = inputVector[0]->GetInformationObject ( idx );
            if ( inInfo ) {
                input = vtkDataSet::SafeDownCast ( inInfo->Get ( vtkDataObject::DATA_OBJECT() ) );
                }

            if ( input ) {
                if ( AllData[idx] != input )  {
                    AllData[idx] = input;
                    this->forceMeshUpdate = true;
                    this->Modified();
                    }
                }
            else {
                // we take the first mesh of a composite
                vtkCompositeDataSet *cinput = vtkCompositeDataSet::GetData ( inputVector[0], idx );
                vtkCompositeDataIterator* iter = cinput->NewIterator();
                for ( iter->InitTraversal(); !iter->IsDoneWithTraversal(); iter->GoToNextItem() ) {
                    vtkDataSet* iinput = vtkDataSet::SafeDownCast ( iter->GetCurrentDataObject() );;
                    if ( iinput && ( AllData[idx] != iinput ) ) {
                        AllData[idx] = iinput;
                        this->forceMeshUpdate = true;
                        this->Modified();
                        break;
                        }

                    }
                if ( iter->IsDoneWithTraversal() ) {
                    cout << "Error class not supported - " << endl;
                    }
                }
            }
        }

    std::vector<std::string> names ;
    names.push_back ( std::string ( "X" ) );
    names.push_back ( std::string ( "Y" ) );
    names.push_back ( std::string ( "Z" ) );

    for ( int cpt =0; cpt < GetNumberOfPXDMFDims(); ++cpt ) {
        vtkDataSet* dObj = AllData[cpt];

        double bounds[6];
        dObj->GetBounds ( bounds );
        int dimensionality = 3;
        if ( bounds[5]-bounds[4] < 1E-15 )
            dimensionality = 2;
        if ( bounds[3]-bounds[2] < 1E-15 )
            dimensionality = 1;
        this->NumberOfDimsInEachGrid->SetValue ( cpt,dimensionality );
        this->MaxOfEachDimension[cpt].resize ( dimensionality );
        this->MinOfEachDimension[cpt].resize ( dimensionality );
        this->PosOfEachDimension[cpt].resize ( dimensionality );


        this->NamesOfEachDimension[cpt].resize ( dimensionality );
        this->UnitsOfEachDimension[cpt].resize ( dimensionality );

        for ( int i = 0; i < dimensionality;  ++i ) {
            if ( dObj->GetFieldData()->HasArray ( ( "AxisTitleFor"+names[i] ).c_str() ) ) {
                vtkStringArray* theArray = vtkStringArray::SafeDownCast ( dObj->GetFieldData()->GetAbstractArray ( ( "AxisTitleFor"+names[i] ).c_str() ) );
                if (theArray->GetInformation()->Has(vtkPXDMFDocumentBaseStructure::NAME()) ){
                  NamesOfEachDimension[cpt][i] = std::string(theArray->GetInformation()->Get(vtkPXDMFDocumentBaseStructure::NAME()));
                } else {
                  NamesOfEachDimension[cpt][i] = theArray->GetValue ( 0 );  
                }
                
                if (theArray->GetInformation()->Has(vtkPXDMFDocumentBaseStructure::UNIT()) ){
                  UnitsOfEachDimension[cpt][i] = std::string(theArray->GetInformation()->Get(vtkPXDMFDocumentBaseStructure::UNIT()));
                } else {
                  UnitsOfEachDimension[cpt][i] = ( "unit" + names[i] ) ;  
                } 
                
                
                
                }
            else {
                NamesOfEachDimension[cpt][i] = ( "dim "+ to_string ( cpt ) + " "+ names[i] ) ;
                UnitsOfEachDimension[cpt][i] = ( "unit" + names[i] ) ;
                };
            }
        this->CalculateMaxMin ( cpt );
        }

    }
//
int vtkReconstruction::RequestData ( vtkInformation* request,
                                     vtkInformationVector** inputVector,
                                     vtkInformationVector* outputVector ) {

    vtkInformation* outInfo = outputVector->GetInformationObject ( 0 );

    PrepareDocumentInput ( request, inputVector,outputVector );

    if ( GetNumberOfPXDMFDims() ) {
        this->NumberOfModesInEachPointField.clear();
        this->NumberOfModesInEachCellField.clear();
        for ( unsigned d=0; d< AllData.size(); ++d ) {

            vtkDataSet* dObj0 = AllData[d];
            for ( int f = 0; f < dObj0->GetPointData()->GetNumberOfArrays() ; ++f ) {
                std::string name = dObj0->GetPointData()->GetArrayName ( f );
                size_t pos = name.find_last_of ( "_" );
                if ( pos == std::string::npos ) continue;

                unsigned  cpt = 0;
                for ( ; cpt < GetNumberOfPXDMFDims(); ++cpt ) {
                    if ( cpt == d ) continue;
                    vtkDataSet* dObjx = AllData[cpt];
                    if ( ! ( dObjx->GetPointData()->GetArray ( name.c_str() ) || this->ExpandFields ) ) break;
                    }
                if ( cpt < GetNumberOfPXDMFDims() ) continue;

                std::string clean_name = name.substr ( 0, pos );
                this->PointFieldData[clean_name] = NULL ;

                this->NumberOfModesInEachPointField[clean_name] = std::max ( ( int ) this->NumberOfModesInEachPointField[clean_name], atoi ( name.substr ( pos+1,name.size()-1 ).c_str() ) +1 ) ;
                }

            for ( int f = 0; f < dObj0->GetCellData()->GetNumberOfArrays() ; ++f ) {
                std::string name = dObj0->GetCellData()->GetArrayName ( f );
                size_t pos = name.find_last_of ( "_" );
                if ( pos == std::string::npos ) continue;

                unsigned cpt = 0;
                for ( ; cpt < GetNumberOfPXDMFDims(); ++cpt ) {
                    if ( cpt == d ) continue;
                    vtkDataSet* dObjx = AllData[cpt];
                    if ( ! ( dObjx->GetCellData()->GetArray ( name.c_str() ) || this->ExpandFields ) ) break;
                    }
                if ( cpt < GetNumberOfPXDMFDims() ) continue;

                std::string clean_name = name.substr ( 0, pos );

                this->CellFieldData[clean_name] = NULL ;

                this->NumberOfModesInEachCellField[clean_name] = std::max ( ( int ) this->NumberOfModesInEachCellField[clean_name], atoi ( name.substr ( pos+1,name.size()-1 ).c_str() ) +1 ) ;
                }
            }
        this->UpdateInternalData();

        vtkDataSet* output = vtkDataSet::GetData ( outInfo );

        /// we do not support multiple timestep requests.
        if ( outInfo->Has ( vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEP() ) ) {
            this->Time = outInfo->Get ( vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEP() );
            }

        this->PointMap.clear();
        this->PointMap.resize ( GetNumberOfPXDMFDims() );
        ///for every dimension allocation for every field
        int numberoffields  = this->PointFieldData.size();
        for ( int i=0; i < GetNumberOfPXDMFDims(); ++i ) {
            this->PointMap[i].resize ( numberoffields );
            /// for every field allocation for every mode
            for ( int j=0; j < numberoffields; ++j ) {
                std::string name = GetPointDataName ( j );
                unsigned numberOfModes = this->GetNumberOfModesOfPointData ( GetPointDataName ( j ) );
                this->PointMap[i][j].resize ( numberOfModes );
                for ( int k=0; k < ( int ) numberOfModes; ++k ) {
                    this->PointMap[i][j][k] = NULL;
                    }
                }
            }

        this->CellMap.clear();
        this->CellMap.resize ( GetNumberOfPXDMFDims() );
        ///for every dimension allocation for every field
        numberoffields  = this->CellFieldData.size();
        for ( int i=0; i < GetNumberOfPXDMFDims(); ++i ) {
            this->CellMap[i].resize ( numberoffields );
            /// for every field allocation for every mode
            for ( int j=0; j < numberoffields; ++j ) {
                std::string name = GetCellDataName ( j );
                unsigned numberOfModes = GetNumberOfModesOfCellData ( GetCellDataName ( j ) );
                this->CellMap[i][j].resize ( numberOfModes );
                for ( int k=0; k < ( int ) numberOfModes; ++k ) {
                    this->CellMap[i][j][k] = NULL;
                    }
                }
            }

        vtkDataObject* space_data = this->GenerateOutput ( request, inputVector, outputVector );
        output->ShallowCopy ( space_data );
        space_data->Delete();
        return 1;
        }
    else {
        vtkErrorMacro ( << "Invalid or missing input" );
        return 0;
        }
    };
//
#include <vtkInformationRequestKey.h>
int vtkReconstruction::ProcessRequest (
    vtkInformation* request,
    vtkInformationVector** inputVector,
    vtkInformationVector* outputVector ) {

    // generate the data
    if ( request->Has ( vtkDemandDrivenPipeline::REQUEST_DATA() ) ) {
        return this->RequestData ( request, inputVector, outputVector );
        }

    // create the output
    if ( request->Has ( vtkDemandDrivenPipeline::REQUEST_DATA_OBJECT() ) ) {
        return this->RequestDataObject ( request, inputVector, outputVector );
        }

    // execute information
    if ( request->Has ( vtkDemandDrivenPipeline::REQUEST_INFORMATION() ) ) {
        return this->RequestInformation ( request, inputVector, outputVector );
        }
    return this->Superclass::ProcessRequest ( request, inputVector, outputVector );
    }
//
int vtkReconstruction::RequestInformation (
    vtkInformation* request,
    vtkInformationVector** inputVector,
    vtkInformationVector* outputVector ) {
    //std::cout << "RequestInformation " << std::endl;

    vtkInformation* outInfo = outputVector->GetInformationObject ( 0 );
    PrepareDocumentInput ( request, inputVector,outputVector );

    // * Publish time information
    if ( this->haveTime ) {
        unsigned time_grid=0;
        for ( ; time_grid < this->GetActivePXDMFDimForTime().size(); ++time_grid ) {
            if ( this->GetActivePXDMFDimForTime() [time_grid] ) {
                break;
                }
            }

        outInfo->Set ( vtkPVInformationKeys::TIME_LABEL_ANNOTATION(), ( this->NamesOfEachDimension[time_grid][0]+ " " + this->UnitsOfEachDimension[time_grid][0] ).c_str() );

        if ( LastTimeDimension == int ( time_grid ) ) {
            outInfo->Set ( vtkStreamingDemandDrivenPipeline::TIME_STEPS(), &this->TimeStepsVector[0], static_cast<int> ( this->TimeStepsVector.size() ) );
            double timeRange[2];
            timeRange[0] = this->TimeStepsVector.front();
            timeRange[1] = this->TimeStepsVector.back();
            outInfo->Set ( vtkStreamingDemandDrivenPipeline::TIME_RANGE(), timeRange, 2 );
            }
        else {

            int nb_element = AllData[time_grid]->GetNumberOfElements ( 1 );
            vtkstd::vector<double> time_steps;
            AllData[time_grid]->PrintSelf ( cerr, vtkIndent() );
            int NumberoOfPonintsPerElement  = 0 ;

            if ( AllData[time_grid]->GetCell ( 0 )->GetCellType()  == VTK_POLY_LINE ||  AllData[time_grid]->GetCell ( 0 )->GetCellType() == VTK_LINE ) {
                NumberoOfPonintsPerElement  = 2 ;
                }
            else {
                if ( AllData[time_grid]->GetCell ( 0 )->GetCellType()  == VTK_POLY_VERTEX ||  AllData[time_grid]->GetCell ( 0 )->GetCellType() == VTK_VERTEX ) {
                    NumberoOfPonintsPerElement  = 1 ;
                    }
                else {
                    std::cout << "ERROR  not coded yet sorry"  << std::endl;
                    std::cout <<  "This Grid  cant be used for time, sorry." << std::endl ;
                    vtkErrorMacro ( "This Grid cant be used for time, sorry." );
                    return 0;

                    }
                }
            switch ( NumberoOfPonintsPerElement ) {
                case 1: {
                    time_steps.resize ( nb_element+ ( NumberoOfPonintsPerElement-1 ) );
                    for ( unsigned i = 0; int ( i ) < nb_element; ++i ) {
                        time_steps[i] = AllData[time_grid]->GetPoint ( AllData[time_grid]->GetCell ( i )->GetPointId ( 0 ) ) [0] ;
                        }
                    break;
                    }
                case 2: {
                    time_steps.resize ( nb_element+ ( NumberoOfPonintsPerElement-1 ) );
                    unsigned cpt =0;
                    for ( unsigned i = 0; int ( i ) < nb_element; ++i ) {
                        for ( unsigned j = 0; int ( j ) < NumberoOfPonintsPerElement; ++j ) {
                            addtime ( time_steps,
                                      AllData[time_grid]->GetPoint ( AllData[time_grid]->GetCell ( i )->GetPointId ( j ) ) [0]
                                      , cpt );
                            }
                        }
                    break;
                    }
                }

            if ( time_steps.size() > 0 ) {
                std::sort ( time_steps.begin(),time_steps.end() );
                outInfo->Set ( vtkStreamingDemandDrivenPipeline::TIME_STEPS(), &time_steps[0], static_cast<int> ( time_steps.size() ) );
                double timeRange[2];
                timeRange[0] = time_steps.front();
                timeRange[1] = time_steps.back();

                outInfo->Set ( vtkStreamingDemandDrivenPipeline::TIME_RANGE(), timeRange, 2 );
                }
            this->LastTimeDimension = time_grid;
            this->TimeStepsVector = time_steps;
            }
        }
    else {
        outInfo->Remove ( vtkPVInformationKeys::TIME_LABEL_ANNOTATION() );
        outInfo->Remove ( vtkStreamingDemandDrivenPipeline::TIME_STEPS() );
        outInfo->Remove ( vtkStreamingDemandDrivenPipeline::TIME_RANGE() );
        }

    if ( this->VtkOutputType!=VTK_UNSTRUCTURED_GRID || this->VtkOutputType!=VTK_POLY_DATA) {
        std::vector<int> space_grid;
        for ( unsigned i = 0 ; i < this->GetActivePXDMFDimsForSpace().size(); ++i ) {
            if ( this->GetActivePXDMFDimsForSpace() [i] ) {
                space_grid.push_back ( i );
                }
            }
        int whole_extent[6] = {0,0,0,0,0,0};
        double origin[3]= {0,0,0};
        double spacing[3] = {1,1,1};

        unsigned dimcpt =0;

        for ( unsigned i =0; i < space_grid.size(); ++i ) {
            for ( unsigned j =0; int ( j ) < this->NumberOfDimsInEachGrid->GetValue ( space_grid[i] ); ++j ) {
                int whole_extentlocal[6] = {0,0,0,0,0,0};
                if ( vtkImageData::SafeDownCast ( this->AllData[space_grid[i]] ) ) {
                    vtkImageData::SafeDownCast ( this->AllData[space_grid[i]] )->GetExtent ( whole_extentlocal );
                    }
                else {
                    if ( vtkRectilinearGrid::SafeDownCast ( this->AllData[space_grid[i]] ) ) 
                        vtkRectilinearGrid::SafeDownCast ( this->AllData[space_grid[i]] )->GetExtent ( whole_extentlocal );;
                    }

                whole_extent[2*dimcpt] =whole_extentlocal[2*j];
                whole_extent[1+2*dimcpt] =whole_extentlocal[2*j+1] ;

                ++dimcpt;
                }
            }

        outInfo->Set ( vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), whole_extent, 6 );

        }

    return 1;
    };
//
int vtkReconstruction::RequestDataObject ( vtkInformation *,
        vtkInformationVector **inputVector,
        vtkInformationVector *outputVector ) {

    PrepareDocumentInput ( NULL, inputVector, NULL );

    int outputType = this->ReadOutputType();


    vtkInformation* info = outputVector->GetInformationObject ( 0 );


    vtkDataSet *output = vtkDataSet::SafeDownCast (
                             info->Get ( vtkDataObject::DATA_OBJECT() ) );

    if ( !output || output->GetDataObjectType() != outputType ) {
        vtkDataSet* newOutput = ( vtkDataSet* ) vtkDataObjectTypes::NewDataObject ( this->VtkOutputType );
        this->GetOutputPortInformation ( 0 )->Set ( vtkDataObject::DATA_EXTENT_TYPE(), newOutput->GetExtentType() );

        info->Set ( vtkDataObject::DATA_OBJECT(), newOutput );
        newOutput->Delete();
        }

    return 1;
    };
//
vtkDataObject* vtkReconstruction::GenerateOutput ( vtkInformation * Info, vtkInformationVector ** inputVector, vtkInformationVector *outputVector ) {

    vtkInformation* outInfo = outputVector->GetInformationObject ( 0 );

    int update_extent[6] = {0, -1, 0, -1, 0, -1};

    if ( outInfo->Has ( vtkStreamingDemandDrivenPipeline::UPDATE_EXTENT() ) ) {
        outInfo->Get ( vtkStreamingDemandDrivenPipeline::UPDATE_EXTENT(),update_extent );
        }

    vtkDataObject* space_data = GenerateSpace();

    if ( !space_data ) {
        vtkErrorMacro ( "Failed to generate data." );
        return 0;
        }

    return space_data;
    };
//
void vtkReconstruction::CalculateMaxMin ( const unsigned j ) {

    this->MaxOfEachDimension[j].resize ( this->GetNumberOfDimsInEachPXDMFDim()->GetValue ( j ) );
    this->MinOfEachDimension[j].resize ( this->GetNumberOfDimsInEachPXDMFDim()->GetValue ( j ) );
    this->PosOfEachDimension[j].resize ( this->GetNumberOfDimsInEachPXDMFDim()->GetValue ( j ) );

    for ( unsigned i =0; int ( i ) < this->GetNumberOfDimsInEachPXDMFDim()->GetValue ( j ); ++i ) {
        this->MinOfEachDimension[j][i] = this->AllData[j]->GetBounds() [2*i];
        this->MaxOfEachDimension[j][i] = this->AllData[j]->GetBounds() [2*i+1];
        if ( this->PosOfEachDimension[j][i] < this->MinOfEachDimension[j][i] || this->PosOfEachDimension[j][i]> this->MaxOfEachDimension[j][i] )
            this->PosOfEachDimension[j][i] = this->MinOfEachDimension[j][i];
        }
    }
//----------------------------------------------------------------------------
vtkDoubleArray* vtkReconstruction::GetPXDMFDimsMinRangeDataInfo() {
    unsigned cpt =0;
    if ( vtkPXDMFDimsMinRangeData->GetSize() == 0 ) {
        for ( unsigned i = 0 ; int ( i ) <  this->GetNumberOfPXDMFDims(); ++i ) {
            cpt += this->GetNumberOfDimsInEachPXDMFDim()->GetValue ( i );
            }
        vtkPXDMFDimsMinRangeData->SetNumberOfComponents ( 1 );
        vtkPXDMFDimsMinRangeData->SetNumberOfValues ( cpt );
        }
    cpt=0;
    for ( unsigned i = 0 ; int ( i ) < this->GetNumberOfPXDMFDims(); ++i ) {
        for ( unsigned j = 0 ; int ( j ) < this->GetNumberOfDimsInEachPXDMFDim()->GetValue ( i ); ++j ) {
            vtkPXDMFDimsMinRangeData->SetValue ( cpt, MinOfEachDimension[i][j] );
            ++cpt;
            }
        }
    return vtkPXDMFDimsMinRangeData;
    };
//
vtkDoubleArray* vtkReconstruction::GetPXDMFDimsPosDataInfo() {
    if ( vtkPXDMFDimsPosData->GetSize() == 0 ) {
        unsigned cpt =0;

        for ( unsigned i = 0 ; int ( i ) < this->GetNumberOfPXDMFDims(); ++i ) {
            cpt += this->GetNumberOfDimsInEachPXDMFDim()->GetValue ( i );
            }
        vtkPXDMFDimsPosData->SetNumberOfComponents ( 1 );
        vtkPXDMFDimsPosData->SetNumberOfValues ( cpt );
        }
    unsigned  cpt=0;
    for ( unsigned i = 0 ; int ( i ) < this->GetNumberOfPXDMFDims(); ++i ) {
        for ( unsigned j = 0 ; int ( j ) < this->GetNumberOfDimsInEachPXDMFDim()->GetValue ( i ); ++j ) {
            vtkPXDMFDimsPosData->SetValue ( cpt, PosOfEachDimension[i][j] );
            ++cpt;
            }
        }
    return vtkPXDMFDimsPosData;
    };
//
const char* vtkReconstruction::GetVisualizationTimeArrayName ( int index ) {
    return GetVisualizationSpaceArrayName ( index );
    };
//
int vtkReconstruction::GetVisualizationTimeArrayStatus ( const char* name ) {
    for ( unsigned i=0; int ( i ) < this->GetNumberOfPXDMFDims(); ++i ) {
        if ( strcmp ( name, GetVisualizationTimeArrayName ( i ) ) == 0 ) {
            return this->GetActivePXDMFDimForTime() [i];
            }
        }
    return 0;
    };
//
void vtkReconstruction::SetVisualizationTimeStatus ( const char* name, int status ) {
    if ( status == 0 ) {
        haveTime = false;
        for ( unsigned i=0; int ( i ) < this->GetNumberOfPXDMFDims(); ++i ) {
            if ( strcmp ( name, GetVisualizationTimeArrayName ( i ) ) == 0 ) {
                this->GetActivePXDMFDimForTime() [i] = false;
                }
            this->haveTime = this->haveTime || this->GetActivePXDMFDimForTime() [i];
            }
        }
    else {
        for ( unsigned i=0; int ( i ) < this->GetNumberOfPXDMFDims(); ++i ) {
            if ( ( strcmp ( name, GetVisualizationTimeArrayName ( i ) ) == 0 ) ) {
                if ( this->GetNumberOfDimsInEachPXDMFDim()->GetValue ( i ) == 1 ) {
                    this->GetActivePXDMFDimForTime() [i] = true;
                    haveTime = 1;
                    }
                else {
                    vtkWarningMacro ( "this Grid dimensionality is different from 1, Cant be used for Time " );
                    }
                }
            }
        }
    this->Modified();
    return;
    };
//
void vtkReconstruction::SetPXDMFDimsPosDataInfo ( int id , double value ) {

    unsigned cpt =0;
    if ( vtkPXDMFDimsPosData->GetSize() == 0 ) {

        for ( unsigned i = 0 ; int ( i ) < this->GetNumberOfPXDMFDims(); ++i ) {
            cpt += this->GetNumberOfDimsInEachPXDMFDim()->GetValue ( i );
            }
        vtkPXDMFDimsPosData->SetNumberOfComponents ( 1 );
        vtkPXDMFDimsPosData->SetNumberOfValues ( cpt );
        }

    // TODO TO check
    if ( id+1 > vtkPXDMFDimsPosData->GetSize() )
        vtkPXDMFDimsPosData->SetNumberOfValues ( id+1 );

    vtkPXDMFDimsPosData->SetValue ( id, value );

    cpt = 0;
    for ( unsigned i =0; int ( i ) < this->GetNumberOfPXDMFDims(); ++i ) {
        for ( unsigned j =0; int ( j ) < this->GetNumberOfDimsInEachPXDMFDim()->GetValue ( i ); ++j ) {
            this->PosOfEachDimension[i][j] = vtkPXDMFDimsPosData->GetValue ( cpt );
            ++cpt ;
            }
        }
    this->Modified();
    };