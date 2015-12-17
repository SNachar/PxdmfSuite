/*=========================================================================

  Program:   PXDMFReader Plugin
  Module:    pqFixedDimensionControl.h

  Copyright (c) GeM, Ecole Centrale Nantes.
  All rights reserved.
  Copyright: See COPYING file that comes with this distribution


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME pqFixedDimensionControl 
// .SECTION Description

#include "QHBoxLayout"

class QSlider;
class QLabel;
class QLineEdit;
class QDoubleValidator;
class QGridLayout;

class pqFixedDimensionControl : public QHBoxLayout{
  Q_OBJECT
public:
  unsigned PXDMFdim;
  unsigned dim;
  pqFixedDimensionControl( QWidget * parent);
  ~pqFixedDimensionControl();
  private:
  double min;
  double max;
  double val;

  QLabel *Label;
  QLineEdit *LineEdit;
  QDoubleValidator* Validator ;
  QLabel *UnitLabel;
  QGridLayout* verticalLayout;
  static int sliderMax;
public slots:
  void setvalWithoutpush(double _val );
  void setval(); 
  void setval(const QString& text);
  void setval(const double _val);
  void setval(const int _val);
signals:
  void valChanged(double);
  //void valChanged(double);
public :
  QSlider *Slider;
  void local_update();
  void setmin(double _min);
  double getmin();
  void setmax(double _max);
  double getmax();
  double getval();  
  void setlabel(QString text);
  QString getLabel();
  double getVal();
  void setUnitLabel(QString text);
  void pushval();
private:
  Q_DISABLE_COPY(pqFixedDimensionControl)
};
//
class MyStride : public QHBoxLayout{
public:
  QLabel * Label;
  QLineEdit* Stride_0; 
  QLineEdit* Stride_1;
  QLineEdit* Stride_2;
  QDoubleValidator* Validator;
  MyStride( QWidget * parent);
  ~MyStride();
};