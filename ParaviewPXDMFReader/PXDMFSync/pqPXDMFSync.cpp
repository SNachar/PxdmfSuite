/*=========================================================================

  Program:   PXDMFReader Plugin
  Module:    pqPXDMFSync.cpp

  Copyright (c) GeM, Ecole Centrale Nantes.
  All rights reserved.
  Copyright: See COPYING file that comes with this distribution


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

// Std Includes 
#include <iostream>

// Vtk Includes 
#include "vtkSMProxy.h"
#include "vtkSMIntVectorProperty.h"
#include "vtkSMStringVectorProperty.h"
#include "vtkSMDoubleVectorProperty.h"

// ParaView Includes
#include "pqPXDMFSync.h"
#include "pqApplicationCore.h"
#include "pqServerManagerModel.h"
#include "pqPipelineSource.h"
#include "pqServerManagerModel.h"

// Plugin Includes
#include "ui_pqPXDMFSync.h"
#include "pqFixedDimensionControl.h"

class pqPXDMFSync::pqInternal : public Ui::pqPXDMFSync{
     
public:
    ~pqInternal() { 
      
    };
    pqInternal() { 
      insideupdate = false;
    }
    std::vector<pqFixedDimensionControl*> pxdmfDimItems;
    bool insideupdate;
};

//
pqPXDMFSync::~pqPXDMFSync()
{
    delete this->Internal;
}

//
pqPXDMFSync::pqPXDMFSync(QWidget* parent, Qt::WindowFlags flags) : QDockWidget(parent, flags) {
    this->Internal = new pqInternal;
    QWidget* widget = new QWidget(this);
    this->Internal->setupUi(widget);
    this->setWidget(widget);
    this->setWindowTitle("PXDMF Sync");
    
    this->connect(this->Internal->addPushButton, SIGNAL(clicked()), SLOT(updateSliders()));

    pqServerManagerModel* smmodel =  pqApplicationCore::instance()->getServerManagerModel();
    
    QObject::connect(smmodel, SIGNAL(sourceAdded(pqPipelineSource*)),
    this, SLOT(ConnectSource(pqPipelineSource*)));
    QObject::connect(smmodel, SIGNAL(sourceRemoved(pqPipelineSource*)),
    this, SLOT(updateSliders()));
}

//
void pqPXDMFSync::ConnectSource(pqPipelineSource* src){
   
    vtkSMProxy* theProxy = src->getProxy();
    if(theProxy->GetVTKClassName() && ( strcmp("vtkPXDMFReader", theProxy->GetVTKClassName()) ==0 ||  strcmp("vtkReconstruction", theProxy->GetVTKClassName()) ==0 ) ){
      //pqSpaceTimeSelector*)
      QObject::connect( src, SIGNAL(dataUpdated(pqPipelineSource*)), this, SLOT(updateSliders(pqPipelineSource*)));      
    }
}
//
void pqPXDMFSync::OnInteraction() {
  this->Internal->insideupdate = true;
}
//
void pqPXDMFSync::OffInteraction( ) {
  this->Internal->insideupdate = false;
}
//
void pqPXDMFSync::updateSliders(pqPipelineSource* s/*==0*/ ) {
    if (this->Internal->SyncState->checkState() == Qt::Unchecked) return;
    if (this->Internal->insideupdate) return;
    
    std::vector<bool> pxdmfDimItemsReuseFlag;
    pxdmfDimItemsReuseFlag.resize(this->Internal->pxdmfDimItems.size());
    for(int i =0; i < pxdmfDimItemsReuseFlag.size(); ++i){
      pxdmfDimItemsReuseFlag[i] = false;
    }
    
    
    pqServerManagerModel* smmodel = pqApplicationCore::instance()->getServerManagerModel();
    QList<pqPipelineSource*> sources = smmodel->findItems<pqPipelineSource*>();
    
    // we recover the values for the filters
    foreach(pqPipelineSource* src, sources) {
      
          //if only one filter change the value recover the values for that filter only
          if (s != src && s ) continue; 
          
          vtkSMProxy* theProxy = src->getProxy();
            if(theProxy->GetVTKClassName() && ( strcmp("vtkPXDMFReader", theProxy->GetVTKClassName()) ==0 ||  strcmp("vtkReconstruction", theProxy->GetVTKClassName()) ==0 ) ){
              
               vtkSMIntVectorProperty *NumberOfPXDMF_Dims =  vtkSMIntVectorProperty::SafeDownCast(theProxy->GetProperty("PXDMF_Dims"));
               unsigned NumberOfPXDMFDims = NumberOfPXDMF_Dims->GetElement(0);

               vtkSMIntVectorProperty *gridDimsDims =  vtkSMIntVectorProperty::SafeDownCast(theProxy->GetProperty("PXDMFDimsDataInfo"));
               vtkSMStringVectorProperty *gridDimsNames = vtkSMStringVectorProperty::SafeDownCast(theProxy->GetProperty("PXDMFDimsNameDataInfo"));
               vtkSMDoubleVectorProperty *PXDMFDimsPosDataInfo =  vtkSMDoubleVectorProperty::SafeDownCast(theProxy->GetProperty("PXDMFDimsPosDataInfo"));
               vtkSMDoubleVectorProperty *gridDimsMins =  vtkSMDoubleVectorProperty::SafeDownCast(theProxy->GetProperty("PXDMFDimsMinRangeDataInfo"));
               vtkSMDoubleVectorProperty *gridDimsMaxs =  vtkSMDoubleVectorProperty::SafeDownCast(theProxy->GetProperty("PXDMFDimsMaxRangeDataInfo"));
               vtkSMStringVectorProperty *SepTimeStatus = vtkSMStringVectorProperty::SafeDownCast(theProxy->GetProperty("VisualizationTimeStatus"));
               vtkSMStringVectorProperty *SepSpaceStatus = vtkSMStringVectorProperty::SafeDownCast(theProxy->GetProperty("VisualizationSpaceStatus"));
               
              vtkSMIntVectorProperty *DoReconstruction =  vtkSMIntVectorProperty::SafeDownCast(theProxy->GetProperty("DoReconstruction"));
              if( DoReconstruction && DoReconstruction->GetElement(0)==0) continue;

              int cpt =0;
              for (unsigned i = 0; i < NumberOfPXDMFDims; ++i) {
                int NumberOfDimensionsPerPXDMFDim = gridDimsDims->GetElement(i);
                if (  strcmp ( SepTimeStatus->GetElement(2*i+1), "0" )==0  &&  strcmp ( SepSpaceStatus->GetElement(2*i+1), "0" )==0 ){
                  for(int j = 0; j < NumberOfDimensionsPerPXDMFDim; ++j) {
                   
                     bool found = false;
                     int cpt2 = 0;
                     foreach(pqFixedDimensionControl* item, this->Internal->pxdmfDimItems) {
                        QString label = item->getLabel();
                        //double val = item->getVal();
                        if (QString(gridDimsNames->GetElement(cpt)) == label ){
                          found = true;
                          pxdmfDimItemsReuseFlag[cpt2] = true;
                          item->setmin(std::min(gridDimsMins->GetElement(cpt),item->getmin()));
                          item->setmax(std::max(gridDimsMaxs->GetElement(cpt),item->getmax()));
                        }
                        ++cpt2;
                     }
                     if(found == 0){
                        pqFixedDimensionControl *out = new pqFixedDimensionControl(this);
                        out->PXDMFdim = 0;
                        out->dim = 0;
                        out->setlabel(QString(gridDimsNames->GetElement(cpt)));

                        out->setmin(gridDimsMins->GetElement(cpt));
                        out->setmax(gridDimsMaxs->GetElement(cpt));
                        out->setval(PXDMFDimsPosDataInfo->GetElement(cpt));
                        out->local_update();

                        this->Internal->pxdmfDimItems.push_back(out);
                        pxdmfDimItemsReuseFlag.push_back(true);
                        this->Internal->verticalLayout->addLayout(out);

                        QObject::connect(out->Slider, SIGNAL(sliderPressed()), this, SLOT(OnInteraction()));      
                        QObject::connect(out->Slider, SIGNAL(sliderReleased()), this, SLOT(OffInteraction()));      
    
                    }
                  cpt++;
                }
              } else {
                for(int j = 0; j < NumberOfDimensionsPerPXDMFDim; ++j) {cpt++;}
              }
              }
              theProxy->UpdateVTKObjects();

            }
    }
    if(!s){
      for(int i = pxdmfDimItemsReuseFlag.size()-1; i >=0 ; --i){
        if(!pxdmfDimItemsReuseFlag[i]){
          //std::cout << "deleting " << this->Internal->pxdmfDimItems[i]->getLabel().data()->toLatin1() << std::cout ;
          delete this->Internal->pxdmfDimItems[i];
          this->Internal->pxdmfDimItems.erase(this->Internal->pxdmfDimItems.begin()+i);
        }
      }
    }
    
    if (s) FixedPXDMFDimsChangedSlot();
   
};

void pqPXDMFSync::FixedPXDMFDimsChangedSlot() {
    if (this->Internal->SyncState->checkState() == Qt::Unchecked) return;
    
    pqServerManagerModel* smmodel = pqApplicationCore::instance()->getServerManagerModel();
    QList<pqPipelineSource*> sources = smmodel->findItems<pqPipelineSource*>();

    foreach(pqFixedDimensionControl* item, this->Internal->pxdmfDimItems) {
        QString label = item->getLabel();
        double val = item->getVal();
        foreach(pqPipelineSource* src, sources) {
          vtkSMProxy* theProxy = src->getProxy();
            if(theProxy->GetVTKClassName() && ( strcmp("vtkPXDMFReader", theProxy->GetVTKClassName()) ==0 ||  strcmp("vtkReconstruction", theProxy->GetVTKClassName()) ==0 ) ){

              vtkSMIntVectorProperty *DoReconstruction =  vtkSMIntVectorProperty::SafeDownCast(theProxy->GetProperty("DoReconstruction"));
              if( DoReconstruction && DoReconstruction->GetElement(0)==0) continue;
              
               vtkSMIntVectorProperty *NumberOfPXDMF_Dims =  vtkSMIntVectorProperty::SafeDownCast(theProxy->GetProperty("PXDMF_Dims"));
               unsigned NumberOfPXDMFDims = NumberOfPXDMF_Dims->GetElement(0);

               vtkSMIntVectorProperty *gridDimsDims =  vtkSMIntVectorProperty::SafeDownCast(theProxy->GetProperty("PXDMFDimsDataInfo"));
               vtkSMStringVectorProperty *gridDimsNames = vtkSMStringVectorProperty::SafeDownCast(theProxy->GetProperty("PXDMFDimsNameDataInfo"));
               vtkSMDoubleVectorProperty *PXDMFDimsPosDataInfo =  vtkSMDoubleVectorProperty::SafeDownCast(theProxy->GetProperty("PXDMFDimsPosDataInfo"));

              int cpt =0;
              for (unsigned i = 0; i < NumberOfPXDMFDims; ++i) {
                  int NumberOfDimensionsPerPXDMFDim = gridDimsDims->GetElement(i);
                  for(int j = 0; j < NumberOfDimensionsPerPXDMFDim; ++j) {
                    if (QString(gridDimsNames->GetElement(cpt)) == label ){
                      PXDMFDimsPosDataInfo->SetElement(cpt, double(val) );
                    }
                  cpt++;
                }
              }
              theProxy->UpdateVTKObjects();

            }
        }
    }
    emit pqApplicationCore::instance()->render();
};