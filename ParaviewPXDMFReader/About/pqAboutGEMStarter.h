/*=========================================================================

  Program:   PXDMFReader Plugin
  Module:    pqAboutGEMStarter.h

  Copyright (c) GeM, Ecole Centrale Nantes.
  All rights reserved.
  Copyright: See COPYING file that comes with this distribution


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME pqAboutGEMStarter
// .SECTION Description

#ifndef __pqAboutGEMStarter_h
#define __pqAboutGEMStarter_h

// Qt Includes.
#include <QObject>

class QSplashScreen;
class pqPipelineSource;
class pqAxesLabels;
class pqPostFilters;

class pqAboutGEMStarter : public QObject {
    Q_OBJECT
    typedef QObject Superclass;
    QSplashScreen* Splash;
protected  slots:
    void close();
    void show();

public:
    pqAboutGEMStarter(QObject *p=0) ;
    ~pqAboutGEMStarter();

//Callback for startup;
    void onStartup();

//Callback for Shutdown;
    void onShutdown();
    
    static std::string version;
    
    //behaviours
private :
    pqAxesLabels* AxesLabelsB;
    pqPostFilters* PostFiltersB;
private:
    pqAboutGEMStarter(const pqAboutGEMStarter&); // Not implemented.
    void operator=(const pqAboutGEMStarter&); // Not implemented.
};

#endif