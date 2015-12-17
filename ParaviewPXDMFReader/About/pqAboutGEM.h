/*=========================================================================

  Program:   PXDMFReader Plugin
  Module:    pqAboutGEM.h

  Copyright (c) GeM, Ecole Centrale Nantes.
  All rights reserved.
  Copyright: See COPYING file that comes with this distribution


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vpqAboutGEM
// .SECTION Description

#ifndef __pqAboutGEM_h
#define __pqAboutGEM_h

// Qt Includes.
#include <QDialog>
#include "ui_AboutPXDMF.h"


class pqAboutGEM : public QDialog, private Ui::AboutPXDMF{
  Q_OBJECT
public:
  pqAboutGEM (QDialog * parent = 0):QDialog(parent, Qt::Window | Qt::WindowStaysOnTopHint) {
    onAboutWindows();
    
  }
  //Callback to plot the about 
  void onAboutWindows();
public slots:
  void SavePDFFile();
};

#endif