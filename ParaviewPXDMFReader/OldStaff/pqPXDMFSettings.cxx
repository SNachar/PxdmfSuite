/*=========================================================================

  Program:   PXDMFReader Plugin
  Module:    pqPXDMFSetting.cxx

  Copyright (c) GeM, Ecole Centrale Nantes.
  All rights reserved.
  Copyright: See COPYING file that comes with this distribution


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "pqPXDMFSettings.h"

// Qt includes
#include "QShortcut"
#include "QLineEdit"


// 
// ParaView Includes.
#include "pqSettings.h"
#include "pqApplicationCore.h"
// 


pqPXDMFSettings::pqPXDMFSettings( QWidget* p){
  setupUi(this);
  
  pqSettings *settings = pqApplicationCore::instance()->settings();
  settings->beginGroup("PXDMFReader");
  
  this->findChild<QSpinBox*>("MaxUIModes")->setValue(settings->value("GUIMaxNbModes",100).toInt());
  if(settings->value("Show",true).toBool()){
    this->findChild<QCheckBox*>("DisableSplash")->setCheckState(Qt::Unchecked);
  } else {
    this->findChild<QCheckBox*>("DisableSplash")->setCheckState(Qt::Checked);
  }
  this->findChild<QLineEdit*>("gmshpath")->setVisible(false);
  this->findChild<QLabel*>("labelgmshpath")->setVisible(false);
  this->findChild<QLineEdit*>("gmshpath")->setText(settings->value("GmshPath","/usr/bin/gmsh").toString());

  settings->endGroup();
  
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
  QShortcut *splash= new QShortcut(tr("Ctrl+S"), this);
  QObject::connect(splash, SIGNAL(activated()), this, SLOT(enableDisableSplashScreen()));
};
//
void pqPXDMFSettings::accept(){
  pqSettings *settings = pqApplicationCore::instance()->settings();
  settings->beginGroup("PXDMFReader");
  settings->setValue("Show", this->findChild<QCheckBox*>("DisableSplash")->checkState() == Qt::Unchecked);
  //bool ok;
  int nmodes = this->findChild<QSpinBox*>("MaxUIModes")->value();
  if ( nmodes > 1){
    settings->setValue("GUIMaxNbModes", nmodes );  
  }
  
  settings->setValue("GmshPath",this->findChild<QLineEdit*>("gmshpath")->text());
  
  settings->endGroup();
  this->close();
};
//
void pqPXDMFSettings::reject(){
  this->close();
};
//
void pqPXDMFSettings::enableDisableSplashScreen(){
  this->findChild<QCheckBox*>("DisableSplash")->setEnabled(true);
  this->findChild<QLineEdit*>("gmshpath")->setVisible(true);
  this->findChild<QLabel*>("labelgmshpath")->setVisible(true);
}