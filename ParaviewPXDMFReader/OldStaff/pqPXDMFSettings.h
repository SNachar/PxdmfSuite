/*=========================================================================

  Program:   PXDMFReader Plugin
  Module:    pqPXDMFSetting.h

  Copyright (c) GeM, Ecole Centrale Nantes.
  All rights reserved.
  Copyright: See COPYING file that comes with this distribution


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME pqPXDMFSettings 
// .SECTION Description

// Paraview Includes.
#include "pqNamedObjectPanel.h"

//interface Includes
#include "ui_pqPXDMFSettings.h"

class pqPXDMFSettings : public QWidget, public Ui::PXDMFSettings {
   Q_OBJECT
public:
  /// constructor
  pqPXDMFSettings( QWidget* p = NULL);
public slots:
  void accept();
  void reject();
  void enableDisableSplashScreen();
};