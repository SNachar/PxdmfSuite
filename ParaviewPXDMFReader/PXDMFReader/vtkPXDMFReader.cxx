/*=========================================================================

  Program:   PXDMFReader Plugin
  Module:    PXDMFReader.cxx

  Copyright (c) GeM, Ecole Centrale Nantes.
  All rights reserved.
  Copyright: See COPYING file that comes with this distribution


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

// STD Includes
#include <sstream>
// 
// 
// vtk Includes.
#include "vtkObjectFactory.h"
#include "vtkXMLParser.h"
#include "vtkStringArray.h"
#include "vtkDoubleArray.h"
#include "vtkIntArray.h"
// 
// Pluing Includes
#include "FileDataAbstract.h"
#include "vtkPXDMFReader.h"
#include "stringhelper.h"
#include <vtkPXDMFGeneralSettings.h>

//#define GEM_DEBUG(x) x
#define GEM_DEBUG(x) 

//----------------------------------------------------------------------------
class vtkPXDMFReaderTester : public vtkXMLParser {
public:
    vtkTypeMacro(vtkPXDMFReaderTester, vtkXMLParser);
    static vtkPXDMFReaderTester* New();
    int TestReadFile() {
        this->Valid = 0;
        if (!this->FileName) return 0;

        ifstream inFile(this->FileName);
        if (!inFile) {
            return 0;
        }

        this->SetStream(&inFile);
        this->Done = 0;

        this->Parse();

        if (this->Done && this->Valid ) return 1;

        return 0;
    }
    void StartElement(const char* name, const char**)
    {
        this->Done = 1;
        if (strcmp(name, "Xdmf") == 0) {
            this->Valid = 1;
        }
    }

protected:
    vtkPXDMFReaderTester() {
        this->Valid = 0;
        this->Done = 0;
    }
private:
    void ReportStrayAttribute(const char*, const char*, const char*) {}
    void ReportMissingAttribute(const char*, const char*) {}
    void ReportBadAttribute(const char*, const char*, const char*) {}
    void ReportUnknownElement(const char*) {}
    void ReportXmlParseError() {}

    int ParsingComplete() {
        return this->Done;
    }
    int Valid;
    int Done;
    vtkPXDMFReaderTester(const vtkPXDMFReaderTester&); // Not implemented
    void operator=(const vtkPXDMFReaderTester&); // Not implemented
};

vtkStandardNewMacro(vtkPXDMFReaderTester);
;
//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkPXDMFReader);
;
//----------------------------------------------------------------------------
vtkPXDMFReader::vtkPXDMFReader() {


    this->SetGUIMaxNbModes(vtkPXDMFGeneralSettings::GetInstance()->GetGUIMaxNbModes());
    
    GEM_DEBUG(std::cout << "SetGUIMaxNbModes" << this->GUIMaxNbModes << std::endl;)
    
    PointDataShowAllModes= true;
    CellDataShowAllModes= true;
    ContinuousUpdate = false;
    
    this->TimeFileDataUpdate = 0;

    this->PointArraysCache = new XdmfArraySelection;
    this->CellArraysCache = new XdmfArraySelection;
    //this->GridsCache = new vtkXdmfArraySelection;
    this->FileName = NULL;

    this->vtkPXDMFDimsNamesData = vtkSmartPointer<vtkStringArray>::New();
    this->vtkPXDMFDimsNamesData->Initialize();
    this->vtkPXDMFDimsNamesData->SetNumberOfValues(0);
    
    this->vtkPXDMFDimsUnitsData = vtkSmartPointer<vtkStringArray>::New();
    this->vtkPXDMFDimsUnitsData->Initialize();
    this->vtkPXDMFDimsUnitsData->SetNumberOfValues(0);

    vtkPXDMFDimsMinRangeData = vtkSmartPointer<vtkDoubleArray> ::New();
    vtkPXDMFDimsMinRangeData->Initialize();
    vtkPXDMFDimsMaxRangeData =vtkSmartPointer<vtkDoubleArray> ::New();
    vtkPXDMFDimsMaxRangeData->Initialize();

    vtkPXDMFDimsPosData = vtkSmartPointer<vtkDoubleArray> ::New();
    vtkPXDMFDimsPosData->Initialize();

};
//----------------------------------------------------------------------------
vtkIntArray* vtkPXDMFReader::GetPXDMFDimsDataInfo() {
    return this->GetNumberOfDimsInEachPXDMFDim();
};
//----------------------------------------------------------------------------
vtkStringArray* vtkPXDMFReader::GetPXDMFDimsNamesDataInfo() {
    if (this->vtkPXDMFDimsNamesData->GetNumberOfValues()==0) {
        for (unsigned i = 0; int(i) < this->GetNumberOfPXDMFDims(); i++)
            for (unsigned j = 0; int(j) < this->GetNumberOfDimsInEachPXDMFDim()->GetValue(i); j++)
                vtkPXDMFDimsNamesData->InsertNextValue(this->GetNamesOfDimension(i,j) );
    }
    return vtkPXDMFDimsNamesData;
};
//----------------------------------------------------------------------------
vtkStringArray* vtkPXDMFReader::GetPXDMFDimsUnitsDataInfo() {
    if (this->vtkPXDMFDimsUnitsData->GetNumberOfValues()==0) {
        for (unsigned i = 0; int(i) < this->GetNumberOfPXDMFDims(); i++)
            for (unsigned j = 0; int(j) < this->GetNumberOfDimsInEachPXDMFDim()->GetValue(i); j++)
                vtkPXDMFDimsUnitsData->InsertNextValue(this->GetUnitsOfDimension(i,j) );
    }
    return vtkPXDMFDimsUnitsData;
};
//----------------------------------------------------------------------------
vtkPXDMFReader::~vtkPXDMFReader() {
    this->SetDomainName(0);
    delete this->PointArraysCache;
    delete this->CellArraysCache;
    //delete this->GridsCache;
}
//----------------------------------------------------------------------------
int vtkPXDMFReader::CanReadFile(const char* candidate_filename) {
    vtkPXDMFReaderTester* tester = vtkPXDMFReaderTester::New();
    tester->SetFileName(candidate_filename);
    int res = tester->TestReadFile();
    tester->Delete();
    return res;
}
//----------------------------------------------------------------------------
bool vtkPXDMFReader::PrepareDocument() {
    GEM_DEBUG(std::cout << " int vtkPXDMFReader::PrepareDocument()" << std::endl;)
    if (!vtkPXDMFDocument::PrepareDocument()) return false;

    //if (TimeFileDataUpdate  == this->XDMFInternal->mtime){
    //  return true;
    //};
    //TimeFileDataUpdate = this->XDMFInternal->mtime;
    
    unsigned cpt = 0;
    for (std::map<std::string, unsigned>::iterator it = GetNumberOfModesOfPointData().begin(); it!=GetNumberOfModesOfPointData().end(); it++) cpt += it->second;
    GEM_DEBUG(std::cout << " cpt " << cpt << " " << GUIMaxNbModes << std::endl;)
    if (cpt> GUIMaxNbModes ) {
        PointDataShowAllModes = false;
    };
    if (!PointDataShowAllModes)
        for (std::map<std::string, unsigned>::iterator it = GetNumberOfModesOfPointData().begin(); it!=GetNumberOfModesOfPointData().end(); it++) {
            this->PointArraysCache->AddArray(it->first.c_str());
        }
    cpt = 0;
    for (std::map<std::string, unsigned>::iterator it = GetNumberOfModesOfCellData().begin(); it!=GetNumberOfModesOfCellData().end(); it++) cpt += it->second;
    if (cpt> GUIMaxNbModes ) {
        CellDataShowAllModes = false;
    };
    if (!CellDataShowAllModes)
        for (std::map<std::string, unsigned>::iterator it = GetNumberOfModesOfCellData().begin(); it!=GetNumberOfModesOfCellData().end(); it++) {
            this->CellArraysCache->AddArray(it->first.c_str());
        }
    return true;

}

//----------------------------------------------------------------------------
//vtkXdmfArraySelection* vtkPXDMFReader::GetPointArraySelection() {
//    if (PointDataShowAllModes) {
//        return this->GetXdmfDocument()->GetActiveDomain()?
//               this->GetXdmfDocument()->GetActiveDomain()->GetPointArraySelection() :
//               this->PointArraysCache;
//    } else {
//        return this->PointArraysCache;
//    }
//}

//----------------------------------------------------------------------------
// vtkXdmfArraySelection* vtkPXDMFReader::GetCellArraySelection() {
//     if (CellDataShowAllModes) {
//         return this->GetXdmfDocument()->GetActiveDomain()?
//                this->GetXdmfDocument()->GetActiveDomain()->GetCellArraySelection() :
//                this->CellArraysCache;
//     } else {
//         return this->CellArraysCache;
//     }
// }
//----------------------------------------------------------------------------
//vtkXdmfArraySelection* vtkPXDMFReader::GetGridSelection() {
//    return this->GetXdmfDocument()->GetActiveDomain()?
//           this->GetXdmfDocument()->GetActiveDomain()->GetGridSelection() :
//           this->GridsCache;
//}

//----------------------------------------------------------------------------
//int vtkPXDMFReader::GetNumberOfGrids() {
//    return this->GetGridSelection()->GetNumberOfArrays();
//}

//----------------------------------------------------------------------------
//void vtkPXDMFReader::SetGridStatus(const char* gridname, int status) {
//    this->GetGridSelection()->SetArrayStatus(gridname, status !=0);
//    this->Modified();
//}

//----------------------------------------------------------------------------
//int vtkPXDMFReader::GetGridStatus(const char* arrayname) {
//    return this->GetGridSelection()->GetArraySetting(arrayname);
//}

//----------------------------------------------------------------------------
//const char* vtkPXDMFReader::GetGridName(int index) {
//    return this->GetGridSelection()->GetArrayName(index);
//}

//----------------------------------------------------------------------------
int vtkPXDMFReader::GetNumberOfPointArrays() {
    if (PointDataShowAllModes)
        return this->XDMFInternal->GetNumberOfPointArrays();
    else
        return this->PointArraysCache->GetNumberOfArrays();
}

//----------------------------------------------------------------------------
void vtkPXDMFReader::SetPointArrayStatus(const char* arrayname, int status) {
    
    if (PointDataShowAllModes) {
          if(this->XDMFInternal->GetPointArraySelection(arrayname) != status ) {
            this->XDMFInternal->changeInSelectedPointCellArrays = true ;
            this->Modified();
          }
          this->XDMFInternal->SetPointArraySelection(arrayname, status != 0);
    } else {
        std::string name = arrayname;
        int pos = name.find_last_of(" ");
        std::string clean_arrayname = name.substr(0, pos);
        this->PointArraysCache->SetArrayStatus(clean_arrayname.c_str(), status != 0);
        this->SetPointStatus(clean_arrayname.c_str(), -1 ,  status );
    }
}

//----------------------------------------------------------------------------
//void vtkPXDMFReader::SetPointArrayStatus(const char* arrayBaseName,const int& arrayMode,const  int& status) {
//    if (arrayMode < int(this->GetNumberOfModesOfPointData()[arrayBaseName] )) {
//        std::ostringstream temp ;
//        temp << arrayBaseName << "_"<< arrayMode;
//        this->SetPointArrayStatus(temp.str().c_str(),status);
//    } else {
//        return ;
//    }
//}

//----------------------------------------------------------------------------
int vtkPXDMFReader::GetPointArrayStatus(const char* arrayname) {
    if (PointDataShowAllModes) {
        return this->XDMFInternal->GetPointArraySelection(arrayname);
    } else {
        std::string name = arrayname;
        int pos = name.find_last_of(" ");
        std::string clean_arrayname = name.substr(0, pos);
        return this->PointArraysCache->GetArraySetting(clean_arrayname.c_str());
    }
}

//----------------------------------------------------------------------------
const char* vtkPXDMFReader::GetPointArrayName(int index) {
    if (PointDataShowAllModes) {
        return this->XDMFInternal->GetPointArrayName(index);
    } else {
        if (PointArrayNames.empty()) {
            for (int j = 0; j < this->PointArraysCache->GetNumberOfArrays() ; ++j) {
                PointArrayNames[j]= (this->PointArraysCache->GetArrayName(j) + std::string(" (")+ to_string(this->GetNumberOfModesOfPointData()[this->PointArraysCache->GetArrayName(j)])+")") ;
            }
        };
        return PointArrayNames[index].c_str();
    }
}

//-----------------------------------------------------
void vtkPXDMFReader::SetRefresh (bool _arg) {


  if(!_arg) return;
  if ( this->FileName ) {
    
    //this->SILUpdateStamp = 0;
    this->DomainName = 0;
    
    this->XDMFInternal->ForceUpdate();
    
    PointArrayNames.clear();
    PointArraysCache->clear();
    CellArrayNames.clear();
    CellArraysCache->clear();
    
    this->TimeFileDataUpdate = 0;
    
    PrepareDocument();
    
    this->Modified();
    // TODO CLEAN
    //std::cout << "this->GetNumberOfPointArrays()" << this->GetNumberOfPointArrays() << std::endl;
    //for(int i =0; i < this->GetNumberOfPointArrays() ; ++i){
    //  std::cout << this->GetPointArrayName(i) << " : " << this->GetPointArrayStatus(this->GetPointArrayName(i)) << std::endl;
    //};
    
  }
}

//----------------------------------------------------------------------------
int vtkPXDMFReader::GetNumberOfCellArrays() {
    if (CellDataShowAllModes)
        return this->XDMFInternal->GetNumberOfCellArrays();
    else
        return this->CellArraysCache->GetNumberOfArrays();
}

//----------------------------------------------------------------------------
void vtkPXDMFReader::SetCellArrayStatus(const char* arrayname, int status) {
    if (CellDataShowAllModes) {
       if(this->XDMFInternal->GetCellArraySelection(arrayname) != status ) {
          this->XDMFInternal->changeInSelectedPointCellArrays = true ;
          this->Modified();
       }
      this->XDMFInternal->SetCellArraySelection(arrayname,status !=0);
    } else {
        std::string name = arrayname;
        int pos = name.find_last_of(" ");
        std::string clean_arrayname = name.substr(0, pos);
        this->CellArraysCache->SetArrayStatus(clean_arrayname.c_str(), status != 0);
        this->SetCellStatus(clean_arrayname.c_str(), -1 ,  status );
        const int nb_mode = this->GetNumberOfModesOfCellData()[clean_arrayname];
    }
}

//----------------------------------------------------------------------------
//void vtkPXDMFReader::SetCellArrayStatus(const char* arrayBaseName,const int& arrayMode,const  int& status) {
//    if (arrayMode < int(this->GetNumberOfModesOfCellData()[arrayBaseName] )) {
//        std::ostringstream temp ;
//        temp << arrayBaseName << "_"<< arrayMode;
//        this->SetCellArrayStatus(temp.str().c_str(),status);
//    } else {
//        return ;
//    }
//}

//----------------------------------------------------------------------------
int vtkPXDMFReader::GetCellArrayStatus(const char* arrayname) {
    if (CellDataShowAllModes) {
        //return this->GetCellArraySelection()->GetArraySetting(arrayname);
        return this->XDMFInternal->GetCellArraySelection(arrayname);
    } else {
        std::string name = arrayname;
        int pos = name.find_last_of(" ");
        std::string clean_arrayname = name.substr(0, pos);
        return this->CellArraysCache->GetArraySetting(clean_arrayname.c_str());
    }
}

//----------------------------------------------------------------------------
const char* vtkPXDMFReader::GetCellArrayName(int index) {
    if (CellDataShowAllModes) {
        return this->XDMFInternal->GetCellArrayName(index);
    } else {
        if (CellArrayNames.empty()) {
            for (int j = 0; j < this->CellArraysCache->GetNumberOfArrays() ; ++j) {
                CellArrayNames[j]= (this->CellArraysCache->GetArrayName(j) + std::string(" (")+ to_string(this->GetNumberOfModesOfCellData()[this->CellArraysCache->GetArrayName(j)])+")") ;
            }
        };
        return CellArrayNames[index].c_str();
    }
}

//----------------------------------------------------------------------------
int vtkPXDMFReader::GetNumberOfVisualizationTimeArrays() {
    return this->GetNumberOfPXDMFDims();
};

//----------------------------------------------------------------------------
const char* vtkPXDMFReader::GetVisualizationTimeArrayName(int index) {
    return GetVisualizationSpaceArrayName(index);
};

//----------------------------------------------------------------------------
int vtkPXDMFReader::GetVisualizationTimeArrayStatus(const char* name) {
    for (unsigned i=0; int(i) < this->GetNumberOfPXDMFDims(); ++i ) {
        if (strcmp(name, GetVisualizationTimeArrayName(i) ) == 0) {
            return this->GetActivePXDMFDimForTime()[i];
        }
    }
    return 0;
};

//----------------------------------------------------------------------------
void vtkPXDMFReader::SetVisualizationTimeStatus(const char* name, int status) {
    /// We check that only grids with dimensionality equal to one can be used for the time
    if (status == 0) {
        haveTime = false;
        for (unsigned i=0; int(i) < this->GetNumberOfPXDMFDims(); ++i ) {
            if (strcmp(name, GetVisualizationTimeArrayName(i) ) == 0) {
                this->GetActivePXDMFDimForTime()[i] = false;
            }
            this->haveTime = this->haveTime || this->GetActivePXDMFDimForTime()[i];
        }
    } else {
        for (unsigned i=0; int(i) < this->GetNumberOfPXDMFDims(); ++i ) {
            if ((strcmp(name, GetVisualizationTimeArrayName(i) ) == 0) ) {
                if (this->GetNumberOfDimsInEachPXDMFDim()->GetValue(i)==1) {
                    this->GetActivePXDMFDimForTime()[i] = true;
                    haveTime = 1;
                } else {
                    vtkWarningMacro("this grid has incorrect dimensionality. Cant be used for Time." );
                }
            }
        }
    }
    this->Modified();
    return;
};

//----------------------------------------------------------------------------
// void vtkPXDMFReader::PassCachedSelections() {
//     if (!this->GetXdmfDocument()->GetActiveDomain()) {
//         return;
//     }
// 
//     this->GetPointArraySelection()->Merge(*this->PointArraysCache);
//     this->GetCellArraySelection()->Merge(*this->CellArraysCache);
//     this->GetGridSelection()->Merge(*this->GridsCache);
// 
//     // Clear the cache.
//     this->PointArraysCache->clear();
//     this->CellArraysCache->clear();
//     this->GridsCache->clear();
//}

//----------------------------------------------------------------------------
void vtkPXDMFReader::PrintSelf(ostream& os, vtkIndent indent) {
    this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
void vtkPXDMFReader::SetPXDMFDimsPosDataInfo(int id , double value) {

    this->PrepareDocument();
    unsigned cpt =0;
    if (vtkPXDMFDimsPosData->GetSize() == 0) {

        for (unsigned i = 0 ; int(i) < this->GetNumberOfPXDMFDims(); ++i) {
            cpt += this->GetNumberOfDimsInEachPXDMFDim()->GetValue(i);
        }
        vtkPXDMFDimsPosData->SetNumberOfComponents(1);
        vtkPXDMFDimsPosData->SetNumberOfValues(cpt);
    }
    vtkPXDMFDimsPosData->SetValue(id, value);

    cpt = 0;
    for (unsigned i =0; int(i) < this->GetNumberOfPXDMFDims(); ++i) {
        for (unsigned j =0; int(j) < this->GetNumberOfDimsInEachPXDMFDim()->GetValue(i); ++j) {
            this->PosOfEachDimension[i][j] = vtkPXDMFDimsPosData->GetValue(cpt);
            ++cpt ;
        }
    }
    this->Modified();
};

//----------------------------------------------------------------------------
void vtkPXDMFReader::SetPXDMFDimsPosDataInfoByName(const char* DimName, double value) {
    int cpt =0;
    int index =-1;
    for (unsigned i =0; int(i) < this->GetNumberOfPXDMFDims(); ++i) {
        for (unsigned j =0; int(j) < this->GetNumberOfDimsInEachPXDMFDim()->GetValue(i); ++j) {
            if (!strcmp(DimName, this->GetNamesOfDimension(i,j).c_str())) {
                index = cpt;
            }
            ++cpt;
        }
    }
    if (index > -1) {
        SetPXDMFDimsPosDataInfo(index, value);
    } else {
        std::cout << "WARNING : Dimension not found" << std::endl;
    }
};

//----------------------------------------------------------------------------
vtkDoubleArray* vtkPXDMFReader::GetPXDMFDimsMinRangeDataInfo() {
    unsigned cpt =0;
    if (vtkPXDMFDimsMinRangeData->GetSize() == 0) {
        for (unsigned i = 0 ; int(i)<  this->GetNumberOfPXDMFDims(); ++i) {
            cpt += this->GetNumberOfDimsInEachPXDMFDim()->GetValue(i);
        }
        vtkPXDMFDimsMinRangeData->SetNumberOfComponents(1);
        vtkPXDMFDimsMinRangeData->SetNumberOfValues(cpt);
    }
    cpt=0;
    for (unsigned i = 0 ; int(i) < this->GetNumberOfPXDMFDims(); ++i) {
        for (unsigned j = 0 ; int(j) < this->GetNumberOfDimsInEachPXDMFDim()->GetValue(i); ++j) {
            vtkPXDMFDimsMinRangeData->SetValue(cpt, MinOfEachDimension[i][j]);
            ++cpt;
        }
    }
    return vtkPXDMFDimsMinRangeData;
};

//----------------------------------------------------------------------------
vtkDoubleArray* vtkPXDMFReader::GetPXDMFDimsMaxRangeDataInfo() {
    unsigned cpt =0;
    
    if (vtkPXDMFDimsMaxRangeData->GetSize() == 0) {
    
        for (unsigned i = 0 ; int(i) < this->GetNumberOfPXDMFDims(); ++i) {
            cpt += this->GetNumberOfDimsInEachPXDMFDim()->GetValue(i);
        }
        vtkPXDMFDimsMaxRangeData->SetNumberOfComponents(1);
        vtkPXDMFDimsMaxRangeData->SetNumberOfValues(cpt);
    } 
    cpt=0;
    for (unsigned i = 0 ; int(i) < this->GetNumberOfPXDMFDims(); ++i) {
        for (unsigned j = 0 ; int(j) < this->GetNumberOfDimsInEachPXDMFDim()->GetValue(i); ++j) {
            vtkPXDMFDimsMaxRangeData->SetValue(cpt, MaxOfEachDimension[i][j]);
            ++cpt;
        }
    }    
    return vtkPXDMFDimsMaxRangeData;
};

//----------------------------------------------------------------------------
vtkDoubleArray* vtkPXDMFReader::GetPXDMFDimsPosDataInfo() {
    if (vtkPXDMFDimsPosData->GetSize() == 0) {
        unsigned cpt =0;

        for (unsigned i = 0 ; int(i) < this->GetNumberOfPXDMFDims(); ++i) {
            cpt += this->GetNumberOfDimsInEachPXDMFDim()->GetValue(i);
        }
        vtkPXDMFDimsPosData->SetNumberOfComponents(1);
        vtkPXDMFDimsPosData->SetNumberOfValues(cpt);
    } 
    unsigned  cpt=0;
    for (unsigned i = 0 ; int(i) < this->GetNumberOfPXDMFDims(); ++i) {
        for (unsigned j = 0 ; int(j) < this->GetNumberOfDimsInEachPXDMFDim()->GetValue(i); ++j) {
            vtkPXDMFDimsPosData->SetValue(cpt, PosOfEachDimension[i][j]);
            ++cpt;
        }
    }
    return vtkPXDMFDimsPosData;
};
