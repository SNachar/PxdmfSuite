/*=========================================================================

  Program:   PXDMFReader Plugin
  Module:    pqAboutGEM.cxx

  Copyright (c) GeM, Ecole Centrale Nantes.
  All rights reserved.
  Copyright: See COPYING file that comes with this distribution


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

// STD Includes 
#include <iostream>

//QT Includes
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>

//plugin Includes
#include "pqAboutGEM.h"
#include "pqAboutGEMStarter.h"

void pqAboutGEM::onAboutWindows(){
 
  setupUi(this);
  Version->setText("Version :  " + tr(pqAboutGEMStarter::version.c_str()));
  QObject::connect(pushButton  , SIGNAL(clicked()), this, SLOT(SavePDFFile()),  Qt::QueuedConnection);
  QFile file(":/PXDMFReader.pdf");
  // in the case the documentation is not compiled
  if(!file.open(QIODevice::ReadOnly)){
    pushButton->setEnabled(false);
  }
};
//-------------------------------------------------------------
void pqAboutGEM::SavePDFFile(){
  QString fileName = QFileDialog::getSaveFileName(this,
                                                  tr("Save PDF Documentation"),
                                                  QDir::homePath()+QDir::separator()+tr(("PXDMFReader"+ pqAboutGEMStarter::version +".pdf").c_str()), 
                                                  tr("pdf (*.pdf);;AllFiles (*)") );
  
  if(fileName.isEmpty()){
    return;
  } else {
    QFile file(":/PXDMFReader.pdf");
    if(!file.open(QIODevice::ReadOnly)){
      QMessageBox::information(this,tr("Unable to find documentation "), file.errorString());
      return;
    }
    //file.close();
    std::cout << "copy from " << ":/PXDMFReader.pdf" << " to " << fileName.toStdString() << std::endl;
    file.copy(fileName);
    QFile::setPermissions(fileName, QFile::ReadOwner | QFile::WriteOwner  );
    QMessageBox::information(this,tr("information "), tr("Documentation saved in ") + fileName );
  }
};