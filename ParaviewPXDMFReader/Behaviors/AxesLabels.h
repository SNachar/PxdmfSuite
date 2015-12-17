/*=========================================================================

  Program:   PXDMFReader Plugin
  Module:   AxesLabels.h

  Copyright (c) GeM, Ecole Centrale Nantes.
  All rights reserved.
  Copyright: See COPYING file that comes with this distribution


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __pqAxesLabels_h
#define __pqAxesLabels_h

#include <QObject>

#include <pqPipelineSource.h>

class pqAxesLabels : public QObject {
    Q_OBJECT
    typedef QObject Superclass;
public:
    pqAxesLabels(QObject *p=0) ;
    ~pqAxesLabels();
public slots:
    void axisLabelUpdate();
    void ConnectSource( pqPipelineSource* src );
private:
    pqAxesLabels(const pqAxesLabels&); // Not implemented.
    void operator=(const pqAxesLabels&); // Not implemented. 
};

#endif