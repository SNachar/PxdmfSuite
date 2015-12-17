/*=========================================================================

  Program:   PXDMFReader Plugin
  Module:    pqPXDMFSync.h

  Copyright (c) GeM, Ecole Centrale Nantes.
  All rights reserved.
  Copyright: See COPYING file that comes with this distribution


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef _pqPXDMFSync_h
#define _pqPXDMFSync_h

// Qt Includes
#include "QDockWidget"

class pqPipelineSource;

class pqPXDMFSync : public QDockWidget { 
  Q_OBJECT 
public:
  pqPXDMFSync(QWidget* parent=0, Qt::WindowFlags flags=0);
  virtual ~pqPXDMFSync();

protected slots: 
  void updateSliders(pqPipelineSource* s = NULL);
  void FixedPXDMFDimsChangedSlot();
  void ConnectSource(pqPipelineSource*);
  void OnInteraction();
  void OffInteraction();

private:
  class pqInternal; 
  pqInternal* Internal;
};


#endif