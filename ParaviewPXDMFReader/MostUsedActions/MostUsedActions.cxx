/*=========================================================================

   Program: PXDMFReader Plugin
   Module:  MostUsedActions.cxx
   
  Copyright (c) GeM, Ecole Centrale Nantes.
  All rights reserved.
  Copyright: See COPYING file that comes with this distribution


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME  MostUsedActions
// .SECTION Description
#include "MostUsedActions.h"

#include <QApplication>
#include <QStyle>
#include <QMainWindow>
#include <QToolBar>


#include "pqApplicationCore.h"
#include "pqObjectBuilder.h"
#include "pqServer.h"
#include "pqServerManagerModel.h"
#include "pqUndoStack.h"
#include "pqCoreUtilities.h"
#include "pqActiveObjects.h"
#include "vtkSMUndoStack.h"

//-----------------------------------------------------------------------------
MostUsedActions::MostUsedActions(QObject* p) : QActionGroup(p)
{
//  // let's use a Qt icon (we could make our own)
//  QIcon icon = qApp->style()->standardIcon(QStyle::SP_MessageBoxCritical);
//  QAction* a = new QAction(icon, "Create Sphere", this);
//  a->setData("SphereSource");
//  this->addAction(a);
//  
//  a = new QAction(icon, "Create Cylinder", this);
//  a->setData("CylinderSource");
//  this->addAction(a);
  QObject::connect(this, SIGNAL(triggered(QAction*)), this, SLOT(onAction(QAction*)));

  QTimer::singleShot ( 3000, this, SLOT ( Connect() ) );
}
//
void MostUsedActions::Connect(){
  pqUndoStack* stack = pqApplicationCore::instance()->getUndoStack();
  QObject::connect(stack, SIGNAL (undoLabelChanged(const QString&) ), this,SLOT( NewChange(const QString&) ) );
}
//-----------------------------------------------------------------------------
void MostUsedActions::NewChange(const QString& name){
  
  pqUndoStack* stack = pqApplicationCore::instance()->getUndoStack();
  vtkUndoSet* uset = stack->getLastUndoSet();
  std::cout  << name.toStdString() << std::endl;;
  //QIcon icon = qApp->style()->standardIcon(QStyle::SP_MessageBoxCritical);
  
  QAction* a = new QAction(this);
  QString cleanName = name;
  if( name.contains("Create ") ){
    cleanName = cleanName.split("Create")[1];
    if( name.contains("'") ){
      cleanName = cleanName.split("'")[1];
    }
    
    if( name.contains("Reader") ){
      a->setData(2); // Reader
    } else {
      a->setData(1); // filter
      
    }
    
  } else {
    return;
  }
  
  a->setText(cleanName);

  
  
  QMainWindow *mainWindow = qobject_cast<QMainWindow *>(pqCoreUtilities::mainWidget());

  if (!mainWindow){
    qWarning("Could not find MainWindow. Cannot load actions from the plugin.");
    return;
  }
  
  QToolBar* toolbar = mainWindow->findChild<QToolBar *> ( "Most Used Toolbar" );
  if (!toolbar){
    qWarning("Could not find toolbar. Cannot load actions from the plugin.");
    return;
  }
  toolbar->removeAction(a);
  if(toolbar->actions().size()>10)
    toolbar->actions().pop_back();
  
  if(toolbar->actions().size()>0)
    toolbar->insertAction(toolbar->actions()[0],a);
  else 
    toolbar->addAction(a);
}
//-----------------------------------------------------------------------------
void MostUsedActions::onAction(QAction* a)
{
  pqApplicationCore* core = pqApplicationCore::instance();
  pqObjectBuilder* builder = core->getObjectBuilder();
  pqServerManagerModel* sm = core->getServerManagerModel();
  pqUndoStack* stack = core->getUndoStack();

  /// Check that we are connect to some server (either builtin or remote).
  if (sm->getNumberOfItems<pqServer*>())
    {
    // just create it on the first server connection
    pqServer* s = sm->getItemAtIndex<pqServer*>(0);
  
    QString source_name = a->text();
    int source_type = a->data().toInt();
    // make this operation undo-able if undo is enabled
    //if(stack)
    //  {
    //  stack->beginUndoSet(QString("Create %1").arg(source_type));
    //  }
    switch(source_type){
      case(0):{
        std::cout << "creating sources " << source_name.toLatin1().data() << std::endl;
        builder->createSource("sources", source_name.toLatin1().data(), s);        
        break;
      } case(1):{
        std::cout << "creating filter " << source_name.toLatin1().data() << std::endl;
        if(pqActiveObjects::instance().activeSource())
          builder->createFilter("filters", source_name.toLatin1().data(), pqActiveObjects::instance().activeSource());        
        break;
      }case(2):{
        std::cout << "creating Reader " << source_name.toLatin1().data() << std::endl;
        break;
      }
    }


    //  {
    //if(stack)
    //  {
    //  stack->endUndoSet();
    //  }
    }
    //pqServerManagerModel* smmodel =  pqApplicationCore::instance()->getServerManagerModel()->
    //warpfilter = oBuilder->createFilter ( QString ( "filters" ),QString ( "Calculator" ),src );
}

