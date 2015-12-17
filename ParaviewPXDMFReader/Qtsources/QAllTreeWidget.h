/*=========================================================================

  Program:   PXDMFReader Plugin
  Module:    QAllTreeWidget.h

  Copyright (c) GeM, Ecole Centrale Nantes.
  All rights reserved.
  Copyright: See COPYING file that comes with this distribution


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME QAllTreeWidget
// .SECTION Description

#ifndef _producttreewidget_h
#define _producttreewidget_h

// Qt Includes.
#include <QTreeWidget>

class QKeyEvent;

class ProductTreeWidget: public QTreeWidget{
  Q_OBJECT
  public:
  ProductTreeWidget(QWidget *wid);
  void keyPressEvent(QKeyEvent* event);
signals:
  void sort();
};

#endif