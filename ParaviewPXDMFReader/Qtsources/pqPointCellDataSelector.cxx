/*=========================================================================

  Program:   PXDMFReader Plugin
  Module:    pqPointCellDataSelector.cpp

  Copyright (c) GeM, Ecole Centrale Nantes.
  All rights reserved.
  Copyright: See COPYING file that comes with this distribution


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// std includes
#include "iostream"

//VTK Includes
#include "vtkSMSourceProxy.h"
#include "vtkSMStringVectorProperty.h"
#include "vtkSMArrayListDomain.h"
#include "vtkSMArraySelectionDomain.h"

//Paraview Includes
#include "pqPropertiesPanel.h"
#include "pqSignalAdaptorSelectionTreeWidget.h"
#include "pqSettings.h"
#include "pqApplicationCore.h"
#include "vtkSMIntVectorProperty.h"

// Plugin Includes
#include "pqPointCellDataSelector.h"
#include "ui_pqPointCellDataSelector.h"

class pqPointCellDataSelector::pqUI: public Ui::PointCellDataSelector {

};
//
pqPropertyLinks& pqPointCellDataSelector::Plinks(){
  return this->links();
}
//
pqPointCellDataSelector::pqPointCellDataSelector(vtkSMProxy *smproxy, vtkSMProperty *smproperty, QWidget *parentObject)
    : Superclass(smproxy, parentObject)
{
    this->UI = new pqUI();
    this->UI->setupUi(this);
    this->setShowLabel(false);
    this->UI->verticalLayout_2->setMargin(pqPropertiesPanel::suggestedMargin());

    this->addPropertyLink(this->UI->PointArrayStatus, "values", SIGNAL(itemClicked(QTreeWidgetItem * , int  )), smproperty);
    this->addPropertyLink(this->UI->CellArrayStatus, "values", SIGNAL(itemClicked(QTreeWidgetItem * , int  )), proxy()->GetProperty("CellArrayStatus"));
    
    
    this->setChangeAvailableAsChangeFinished(true);
    
    new pqSignalAdaptorSelectionTreeWidget(this->UI->PointArrayStatus,  proxy()->GetProperty("PointArrayStatus"));
    new pqSignalAdaptorSelectionTreeWidget(this->UI->CellArrayStatus,  proxy()->GetProperty("CellArrayStatus"));
     
    proxy()->UpdatePropertyInformation();

     QObject::connect(this->UI->PointArrayStatus,
                      SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(PointDataChanged(QTreeWidgetItem *, int)),
                      Qt::QueuedConnection);
 
     QObject::connect(this->UI->CellArrayStatus,
                      SIGNAL(itemChanged( QTreeWidgetItem *, int)), this, SLOT(CellDataChanged(QTreeWidgetItem *, int)),
                      Qt::QueuedConnection);

}

//
pqPointCellDataSelector::~pqPointCellDataSelector() {
    delete this->UI;
};

//
void pqPointCellDataSelector::apply(){
  pqPropertyWidget::apply();
  
  /// we push the data
   QTreeWidget* point = this->UI->PointArrayStatus;
   vtkSMStringVectorProperty *PointArrayInfo =  vtkSMStringVectorProperty::SafeDownCast(proxy()->GetProperty("PointArrayStatus"));
   for(int i =0; i< point->topLevelItemCount();i++){
     std::string val = PointArrayInfo->GetElement(i*2+1);
     std::string state = "0";
     if(point->topLevelItem(i)->checkState(0)) state = "1";
     PointArrayInfo->SetElement(i*2+1,state.c_str());
     PointArrayInfo->Modified();   
   } 
   
   /// we push the data
   QTreeWidget* cell = this->UI->CellArrayStatus;
   vtkSMStringVectorProperty *CellArrayInfo =  vtkSMStringVectorProperty::SafeDownCast(proxy()->GetProperty("CellArrayStatus"));
   for(int i =0; i< cell->topLevelItemCount();i++){
     std::string val = CellArrayInfo->GetElement(i*2+1);
     std::string state = "0";
     if(cell->topLevelItem(i)->checkState(0)) state = "1";
     CellArrayInfo->SetElement(i*2+1,state.c_str());
     CellArrayInfo->Modified();   
   }   
   
  pqPropertyWidget::apply();
  /// we push the data

  
}

//
void pqPointCellDataSelector::PointDataChanged(QTreeWidgetItem *item, int) {
  this->changeAvailable();

};

//
void pqPointCellDataSelector::CellDataChanged(QTreeWidgetItem *item, int) {
  this->changeAvailable();
};
