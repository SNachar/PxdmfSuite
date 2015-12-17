/*=========================================================================

  Program:   PXDMFReader Plugin
  Module:    pqPXDMFReaderPanel.cxx

  Copyright (c) GeM, Ecole Centrale Nantes.
  All rights reserved.
  Copyright: See COPYING file that comes with this distribution


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "pqPXDMFReaderPanel.h"

//VTK Includes
#include "vtkObject.h"
#include "vtkSMProxy.h"
#include "vtkSMIntVectorProperty.h"
#include "vtkSMStringVectorProperty.h"
#include "vtkSMDoubleVectorProperty.h"
#include "vtkSMSourceProxy.h"

// Paraview Includes.
#include "pqApplicationCore.h"

// Qt Includes.
#include "QLineEdit"
#include "QGroupBox"
#include "QShortcut"

// Plugin includes.
#include "ui_pqPXDMFReaderPanelNew.h"
#include "pqPXDMFSettings.h"
#include "pqFixedDimensionControl.h"


class pqPXDMFReaderPanel::pqInternals
{
public:
    QPointer<QWidget> DetachWindow;
};

class pqPXDMFReaderPanel::pqUI : public Ui::PXDMFReaderPanel {
public:
    //pqSignalAdaptorTreeWidget* PointAdaptor;
    pqUI() {
        // this->PointAdaptor =0;
    };
    ~pqUI() {
        //delete this->PointAdaptor;
    };

};
//
pqPXDMFReaderPanel::pqPXDMFReaderPanel(pqProxy* pxy, QWidget* p) :
    pqNamedObjectPanel(pxy, p), mystride(NULL), Internals(new pqInternals()) {

    ininit = true;
    this->UI = new pqUI;
    this->UI->setupUi(this);

    this->linkServerManagerProperties();

    QObject::connect(this->findChild<QTreeWidget*>("VisualizationTimeStatus"),
                     SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(TimeDimsChanged(QTreeWidgetItem *, int)),
                     Qt::QueuedConnection);

    QObject::connect(this->findChild<QTreeWidget*>("VisualizationSpaceStatus"),
                     SIGNAL(itemClicked( QTreeWidgetItem *, int)), this, SLOT(SpaceDimsChanged(QTreeWidgetItem *, int)),
                     Qt::QueuedConnection);

    vtkObject::SetGlobalWarningDisplay(1);
    proxy()->DebugOn();

    proxy()->UpdatePropertyInformation();

    vtkSMIntVectorProperty *UseStride =  vtkSMIntVectorProperty::SafeDownCast(proxy()->GetProperty("UseStride"));
    vtkObject::SetGlobalWarningDisplay(0);
    proxy()->SetDebug(0);

    if (UseStride->GetElement(0)) this->AddStride();


    QObject::connect(this->findChild<QCheckBox*>("UseInterpolation"),  SIGNAL(clicked(bool)), this, SLOT(SetUseinterpolation(bool)),  Qt::QueuedConnection);
    QObject::connect(this->findChild<QCheckBox*>("ComputeDerivatives"),  SIGNAL(clicked(bool)), this, SLOT(SetComputeDerivatives(bool)),  Qt::QueuedConnection);
    //QObject::connect(this->findChild<QCheckBox*>("ContinuousUpdate"),  SIGNAL(clicked(bool)), this, SLOT(SetContinuousUpdate(bool)),  Qt::QueuedConnection);


    vtkSMIntVectorProperty *NumberOfPXDMF_Dims =  vtkSMIntVectorProperty::SafeDownCast(proxy()->GetProperty("PXDMF_Dims"));

    if (NumberOfPXDMF_Dims) {
        this->NumberOfPXDMFDims = NumberOfPXDMF_Dims->GetElement(0);
        vtkSMIntVectorProperty *gridsDimsDims =  vtkSMIntVectorProperty::SafeDownCast(proxy()->GetProperty("PXDMFDimsDataInfo"));

        if (gridsDimsDims) {
            this->NumberOfDimensionsPerPXDMFDim.resize(NumberOfPXDMFDims);
            this->NamesOfEachPXDMFDimension.resize(NumberOfPXDMFDims);
            this->UnitsOfEachPXDMFDimension.resize(NumberOfPXDMFDims);
            this->MinsOfEachPXDMFDimension.resize(NumberOfPXDMFDims);
            this->MaxsOfEachPXDMFDimension.resize(NumberOfPXDMFDims);
            for (unsigned i =0; int(i) < NumberOfPXDMFDims; ++i) {
                this->NumberOfDimensionsPerPXDMFDim[i] = gridsDimsDims->GetElement(i);
                this->NamesOfEachPXDMFDimension[i].resize(this->NumberOfDimensionsPerPXDMFDim[i]);
                this->UnitsOfEachPXDMFDimension[i].resize(this->NumberOfDimensionsPerPXDMFDim[i]);
                this->MinsOfEachPXDMFDimension[i].resize(this->NumberOfDimensionsPerPXDMFDim[i]);
                this->MaxsOfEachPXDMFDimension[i].resize(this->NumberOfDimensionsPerPXDMFDim[i]);
            }
            vtkSMStringVectorProperty *gridsDimsNames = vtkSMStringVectorProperty::SafeDownCast(proxy()->GetProperty("PXDMFDimsNameDataInfo"));
            vtkSMStringVectorProperty *gridsDimsUnits = vtkSMStringVectorProperty::SafeDownCast(proxy()->GetProperty("PXDMFDimsUnitDataInfo"));
            vtkSMDoubleVectorProperty *gridsDimsMins =  vtkSMDoubleVectorProperty::SafeDownCast(proxy()->GetProperty("PXDMFDimsMinRangeDataInfo"));
            if (gridsDimsMins  == 0) {
                std::cout << "error gridsDimsMins " << std::endl;
                exit(0);
            }
            vtkSMDoubleVectorProperty *gridsDimsMaxs =  vtkSMDoubleVectorProperty::SafeDownCast(proxy()->GetProperty("PXDMFDimsMaxRangeDataInfo"));
            if (gridsDimsMaxs == 0) {
                std::cout << "error gridsDimsMaxs "  << std::endl;
                exit(0);
            }
            if (gridsDimsNames ) {
                int cpt = 0;
                for (unsigned i =0; int(i) < NumberOfPXDMFDims; ++i) {
                    for (unsigned j =0; int(j) < this->NumberOfDimensionsPerPXDMFDim[i]; ++j) {
                        this->NamesOfEachPXDMFDimension[i][j] = gridsDimsNames->GetElement(cpt);
                        this->UnitsOfEachPXDMFDimension[i][j] = gridsDimsUnits->GetElement(cpt);
                        this->MinsOfEachPXDMFDimension[i][j] = gridsDimsMins->GetElement(cpt);
                        this->MaxsOfEachPXDMFDimension[i][j] = gridsDimsMaxs->GetElement(cpt);
                        ++cpt;
                    }
                }
            }

        }
    } else {
        std::cout << " Property 'PXDMF_Dims' does not exist"  << std::endl;
    }


    QTreeWidget* time = this->findChild<QTreeWidget*>("VisualizationTimeStatus");
    int time_nb = time->topLevelItemCount();
    bool time_checked=0;
    for (unsigned i = 0; int(i) < time_nb ; ++i) {
        if (time->topLevelItem(i)->checkState(0) == Qt::Checked ) {
            time_checked = true;
        }
    }
    if(time_checked) {
        for (unsigned i = 0; int(i) < time_nb ; ++i) {
            if (time->topLevelItem(i)->checkState(0) == Qt::Unchecked ) {
                if (!time->topLevelItem(i)->isDisabled()) time->topLevelItem(i)->setDisabled(true);
            }
        }
    }


    FixedDimensionsBox = new QGroupBox(tr("Fixed Dimensions"));
    fixlayout = new QVBoxLayout;
    fixlayout->setMargin(0);
    FixedDimensionsBox->setLayout(fixlayout);
    dynamic_cast<QGridLayout*>(this->layout())->addWidget(FixedDimensionsBox);

    this->findChild<QCheckBox*>("ComputeDerivatives")->setEnabled(this->findChild<QCheckBox*>("UseInterpolation")->isChecked());

    UpdateExtraDims();
    ininit = false;
//   QObject::connect(this , SIGNAL(onaccept()), this, SLOT(FixPXDMFDimsChangedFromServerSlot()));
    //QObject::connect(this->propertyManager(), SIGNAL(accepted()), this,SLOT(FixPXDMFDimsChangedFromServerSlot()));

    QObject::connect(this->findChild<QPushButton*>("Detach"),  SIGNAL(clicked()), this, SLOT(toggleDetach()),  Qt::QueuedConnection);
    QObject::connect(this->findChild<QPushButton*>("Refresh"),  SIGNAL(clicked()), this, SLOT(refresh()),  Qt::QueuedConnection);

    //<widget class="QPushButton" name="Detach">
    //this->UI->PointAdaptor =  new pqSignalAdaptorTreeWidget(this->UI->PointArrayStatus, true);
    //this->UI->PointAdaptor->setObjectName(QString::fromUtf8("PointAdaptor"));
    //this->UI->PointAdaptor->setItemCreatorFunction(&callbackTreeWidgetItem);
    //this->UI->PointArrayStatus->
    //this->
    //this->findChild<QGridLayout*>("gridLayout_2")->addWidget(PointAdaptor, 0, 0, 1, 1);
    //this->ad
    //QObject::connect(UI->PointArrayStatus,  SIGNAL(sort(int)), PointAdaptor, SLOT(valuesChanged()),  Qt::QueuedConnection);
    //his->addWidget
    //this->UI->PointAdaptor->sortColumn();
    // QObject::connect(this->UI->PointArrayStatus,
    //SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)),
    //this, SLOT(onCurrentIndexChanged(QTreeWidgetItem*)));
    //this->UI->PointAdaptor->up

    QShortcut *config= new QShortcut(tr("Ctrl+P"), this);
    QObject::connect(config, SIGNAL(activated()), this, SLOT(configPanel()));
}
//
void pqPXDMFReaderPanel::configPanel() {
    QWidget* setupWindow = new pqPXDMFSettings(this);
    setupWindow->show();
}
//
pqPXDMFReaderPanel::~pqPXDMFReaderPanel()
{
    if (this->Internals->DetachWindow) {
        this->Internals->DetachWindow->layout()->removeWidget( this->FixedDimensionsBox );
        delete this->Internals->DetachWindow;
    }

    delete this->UI;
}

//
void pqPXDMFReaderPanel::DeleteFixedDimension(unsigned gridDimToDelete, unsigned dimToDelete) {
    std::vector<pqFixedDimensionControl*>::iterator it = FixedDimensions.begin();

    while ( it !=  FixedDimensions.end()) {
        if ((*it)->PXDMFdim == gridDimToDelete && (*it)->dim == dimToDelete) {
            this->layout()->removeItem(*it);
            delete *it;
            it = FixedDimensions.erase(it);
        } else {
            ++it;
        }
    }
    if(FixedDimensions.size()  == 0 )  FixedDimensionsBox->setVisible(false);
};
//
pqFixedDimensionControl* pqPXDMFReaderPanel::AddFixedDimension(const unsigned gridDim, const unsigned dim,const QString &text  ,const QString &unit,const double &min,const  double &max, const int &internalpos) {

    for (unsigned i = 0; i < FixedDimensions.size(); ++i) {
        if (FixedDimensions[i]->dim ==dim && FixedDimensions[i]->PXDMFdim == gridDim ) return NULL;
    }
    pqFixedDimensionControl *out = new pqFixedDimensionControl(this);
    out->PXDMFdim = gridDim;
    out->dim = dim;
    out->setlabel(text);
    out->setUnitLabel(unit);
    out->setmin(min);
    out->setmax(max);
    out->setval((min+max)/2);
    out->local_update();

    FixedDimensions.push_back(out);
    return out;

}
//
void pqPXDMFReaderPanel::FixedPXDMFDimsChangedSlot() {
    FixedPXDMFDimsChanged(0);
}
//void pqPXDMFReaderPanel::FixedPXDMFDimsChangedFromServerSlot(){
//  FixPXDMFDimsChanged(1);
//   std::cout << "in pqPXDMFReaderPanel::FixedPXDMFDimsChangedFromServerSlot()" << std::endl;
//}
//
void pqPXDMFReaderPanel::updateInformationAndDomains() {
//   std::cout << "in pqPXDMFReaderPanel::updateInformationAndDomains()" << std::endl;
    pqProxyPanel::updateInformationAndDomains();
    FixedPXDMFDimsChanged(1);
}

//
void pqPXDMFReaderPanel::FixedPXDMFDimsChanged(bool fromserver = 0 ) {
    proxy()->UpdatePropertyInformation();
    vtkSMDoubleVectorProperty *PXDMFDimsPosDataInfo =  vtkSMDoubleVectorProperty::SafeDownCast(proxy()->GetProperty("PXDMFDimsPosDataInfo"));
    vtkSMDoubleVectorProperty *FixedDimensionsUsed =  vtkSMDoubleVectorProperty::SafeDownCast(proxy()->GetProperty("FixedDimensionsUsed"));

    if(fromserver) {
        if( FixedDimensionsUsed == 0  ) {
            std::cerr << "Error getting FixedDimensionsUsed  " << std::endl;
            return;
        }
        if ( FixedDimensionsUsed->GetNumberOfElements() == 0  ) {
            std::cerr << "Error FixedDimensionsUsed->GetNumberOfElements()  " << FixedDimensionsUsed->GetNumberOfElements() << std::endl;
            return;
        }
    }

    int cpt = 0;
    for (unsigned i =0; int(i) < this->NumberOfPXDMFDims; ++i) {
        for (unsigned j =0; int(j) < this->NumberOfDimensionsPerPXDMFDim[i]; ++j) {
            for (unsigned k=0; k < this->FixedDimensions.size(); ++k) {
                if (i == this->FixedDimensions[k]->PXDMFdim && j == this->FixedDimensions[k]->dim ) {
                    if(fromserver) {
                        this->FixedDimensions[k]->setvalWithoutpush(FixedDimensionsUsed->GetElement(cpt));
                        //std::cout << cpt << " : " <<  FixedDimensionsUsed->GetElement(cpt) << std::endl;
                    } else {
                        PXDMFDimsPosDataInfo->SetElement(cpt,this->FixedDimensions[k]->getval()) ;
                    }

                }
            }
            ++cpt;
        }
    }
    if(fromserver) return;



    if(!ininit) {
        if(this->findChild<QCheckBox*>("ContinuousUpdate")->isChecked()) {
            vtkSMSourceProxy* sourceProxy = vtkSMSourceProxy::SafeDownCast(this->proxy());
            if (!sourceProxy) {
                return;
            }
            //this->view()->render();
            proxy()->UpdateVTKObjects();
            emit pqApplicationCore::instance()->render();
        } else {
            this->setModified();
        }
    }
};
//
void pqPXDMFReaderPanel::SpaceDimsChanged(QTreeWidgetItem *item, int ) {

    QTreeWidget* time = this->findChild<QTreeWidget*>("VisualizationTimeStatus");
    //int time_nb = time->topLevelItemCount();

    QTreeWidget* space = this->findChild<QTreeWidget*>("VisualizationSpaceStatus");
    //int space_nb = space->topLevelItemCount();
    unsigned num = space->indexOfTopLevelItem(item);

    if (item->isDisabled()) return;

    int cpt = 0;
    for (unsigned i = 0; int(i) < NumberOfPXDMFDims; ++i)if (space->topLevelItem(i)->checkState(0) == Qt::Checked && i != num) {
            cpt += NumberOfDimensionsPerPXDMFDim[i];
        };
    if (NumberOfDimensionsPerPXDMFDim[num]+ cpt > 3)  item->setCheckState(0,Qt::Unchecked);

    if (time->topLevelItem(num)->checkState(0) == Qt::Checked)  item->setCheckState(0,Qt::Unchecked);

    UpdateExtraDims();
}
//
void pqPXDMFReaderPanel::TimeDimsChanged(QTreeWidgetItem *item, int) {

    QTreeWidget* time = this->findChild<QTreeWidget*>("VisualizationTimeStatus");
    int time_nb = time->topLevelItemCount();
    int num = time->indexOfTopLevelItem(item);
    QTreeWidget* space = this->findChild<QTreeWidget*>("VisualizationSpaceStatus");
    //int space_nb = space->topLevelItemCount();

    if (item->isDisabled()) return;
    ///verification that a time dimension must be a one dimensional dimension
    if (NumberOfDimensionsPerPXDMFDim[num] != 1) {
        item->setCheckState(0,Qt::Unchecked);
        this->setModified();
        return;
    }
    ///verification that cant check a space checked dimension
    if (space->topLevelItem(num)->checkState(0) == Qt::Checked) {
        item->setCheckState(0,Qt::Unchecked);
        return;
    }

    if (item->checkState(0) == Qt::Checked) {
        for (unsigned i = 0; int(i) < time_nb ; ++i) {
            if (time->topLevelItem(i)->checkState(0) == Qt::Unchecked )
                if (!time->topLevelItem(i)->isDisabled())
                    time->topLevelItem(i)->setDisabled(true);
        }
    } else {
        for (unsigned i = 0; int(i) < time_nb ; ++i) {
            if (time->topLevelItem(i)->isDisabled()) time->topLevelItem(i)->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsUserCheckable |Qt::ItemIsSelectable);
        }
    }
    UpdateExtraDims();

}
//
void pqPXDMFReaderPanel::UpdateExtraDims() {

    QTreeWidget* time = this->findChild<QTreeWidget*>("VisualizationTimeStatus");
    QTreeWidget* space = this->findChild<QTreeWidget*>("VisualizationSpaceStatus");

    int pos = 0;

    QGridLayout * main_layout = dynamic_cast<QGridLayout*>(this->layout());

    if (main_layout == 0 ) {
        exit(0);
    }

    pos = main_layout->rowCount();

    proxy()->UpdatePropertyInformation();
    vtkSMDoubleVectorProperty *PXDMFDimsPosDataInfo =  vtkSMDoubleVectorProperty::SafeDownCast(proxy()->GetProperty("PXDMFDimsPosDataInfo"));

    if(PXDMFDimsPosDataInfo) {
        bool flag = false; // false if nothing changed;
        unsigned cpt = 0;
        for (unsigned i = 0; int(i) < NumberOfPXDMFDims ; i++) {
            for (unsigned j = 0; int(j) < NumberOfDimensionsPerPXDMFDim[i]; ++j ) {
                if ( time->topLevelItem(i)->checkState(0) == Qt::Unchecked  &&  space->topLevelItem(i)->checkState(0) == Qt::Unchecked ) {
                    pqFixedDimensionControl* extradim = AddFixedDimension(i, j, NamesOfEachPXDMFDimension[i][j].c_str() ,UnitsOfEachPXDMFDimension[i][j].c_str() ,MinsOfEachPXDMFDimension[i][j] , MaxsOfEachPXDMFDimension[i][j], cpt);
                    flag = true;
                    if (extradim) {
                        if (PXDMFDimsPosDataInfo->GetNumberOfElements() > cpt)
                            extradim->setvalWithoutpush(PXDMFDimsPosDataInfo->GetElement(cpt));
//		      main_layout->addLayout(extradim ,pos,0);
                        fixlayout->addLayout(extradim);
                        FixedDimensionsBox->setVisible(true);
                    }
                    ++pos;
                } else {
                    DeleteFixedDimension(i,j);
                    flag = true;
                }
                cpt++;
            }
        }
        if(flag) {
            FixedPXDMFDimsChanged();
        }
    }
};
//
void pqPXDMFReaderPanel::AddStride() {
    int pos = 0;
    QGridLayout * main_layout = dynamic_cast<QGridLayout*>(this->layout());
    if (main_layout == 0 ) {
        exit(0);
    }
    pos = main_layout->rowCount();
    if (mystride) return;
    mystride = new MyStride(this);
    main_layout->addLayout(mystride ,pos,0);
};
//
void pqPXDMFReaderPanel::DeleteStride() {
    if (mystride) {
        delete mystride;
        mystride = NULL;
    }
};
//
void pqPXDMFReaderPanel::SetUseinterpolation(bool state) {
    proxy()->UpdatePropertyInformation();
    vtkSMIntVectorProperty *UseInterpolationSM =  vtkSMIntVectorProperty::SafeDownCast(proxy()->GetProperty("UseInterpolation"));

    if (UseInterpolationSM == 0) {
        std::cout << "Error in seting UseInterpolation" << std::endl;
    }
    UseInterpolationSM->SetElements1(this->findChild<QCheckBox*>("UseInterpolation")->isChecked());
    if(this->findChild<QCheckBox*>("UseInterpolation")->isChecked()) {
        this->findChild<QCheckBox*>("ComputeDerivatives")->setDisabled(0);
    } else {
        this->findChild<QCheckBox*>("ComputeDerivatives")->setChecked(0);
        this->findChild<QCheckBox*>("ComputeDerivatives")->setDisabled(1);
    }
    this->setModified();
    FixedPXDMFDimsChanged(true);

}
//
void pqPXDMFReaderPanel::SetComputeDerivatives(bool state) {
    proxy()->UpdatePropertyInformation();
    vtkSMIntVectorProperty *ComputeDerivativesSM =  vtkSMIntVectorProperty::SafeDownCast(proxy()->GetProperty("ComputeDerivatives"));

    if (ComputeDerivativesSM == 0) {
        std::cout << "Error in seting ComputeDerivatives" << std::endl;
    }
    ComputeDerivativesSM->SetElements1(this->findChild<QCheckBox*>("ComputeDerivatives")->isChecked());
    this->setModified();
}
//
//void pqPXDMFReaderPanel::SetContinuousUpdate(bool state) {
    //this->ContinuousUpdate = this->findChild<QCheckBox*>("ContinuousUpdate")->isChecked();
//}
//
//
void pqPXDMFReaderPanel::SetStride() {
    if (mystride == NULL) return;
    proxy()->UpdatePropertyInformation();
    vtkSMIntVectorProperty *StrideSM =  vtkSMIntVectorProperty::SafeDownCast(proxy()->GetProperty("Stride"));

    if (StrideSM == 0) {
        std::cout << "Error in seting stide" << std::endl;
    }
    StrideSM->SetElements3(mystride->Stride_0->text().toDouble(), mystride->Stride_1->text().toDouble(), mystride->Stride_2->text().toDouble());
    this->setModified();

};
//
MyStride::~MyStride() {
    delete Label;
    delete Stride_0;
    delete Stride_1;
    delete Stride_2;
    delete Validator;
}
//
MyStride::MyStride( QWidget * parent): QHBoxLayout() {
    this->setSpacing(5);
    Label= new QLabel(parent);
    Label->setText("Stride");
    Stride_0  = new QLineEdit(parent);
    Stride_1  = new QLineEdit(parent);
    Stride_2  = new QLineEdit(parent);
    Stride_0->setText("1");
    Stride_1->setText("1");
    Stride_2->setText("1");
    Validator = new QDoubleValidator(this);
    Validator->setDecimals(0);
    Validator->setBottom(1);
    Validator->setNotation(QDoubleValidator::ScientificNotation);
    Stride_0->setValidator(Validator);
    Stride_1->setValidator(Validator);
    Stride_2->setValidator(Validator);
    this->addWidget(Label);
    this->addWidget(Stride_0);
    this->addWidget(Stride_1);
    this->addWidget(Stride_2);

    QObject::connect(Stride_0,  SIGNAL(editingFinished()), parent, SLOT(SetStride()),  Qt::QueuedConnection);
    QObject::connect(Stride_1,  SIGNAL(editingFinished()), parent, SLOT(SetStride()),  Qt::QueuedConnection);
    QObject::connect(Stride_2,  SIGNAL(editingFinished()), parent, SLOT(SetStride()),  Qt::QueuedConnection);

};


void pqPXDMFReaderPanel::refresh() {

    proxy()->UpdatePropertyInformation();
    vtkSMIntVectorProperty *refreshSM =  vtkSMIntVectorProperty::SafeDownCast(proxy()->GetProperty("Refresh"));

    if (refreshSM == 0) {
        std::cout << "Error in seting refresh" << std::endl;
    }

    refreshSM->SetElements1(1);
    proxy()->UpdateVTKObjects();


    refreshSM->SetElements1(0);
    proxy()->MarkAllPropertiesAsModified();
    refreshSM->Modified();
    proxy()->UpdateVTKObjects();

    proxy()->GetProperty("PointArrayStatus")->Modified();
    proxy()->GetProperty("PointArrayInfo")->Modified();

    proxy()->UpdatePropertyInformation();
    //this->modified();
    //this->view()->render();
    emit pqApplicationCore::instance()->render();
}
//
void pqPXDMFReaderPanel::toggleDetach(){
  
    if (this->Internals->DetachWindow) {
        this->Internals->DetachWindow->layout()->removeWidget( this->FixedDimensionsBox );
        //this->layout()->addWidget(this->Internals->TabWidget);
        dynamic_cast<QGridLayout*>(this->layout())->addWidget(FixedDimensionsBox);
        delete this->Internals->DetachWindow;
        this->Internals->DetachWindow = NULL;
    } else {
        QWidgetDetach* DetachWindow = new QWidgetDetach(this, Qt::Window | Qt::WindowStaysOnTopHint);
        DetachWindow->setWindowTitle ( "PXDMF Reader"  );
        //QWidget* DetachWindow = new QWidget(this, Qt::Widget);
        this->Internals->DetachWindow = DetachWindow;
        DetachWindow->setObjectName("Fixed Dimentions");
        this->layout()->removeWidget(this->FixedDimensionsBox);

        QVBoxLayout* vbox = new QVBoxLayout(DetachWindow);
        vbox->setSpacing(0);
        vbox->setMargin(0);

        QPushButton* RefreshB = new QPushButton(DetachWindow);
        RefreshB->setText("Refresh");
        vbox->addWidget(RefreshB);
        QObject::connect(RefreshB, SIGNAL(clicked(bool)), this, SLOT(refresh()));

        vbox->addWidget(this->FixedDimensionsBox);
        DetachWindow->show();
        QObject::connect(DetachWindow, SIGNAL(close ()), this, SLOT(toggleDetach()));
    }
}