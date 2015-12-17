/*=========================================================================

  Program:   PXDMFReader Plugin
  Module:    QAllTreeWidget.cxx

  Copyright (c) GeM, Ecole Centrale Nantes.
  All rights reserved.
  Copyright: See COPYING file that comes with this distribution


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

//std includes
#include <iostream> 

// Qt Includes.
#include <QKeyEvent>
#include <QAllTreeWidget.h>

class TreeWidgetItem : public QTreeWidgetItem
{  
public:
    TreeWidgetItem(QTreeWidget *tree) : QTreeWidgetItem(tree)  {}
    TreeWidgetItem(QTreeWidget * parent, const QStringList & strings)
                   : QTreeWidgetItem (parent,strings)  {}  
    bool operator< (const QTreeWidgetItem &other) const    
    {  
        
        int sortCol = treeWidget()->sortColumn();  
        int index = text(sortCol).lastIndexOf("_");
        if(index == -1){
          return text(sortCol)<  other.text(sortCol);
        }
        //std::cout << text(sortCol).left(index).toAscii().constData() << " - " << text(sortCol).right(index).toAscii().constData() << std::cout ;
        if(text(sortCol).left(index) != other.text(sortCol).left(index)){
          return text(sortCol).left(index) < other.text(sortCol).left(index);
        }
        
        return text(sortCol).right(index).toInt() < other.text(sortCol).right(index).toInt() ;
    }
}; 

QTreeWidgetItem* callbackTreeWidgetItem(QTreeWidget* parent, const QStringList& val){
  return new TreeWidgetItem(parent,val);
};

ProductTreeWidget::ProductTreeWidget(QWidget *wid): QTreeWidget(wid) {
    this->setHeaderHidden(true);
    this->setSortingEnabled(false);
    //this->setModel(model);this->Implementation->LocationsAdaptor->setItemCreatorFunction(
    
};
//
void ProductTreeWidget::keyPressEvent(QKeyEvent* event) {
    QTreeWidget::keyPressEvent(event);
    switch (event->key()) {
    case Qt::Key_S: {

      std::cout << "sort " << std::endl;
      break;
    }
      
    case Qt::Key_Space: {

        Qt::CheckState state;
        if(currentItem()->checkState(0) == Qt::Unchecked) {
            state = Qt::Unchecked;
        } else {
            state = Qt::Checked;
        }
        emit QTreeWidget::itemClicked(currentItem(), 0);
        for(int i = 0; i < this->topLevelItemCount (); ++i) {
            QTreeWidgetItem * itm = this->topLevelItem(i);
            if(currentItem() == itm) continue;
            if (itm->isSelected()) {
                if(itm->checkState(0) != state) {
                    itm->setCheckState(0, state);
                    emit QTreeWidget::itemClicked(itm, 0);
                }
            }
        }
        break;
    }
    case Qt::Key_Select:
        if (currentItem()) {
            emit QTreeWidget::itemClicked(currentItem(), 0);
        }
    }
}
