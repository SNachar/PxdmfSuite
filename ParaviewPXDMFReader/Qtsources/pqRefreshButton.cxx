/*=========================================================================

  Program:   PXDMFReader Plugin
  Module:    pqRefresh.cpp

  Copyright (c) GeM, Ecole Centrale Nantes.
  All rights reserved.
  Copyright: See COPYING file that comes with this distribution


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

//VTK Includes
#include "vtkSMProxy.h"
#include "vtkSMProperty.h"
#include "vtkSMIntVectorProperty.h"

//Qt Includes
#include <QVBoxLayout>
#include <QPushButton>
#include <QTreeWidget>

//Paraview Includes
#include "pqPVApplicationCore.h"

// Pluing Includes
#include "pqRefreshButton.h"
#include "pqPointCellDataSelector.h"
pqRefreshButton::pqRefreshButton(vtkSMProxy *       smProxy,     vtkSMProperty *         proxyProperty,     QWidget *       pWidget  ) : pqPropertyWidget(smProxy, pWidget), Property(proxyProperty){
     
    QVBoxLayout *l = new QVBoxLayout;
    l->setSpacing(0);
    l->setMargin(0);
    QPushButton *button = new QPushButton(proxyProperty->GetXMLLabel());
    connect(button, SIGNAL(clicked()), this, SLOT(buttonClicked()));
    l->addWidget(button);

    this->setShowLabel(false);
    this->setLayout(l);

  PV_DEBUG_PANELS() << "pqRefreshButton for a property with "
                << "the panel_widget=\"RefreshButton\" attribute";
        };

pqRefreshButton::~pqRefreshButton()
{
}

void pqRefreshButton::buttonClicked()
{
  
    proxy()->UpdatePropertyInformation();
    vtkSMIntVectorProperty *refreshSM =  vtkSMIntVectorProperty::SafeDownCast(proxy()->GetProperty("Refresh"));

    
    if (refreshSM == 0) {
        std::cout << "Error in seting refresh" << std::endl;
        return;
    }

    refreshSM->SetElements1(1);
    proxy()->UpdateVTKObjects();


    refreshSM->SetElements1(0);
    proxy()->MarkAllPropertiesAsModified();
    refreshSM->Modified();
    proxy()->UpdateVTKObjects();

    
    proxy()->GetProperty("PointArrayInfo")->Modified();
    proxy()->GetProperty("CellArrayInfo")->Modified();
    
    proxy()->GetProperty("PointArrayStatus")->Modified();
    proxy()->GetProperty("CellArrayStatus")->Modified();
    
    proxy()->GetProperty("VisualizationSpaceStatus")->Modified();
    proxy()->GetProperty("VisualizationSpaceInfo")->Modified();
    
    proxy()->GetProperty("VisualizationTimeStatus")->Modified();
    proxy()->GetProperty("VisualizationTimeInfo")->Modified();
    
    proxy()->UpdatePropertyInformation();
    pqPointCellDataSelector* pointer = this->parentWidget()->findChild<pqPointCellDataSelector*>();
    if(pointer){
      //std::cout << "Widget found " << std::endl;
      QTreeWidget* ppointer = this->parentWidget()->findChild<QTreeWidget*>("PointArrayStatus");
      //ppointer->clear();
      proxy()->GetProperty("PointArrayStatus")->Modified();
      proxy()->GetProperty("CellArrayStatus")->Modified();
    }
    
    //emit pqApplicationCore::instance()->render();
}
