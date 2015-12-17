/*=========================================================================

  Program:   meshReader Plugin
  Module:    MostUsedActions.h

  Copyright (c) GeM, Ecole Centrale Nantes.
  All rights reserved.
  Copyright: See COPYING file that comes with this distribution


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __MostUsedActionsd_h
#define __MostUsedActions_h

#include <QActionGroup>
#include <map>
/// This example illustrates adding a toolbar to ParaView to create a sphere and
/// a cylinder source.
class MostUsedActions : public QActionGroup
{
  Q_OBJECT
public:
  MostUsedActions(QObject* p);
public slots:
  /// Callback for each action triggerred.
  void onAction(QAction* a);
  void Connect();
  void NewChange(const QString&);
private:
  std::map<std::string,int> counter;
  
};

#endif //MostUsedActions

