/*=========================================================================

  Program:   PXDMFReader Plugin
  Module:    PostFilters.cxx

  Copyright (c) GeM, Ecole Centrale Nantes.
  All rights reserved.
  Copyright: See COPYING file that comes with this distribution


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "PostFilters.h"

//vtk Includes 
#include "vtkSMProxy.h"
#include "vtkSMSourceProxy.h"
#include "vtkSMOutputPort.h"
#include "vtkSMIntVectorProperty.h"
#include "vtkSMStringVectorProperty.h"
#include "vtkSMDoubleVectorProperty.h"

//ParaView Includes 
#include "vtkPVDataInformation.h"
#include "vtkPVDataSetAttributesInformation.h"
#include "vtkPVArrayInformation.h"
#include <pqActiveObjects.h>
#include "pqObjectBuilder.h"
#include "pqServerManagerModel.h"
#include "pqApplicationCore.h"

//Plugin Includes
#include "vtkPXDMFGeneralSettings.h"

#if defined(_WIN32) || defined(_WIN64)
  #define snprintf _snprintf
  #define vsnprintf _vsnprintf
  #define strcasecmp _stricmp
  #define strncasecmp _strnicmp
#endif

pqPostFilters::pqPostFilters ( QObject *p/*=0*/ )  : QObject ( p ) {

    pqServerManagerModel* smmodel =  pqApplicationCore::instance()->getServerManagerModel();
    QObject::connect ( smmodel, SIGNAL ( sourceAdded ( pqPipelineSource* ) ), this, SLOT ( ConnectSource ( pqPipelineSource* ) ) );

    this->postReconstructionMapping = false;
    this->postReconstructionThreshold = false;
    this->postReconstructionAnnotateFixedDims = false;
    
    };
//
pqPostFilters::~pqPostFilters() {
    };
//
void pqPostFilters::ConnectSource ( pqPipelineSource* src ) {

    this->postReconstructionMapping = vtkPXDMFGeneralSettings::GetInstance()->GetPostReconstructionThreshold();
    this->postReconstructionThreshold = vtkPXDMFGeneralSettings::GetInstance()->GetPostReconstructionThreshold();
    this->postReconstructionAnnotateFixedDims = vtkPXDMFGeneralSettings::GetInstance()->GetPostReconstructionAnnotateFixedDims();
    

    vtkSMProxy* theProxy = src->getProxy();
    if ( theProxy->GetVTKClassName() && ( strcmp ( "vtkPXDMFReader", theProxy->GetVTKClassName() ) ==0 ||  strcmp ( "vtkReconstruction", theProxy->GetVTKClassName() ) ==0 ) ) {
        //pqSpaceTimeSelector*)
        QObject::connect ( src, SIGNAL ( dataUpdated ( pqPipelineSource* ) ), this, SLOT ( SetMappAndThredhold ( pqPipelineSource* ) ) );
        }
    }
//
void pqPostFilters::SetMappAndThredhold ( pqPipelineSource* src ) {

    QObject::disconnect ( src, SIGNAL ( dataUpdated ( pqPipelineSource* ) ), this, SLOT ( SetMappAndThredhold ( pqPipelineSource* ) ) );

    vtkSMProxy* theProxy = src->getProxy();
    if ( theProxy->GetVTKClassName() && ( strcmp ( "vtkPXDMFReader", theProxy->GetVTKClassName() ) ==0 ||  strcmp ( "vtkReconstruction", theProxy->GetVTKClassName() ) ==0 ) ) {
        vtkSMSourceProxy* theSource =  vtkSMSourceProxy::SafeDownCast ( theProxy );
        if ( theSource ) {
          
            // do not apply anything if the DoReconstruction is not activated
            vtkSMIntVectorProperty *DoReconstruction =  vtkSMIntVectorProperty::SafeDownCast(theProxy->GetProperty("DoReconstruction"));
            if( DoReconstruction && DoReconstruction->GetElement(0)==0) return;
          
            vtkSMOutputPort* theOPort  = theSource->GetOutputPort ( unsigned ( 0 ) );
            vtkPVDataInformation* theDataInfo =  theOPort->GetDataInformation();
            vtkPVDataSetAttributesInformation* thePointData = theDataInfo->GetPointDataInformation();
            int nbparrays = thePointData->GetNumberOfArrays();

            int MappPresent = -1;


            // we search for the mapping
            for ( int i = 0; i < nbparrays; i++ ) {
                vtkPVArrayInformation* info = thePointData->GetArrayInformation ( i )  ;
                if ( strncasecmp ( "mapping",info->GetName(),4 ) ==0 ) {
                    MappPresent = i;
                    break;
                    }
                }


            pqObjectBuilder* oBuilder =  pqApplicationCore::instance()->getObjectBuilder();

            pqPipelineSource*  warpfilter = 0;

            vtkSMIntVectorProperty *NumberOfPXDMF_Dims =  vtkSMIntVectorProperty::SafeDownCast ( theProxy->GetProperty ( "PXDMF_Dims" ) );
            unsigned NumberOfPXDMFDims = NumberOfPXDMF_Dims->GetElement ( 0 );

            vtkSMIntVectorProperty *gridsDimsDims =  vtkSMIntVectorProperty::SafeDownCast ( theProxy->GetProperty ( "PXDMFDimsDataInfo" ) );
            vtkSMStringVectorProperty *gridsDimsNames = vtkSMStringVectorProperty::SafeDownCast ( theProxy->GetProperty ( "PXDMFDimsNameDataInfo" ) );
            vtkSMStringVectorProperty *SepSpaceStatus = vtkSMStringVectorProperty::SafeDownCast ( theProxy->GetProperty ( "VisualizationSpaceStatus" ) );

            int cpt = 0;
            std::vector<std::string> pos;

            for ( unsigned i = 0; i < NumberOfPXDMFDims; ++i ) {
                int NumberOfDimensionsPerPXDMFDim = gridsDimsDims->GetElement ( i );
                if ( strcmp ( SepSpaceStatus->GetElement ( 2*i+1 ), "0" ) !=0 ) {
                    for ( int j = 0; j < NumberOfDimensionsPerPXDMFDim; ++j ) {
                        pos.push_back ( gridsDimsNames->GetElement ( cpt ) );
                        cpt++;
                        }
                    }
                else {
                    for ( int j = 0; j < NumberOfDimensionsPerPXDMFDim; ++j ) {
                        cpt++;
                        }
                    }
                }

            if ( MappPresent >=0 && ( this->postReconstructionMapping == 1) ) {
                /// for now only scalar mapping are possible
                //warpfilter = oBuilder->createFilter ( QString ( "filters" ),QString ( "vtkWarpVector" ),src );

                std::map<std::string,std::string> CoordMapp_NameMapp;
                for ( int i = 0; i < nbparrays; i++ ) {
                    vtkPVArrayInformation* info = thePointData->GetArrayInformation ( i )  ;
                    std::string name = info->GetName();
                    //std::cout << name << std::endl;
                    if ( strncasecmp ( "mapping",name.c_str(),4 ) ==0 ){
                        if( info->GetNumberOfComponents() == 1 ) {
                           std::string suf = name.substr ( name.length()-1,name.length() );
                           CoordMapp_NameMapp[suf ] = name;
                        } else { 
                            if (info->GetNumberOfComponents() == 3 ){
                              /// 3 componnest 
                              std::string suf = name.substr ( name.length()-3,name.length()-2 );
                              CoordMapp_NameMapp[suf ] = name + "_X";
                              suf = name.substr ( name.length()-2,name.length()-1 );
                              CoordMapp_NameMapp[suf ] = name + "_Y";                            
                              suf = name.substr ( name.length()-1,name.length() );
                              CoordMapp_NameMapp[suf ] = name + "_Z";           
                            }
                          
                        }
                    }
                }

                if ( CoordMapp_NameMapp.size() ) {
                    std::string formula ;
                    bool createcalc=false ;
                    if ( pos.size() >0 && CoordMapp_NameMapp.count ( pos[0] ) ) {
                        createcalc=1;
                        formula = formula + "iHat*" + CoordMapp_NameMapp[pos[0]];
                    } else {
                      formula = formula += "iHat*coordsX"; 
                    }
                        
                    if ( pos.size() >1 && CoordMapp_NameMapp.count ( pos[1] ) ) {
                        createcalc=1;
                        formula = formula + " + jHat*" + CoordMapp_NameMapp[pos[1]];
                    } else {
                      formula = formula += " + jHat*coordsY"; 
                    }
                        
                    if ( pos.size() >2 && CoordMapp_NameMapp.count ( pos[2] ) ) {
                        createcalc=1;
                        formula = formula + " + kHat*" + CoordMapp_NameMapp[pos[2]];
                    }else {
                      formula = formula += " + kHat*coordsZ"; 
                    }

                    if ( createcalc ) {
                        warpfilter = oBuilder->createFilter ( QString ( "filters" ),QString ( "Calculator" ),src );
                        warpfilter->rename(QString("Mapping"));
                        vtkSMProxy* calcProxy = warpfilter->getProxy();
                        vtkSMIntVectorProperty *CoordinateResults =  vtkSMIntVectorProperty::SafeDownCast ( calcProxy->GetProperty ( "CoordinateResults" ) );
                        CoordinateResults->SetElement ( 0,true );
                        vtkSMStringVectorProperty *Function =  vtkSMStringVectorProperty::SafeDownCast ( calcProxy->GetProperty ( "Function" ) );
                        Function->SetElement ( 0,formula.c_str() );
                        }
                    }



                if ( warpfilter )
                    warpfilter->updatePipeline();

                }
            // apply

            /// we search for the indicatrice *****************************************************************************
            int indiPresent = -1;
            vtkPVDataSetAttributesInformation* theCellData = theDataInfo->GetCellDataInformation();
            int nbcarrays = theCellData->GetNumberOfArrays();
            for ( int i = 0; i < nbcarrays; i++ ) {
                vtkPVArrayInformation* info = theCellData->GetArrayInformation ( i )  ;
                //std::cout << info->GetName() << std::endl;
                if ( strncasecmp ( "indic",info->GetName(),4 ) ==0 ) {
                    indiPresent = i;
                    break;
                    }
                }

            if ( indiPresent >=0 && this->postReconstructionThreshold == 1 ) {
                if ( warpfilter )
                    ApplyThreshold ( theCellData->GetArrayInformation ( indiPresent )->GetName(),warpfilter );
                else
                    ApplyThreshold ( theCellData->GetArrayInformation ( indiPresent )->GetName(),src );
                }
                
            /// Apply Annotate Field Data *****************************************************************************
            
            
            if ( this->postReconstructionAnnotateFixedDims == 1 ) {
              vtkPVDataSetAttributesInformation* theFieldData = theDataInfo->GetFieldDataInformation();
              if(theFieldData->GetNumberOfArrays() )
                oBuilder->createFilter ( QString ( "filters" ),QString ( "AnnotateFieldData" ),src );
                pqActiveObjects::instance().setActiveSource ( src );
              }
            }
        }
    }
//
pqPipelineSource* pqPostFilters::ApplyThreshold ( char* name, pqPipelineSource* input ) {
    pqPipelineSource* threshold;

    threshold = pqApplicationCore::instance()->getObjectBuilder()->createFilter ( QString ( "filters" ),QString ( "Threshold" ),input );

    vtkSMProxy* thresholdProxy = threshold->getProxy();
    vtkSMDoubleVectorProperty *CoordinateResults =  vtkSMDoubleVectorProperty::SafeDownCast ( thresholdProxy->GetProperty ( "ThresholdBetween" ) );
    CoordinateResults->SetElement ( 0,0.5 );
    CoordinateResults->SetElement ( 1,2 );

    vtkSMStringVectorProperty *InputArrayToProcess =  vtkSMStringVectorProperty::SafeDownCast ( thresholdProxy->GetProperty ( "SelectInputScalars" ) );
    InputArrayToProcess->SetElement ( 0,"0" );
    InputArrayToProcess->SetElement ( 1,"0" );
    InputArrayToProcess->SetElement ( 2,"0" );
    InputArrayToProcess->SetElement ( 3,"1" );
    InputArrayToProcess->SetElement ( 4,name );
    threshold->updatePipeline();
    return threshold;
    }
