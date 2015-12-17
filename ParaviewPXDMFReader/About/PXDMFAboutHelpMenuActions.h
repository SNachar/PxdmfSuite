/*=========================================================================

  Program:   PXDMFReader Plugin
  Module:    PXDMFAboutHelpMenuActions.h

  Copyright (c) GeM, Ecole Centrale Nantes.
  All rights reserved.
  Copyright: See COPYING file that comes with this distribution


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME PXDMFAboutHelpMenuActions
// .SECTION Description


#ifndef __PXDMFAboutHelpMenuActions_h
#define __PXDMFAboutHelpMenuActions_h

#include <QActionGroup>

class PXDMFAboutHelpMenuActions : public QActionGroup {
  Q_OBJECT
public:
  PXDMFAboutHelpMenuActions(QObject* p);
public slots:
  /// Callback for each action triggerred.
  void onAction(QAction* a);
};

#endif

