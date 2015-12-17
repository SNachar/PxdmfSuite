/*=========================================================================

  Program:   PXDMFReader Plugin
  Module:    PXDMFAboutHelpMenuActions.cxx

  Copyright (c) GeM, Ecole Centrale Nantes.
  All rights reserved.
  Copyright: See COPYING file that comes with this distribution


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "PXDMFAboutHelpMenuActions.h"

// Qt Includes.
#include <QApplication>
#include <QStyle>

//plugin include
#include "pqAboutGEM.h"

//-----------------------------------------------------------------------------
PXDMFAboutHelpMenuActions::PXDMFAboutHelpMenuActions(QObject* p) : QActionGroup(p)
{
    QAction* a = new QAction( "About PXDMF Reader...", this);
    a->setData(0);
    this->addAction(a);
    QObject::connect(this, SIGNAL(triggered(QAction*)), this, SLOT(onAction(QAction*)));
}

//-----------------------------------------------------------------------------
void PXDMFAboutHelpMenuActions::onAction(QAction* a)
{
    switch(a->data().toInt()) {
    case 0: {
        QWidget * aboutpanel = new pqAboutGEM(NULL);
        aboutpanel->show();
        break;
    }
    };
}
