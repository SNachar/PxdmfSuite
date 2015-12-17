#include "XDMF2Data.h"

// ParaView/VTK Includes
#include "XdmfInformation.h"
#include "vtkDataSet.h"
#include "vtkXdmfReader.h"
#include "vtkXdmfHeavyData.h"
#include "vtkXdmfReaderInternal.h"



// plugin Includes.
#include "stringhelper.h"

XDMF2Data::XDMF2Data() {
    this->XdmfDocument = new vtkXdmfDocument;
    this->PreviouseDomain = NULL;
    this->vtkGrids.resize ( 0 );
    }
//
XDMF2Data::~XDMF2Data() {
    delete XdmfDocument;
    XdmfDocument = NULL;
    this->vtkGrids.resize(0);
    }
//
bool  XDMF2Data::Init ( char* filename ) {
    if ( !this->fileParsed ) {
        delete this->XdmfDocument;
        this->XdmfDocument = new vtkXdmfDocument;
        if ( !this->XdmfDocument->Parse ( filename ) ) {
            return false;
            }
        this->fileParsed = 1;
        }
        
    return true;
    }
//
bool XDMF2Data::SetActiveDommain ( char* domainName ) {
    if ( domainName ) {
        if ( !this->XdmfDocument->SetActiveDomain ( domainName ) ) {
            return false;
            }
        }
    else {
        this->XdmfDocument->SetActiveDomain ( static_cast<int> ( 0 ) );
        if ( this->XdmfDocument->GetActiveDomain() ) {
            return true;
            }
        else {
            return false;
            };
        }
    return true;
    };
//
bool XDMF2Data ::NeedUpdate() {
    vtkXdmfDomain * domain = this->XdmfDocument->GetActiveDomain();
    if ( this->PreviouseDomain  == domain ) return false;
    
    ++this->mtime;
    this->PreviouseDomain = domain;

    this->PXDMFDimensionXdmfGrids.resize ( 0 );
    this->vtkGrids.resize(0);
    
    NumberOfPXDMFDims = 0;
    int nb_grids = this->GetNumberOfGrids();
    for ( unsigned i=0; int ( i ) < nb_grids; ++i ) {
        xdmf2::XdmfGrid *Grid =  this->XdmfDocument->GetActiveDomain()->GetGrid ( i );
        //TODO erase this if
        if ( strncmp ( Grid->GetName(),"PGD",3 ) == 0 ) ++NumberOfPXDMFDims;
        }
    this->PXDMFDimensionXdmfGrids.resize ( NumberOfPXDMFDims );
    this->vtkGrids.resize( NumberOfPXDMFDims );
    for ( int i =0; i <nb_grids ; ++i ) {
        //std::cout << "cleaning grid "  << i << std::endl;
        this->PXDMFDimensionXdmfGrids[i] = NULL;
        }
    return true;
    };
//
GridInfo XDMF2Data ::GetGridData ( int j ) {
    
    GridInfo data;
    bool MetaDataNumberOfDimsPresent = 0;
    for ( unsigned i=0; int ( i ) < this->GetNumberOfGrids(); ++i ) {
        xdmf2::XdmfGrid *Grid =  this->XdmfDocument->GetActiveDomain()->GetGrid ( i );
        ///To find the dimension of this grid
        if ( strcmp ( Grid->GetName(), ( std::string ( "PGD" ) +to_string ( j+1 ) ).c_str() ) == 0 ) {
            data.name = ( std::string ( "PGD" ) +to_string ( j+1 ) );
            data.type = this->XdmfDocument->GetActiveDomain()->GetVTKDataType(Grid);
            //std::cout << "filling grid "  << i << std::endl;
            this->PXDMFDimensionXdmfGrids[j] = Grid;
            int nb_information = Grid->GetNumberOfInformations();
            for ( unsigned l = 0; int ( l ) < nb_information; ++l ) {
                if ( strcmp ( Grid->GetInformation ( l )->GetName(),"Dims" ) == 0 ) {

                    int dim= 0;
                    from_string ( Grid->GetInformation ( l )->GetValue(), dim );
                    MetaDataNumberOfDimsPresent = true;
                    data.dimensionality = std::max ( data.dimensionality,dim );
                    }
                }
            break;} 
        }
    data.coordNames.resize(data.dimensionality);
    data.coordUnits.resize(data.dimensionality);
    
    int nb_information = this->PXDMFDimensionXdmfGrids[j]->GetNumberOfInformations();
    for (unsigned i=0; int(i)< data.dimensionality; ++i) {
        //UnitsOfEachDimension[j][i] = "none";
        for (unsigned l = 0; int(l) < nb_information; ++l) {
               if (strcmp (this->PXDMFDimensionXdmfGrids[j]->GetInformation(l)->GetName(),(std::string("Dim")+to_string(i)).c_str()) == 0) {
                  data.coordNames[i] = this->PXDMFDimensionXdmfGrids[j]->GetInformation(l)->GetValue();
                }
                if (strcmp (this->PXDMFDimensionXdmfGrids[j]->GetInformation(l)->GetName(),(std::string("Unit")+to_string(i)).c_str()) == 0) {
                  data.coordUnits[i] = this->PXDMFDimensionXdmfGrids[j]->GetInformation(l)->GetValue();
                }
        }
        this->PXDMFDimensionXdmfGrids[j] ->GetGeometry()->UpdateInformation();
        this->PXDMFDimensionXdmfGrids[j] ->GetGeometry()->Update();
        

    }
    this->CalculateMaxMin(j,data);
    for (unsigned i=0; int(i)< data.dimensionality; ++i) {
              if (!MetaDataNumberOfDimsPresent  ){
          if (data.maxs[i]==data.mins[i]){ 
            data.dimensionality = i;
          }else{ 
            std::string dimname ;
            switch(i){
              case(0): dimname = "X"; break;
              case(1): dimname = "Y"; break;
              case(2): dimname = "Z"; break;
            }
            data.coordNames[i] = "dim " + to_string(j) +" " + dimname ;
          }
        }
    }
    
      
      
      return data;
    };
//
int XDMF2Data ::GetNumberOfGrids() {
    return this->PreviouseDomain->GetNumberOfGrids();
};
//
void  XDMF2Data ::CalculateMaxMin(const unsigned j, GridInfo& data) {
    
    data.mins.resize(data.dimensionality);
    data.maxs.resize(data.dimensionality);
    
    int grid_type = this->PreviouseDomain->GetVTKDataType(this->PreviouseDomain->GetGrid(j));
    switch (grid_type) {
    case VTK_IMAGE_DATA :
    case VTK_UNIFORM_GRID: {
        for (unsigned i =0; int(i) < data.dimensionality; ++i) {
            data.maxs[i]=this->PXDMFDimensionXdmfGrids[j]->GetGeometry()->GetOrigin()[2-i] + this->PXDMFDimensionXdmfGrids[j]->GetGeometry()->GetDxDyDz()[2-i]*(this->PXDMFDimensionXdmfGrids[j]->GetTopology()->GetShapeDesc()->GetDimension(2-i)-1);
            data.mins[i]=this->PXDMFDimensionXdmfGrids[j]->GetGeometry()->GetOrigin()[2-i];
            data.origin[i] = this->PXDMFDimensionXdmfGrids[j]->GetGeometry()->GetOrigin()[2-i];
            data.spacing[i] = this->PXDMFDimensionXdmfGrids[j]->GetGeometry()->GetDxDyDz()[2-i];
            data.extents[i] = (this->PXDMFDimensionXdmfGrids[j]->GetTopology()->GetShapeDesc()->GetDimension(2-i)-1);
        }
        break;
    }
    case VTK_RECTILINEAR_GRID: {
        switch(data.dimensionality){
          case (3):{
            data.maxs[2]=this->PXDMFDimensionXdmfGrids[j]->GetGeometry()->GetVectorZ()->GetValueAsFloat64(this->PXDMFDimensionXdmfGrids[j]->GetGeometry()->GetVectorZ()->GetNumberOfElements()-1);
            data.mins[2]=this->PXDMFDimensionXdmfGrids[j]->GetGeometry()->GetVectorZ()->GetValueAsFloat64(0);      
          } case(2): {   
            data.maxs[1]=this->PXDMFDimensionXdmfGrids[j]->GetGeometry()->GetVectorY()->GetValueAsFloat64(this->PXDMFDimensionXdmfGrids[j]->GetGeometry()->GetVectorY()->GetNumberOfElements()-1);
            data.mins[1]=this->PXDMFDimensionXdmfGrids[j]->GetGeometry()->GetVectorY()->GetValueAsFloat64(0);
          }case (1):{  
            data.maxs[0] = this->PXDMFDimensionXdmfGrids[j]->GetGeometry()->GetVectorX()->GetValueAsFloat64(this->PXDMFDimensionXdmfGrids[j]->GetGeometry()->GetVectorX()->GetNumberOfElements()-1);
            data.mins[0] = this->PXDMFDimensionXdmfGrids[j]->GetGeometry()->GetVectorX()->GetValueAsFloat64(0);
          }
        }
        break;
    } case VTK_STRUCTURED_GRID: {
    } case VTK_UNSTRUCTURED_GRID: {

        for (unsigned i =0; int(i) < data.dimensionality; ++i) {
            double min = this->PXDMFDimensionXdmfGrids[j]->GetGeometry()->GetPoints()->GetValueAsFloat64(i);
            double max = this->PXDMFDimensionXdmfGrids[j]->GetGeometry()->GetPoints()->GetValueAsFloat64(i);
            for (unsigned k = i+3; k <  this->PXDMFDimensionXdmfGrids[j]->GetGeometry()->GetPoints()->GetNumberOfElements(); k=k+3 ) {
                double temp = this->PXDMFDimensionXdmfGrids[j]->GetGeometry()->GetPoints()->GetValueAsFloat64(k);
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
        }
        break;
    }
    }
}
//
int XDMF2Data::GetNumberOfAttributes(){ 
  return this->PXDMFDimensionXdmfGrids[0]->GetNumberOfAttributes();
}
//
AttributeInfo XDMF2Data::GetAttributeData(int j ){
  AttributeInfo data;
  data.name =   this->PXDMFDimensionXdmfGrids[0]->GetAttribute(j)->GetName();
  data.center = this->PXDMFDimensionXdmfGrids[0]->GetAttribute(j)->GetAttributeCenter();
  return data;
};
//
vtkDataSet* XDMF2Data::GetVTKDataSet(int i, int* stride, int* extents) {

  vtkXdmfReader* This = vtkXdmfReader::New();
  vtkDataSet* data ;

  {
    vtkXdmfHeavyData dataReader(this->XdmfDocument->GetActiveDomain(), This);

    unsigned int updatePiece = 0;
    unsigned int updateNumPieces = 1;
    int ghost_levels = 0;
    dataReader.Piece = updatePiece;
    dataReader.NumberOfPieces = updateNumPieces;
    dataReader.GhostLevels = ghost_levels;

    dataReader.Stride[0] = stride[0];
    dataReader.Stride[1] = stride[1];
    dataReader.Stride[2] = stride[2];
    dataReader.Extents[0] = extents[0];
    dataReader.Extents[1] = extents[1];
    dataReader.Extents[2] = extents[2];
    dataReader.Extents[3] = extents[3];
    dataReader.Extents[4] = extents[4];
    dataReader.Extents[5] = extents[5];
    data = vtkDataSet::SafeDownCast(dataReader.ReadData(this->PXDMFDimensionXdmfGrids[i]));
    vtkGrids[i]  = vtkSmartPointer<vtkDataSet>::Take(data);
  }
  This->Delete();
  return data;
}
void XDMF2Data::ForceUpdate(){
  this->fileParsed = false;
  this->changeInSelectedPointCellArrays = true;
  this->PreviouseDomain = NULL;
  this->vtkGrids.resize(0);
};

int XDMF2Data::GetDomainVTKDataType(){
  int outputType= this->XdmfDocument->GetActiveDomain()->GetVTKDataType();
  if (this->XdmfDocument->GetActiveDomain()->GetSetsSelection()-> GetNumberOfArrays() > 0){
  // if the data has any sets, then we are forced to using multiblock.
    outputType = VTK_MULTIBLOCK_DATA_SET;
  }
  return outputType;
};
//
void XDMF2Data::SetPointArraySelection(const char* name ,bool status ){
  if(this->XdmfDocument && this->XdmfDocument->GetActiveDomain() && this->XdmfDocument->GetActiveDomain()->GetPointArraySelection()->HasArray(name))
    this->XdmfDocument->GetActiveDomain()->GetPointArraySelection()->SetArrayStatus(name, status);
};
//
bool XDMF2Data::GetPointArraySelection(const char * name){
  if(this->XdmfDocument && this->XdmfDocument->GetActiveDomain() &&  this->XdmfDocument->GetActiveDomain()->GetPointArraySelection()->HasArray(name))
    return this->XdmfDocument->GetActiveDomain()->GetPointArraySelection()->GetArraySetting(name);
  // by defautl we return true
  return true;
}
//
const char* XDMF2Data::GetPointArrayName(int& index){
  return this->XdmfDocument->GetActiveDomain()->GetPointArraySelection()->GetArrayName(index);
};
//
int XDMF2Data::GetNumberOfPointArrays(){
  return this->XdmfDocument->GetActiveDomain()->GetPointArraySelection()->GetNumberOfArrays();
}
//
void XDMF2Data::SetCellArraySelection(const char* name ,bool status ){
    if(this->XdmfDocument)
      if(this->XdmfDocument->GetActiveDomain())
        this->XdmfDocument->GetActiveDomain()->GetCellArraySelection()->SetArrayStatus(name, status );
};
//
int XDMF2Data::GetNumberOfCellArrays(){
    return this->XdmfDocument->GetActiveDomain()->GetCellArraySelection()->GetNumberOfArrays();
};
//
bool XDMF2Data::GetCellArraySelection(const char * name){
  if(this->XdmfDocument)
    if(this->XdmfDocument->GetActiveDomain())
      if(this->XdmfDocument->GetActiveDomain()->GetCellArraySelection()->HasArray(name))
        return this->XdmfDocument->GetActiveDomain()->GetCellArraySelection()->GetArraySetting(name);
  // by defautl we return true
  return true;
};
//
const char* XDMF2Data::GetCellArrayName(int& index){
    return this->XdmfDocument->GetActiveDomain()->GetCellArraySelection()->GetArrayName(index);
};
//


