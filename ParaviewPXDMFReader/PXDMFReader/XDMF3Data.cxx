#include "XDMF3Data.h"

#include "stringhelper.h"
#include "vtkType.h"
#include "vtkUnstructuredGrid.h"
#include "vtkUniformGrid.h"
#include "vtkRectilinearGrid.h"
#include "vtkStructuredGrid.h"
#include "vtkXdmf3DataSet.h"
#include "vtkXdmf3ArraySelection.h"
#include "vtkXdmf3DataSet.h"
#include "vtkXdmf3LightDataHandler.h"
#include "vtkXdmf3SILBuilder.h"

#include "XdmfUnstructuredGrid.hpp"
#include "XdmfRegularGrid.hpp"
#include "XdmfDomain.hpp"
#include "XdmfReader.hpp"
#include "XdmfRectilinearGrid.hpp"
#include "XdmfInformation.hpp"
#include "XdmfGeometry.hpp"
#include "XdmfCurvilinearGrid.hpp"
#include "XdmfGridCollection.hpp"

//#define GEM_DEBUG(x) x
#define GEM_DEBUG(x) 
 
XDMF3Data::XDMF3Data() {
    this->xdmfReader = XdmfReader::New();               // delete ok (shared_ptr)
    this->xdmfGrids.resize ( 0 );
    this->vtkGrids.resize ( 0 );
    this->FieldArrays = new vtkXdmf3ArraySelection;
    this->CellArrays = new vtkXdmf3ArraySelection;
    this->PointArrays = new vtkXdmf3ArraySelection;
    }
//
XDMF3Data::~XDMF3Data() {
  this->vtkGrids.resize(0);
  delete this->FieldArrays;
  delete this->CellArrays;
  delete this->PointArrays;
}
//
bool XDMF3Data::Init ( char* filename ) {
    if ( !this->fileParsed ) {
        this->xdmfReader = XdmfReader::New();               // delete ok (shared_ptr)
        try{
          this->Domain = shared_dynamic_cast<XdmfDomain> ( this->xdmfReader->read ( filename ) );
        } catch( const std::exception & e )  {
           vtkOStrStreamWrapper vtkmsg;  
           vtkmsg << "*********************************************************************************************** \n";
           vtkmsg << "******  Be sure to update your pxdmf file to the new version using the updatepxdmf tool  ****** \n";
           vtkmsg << "*********************************************************************************************** \n";
           vtkmsg << "Failed to parse xdmf file: " << filename << "\n";
           vtkmsg << "The error from the parser is : " << e.what() << "\n";
           vtkOutputWindowDisplayErrorText(vtkmsg.str());
            return 0;
        }
        if ( !this->Domain ) return false;
        
        unsigned int updatePiece = 0;
        unsigned int updateNumPieces = 1;
        vtkXdmf3SILBuilder SILBuilder;
        SILBuilder.Initialize();
        vtkXdmf3ArraySelection GridsCache;
        vtkXdmf3ArraySelection SetsCache;
        shared_ptr<vtkXdmf3LightDataHandler> visitor = vtkXdmf3LightDataHandler::New (               // delete ok (shared_ptr)
              &SILBuilder,
              this->FieldArrays,
              this->CellArrays,
              this->PointArrays,
              &GridsCache,
              &SetsCache,
              updatePiece,
              updateNumPieces);
        visitor->InspectXDMF(this->Domain, -1);
        this->fileParsed = 1;
        int ii = this->GetNumberOfAttributes();
        }
    return true;
    }
//
bool XDMF3Data ::NeedUpdate() {
    GEM_DEBUG(std::cout << "NeedUpdate " << this->xdmfGrids.size()<< std::endl;)
    if ( this->xdmfGrids.size() > 0 ) return false;

    this->xdmfGrids.resize ( 0 );
    this->vtkGrids.resize(0);
    
    this->gridsInfo.resize ( 0 );
    NumberOfPXDMFDims = this->GetNumberOfGrids();
    /// in the new backend we read all the grids not only the PGDX grids
    /// please se the function XDMF2Data ::NeedUpdate for more info
    this->xdmfGrids.resize ( NumberOfPXDMFDims );
    this->vtkGrids.resize( NumberOfPXDMFDims );
    this->gridsInfo.resize (NumberOfPXDMFDims );
    for ( int i =0; i <GetNumberOfGrids() ; ++i ) {
        this->xdmfGrids[i] = shared_ptr<XdmfGrid>();
        this->gridsInfo[i].dimensionality = 0;
    }
    
    return true;
};
//
void XDMF3Data::PoputateGridData (int j , GridInfo& data, shared_ptr<XdmfGrid> grid ) {
 
  data.name = grid->getName();
    shared_ptr <XdmfInformation > info = grid->getInformation ( "Dims" );

    bool MetaDataNumberOfDimsPresent = false;
    if ( info ) {
        MetaDataNumberOfDimsPresent = true;

        from_string ( info->getValue(),data.dimensionality );

        data.coordNames.resize ( data.dimensionality );
        data.coordUnits.resize ( data.dimensionality );

        for ( int i = 0; i < data.dimensionality; i++ ) {
            data.coordNames[i] = grid->getInformation ( ( std::string ( "Dim" ) +to_string ( i ) ).c_str() )->getValue();
            shared_ptr <XdmfInformation >  unitInfo = grid->getInformation ( ( std::string ( "Unit" ) +to_string ( i ) ).c_str() );
            if (unitInfo)
              data.coordUnits[i] = unitInfo->getValue();
            }
    } else {
      /// to metadata
      data.coordNames.resize ( 3 );
      data.coordUnits.resize ( 3 );
    }
    
    data.mins.resize ( 3 );
    data.maxs.resize ( 3 );
    
    CalculateMaxMin(grid, data);
    
    for (unsigned i=0; int(i)< data.dimensionality; ++i) {
      if ( !MetaDataNumberOfDimsPresent ) {
        if ( data.maxs[i]==data.mins[i] ) {
            data.dimensionality = i;
        }
        else {
            std::string dimname ;
            switch ( i ) {
                case ( 0 ) : dimname = "X"; break;
                case ( 1 ) : dimname = "Y"; break;
                case ( 2 ) : dimname = "Z"; break;
                }
            data.coordNames[i] = "C" + to_string ( j ) +" " + dimname ;
        }
      }
    }
 
}
//
shared_ptr <XdmfGrid > XDMF3Data::GetGridRecursive (shared_ptr <XdmfGridCollection > fgrid, int& type, int& j ) {
  
    int nUnstructuredGrids = fgrid->getNumberUnstructuredGrids();
    int nRectilinearGrids = fgrid->getNumberRectilinearGrids();
    int nCurvilinearGrids= fgrid->getNumberCurvilinearGrids();
    int nRegularGrids = fgrid->getNumberRegularGrids();
    int nGridCollections = fgrid->getNumberGridCollections();

    shared_ptr<XdmfGrid> grid;

    if ( j < nUnstructuredGrids ) {
        grid = fgrid->getUnstructuredGrid ( j );
        type = VTK_UNSTRUCTURED_GRID;
    } else {
        j = j - nUnstructuredGrids;
        if ( j < nRectilinearGrids ) {
            grid = fgrid->getRectilinearGrid ( j );
            type = VTK_RECTILINEAR_GRID;
        } else {
          j = j - nRectilinearGrids;
          if ( j < nCurvilinearGrids ) {
            grid = fgrid->getCurvilinearGrid ( j );
            type = VTK_STRUCTURED_GRID;
            } else { 
              j = j - nCurvilinearGrids;
              if ( j < nRegularGrids ) {
                grid = fgrid->getRegularGrid ( j );
                type = VTK_UNIFORM_GRID;
              } else {
                  j = j- nRegularGrids;
                  if(nGridCollections ){
                    for(int c = 0; c < nGridCollections; ++c){
                      grid = GetGridRecursive (fgrid->getGridCollection ( c ),  type, j ) ;
                    }
                  }
                
              }
            }
        }
    }
    return grid;
}
//
GridInfo XDMF3Data::GetGridData ( int j ) {
    
    if(this->gridsInfo[j].dimensionality != 0) return this->gridsInfo[j];
    
    GEM_DEBUG(std::cout << "GetGridData" << std::endl;)

    GridInfo& data= this->gridsInfo[j];
    data.dimensionality = 3;
    int nUnstructuredGrids = this->Domain->getNumberUnstructuredGrids();
    int nRectilinearGrids = this->Domain->getNumberRectilinearGrids();
    int nCurvilinearGrids= this->Domain->getNumberCurvilinearGrids();
    int nRegularGrids = this->Domain->getNumberRegularGrids();
    int nGridCollections = this->Domain->getNumberGridCollections();

    shared_ptr<XdmfGrid> grid;

    if ( j < nUnstructuredGrids ) {
        grid = this->Domain->getUnstructuredGrid ( j );
        data.type = VTK_UNSTRUCTURED_GRID;
    } else {
        j = j - nUnstructuredGrids;
        if ( j < nRectilinearGrids ) {
            grid = this->Domain->getRectilinearGrid ( j );
            data.type = VTK_RECTILINEAR_GRID;
        } else {
          j = j - nRectilinearGrids;
          if ( j < nCurvilinearGrids ) {
            grid = this->Domain->getCurvilinearGrid ( j );
            data.type = VTK_STRUCTURED_GRID;
            } else { 
              j = j - nCurvilinearGrids;
              if ( j < nRegularGrids ) {
                grid = this->Domain->getRegularGrid ( j );
                data.type = VTK_UNIFORM_GRID;
              } else {
                  j = j- nRegularGrids;
                  if(nGridCollections ){
                    for(int c=0; c < nGridCollections;++c){
                      grid =  GetGridRecursive( this->Domain->getGridCollection ( c ),data.type ,  j );
                      }
                  }
                
              }
            }
        }
    }

    this->xdmfGrids[j] = grid;
    
    PoputateGridData ( j, data, grid ) ;
    
    return data;
    };
//
int XDMF3Data::GetNumberOfGridsRecursive(boost::shared_ptr<XdmfGridCollection > group){
  
  int res =  0;
  
  res += group->getNumberUnstructuredGrids();
  res += group->getNumberRectilinearGrids();
  res += group->getNumberCurvilinearGrids();
  res += group->getNumberRegularGrids();
  int nCollectionGrids = group->getNumberGridCollections();
  
  for(int i =0; i < nCollectionGrids; ++i){
    res += GetNumberOfGridsRecursive(group->getGridCollection( i ));
  };

  return res;
  
};
int XDMF3Data::GetNumberOfGrids() {

    unsigned res  = 0;
    res += this->Domain->getNumberUnstructuredGrids();
    res += this->Domain->getNumberRectilinearGrids();
    res += this->Domain->getNumberCurvilinearGrids();
    res += this->Domain->getNumberRegularGrids();

    int nCollectionGrids = this->Domain->getNumberGridCollections();
    for(int i =0; i < nCollectionGrids; ++i){
      res += GetNumberOfGridsRecursive(this->Domain->getGridCollection ( i ));
    };
    //std:cout << "GetNumberOfGrids :  " << res << std::endl;
    return res;
    }
//
vtkDataSet* XDMF3Data::GetVTKDataSet ( int i,int* stride, int* extents ) {
    GEM_DEBUG(std::cout << "GetVTKDataSet" << std::endl;)
    
    GridInfo info = this->GetGridData (i);
    
    vtkDataSet* data;
    switch(info.type){
      case(VTK_UNSTRUCTURED_GRID):{
         vtkUnstructuredGrid* udata = vtkUnstructuredGrid::New();                       // delete ok (vtkSmartPointer)
         vtkXdmf3DataSet::XdmfToVTK(FieldArrays, CellArrays, PointArrays, 
              boost::dynamic_pointer_cast<XdmfUnstructuredGrid>(this->xdmfGrids[i]).get(), udata);
         data = udata;
         break;
      } case(VTK_RECTILINEAR_GRID):{
         vtkRectilinearGrid* rdata = vtkRectilinearGrid::New();                         // delete ok (vtkSmartPointer)
         vtkXdmf3DataSet::XdmfToVTK(FieldArrays, CellArrays, PointArrays, 
              boost::dynamic_pointer_cast<XdmfRectilinearGrid>(this->xdmfGrids[i]).get(), rdata);
         data = rdata;
         break;
      } case(VTK_STRUCTURED_GRID):{
         vtkStructuredGrid* sdata = vtkStructuredGrid::New();                           // delete ok (vtkSmartPointer)
         vtkXdmf3DataSet::XdmfToVTK(FieldArrays, CellArrays, PointArrays, 
              boost::dynamic_pointer_cast<XdmfCurvilinearGrid>(this->xdmfGrids[i]).get(), sdata);
         data = sdata;
         break;
      } case(VTK_IMAGE_DATA):
      case(VTK_UNIFORM_GRID):{
         vtkUniformGrid* udata = vtkUniformGrid::New();                                 // delete ok (vtkSmartPointer)
         vtkXdmf3DataSet::XdmfToVTK(FieldArrays, CellArrays, PointArrays, 
              boost::dynamic_pointer_cast<XdmfRegularGrid>(this->xdmfGrids[i]).get(), udata);
         data = udata;          
         break;
      }
    }
    vtkGrids[i]  = vtkSmartPointer<vtkDataSet>::Take(data);
    return data;  
    };
    //
void XDMF3Data::ForceUpdate() {
    std::cout << "ForceUpdate" << std::endl;
    this->fileParsed = false;
    this->changeInSelectedPointCellArrays = true;
    this->xdmfGrids.resize(0);
    this->vtkGrids.resize(0);
    this->gridsInfo.resize(0);
    };

int XDMF3Data::GetDomainVTKDataType() {
  GEM_DEBUG(std::cout << "XDMF3Data::GetDomainVTKDataType" << std::endl);
  if ( this->GetNumberOfGrids() > 1 ) {
    return VTK_MULTIBLOCK_DATA_SET;
  } else {
    return GetGridData ( 0 ).type;
  }
};
//
int XDMF3Data::GetNumberOfAttributes() {
    GEM_DEBUG(std::cout << "GetNumberOfAttributes " << GetNumberOfPointArrays() + GetNumberOfCellArrays() << std::endl;)
    return GetNumberOfPointArrays() + GetNumberOfCellArrays();
};
//
AttributeInfo XDMF3Data::GetAttributeData ( int j ) {
  GEM_DEBUG(std::cout << "GetAttributeData" << std::endl;)
  AttributeInfo data;
  
  if ( j < GetNumberOfPointArrays() ) {
      data.name = GetPointArrayName(j);
      data.center = XDMF_ATTRIBUTE_CENTER_NODE;
  } else {
    j -= GetNumberOfPointArrays();
    if ( j < GetNumberOfCellArrays() ) {
      data.name = GetCellArrayName(j );
      data.center = XDMF_ATTRIBUTE_CENTER_CELL;
    }
  }
  return data;
};
//
int XDMF3Data::GetNumberOfPointArrays() {
    //std::cout << "GetNumberOfPointArrays" << std::endl;
    return this->PointArrays->GetNumberOfArrays();
    };
    
void XDMF3Data::SetPointArraySelection ( const char* name,bool status) {
  //std::cout << "SetPointArraySelection" << std::endl;
  //std::cout << "name  "<< name << "  status"<<  status << std::endl;
  this->PointArrays->SetArrayStatus(name, status);
};
//
bool XDMF3Data::GetPointArraySelection ( const char * name ) {
  //std::cout << "GetPointArraySelection" << std::endl;
  if(this->PointArrays->HasArray(name)) 
    return this->PointArrays->GetArraySetting(name);
  return true;   ///< by defautl we return true
};
//
const char* XDMF3Data::GetPointArrayName ( int& index ) {
  //std::cout << "GetPointArrayName" << std::endl;
  return this->PointArrays->GetArrayName(index);
};
//    
int XDMF3Data::GetNumberOfCellArrays() {
  //std::cout << "GetNumberOfCellArrays" << std::endl;
  return this->CellArrays->GetNumberOfArrays();
};
//
void XDMF3Data::SetCellArraySelection ( const char* name ,bool status) {
  //std::cout << "SetCellArraySelection" << std::endl;
  this->CellArrays->SetArrayStatus(name, status );
};
//
bool XDMF3Data::GetCellArraySelection ( const char * name ) {
  //std::cout << "GetCellArraySelection" << std::endl;
  if(this->CellArrays->HasArray(name)) 
    return this->CellArrays->GetArraySetting(name);
  return true;  ///< by default we return true
};
//
const char* XDMF3Data::GetCellArrayName ( int& index ) {
  //std::cout << "GetCellArrayName" << std::endl;
  return this->CellArrays->GetArrayName(index);
};
//
bool XDMF3Data::SetActiveDommain ( char* domainName ) {
    return true;
    };
//
void  XDMF3Data ::CalculateMaxMin(shared_ptr <XdmfGrid >& grid, GridInfo& data) {
   GEM_DEBUG(std::cout << "CalculateMaxMin" << std::endl;)
   
   switch(data.type){
     case(VTK_STRUCTURED_GRID):
     case(VTK_UNSTRUCTURED_GRID):{
       shared_ptr<XdmfUnstructuredGrid > ugrid = shared_dynamic_cast<XdmfUnstructuredGrid>(grid);
       
       
       const int nbpoints = grid->getGeometry()->getNumberPoints();
       
       vtkDataArray *vPoints = NULL;
       shared_ptr<XdmfGeometry > geo = ugrid->getGeometry() ;
       vPoints = vtkXdmf3DataSet::XdmfToVTKArray(geo.get(), "", 3);
       
       for (unsigned i =0; int(i) < data.dimensionality; ++i) {
            double min = vPoints->GetComponent(0,i);
            double max = min;
            for (unsigned k = i; k <  nbpoints; k++ ) {
                double temp = vPoints->GetComponent(k,i);
                if ( min > temp ) {
                    min = temp;
                } else {
                    if (max< temp )  {
                        max = temp;
                    }
                }
            }
            data.maxs[i]=max;
            data.mins[i]=min;
            GEM_DEBUG( std::cout << "data.maxs["<< i <<"]" << data.maxs[i] << std::endl;)
            GEM_DEBUG( std::cout << "data.mins["<< i <<"]" << data.mins[i]<< std::endl;)
        }
       
       break;
    } 
    case VTK_IMAGE_DATA :
    case VTK_UNIFORM_GRID: {
        shared_ptr<XdmfRegularGrid> rgrid =  shared_dynamic_cast<XdmfRegularGrid>(grid);
        for (unsigned i =0; int(i) < data.dimensionality; ++i) {
          double origin = rgrid->getOrigin()->getValue<double>(2-i) ;
          double delta = rgrid->getBrickSize()->getValue<double>(2-i) ;
          int n = rgrid->getDimensions()->getValue<int>(2-i) ;
          std::cout << "grid  n " << n << std::endl;
          data.maxs[i] = origin+delta*(n-1);
          data.mins[i] = origin;
          
          data.origin[i] = origin;
          data.spacing[i] = delta;
          data.extents[i] = n-1;
        }
        break;
    }
    case VTK_RECTILINEAR_GRID: {
        shared_ptr<XdmfRectilinearGrid> rgrid = shared_dynamic_cast<XdmfRectilinearGrid>(grid);
        for (unsigned i =0 ; i < data.dimensionality; ++i){
          data.mins[i] = rgrid->getCoordinates(i)->getValue<double>(0);
          const int n = rgrid->getCoordinates(i)->getSize();
          data.maxs[i] = rgrid->getCoordinates(i)->getValue<double>(n-1);
        }
        break;
    }
    std::cout << "3 this type of mesh cant be treated. Sorry" << grid->getItemTag() << std::endl; 
    }
}












  
    