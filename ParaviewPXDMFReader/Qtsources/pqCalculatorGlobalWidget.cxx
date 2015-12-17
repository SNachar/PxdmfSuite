/*=========================================================================

  Program:   PXDMFReader Plugin
  Module:    pqCalculatorGlobalWidget.cxx

  Copyright (c) GeM, Ecole Centrale Nantes.
  All rights reserved.
  Copyright: See COPYING file that comes with this distribution


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "ui_pqCalculatorWidget.h"

// Vtk Includes 
#include "vtkPVArrayInformation.h"
#include "vtkPVDataInformation.h"
#include "vtkPVDataSetAttributesInformation.h"
#include "vtkSMSourceProxy.h"
#include "vtkSMUncheckedPropertyHelper.h"

// ParaView Includes
#include "pqOutputPort.h"
#include "pqPipelineFilter.h"

// Qt Includes
#include <QMenu>
#include <QPointer>
#include <QSignalMapper>
#include <QtDebug>

// Plugin Includes
#include "pqCalculatorGlobalWidget.h"

class pqCalculatorGlobalWidget::pqInternals : public Ui::CalculatorWidget
{
public:
  QPointer<QMenu> ScalarsMenu;
  QPointer<QMenu> VectorsMenu;

  pqInternals() : ScalarsMenu(new QMenu()), VectorsMenu(new QMenu())
  {
  }
  ~pqInternals()
    {
    delete this->ScalarsMenu;
    delete this->VectorsMenu;
    }
};

//-----------------------------------------------------------------------------
pqCalculatorGlobalWidget::pqCalculatorGlobalWidget(
  vtkSMProxy* smproxy, vtkSMProperty* smproperty, QWidget* parentObject)
  : Superclass(smproxy, parentObject), Internals (new pqInternals())
{
  this->Internals->setupUi(this);
  this->setShowLabel(false);
  this->setChangeAvailableAsChangeFinished(false);

  this->Internals->Vectors->setMenu(this->Internals->VectorsMenu);
  this->Internals->Scalars->setMenu(this->Internals->ScalarsMenu);

  // clicking on any button or any part of the panel where another button
  // doesn't take focus will cause the line edit to have focus 
  this->setFocusProxy(this->Internals->Function);


  // before the menus are poped up, fill them up with the list of available
  // arrays.
  QObject::connect(this->Internals->ScalarsMenu, SIGNAL(aboutToShow()),
                   this, SLOT(updateVariableNames()));
  QObject::connect(this->Internals->VectorsMenu, SIGNAL(aboutToShow()),
                   this, SLOT(updateVariableNames()));

  // update the text when the user choses an variable in the menu.
  QObject::connect(this->Internals->VectorsMenu, SIGNAL(triggered(QAction*)),
                   this, SLOT(variableChosen(QAction*)));
  QObject::connect(this->Internals->ScalarsMenu, SIGNAL(triggered(QAction*)),
                   this, SLOT(variableChosen(QAction*)));


  //--------------------------------------------------------------------------
  // connect all buttons for which the text of the button 
  // is the same as what goes into the function 
  QRegExp regexp("^([ijk]Hat|ln|log10|sin|cos|"
                 "tan|asin|acos|atan|sinh|cosh|tanh|"
                 "sqrt|exp|ceil|floor|abs|norm|mag|"
                 "LeftParentheses|RightParentheses|"
                 "Divide|Multiply|Minus|Plus)$");
                 
  QList<QToolButton*> buttons;
  buttons = this->findChildren<QToolButton*>(regexp);
  foreach(QToolButton* tb, buttons)
    {
    QSignalMapper* mapper = new QSignalMapper(tb);
    QObject::connect(tb, SIGNAL(pressed()),
                     mapper, SLOT(map()));
    mapper->setMapping(tb, tb->text());
    QObject::connect(mapper, SIGNAL(mapped(const QString&)),
                     this, SLOT(buttonPressed(const QString&)));
    }
  
  QToolButton* tb = this->Internals->xy;
  QSignalMapper* mapper = new QSignalMapper(tb);
  QObject::connect(tb, SIGNAL(pressed()),
                   mapper, SLOT(map()));
  mapper->setMapping(tb, "^");
  QObject::connect(mapper, SIGNAL(mapped(const QString&)),
                   this, SLOT(buttonPressed(const QString&)));
  
  tb = this->Internals->v1v2;
  mapper = new QSignalMapper(tb);
  QObject::connect(tb, SIGNAL(pressed()),
                   mapper, SLOT(map()));
  mapper->setMapping(tb, ".");
  QObject::connect(mapper, SIGNAL(mapped(const QString&)),
                   this, SLOT(buttonPressed(const QString&)));
  //--------------------------------------------------------------------------

  this->addPropertyLink(this->Internals->Function, "text",
    SIGNAL(textChanged(const QString&)), smproperty);

  // now when editing is finished, we will fire the changeFinished() signal.
  this->connect(this->Internals->Function, SIGNAL(textChangedAndEditingFinished()),
    this, SIGNAL(changeFinished()));
}

//-----------------------------------------------------------------------------
pqCalculatorGlobalWidget::~pqCalculatorGlobalWidget()
{
  delete this->Internals;
  this->Internals = 0;
}

//-----------------------------------------------------------------------------
void pqCalculatorGlobalWidget::variableChosen(QAction* menuAction)
{
  if (menuAction)
    {
    this->Internals->Function->insert(menuAction->text());
    }
}

//-----------------------------------------------------------------------------
void pqCalculatorGlobalWidget::buttonPressed(const QString& buttonText)
{
  this->Internals->Function->insert(buttonText);
}

//-----------------------------------------------------------------------------
void pqCalculatorGlobalWidget::updateVariableNames()
{
  vtkSMUncheckedPropertyHelper attributeType(
    this->proxy(), "AttributeMode");
  this->updateVariables(attributeType.GetAsString(0));
}

//-----------------------------------------------------------------------------
void pqCalculatorGlobalWidget::updateVariables(const QString& mode)
{
  this->Internals->VectorsMenu->clear();
  this->Internals->ScalarsMenu->clear();

  if(mode == "Point Data")
    {
    this->Internals->VectorsMenu->addAction("coords");
    this->Internals->ScalarsMenu->addAction("coordsX");
    this->Internals->ScalarsMenu->addAction("coordsY");
    this->Internals->ScalarsMenu->addAction("coordsZ");
    }

  vtkPVDataSetAttributesInformation* fdi = NULL;
  vtkSMSourceProxy* input = vtkSMSourceProxy::SafeDownCast(
    vtkSMUncheckedPropertyHelper(this->proxy(), "Input").GetAsProxy(0));
  if (!input)
    {
    return;
    }

  if(mode == "Point Data")
    {
    fdi = input->GetDataInformation(0)->GetPointDataInformation();
    }
  else if(mode == "Cell Data")
    {
    fdi = input->GetDataInformation(0)->GetCellDataInformation();
    }
  
  if (!fdi)
    {
    return;
    }

  for(int i=0; i<fdi->GetNumberOfArrays(); i++)
    {
    vtkPVArrayInformation* arrayInfo = fdi->GetArrayInformation(i);
    if (arrayInfo->GetDataType() == VTK_STRING
        || arrayInfo->GetDataType() == VTK_VARIANT )
      {
      continue;
      }

    int numComponents = arrayInfo->GetNumberOfComponents();
    QString name = arrayInfo->GetName();

    for(int j=0; j<numComponents; j++)
      {
      if(numComponents == 1)
        {
        this->Internals->ScalarsMenu->addAction(name);
        }
      else
        {        
        QString compName(arrayInfo->GetComponentName( j ));        
        QString n = name + QString( "_%1").arg( compName );
        QStringList d;
        d.append(name);
        d.append(QString("%1").arg(j));
        QAction* a = new QAction(n, this->Internals->ScalarsMenu);
        a->setData(d);
        this->Internals->ScalarsMenu->addAction(a);
        }
      }

    if(numComponents == 3)
      {
      this->Internals->VectorsMenu->addAction(name);
      }
    }    
    
    // to add field arrays to the calculator
    //fdi = f->getInput(f->getInputPortName(0), 0)->getDataInformation()->GetFieldDataInformation();
    fdi = input->GetDataInformation(0)->GetFieldDataInformation();

    if(!fdi) {
    return;
    }
    
    for(int i=0; i<fdi->GetNumberOfArrays(); i++)
    {
    vtkPVArrayInformation* arrayInfo = fdi->GetArrayInformation(i);
    if (arrayInfo->GetDataType() == VTK_STRING
        || arrayInfo->GetDataType() == VTK_VARIANT )
      {
      continue;
      }
    QString name = arrayInfo->GetName();
    
    if(!name.toStdString().compare("AxisBaseForX") ||
      !name.toStdString().compare("AxisBaseForY") ||
      !name.toStdString().compare("AxisBaseForZ") ||
      !name.toStdString().compare("OrientedBoundingBox") ||
      !name.toStdString().compare("LabelRangeActiveFlag") ||
      !name.toStdString().compare("LabelRangeForX") ||
      !name.toStdString().compare("LabelRangeForY") ||
      !name.toStdString().compare("LabelRangeForZ") ||
      !name.toStdString().compare("LinearTransformForX") ||
      !name.toStdString().compare("LinearTransformForY") ||
      !name.toStdString().compare("LinearTransformForZ") 
      
    ) continue;
    
    int numComponents = arrayInfo->GetNumberOfComponents();
    
    
    for(int j=0; j<numComponents; j++)
      {
      if(numComponents == 1)
        {
        this->Internals->ScalarsMenu->addAction(name);
        }
      else
        {        
        QString compName(arrayInfo->GetComponentName( j ));        
        QString n = name + QString( "_%1").arg( compName );
        QStringList d;
        d.append(name);
        d.append(QString("%1").arg(j));
        QAction* a = new QAction(n, this->Internals->ScalarsMenu);
        a->setData(d);
        this->Internals->ScalarsMenu->addAction(a);
        }
      }

    if(numComponents == 3){
      this->Internals->VectorsMenu->addAction(name);
      }
    }
};
