/*=========================================================================

  Program:   PXDMFReader Plugin
  Module:    QWidgetDetach.h

  Copyright (c) GeM, Ecole Centrale Nantes.
  All rights reserved.
  Copyright: See COPYING file that comes with this distribution


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME QWidgetDetach
// .SECTION Description

#include <QWidget>

/// to handel the detach Fixed dimensions
class QWidgetDetach : public QWidget { 
  Q_OBJECT
public:
  QWidgetDetach (QWidget* parent = 0, Qt::WindowFlags f = 0 ): QWidget( parent ,  f ){};
  void closeEvent(QCloseEvent *event){
      emit close();
  }
  virtual ~QWidgetDetach(){};
Q_SIGNALS:
  void close();
};