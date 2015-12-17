/*=========================================================================

  Program:   PXDMFReader Plugin
  Module:    pqSpaceTimeSelector.cpp

  Copyright (c) GeM, Ecole Centrale Nantes.
  All rights reserved.
  Copyright: See COPYING file that comes with this distribution


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

// Qt Includes.
#include <QTreeWidget>
#include <QHeaderView>
#include <QApplication>
#include <QGroupBox>
#include <QGridLayout>
#include <qcheckbox.h>

//VTK Includes
#include <vtkSMIntVectorProperty.h>
#include <vtkSMStringVectorProperty.h>
#include <vtkSMDoubleVectorProperty.h>
#include "vtkSMSourceProxy.h"

//Paraview Includes
#include "pqPropertiesPanel.h"
#include "pqApplicationCore.h"
#include "pqSignalAdaptorSelectionTreeWidget.h"
#include "pqActiveObjects.h"

// Pluing Includes
#include "pqSpaceTimeSelector.h"
#include "pqFixedDimensionControl.h"
#include "QWidgetDetach.h"
#include "ui_pqSpaceTimeSelector.h"

class pqSpaceTimeSelector::pqInternals {
    public:
        QPointer<QWidget> DetachWindow;
    };

class pqSpaceTimeSelector::pqUI: public Ui::SpaceTimeSelector {

    };

pqSpaceTimeSelector::pqSpaceTimeSelector ( vtkSMProxy *smproxy, vtkSMProperty *smproperty, QWidget *parentObject )
    : Superclass ( smproxy, parentObject ) {

    ininit = true;

    this->UI = new pqUI();
    this->UI->setupUi ( this );
    this->setShowLabel ( false );
    this->UI->verticalLayout->setMargin ( pqPropertiesPanel::suggestedMargin() );

    this->Internals = new pqInternals();

    this->addPropertyLink ( this->UI->VisualizationSpaceStatus, "values", SIGNAL ( itemChanged ( QTreeWidgetItem * , int ) ), smproperty );
    this->addPropertyLink ( this->UI->VisualizationTimeStatus, "values", SIGNAL ( itemChanged ( QTreeWidgetItem * , int ) ), proxy()->GetProperty ( "VisualizationTimeStatus" ) );

    this->setChangeAvailableAsChangeFinished ( true );



    new pqSignalAdaptorSelectionTreeWidget ( this->UI->VisualizationSpaceStatus,  proxy()->GetProperty ( "VisualizationSpaceStatus" ) )       ;
    new pqSignalAdaptorSelectionTreeWidget ( this->UI->VisualizationTimeStatus,  proxy()->GetProperty ( "VisualizationTimeStatus" ) )       ;

    proxy()->UpdatePropertyInformation();

    QObject::connect ( this->UI->VisualizationTimeStatus,
                       SIGNAL ( itemChanged ( QTreeWidgetItem*,int ) ), this, SLOT ( TimeDimsChanged ( QTreeWidgetItem *, int ) ),
                       Qt::QueuedConnection );

    QObject::connect ( this->UI->VisualizationSpaceStatus,
                       SIGNAL ( itemChanged ( QTreeWidgetItem *, int ) ), this, SLOT ( SpaceDimsChanged ( QTreeWidgetItem *, int ) ),
                       Qt::QueuedConnection );

    vtkSMIntVectorProperty *NumberOfPXDMF_Dims =  vtkSMIntVectorProperty::SafeDownCast ( proxy()->GetProperty ( "PXDMF_Dims" ) );

    if ( NumberOfPXDMF_Dims ) {
        this->NumberOfPXDMFDims = NumberOfPXDMF_Dims->GetElement ( 0 );
        vtkSMIntVectorProperty *gridDimsDims =  vtkSMIntVectorProperty::SafeDownCast ( proxy()->GetProperty ( "PXDMFDimsDataInfo" ) );

        if ( gridDimsDims ) {

            this->NumberOfDimensionsPerPXDMFDim.resize ( NumberOfPXDMFDims );
            this->NamesOfEachPXDMFDimension.resize ( NumberOfPXDMFDims );
            this->UnitsOfEachPXDMFDimension.resize ( NumberOfPXDMFDims );
            this->MinsOfEachPXDMFDimension.resize ( NumberOfPXDMFDims );
            this->MaxsOfEachPXDMFDimension.resize ( NumberOfPXDMFDims );
            for ( unsigned i =0; int ( i ) < NumberOfPXDMFDims; ++i ) {
                this->NumberOfDimensionsPerPXDMFDim[i] = gridDimsDims->GetElement ( i );
                this->NamesOfEachPXDMFDimension[i].resize ( this->NumberOfDimensionsPerPXDMFDim[i] );
                this->UnitsOfEachPXDMFDimension[i].resize ( this->NumberOfDimensionsPerPXDMFDim[i] );
                this->MinsOfEachPXDMFDimension[i].resize ( this->NumberOfDimensionsPerPXDMFDim[i] );
                this->MaxsOfEachPXDMFDimension[i].resize ( this->NumberOfDimensionsPerPXDMFDim[i] );
                }
            vtkSMStringVectorProperty *gridDimsNames = vtkSMStringVectorProperty::SafeDownCast ( proxy()->GetProperty ( "PXDMFDimsNameDataInfo" ) );
            vtkSMStringVectorProperty *gridDimsUnits = vtkSMStringVectorProperty::SafeDownCast ( proxy()->GetProperty ( "PXDMFDimsUnitDataInfo" ) );
            vtkSMDoubleVectorProperty *gridDimsMins =  vtkSMDoubleVectorProperty::SafeDownCast ( proxy()->GetProperty ( "PXDMFDimsMinRangeDataInfo" ) );
            if ( !gridDimsNames || !gridDimsUnits || !gridDimsMins ) {
                std::cout << "error gridDimsNames or gridDimsUnits or gridDimsMins " << std::endl;
                exit ( 0 );
                }
            vtkSMDoubleVectorProperty *gridDimsMaxs =  vtkSMDoubleVectorProperty::SafeDownCast ( proxy()->GetProperty ( "PXDMFDimsMaxRangeDataInfo" ) );
            if ( gridDimsMaxs == 0 ) {
                std::cout << "error gridDimsMaxs "  << std::endl;
                exit ( 0 );
                }
            if ( gridDimsNames ) {
                int cpt = 0;
                for ( unsigned i =0; int ( i ) < NumberOfPXDMFDims; ++i ) {


                    for ( unsigned j =0; int ( j ) < this->NumberOfDimensionsPerPXDMFDim[i]; ++j ) {
                        this->NamesOfEachPXDMFDimension[i][j] = gridDimsNames->GetElement ( cpt );
                        this->UnitsOfEachPXDMFDimension[i][j] = gridDimsUnits->GetElement ( cpt );
                        this->MinsOfEachPXDMFDimension[i][j] = gridDimsMins->GetElement ( cpt );
                        this->MaxsOfEachPXDMFDimension[i][j] = gridDimsMaxs->GetElement ( cpt );
                        ++cpt;
                        }
                    }
                }

            }
        }
    else {
        std::cout << " Property 'PXDMF_Dims' does not exist"  << std::endl;
        exit ( 1 );
        }


    int time_nb = this->UI->VisualizationTimeStatus->topLevelItemCount();
    bool time_checked=0;
    for ( unsigned i = 0; int ( i ) < time_nb ; ++i ) {
        if ( this->UI->VisualizationTimeStatus->topLevelItem ( i )->checkState ( 0 ) == Qt::Checked ) {
            time_checked = true;
            }
        }
    if ( time_checked ) {
        for ( unsigned i = 0; int ( i ) < time_nb ; ++i ) {
            if ( this->UI->VisualizationTimeStatus->topLevelItem ( i )->checkState ( 0 ) == Qt::Unchecked ) {
                if ( !this->UI->VisualizationTimeStatus->topLevelItem ( i )->isDisabled() ) this->UI->VisualizationTimeStatus->topLevelItem ( i )->setDisabled ( true );
                }
            }
        }


    FixedDimensionsBox = new QGroupBox ( tr ( "Fixed Dimensions" ) );
    FixedDimensionsBox->setCheckable ( true );

    fixlayout = new QVBoxLayout;
    fixlayout->setMargin ( 0 );
    FixedDimensionsBox->setLayout ( fixlayout );

    this->UI->verticalLayout->addWidget ( FixedDimensionsBox );

    UpdateExtraDims();
    ininit = false;

    //QObject::connect(this->findChild<QPushButton*>("Detach"),  SIGNAL(clicked()), this, SLOT(toggleDetach()),  Qt::QueuedConnection);
    QObject::connect ( FixedDimensionsBox,
                       SIGNAL ( clicked() ),  SLOT ( toggleDetach() ),  Qt::QueuedConnection );


    this->UI->VisualizationSpaceStatus->setMaximumHeight ( 32*this->NumberOfPXDMFDims );
    this->UI->VisualizationTimeStatus->setMaximumHeight ( 32*this->NumberOfPXDMFDims );


    pqPipelineSource* source = pqActiveObjects::instance().activeSource();

    QObject::connect ( source, SIGNAL ( dataUpdated ( pqPipelineSource* ) ),  this, SLOT ( FixedPXDMFDimsChangedFromServerSlot() ),  Qt::QueuedConnection );
   
    
    }

pqSpaceTimeSelector::~pqSpaceTimeSelector() {
    delete this->UI;
    delete this->Internals;

    };
//
//
void pqSpaceTimeSelector::toggleDetach() {
    if ( this->Internals->DetachWindow ) {
        this->FixedDimensionsBox->setChecked ( true );
        this->Internals->DetachWindow->layout()->removeWidget ( this->FixedDimensionsBox );
        this->UI->verticalLayout->addWidget ( FixedDimensionsBox );
        delete this->Internals->DetachWindow;
        this->Internals->DetachWindow = NULL;
        }
    else {
        this->FixedDimensionsBox->setChecked ( true );
        QWidgetDetach* DetachWindow = new QWidgetDetach ( this, Qt::Window | Qt::WindowStaysOnTopHint );
        //QWidget* DetachWindow = new QWidget(this, Qt::Widget);

        vtkSMStringVectorProperty *filename = vtkSMStringVectorProperty::SafeDownCast ( proxy()->GetProperty ( "FileName" ) );
        if ( filename ) {
            DetachWindow->setWindowTitle ( filename->GetElement ( 0 ) );
            }

        this->Internals->DetachWindow = DetachWindow;
        DetachWindow->setObjectName ( "Fixed Dimentions" );
        this->layout()->removeWidget ( this->FixedDimensionsBox );

        QVBoxLayout* vbox = new QVBoxLayout ( DetachWindow );
        vbox->setSpacing ( 0 );
        vbox->setMargin ( 0 );

        vbox->addWidget ( this->FixedDimensionsBox );
        DetachWindow->show();
        QObject::connect ( DetachWindow, SIGNAL ( close () ), this, SLOT ( toggleDetach() ) );
        }
    }
//
void pqSpaceTimeSelector::TimeDimsChanged ( QTreeWidgetItem *item, int ) {

    QTreeWidget* time = this->UI->VisualizationTimeStatus;
    int time_nb = time->topLevelItemCount();
    int num = time->indexOfTopLevelItem ( item );
    QTreeWidget* space = this->UI->VisualizationSpaceStatus;
    
    if ( item->isDisabled() ) return;
    ///verification that a time dimension must be a one dimensional dimension
    if ( NumberOfDimensionsPerPXDMFDim[num] != 1 ) {
        item->setCheckState ( 0,Qt::Unchecked );
        return;
        }
    ///verification that cant check a space checked dimension
    if ( space->topLevelItem ( num )->checkState ( 0 ) == Qt::Checked ) {
        item->setCheckState ( 0,Qt::Unchecked );
        return;
        }

    if ( item->checkState ( 0 ) == Qt::Checked ) {
        for ( unsigned i = 0; int ( i ) < time_nb ; ++i ) {
            if ( time->topLevelItem ( i )->checkState ( 0 ) == Qt::Unchecked )
                if ( !time->topLevelItem ( i )->isDisabled() )
                    time->topLevelItem ( i )->setDisabled ( true );
            }
        }
    else {
        for ( unsigned i = 0; int ( i ) < time_nb ; ++i ) {
            if ( time->topLevelItem ( i )->isDisabled() ) time->topLevelItem ( i )->setFlags ( Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsUserCheckable |Qt::ItemIsSelectable );
            }
        }
    this->changeAvailable();
    UpdateExtraDims();
    emit SpaceTimeDimsChanded();

    }
//
QCheckBox* getDoReconstructionWidget(pqSpaceTimeSelector* This){
  QList<QCheckBox*> list = This->parentWidget()->findChildren<QCheckBox*>();
  for(int i =0; i < list.count(); i++){
    if(strcmp(list.at(i)->text().toStdString().c_str(),"Do Reconstruction" )==0){
      return list.at(i);
    }
  }
  return NULL;
}
//
bool DoReconstructionOn(pqSpaceTimeSelector* This){
  QCheckBox* dor = getDoReconstructionWidget(This);
  if(dor)
    return dor->isChecked();
  else
    return true;
}
//
void pqSpaceTimeSelector::CleanVisualizationSpaceStatus(){ 
  QTreeWidget* space = this->UI->VisualizationSpaceStatus;
 
  if ( DoReconstructionOn(this) ) {
    int cpt = 0;
    for ( unsigned i = 0; int ( i ) < NumberOfPXDMFDims; ++i ) {
      if ( space->topLevelItem ( i )->checkState ( 0 ) == Qt::Checked  ) {
        cpt += NumberOfDimensionsPerPXDMFDim[i];
      };
      if(cpt > 3){
        space->topLevelItem ( i )->setCheckState ( 0, Qt::Unchecked );
      }
    }
    this->FixedDimensionsBox->setEnabled(true);
    this->UI->tab_22->setEnabled(true);
    UpdateExtraDims();
  } else{
          this->FixedDimensionsBox->setEnabled(false);
          this->UI->tab_22->setEnabled(false);
    }
}
//
void pqSpaceTimeSelector::apply() {

    pqPropertyWidget::apply();

    /// We push the dimension for space
    QTreeWidget* space = this->UI->VisualizationSpaceStatus;
    vtkSMStringVectorProperty *VisualizationSpaceStatus =  vtkSMStringVectorProperty::SafeDownCast ( proxy()->GetProperty ( "VisualizationSpaceStatus" ) );
    for ( int i =0; i<this->NumberOfPXDMFDims ; i++ ) {
        std::string val = VisualizationSpaceStatus->GetElement ( i*2+1 );
        std::string state = "0";
        if ( space->topLevelItem ( i )->checkState ( 0 ) ) state = "1";
        if ( strcmp ( state.c_str(),val.c_str() ) != 0 ) {
            VisualizationSpaceStatus->SetElement ( i*2+1,state.c_str() );
            VisualizationSpaceStatus->Modified();
            }
        }

    /// We push the dimension for time
    QTreeWidget* time = this->UI->VisualizationTimeStatus;
    vtkSMStringVectorProperty *VisualizationTimeStatus =  vtkSMStringVectorProperty::SafeDownCast ( proxy()->GetProperty ( "VisualizationTimeStatus" ) );
    for ( int i =0; i<this->NumberOfPXDMFDims ; i++ ) {
        std::string val = VisualizationTimeStatus->GetElement ( i*2+1 );
        std::string state = "0";
        if ( time->topLevelItem ( i )->checkState ( 0 ) ) state = "1";
        if ( strcmp ( state.c_str(),val.c_str() ) != 0 ) {
            VisualizationTimeStatus->SetElement ( i*2+1,state.c_str() );
            VisualizationTimeStatus->Modified();
            }
        }
    };

void pqSpaceTimeSelector::SpaceDimsChanged ( QTreeWidgetItem* item, int ) {

  QCheckBox* dor = getDoReconstructionWidget(this);

  if(dor)
    connect(dor,SIGNAL(clicked()),this,SLOT (CleanVisualizationSpaceStatus()));  
  
  
  QTreeWidget* time = this->UI->VisualizationTimeStatus;
  QTreeWidget* space = this->UI->VisualizationSpaceStatus;

  unsigned num = space->indexOfTopLevelItem ( item );
  if ( item->isDisabled() ) return;
  
  if ( DoReconstructionOn(this) ) {
    
    int cpt = 0;
    for ( unsigned i = 0; int ( i ) < NumberOfPXDMFDims; ++i ) {
      if ( space->topLevelItem ( i )->checkState ( 0 ) == Qt::Checked && i != num ) {
        cpt += NumberOfDimensionsPerPXDMFDim[i];
      };
      if(cpt > 3){
        space->topLevelItem ( i )->setCheckState ( 0, Qt::Unchecked );
      }
    }
  
  
    if ( NumberOfDimensionsPerPXDMFDim[num] + cpt > 3 ) {
        item->setCheckState ( 0, Qt::Unchecked );
      } else {
        if ( time->topLevelItem ( num )->checkState ( 0 ) == Qt::Checked ) {
            item->setCheckState ( 0, Qt::Unchecked );
          }
        else {
            this->changeAvailable();
            UpdateExtraDims();
            emit SpaceTimeDimsChanded();
          }
      }
  }
}
//
//
void pqSpaceTimeSelector::DeleteFixedDimension ( unsigned gridDimToDelete, unsigned dimToDelete ) {
    std::vector<pqFixedDimensionControl*>::iterator it = FixedDimensions.begin();

    while ( it !=  FixedDimensions.end() ) {
        if ( ( *it )->PXDMFdim == gridDimToDelete && ( *it )->dim == dimToDelete ) {
            this->UI->verticalLayout->removeItem ( *it );
            delete *it;
            it = FixedDimensions.erase ( it );
            }
        else {
            ++it;
            }
        }
    if ( FixedDimensions.size()  == 0 )  FixedDimensionsBox->setVisible ( false );
    };
//

//
void pqSpaceTimeSelector::FixedPXDMFDimsChangedSlot() {
    FixedPXDMFDimsChanged ( 0 );
    }
//
void pqSpaceTimeSelector::FixedPXDMFDimsChanged ( bool fromserver ) {
    proxy()->UpdatePropertyInformation();
    vtkSMDoubleVectorProperty *PXDMFDimsPosDataInfo =  vtkSMDoubleVectorProperty::SafeDownCast ( proxy()->GetProperty ( "PXDMFDimsPosDataInfo" ) );
    if ( !PXDMFDimsPosDataInfo ) {
        std::cout << "error PXDMFDimsPosDataInfo " << std::endl;
        exit ( 0 );
        }
    vtkSMDoubleVectorProperty *FixedDimensionsUsed =  vtkSMDoubleVectorProperty::SafeDownCast ( proxy()->GetProperty ( "FixedDimensionsUsed" ) );
    if ( !FixedDimensionsUsed ) {
        std::cout << "error FixedDimensionsUsed " << std::endl;
        exit ( 0 );
        }

    if ( fromserver ) {
        if ( FixedDimensionsUsed == 0 ) {
            std::cerr << "Error getting FixedDimensionsUsed  " << std::endl;
            return;
            }
        if ( FixedDimensionsUsed->GetNumberOfElements() == 0 ) {
            std::cerr << "Error FixedDimensionsUsed->GetNumberOfElements()  " << FixedDimensionsUsed->GetNumberOfElements() << std::endl;
            return;
            }
        }

    int cpt = 0;
    for ( unsigned i =0; int ( i ) < this->NumberOfPXDMFDims; ++i ) {
        for ( unsigned j =0; int ( j ) < this->NumberOfDimensionsPerPXDMFDim[i]; ++j ) {
            for ( unsigned k=0; k < this->FixedDimensions.size(); ++k ) {
                if ( i == this->FixedDimensions[k]->PXDMFdim && j == this->FixedDimensions[k]->dim ) {
                    if ( fromserver ) {
                        this->FixedDimensions[k]->setvalWithoutpush ( FixedDimensionsUsed->GetElement ( cpt ) );
                        }
                    else {
                        PXDMFDimsPosDataInfo->SetElement ( cpt,this->FixedDimensions[k]->getval() ) ;
                        }

                    }
                }
            ++cpt;
            }
        }
    if ( fromserver ) return;
    if ( !ininit ) {
        vtkSMIntVectorProperty *ContinuousUpdate =  vtkSMIntVectorProperty::SafeDownCast ( proxy()->GetProperty ( "ContinuousUpdate" ) );

        if ( ContinuousUpdate && ContinuousUpdate->GetElement ( 0 ) ) {

            proxy()->UpdateVTKObjects();

            emit pqApplicationCore::instance()->render();
            //
            }
        }
    };
//
pqFixedDimensionControl* pqSpaceTimeSelector::AddFixedDimension ( const unsigned grid, const unsigned dim,const QString &text  ,const QString &unit,const double &min,const  double &max, const int &internalpos ) {

    for ( unsigned i = 0; i < FixedDimensions.size(); ++i ) {
        if ( FixedDimensions[i]->dim ==dim && FixedDimensions[i]->PXDMFdim == grid ) return NULL;
        }
    pqFixedDimensionControl *out = new pqFixedDimensionControl ( this );
    out->PXDMFdim = grid;
    out->dim = dim;
    out->setlabel ( text );
    out->setUnitLabel ( unit );
    out->setmin ( min );
    out->setmax ( max );
    out->setval ( ( min+max ) /2 );
    out->local_update();

    this->addPropertyLink ( out, "val", SIGNAL ( valChanged ( double ) ), this->proxy()->GetProperty ( "PXDMFDimsPosDataInfo" ),internalpos );
    FixedDimensions.push_back ( out );

    return out;

    }
void pqSpaceTimeSelector::FixedPXDMFDimsChangedFromServerSlot() {
    FixedPXDMFDimsChanged ( 1 );
    }
//
void pqSpaceTimeSelector::UpdateExtraDims() {
  
    int pos = 0;

    pos = this->UI->verticalLayout->count();

    proxy()->UpdatePropertyInformation();
    vtkSMDoubleVectorProperty *PXDMFDimsPosDataInfo =  vtkSMDoubleVectorProperty::SafeDownCast ( proxy()->GetProperty ( "PXDMFDimsPosDataInfo" ) );

    if ( PXDMFDimsPosDataInfo ) {
        bool flag = false; // false if nothing changed;
        unsigned cpt = 0;
        for ( unsigned i = 0; int ( i ) < NumberOfPXDMFDims ; i++ ) {
            for ( unsigned j = 0; int ( j ) < NumberOfDimensionsPerPXDMFDim[i]; ++j ) {

                if ( this->UI->VisualizationTimeStatus->topLevelItemCount() > ( int ) i && this->UI->VisualizationSpaceStatus->topLevelItemCount() > ( int ) i && this->UI->VisualizationTimeStatus->topLevelItem ( i )->checkState ( 0 ) == Qt::Unchecked  &&  this->UI->VisualizationSpaceStatus->topLevelItem ( i )->checkState ( 0 ) == Qt::Unchecked ) {
                    pqFixedDimensionControl* extradim = AddFixedDimension ( i, j, NamesOfEachPXDMFDimension[i][j].c_str() ,UnitsOfEachPXDMFDimension[i][j].c_str() ,MinsOfEachPXDMFDimension[i][j] , MaxsOfEachPXDMFDimension[i][j], cpt );
                    flag = true;
                    if ( extradim ) {
                        if ( PXDMFDimsPosDataInfo->GetNumberOfElements() > cpt )
                            extradim->setvalWithoutpush ( PXDMFDimsPosDataInfo->GetElement ( cpt ) );
                        fixlayout->addLayout ( extradim );
                        FixedDimensionsBox->setVisible ( true );
                        }
                    ++pos;
                    }
                else {
                    DeleteFixedDimension ( i,j );
                    flag = true;
                    }
                cpt++;
                }
            }
        if ( flag ) {
            FixedPXDMFDimsChanged();
            }
        }
    else {
        std::cout << "error PXDMFDimsPosDataInfo " << std::endl;
        exit ( 0 );
        }
    };
