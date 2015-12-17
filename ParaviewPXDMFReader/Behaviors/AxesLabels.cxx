/*=========================================================================

  Program:   PXDMFReader Plugin
  Module:    AxesLabels.cxx

  Copyright (c) GeM, Ecole Centrale Nantes.
  All rights reserved.
  Copyright: See COPYING file that comes with this distribution


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "AxesLabels.h"

// c++ includes
#include "vector"

//Paraview includes
#include "pqSettings.h"
#include "pqView.h"
#include "pqActiveObjects.h"
#include "vtkPVAxesWidget.h"
#include "vtkPVAxesActor.h"
#include "vtkSMViewProxy.h"
#include "vtkPVRenderView.h"
#include "pqServerManagerModel.h"
#include "pqApplicationCore.h"
#include "vtkSMIntVectorProperty.h"
#include "vtkSMStringVectorProperty.h"
#include "vtkRenderer.h"
#include "vtkSMSourceProxy.h"
#include "vtkSMRepresentationProxy.h"
#include "pqPipelineFilter.h"

class vtkPVRenderViewWithAccess : public vtkPVRenderView {
    public:
        vtkPVAxesWidget*  GetOrientationWidget( ) {
            return OrientationWidget;
            }
    };
//
pqAxesLabels::~pqAxesLabels( )    {
    }
//
pqAxesLabels::pqAxesLabels ( QObject *p/*=0*/ )  : QObject ( p )  {
    pqServerManagerModel* smmodel =  pqApplicationCore::instance()->getServerManagerModel();
    QObject::connect ( smmodel, SIGNAL ( sourceAdded ( pqPipelineSource* ) ), this, SLOT ( ConnectSource ( pqPipelineSource* ) ) );
    QObject::connect ( smmodel, SIGNAL ( sourceRemoved ( pqPipelineSource* ) ), this, SLOT ( axisLabelUpdate ( ) ) );



    }
//
void pqAxesLabels::ConnectSource ( pqPipelineSource* src ) {
    vtkSMProxy* theProxy = src->getProxy();

    //if ( theProxy->GetVTKClassName() && ( strcmp ( "vtkPXDMFReader", theProxy->GetVTKClassName() ) ==0 ||  strcmp ( "vtkReconstruction", theProxy->GetVTKClassName() ) ==0 ) ) {
        QObject::connect ( src, SIGNAL ( dataUpdated ( pqPipelineSource* ) ), this, SLOT ( axisLabelUpdate () ) );
        QObject::connect ( src, SIGNAL ( representationAdded ( pqPipelineSource*,pqDataRepresentation*,int ) ), this, SLOT ( axisLabelUpdate () ) );
        QObject::connect ( src, SIGNAL ( representationRemoved ( pqPipelineSource*,pqDataRepresentation*,int ) ), this, SLOT ( axisLabelUpdate () ) );
        QObject::connect ( src, SIGNAL ( visibilityChanged ( pqPipelineSource*,pqDataRepresentation* ) ), this, SLOT ( axisLabelUpdate () ) );
    //}
}
//

void pqAxesLabels::axisLabelUpdate() {
  
    pqSettings *settings = pqApplicationCore::instance()->settings();
    bool show = settings->value ( "PxdmfSettings.UpdateOrientationAxesLabels",true ).toBool();
    if ( !show ) {
      return ;
    }
    std::vector<std::string> pos;
    pos.resize ( 3 );
    // zero = original names, 1 pos names, 2 empty
    char posflag[3] = {0, 0, 0};

    pqServerManagerModel* smmodel =  pqApplicationCore::instance()->getServerManagerModel();
    if(!smmodel) return;
    QList<pqPipelineSource*> Pipelineitems =  smmodel->findItems<pqPipelineSource*>();

    pqView *view = 0;
    view = pqActiveObjects::instance().activeView();
    if(!view) return;

    foreach ( pqPipelineSource* src,Pipelineitems ) {

        vtkSMProxy* theProxy = src->getProxy();
        //vtkSMSourceProxy* sProxy = vtkSMSourceProxy::SafeDownCast(theProxy);

        QList<pqDataRepresentation*> dataRepitems = src->getRepresentations ( view );

        int cptt = dataRepitems.size();
        foreach ( pqDataRepresentation* dataRep,dataRepitems ) {
            if ( dataRep->isVisible() ) break;
            cptt--;
            }
        // if no representation or all reps are not visible.
        if ( cptt == 0 ) continue;

        if ( !theProxy->GetVTKClassName() || ( strcmp ( "vtkPXDMFReader", theProxy->GetVTKClassName() ) !=0 &&  strcmp ( "vtkReconstruction", theProxy->GetVTKClassName() ) !=0 ) ) {
            // we go up the pipeline to recover the vtkPXDMFReader or vtkReconstruction
            bool ok = 0;
            pqPipelineFilter* src2 = qobject_cast<pqPipelineFilter*>(src);
            while(src2){               
               src = src2->getInput (0);
               src2 =  qobject_cast<pqPipelineFilter*>(src); 
               theProxy = src->getProxy();
               if ( theProxy->GetVTKClassName() && strcmp ( "vtkReconstruction", theProxy->GetVTKClassName() ) ==0 ){
                 break;
              }
            }
            if ( !theProxy->GetVTKClassName() || ( strcmp ( "vtkPXDMFReader", theProxy->GetVTKClassName() ) !=0 &&  strcmp ( "vtkReconstruction", theProxy->GetVTKClassName() ) !=0 ) ) {
             continue;  
            } 
            
        };
        
        
        vtkSMIntVectorProperty *doReconstruction =  vtkSMIntVectorProperty::SafeDownCast ( theProxy->GetProperty ( "DoReconstruction" ) );
        if ( doReconstruction && doReconstruction->GetElement ( 0 ) ==0 ) {
          continue;
        }
          
          
        vtkSMIntVectorProperty *NumberOfPXDMF_Dims =  vtkSMIntVectorProperty::SafeDownCast ( theProxy->GetProperty ( "PXDMF_Dims" ) );
        vtkSMIntVectorProperty *gridsDimsDims =  vtkSMIntVectorProperty::SafeDownCast ( theProxy->GetProperty ( "PXDMFDimsDataInfo" ) );
        vtkSMStringVectorProperty *SepSpaceStatus = vtkSMStringVectorProperty::SafeDownCast ( theProxy->GetProperty ( "VisualizationSpaceStatus" ) );
        vtkSMStringVectorProperty *gridsDimsNames = vtkSMStringVectorProperty::SafeDownCast ( theProxy->GetProperty ( "PXDMFDimsNameDataInfo" ) );

        unsigned NumberOfPXDMFDims = NumberOfPXDMF_Dims->GetElement ( 0 );


        int cpt  =0;
        int coorcpt = 0;
        for ( unsigned i = 0; i < NumberOfPXDMFDims; ++i ) {
            int NumberOfDimensionsPerPXDMFDim = gridsDimsDims->GetElement ( i );
            if ( strcmp ( SepSpaceStatus->GetElement ( 2*i+1 ), "0" ) !=0 ) {
                for ( int j = 0; j < NumberOfDimensionsPerPXDMFDim; ++j ) {
                    const char* name = gridsDimsNames->GetElement ( cpt );
                    if ( pos[coorcpt].size() == 0 && posflag[coorcpt] == 0 ) {
                        pos[coorcpt] = name;
                        posflag[coorcpt] = 1;
                        }
                    else {
                        if ( strcmp ( pos[coorcpt].c_str(),name ) != 0 ) {
                            pos[coorcpt] = "";
                            posflag[coorcpt] = 2;
                            }
                        }
                    coorcpt++;
                    cpt++;
                    }
                }
            else {
                for ( int j = 0; j < NumberOfDimensionsPerPXDMFDim; ++j ) {
                    cpt++;
                    }
                }
            }
        };

    if ( view ) {
        vtkSMViewProxy* viewProxy =  view->getViewProxy();
        if ( viewProxy ) {
            if ( viewProxy->GetVTKClassName() && ( strcmp ( "vtkPVRenderView", viewProxy->GetVTKClassName() ) ==0 ) ) {
                vtkPVRenderView* RV =  vtkPVRenderView::SafeDownCast ( viewProxy->GetClientSideView() );
                vtkPVAxesWidget * AW = static_cast<vtkPVRenderViewWithAccess*> ( RV )->GetOrientationWidget();
                // AW->GetParentRenderer()->Size(1) =  AW->GetParentRenderer()->GetSize()[1]*2;
                //double *vp = AW->GetViewport();
                //int*  s = AW->GetRenderer()->GetSize();
                //std::cout << "-------------------" << std::endl;
                //for (int i =0; i < 2; i++){
                //  std::cout << "s["<<i<<"] " << s[i] << std::endl;
                //}
                //for (int i =0; i < 4; i++){
                //  std::cout << "vp["<<i<<"] " << vp[i] << std::endl;
                //}
                AW->SetViewport ( 0, 0, 0.4, 0.2 );
                vtkPVAxesActor* axis = vtkPVAxesActor::SafeDownCast ( AW->GetAxesActor() );
                if ( axis ) {

                    if ( posflag[0] == 0 ) {
                        axis->SetXAxisLabelText ( "X" );
                        axis->SetYAxisLabelText ( "Y" );
                        axis->SetZAxisLabelText ( "Z" );

                        }
                    else {
                        if ( posflag[0] ==1 ) {
                            axis->SetXAxisLabelText ( pos[0].c_str() );
                            }
                        else {
                            axis->SetXAxisLabelText ( "" );
                            }
                            
                        if ( posflag[1] == 0  || posflag[1] == 2 ) {
                            axis->SetYAxisLabelText ( "" );
                            }
                        else {
                            axis->SetYAxisLabelText ( pos[1].c_str() );
                            }
                        if ( posflag[2] == 0  || posflag[2] == 2 ) {
                            axis->SetZAxisLabelText ( "" );
                            }
                        else {
                            axis->SetZAxisLabelText ( pos[2].c_str() );
                            }
                        }
                    }
                }
            }
        }
    }