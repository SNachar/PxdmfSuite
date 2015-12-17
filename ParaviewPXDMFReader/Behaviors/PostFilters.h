/*=========================================================================

  Program:   PXDMFReader Plugin
  Module:    PostFilters.cxx

  Copyright (c) GeM, Ecole Centrale Nantes.
  All rights reserved.
  Copyright: See COPYING file that comes with this distribution


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __pqPostFilters_h
#define __pqPostFilters_h

#include <QObject>

class pqPipelineSource;

class pqPostFilters : public QObject {
    Q_OBJECT
    typedef QObject Superclass;
public:
    pqPostFilters(QObject *p=0) ;
    ~pqPostFilters();
    int postReconstructionMapping;
    int postReconstructionThreshold;
    int postReconstructionAnnotateFixedDims;
    /// Not Apply Any Filter                   0
    //  Mapping/Threshold/AnnotateFidexDims    1
    //  Mapping/AnnotateFidexDims              2
    //  Threshold/AnnotateFidexDims            3
    pqPipelineSource* ApplyThreshold(char* name, pqPipelineSource* input);
public slots:  
    void ConnectSource(pqPipelineSource*);
    void SetMappAndThredhold(pqPipelineSource* src);
private:
    pqPostFilters(const pqPostFilters&); // Not implemented.
    void operator=(const pqPostFilters&); // Not implemented. 
};

#endif