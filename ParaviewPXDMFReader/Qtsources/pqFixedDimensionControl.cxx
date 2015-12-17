/*=========================================================================

  Program:   PXDMFReader Plugin
  Module:    pqFixedDimensionControl.cxx

  Copyright (c) GeM, Ecole Centrale Nantes.
  All rights reserved.
  Copyright: See COPYING file that comes with this distribution


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// Qt Includes.
#include "QSlider"
#include "QLabel"
#include "QLineEdit"
#include "QDoubleValidator"

// Plugin Includes
#include "pqFixedDimensionControl.h"

int pqFixedDimensionControl::sliderMax = 10000;
//
pqFixedDimensionControl::pqFixedDimensionControl( QWidget * parent): QHBoxLayout() {
    min = 0;
    max = 1;
    val = 0.5;
    PXDMFdim = 0;
    dim =0;
    this->setSpacing(5);
    Label= new QLabel();
    UnitLabel= new QLabel();
    
    Slider =new QSlider(Qt::Horizontal);

    Slider->setTracking(false);
    LineEdit  = new QLineEdit();
    Validator = new QDoubleValidator(this);
    Validator->setDecimals(10);
    Validator->setNotation(QDoubleValidator::ScientificNotation);
    LineEdit->setValidator(Validator);

    verticalLayout = new QGridLayout;
    verticalLayout->setMargin(0);
    this->insertLayout(0,verticalLayout);
    Label->setAlignment(Qt::AlignHCenter);
    UnitLabel->setAlignment(Qt::AlignHCenter);
    verticalLayout->addWidget(Label);
    UnitLabel->setVisible(false);
    
    this->addWidget(Slider);
    this->addWidget(LineEdit);

    Slider->setMinimum(0);
    Slider->setMaximum(pqFixedDimensionControl::sliderMax);

    LineEdit->setMaximumWidth ( 100);

    QObject::connect(Slider  , SIGNAL(valueChanged(int)), this, SLOT(setval(int)),  Qt::QueuedConnection);
    QObject::connect(Slider  , SIGNAL(sliderMoved(int)), this, SLOT(setval(int)),  Qt::QueuedConnection);
    QObject::connect(LineEdit, SIGNAL(editingFinished()), this, SLOT(setval()),  Qt::QueuedConnection);
    QObject::connect(this    , SIGNAL(valChanged(double)), parent, SLOT(FixedPXDMFDimsChangedSlot()),  Qt::UniqueConnection);

    this->setMargin(0);
};

//
pqFixedDimensionControl::~pqFixedDimensionControl() {
    delete Slider;
    delete Label;
    delete LineEdit;
    delete Validator;
    delete UnitLabel;
};

//
void pqFixedDimensionControl::local_update() {
    QString text;
    text.setNum( val,'g', 8);
    Slider->blockSignals(TRUE);
    LineEdit->blockSignals(TRUE);
    LineEdit->setText(text);
    Slider->setValue( (val-min)/(max-min)*pqFixedDimensionControl::sliderMax);
    Slider->blockSignals(false);
    LineEdit->blockSignals(false);
}

//
void pqFixedDimensionControl::pushval() {

    this->local_update();

    Slider->blockSignals(TRUE);
    LineEdit->blockSignals(TRUE);

    emit valChanged(getval());

    this->Slider->blockSignals(false);
    this->LineEdit->blockSignals(false);
};

//
void pqFixedDimensionControl::setval() {
    this->setval(this->LineEdit->text());
};

//
void pqFixedDimensionControl::setval(const QString& text) {
 if(this->val != text.toDouble() ){
      this->val = text.toDouble();
      pushval();
     
  }
};

//
void pqFixedDimensionControl::setval(const double _val) {
   if(this->val !=  _val){
    this->val = _val;
    pushval();
   }
};

//
double  pqFixedDimensionControl::getVal() {
    return this->val;
};

//
void pqFixedDimensionControl::setval(const int _val) {
    const double v = this->min+(_val*(this->max-this->min))/pqFixedDimensionControl::sliderMax;
    if(this->val !=  v){
      this->val = v;
      pushval();
    }
};

//
void pqFixedDimensionControl::setmin(double _min) {
    this->min =_min;
    Validator->setBottom(min);
};

//
double pqFixedDimensionControl::getmin() {
    return this->min;
};

//
void pqFixedDimensionControl::setmax(double _max) {
    this->max = _max;
    Validator->setTop(max);
};

//
double pqFixedDimensionControl::getmax() {
    return this->max;
};

//
double pqFixedDimensionControl::getval() {
    return this->val;
};

//
void pqFixedDimensionControl::setvalWithoutpush(double _val ) {
    this->val = _val;
    this->local_update();
};

//
void pqFixedDimensionControl::setlabel(QString text) {
    Label->setText(text);
};

//
QString pqFixedDimensionControl::getLabel() {
   return Label->text();
};

//
void pqFixedDimensionControl::setUnitLabel(QString text) {
  if(text != "" ){
      if(!UnitLabel->isVisible()){
        verticalLayout->addWidget(UnitLabel);
        UnitLabel->setVisible(true);      
      }      
      UnitLabel->setText(text);
    }
};
