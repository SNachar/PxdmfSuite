/*=========================================================================

  Program:   PXDMFReader Plugin
  Module:    vtkPXDMFDocumentBaseStructure.cpp

  Copyright (c) GeM, Ecole Centrale Nantes.
  All rights reserved.
  Copyright: See COPYING file that comes with this distribution


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

//vtk inludes.
#include "vtkDoubleArray.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkUnstructuredGrid.h"
#include "vtkRectilinearGrid.h"
#include "vtkStructuredGrid.h"
#include "vtkUniformGrid.h"
#include <vtkPolyData.h>
#include "vtkInformation.h"
#include "vtkInformationKey.h"
#include "vtkInformationStringKey.h"
#include "vtkInformationDoubleVectorKey.h"
#include "vtkStringArray.h"
#include "vtkIdList.h"
#include "vtkCellArray.h"
#include "vtkFloatArray.h"
#include "vtkIntArray.h"

// PXDMFReader Pluing Includes
#include "blashelper.h"
#include "vtkPXDMFDocumentBaseStructure.h"
#include "stringhelper.h"
#include <vtkCharArray.h>
#include <vtkBitArray.h>

//#define GEM_DEBUG(x) x
#define GEM_DEBUG(x) 

bool vtkPXDMFDocumentBaseStructure::QUITE = false;
//
void vtkPXDMFDocumentBaseStructure::SetNumberOfPXDMFDims ( const int dims ) {
  this->NumberOfPXDMFDims = dims;

  this->AllData.resize ( dims );

  for ( int i = 0; i < dims; ++i ) {
      this->AllData[i] = NULL;
    }

  forceMeshUpdate = true;

  this->ddataarrays.resize ( dims );
  this->fdataarrays.resize ( dims );

  this->pointIdGrids = vtkSmartPointer<vtkIdList>::New();
  this->cellIdGrids = vtkSmartPointer<vtkIdList>::New();

  this->pointIdGrids->SetNumberOfIds ( dims );
  this->cellIdGrids->SetNumberOfIds ( dims );
  this->cellGrids.resize ( dims );
  this->cellweights.resize ( dims );
  this->cellpcoords.resize ( dims );

  for ( unsigned i = 0; int ( i ) < dims; ++i ) {
      this->pointIdGrids->SetId ( i, 0 );
      this->cellGrids[i] = 0;
      this->cellIdGrids->SetId ( i, 0 );
      this->cellweights[i].resize ( 50 ); // to handle  elements with 50 nodes
      this->cellpcoords[i].resize ( 3 );
    };

  this->NumberOfDimsInEachGrid->SetNumberOfComponents ( 1 );
  this->NumberOfDimsInEachGrid->SetNumberOfValues ( dims );
  this->MaxOfEachDimension.resize ( dims );
  this->MinOfEachDimension.resize ( dims );
  this->PosOfEachDimension.resize ( dims );
  this->NamesOfEachDimension.resize ( dims );
  this->UnitsOfEachDimension.resize ( dims );

  if ( this->ActivePXDMFDimsForSpace.size() != unsigned ( dims ) ) {
      this->ActivePXDMFDimsForSpace.resize ( dims );

      for ( int i = 0; i < dims; ++i ) {
          this->ActivePXDMFDimsForSpace[i] = 0;
        }
    }

  if ( this->ActivePXDMFDimForTime.size() != unsigned ( dims ) ) {
      this->ActivePXDMFDimForTime.resize ( dims );

      for ( int i = 0; i < dims; ++i ) {
          this->ActivePXDMFDimForTime[i] = 0;
        }
    }
} ;
//
int vtkPXDMFDocumentBaseStructure::GetNumberOfPXDMFDims() const {
  return this->NumberOfPXDMFDims;
};
//
vtkIntArray* vtkPXDMFDocumentBaseStructure::GetNumberOfDimsInEachPXDMFDim() {
  return this->NumberOfDimsInEachGrid;
}
//
std::string vtkPXDMFDocumentBaseStructure::GetNamesOfDimension ( const int grid, const int dim ) const {
  return this->NamesOfEachDimension[grid][dim];
};
//
std::string vtkPXDMFDocumentBaseStructure::GetUnitsOfDimension ( const int grid, const int dim ) {
  return this->UnitsOfEachDimension[grid][dim];
};
//
unsigned& vtkPXDMFDocumentBaseStructure::GetNumberOfModesOfPointData ( const vtkstd::string& _name ) {
  return this->NumberOfModesInEachPointField[_name];
};
//
unsigned& vtkPXDMFDocumentBaseStructure::GetNumberOfModesOfPointDataByNumber ( unsigned& nb ) {
  return this->NumberOfModesInEachPointFieldByNumber[nb];
};
//
unsigned& vtkPXDMFDocumentBaseStructure::GetNumberOfModesOfCellData ( const vtkstd::string& _name ) {
  return this->NumberOfModesInEachCellField[_name];
};
//
unsigned& vtkPXDMFDocumentBaseStructure::GetNumberOfModesOfCellDataByNumber ( unsigned& nb ) {
  return this->NumberOfModesInEachCellFieldByNumber[nb];
};
//
vtkstd::vector<char>& vtkPXDMFDocumentBaseStructure::GetActivePXDMFDimsForSpace() {
  return this->ActivePXDMFDimsForSpace;
};
//
vtkstd::vector<char>& vtkPXDMFDocumentBaseStructure::GetActivePXDMFDimForTime() {
  return this->ActivePXDMFDimForTime;
};
//
vtkstd::map<vtkstd::string, unsigned>&  vtkPXDMFDocumentBaseStructure::GetNumberOfModesOfPointData() {
  return this->NumberOfModesInEachPointField;
};
//
vtkstd::map<vtkstd::string, unsigned>& vtkPXDMFDocumentBaseStructure::GetNumberOfModesOfCellData() {
  return this->NumberOfModesInEachCellField;
};
//
vtkInformationKeyMacro ( vtkPXDMFDocumentBaseStructure, UNIT, String );
vtkInformationKeyMacro ( vtkPXDMFDocumentBaseStructure, NAME, String );
vtkInformationKeyMacro ( vtkPXDMFDocumentBaseStructure, BOUND_MIN_MAX_PXDMFDim_Dim, DoubleVector );
vtkInformationKeyMacro ( vtkPXDMFDocumentBaseStructure, UPDATE_FIXED_DIMENSIONS, DoubleVector );
//
int vtkPXDMFDocumentBaseStructure::ReadOutputType() {
  GEM_DEBUG( std::cout << "ReadOutputType " << std::endl;)

  int nb_grid = GetNumberOfPXDMFDims();
  this->VtkOutputType = -1;
  bool flagspacedim = false;

  for ( unsigned i = 0; int ( i ) < nb_grid; i++ ) {

      int grid_type = AllData[i]->GetDataObjectType();

      if ( GetActivePXDMFDimsForSpace() [i] == 0 ) continue;

      flagspacedim = true;
      GEM_DEBUG( std::cout << " AllData " << i << grid_type << std::endl;)

      switch ( grid_type ) {
        case VTK_IMAGE_DATA: {
          if ( this->VtkOutputType == -1 ) {
              this->VtkOutputType  = VTK_UNIFORM_GRID;
            }

          break;
        }
        case VTK_UNIFORM_GRID: {
          if ( this->VtkOutputType == -1 ) {
              this->VtkOutputType  = VTK_UNIFORM_GRID;
            }

          break;
        }
        case VTK_RECTILINEAR_GRID: {
          if ( this->VtkOutputType == -1 || this->VtkOutputType  == VTK_UNIFORM_GRID ) {
              this->VtkOutputType  = VTK_RECTILINEAR_GRID;
            }

          break;
        }
        case VTK_STRUCTURED_GRID: {
          if ( this->VtkOutputType == -1 || this->VtkOutputType  == VTK_UNIFORM_GRID || this->VtkOutputType  == VTK_RECTILINEAR_GRID ) {
              this->VtkOutputType  = VTK_STRUCTURED_GRID;
            }

          break;
        }
        case VTK_POLY_DATA:

          if ( this->VtkOutputType == -1 ) {
              this->VtkOutputType  = VTK_POLY_DATA;
            }
          else {
              this->VtkOutputType  = VTK_UNSTRUCTURED_GRID;
            }

          break;
        case VTK_UNSTRUCTURED_GRID: {
          if ( this->VtkOutputType == -1 || this->VtkOutputType == VTK_POLY_DATA || this->VtkOutputType == VTK_UNIFORM_GRID || this->VtkOutputType  == VTK_RECTILINEAR_GRID ||  this->VtkOutputType  == VTK_STRUCTURED_GRID ) {
              this->VtkOutputType  = VTK_UNSTRUCTURED_GRID;
            }

          break;
        }
        }
    }

  if ( !flagspacedim ) this->VtkOutputType = VTK_UNSTRUCTURED_GRID;

  return this->VtkOutputType;
}
//

//
vtkPXDMFDocumentBaseStructure::vtkPXDMFDocumentBaseStructure() {

  this->UseInterpolation  = false;
  this->ComputeDerivatives = false ;
  this->ExpandFields =  false ;

  this->LastTimeDimension = -1;
  this->forceMeshUpdate = true;

  this->haveTime = 0;

  this->NumberOfDimsInEachGrid = vtkSmartPointer<vtkIntArray>::New();
  this->NumberOfDimsInEachGrid->Initialize();

  for ( unsigned i = 0; i < 3; ++i ) {
      this->update_extent[2 * i] = 0;
      this->update_extent[2 * i + 1] = -1;
    }

  this->SetNumberOfPXDMFDims ( 0 );

  this->Points = vtkSmartPointer<vtkPoints>::Take ( vtkPoints::New ( VTK_DOUBLE ) );
  this->Cells = vtkSmartPointer<vtkCellArray> ::New();
  this->CellLocationsArray = vtkSmartPointer<vtkIdTypeArray>::New();
  this->CellTypesArray = vtkSmartPointer<vtkUnsignedCharArray>::New();

}

//
std::string vtkPXDMFDocumentBaseStructure::GetCellDataName ( unsigned number ) {
  TPointdata_name::iterator it = this->CellFieldData.begin();
  TPointdata_name::iterator fit = this->CellFieldData.end();
  std::vector<std::string> res;

  ///alocation and zero setting
  for ( ; it != fit; ++it ) {
      if ( number == 0 ) return it->first;

      --number;
    };

  return "none";
}

//
std::string vtkPXDMFDocumentBaseStructure::GetPointDataName ( unsigned number ) {
  TPointdata_name::iterator it = this->PointFieldData.begin();
  TPointdata_name::iterator fit = this->PointFieldData.end();
  std::vector<std::string> res;

  ///alocation and zero setting
  for ( ; it != fit; ++it ) {
      if ( number == 0 ) return it->first;

      --number;
    };

  return "none";
}
void vtkPXDMFDocumentBaseStructure::reconstruction ( vtkIdList* pointIdGrids, const vtkstd::vector<int>& spacedata, const unsigned fieldnumber , unsigned& mode,  vtkDoubleArray* field, const unsigned pointdata, std::vector<vtkCell*>* cellGrids , std::vector<std::vector<double> >* cellweights ) {

  if ( field == 0 )  return;

  std::vector<vtkDoubleArray*>& dataarrays = this->ddataarrays;

  unsigned sizes[3];
  
  for ( int i = 0; ( i < 3 ) && ( spacedata[i] >= 0 ); ++i ) {
      sizes[i] = ( pointdata ) ? this->AllData[spacedata[i]]->GetNumberOfPoints() :  this->AllData[spacedata[i]]->GetNumberOfCells();
    }

  for ( unsigned i = 0; int ( i ) < this->NumberOfPXDMFDims; ++i ) {
      if ( pointdata ) {
          dataarrays[i] = static_cast<vtkDoubleArray*> ( getArrayPoint ( fieldnumber, mode, i ) );;
        }
      else {
          dataarrays[i] = static_cast<vtkDoubleArray*> ( getArrayCell ( fieldnumber, mode, i ) );;

        }

      if ( dataarrays[i] == 0 && !this->ExpandFields ) {
          return;
        }
    }

  unsigned nb_Components = field->GetNumberOfComponents();
  double val = 1;
  double tmpval = 0;

  double one[6] = { 1., 1., 1., 1., 1., 1. };

  for ( unsigned Comp = 0; Comp < nb_Components; ++Comp ) {
      val = 1;

      for ( unsigned o = 0; int ( o ) < this->NumberOfPXDMFDims; ++o ) {
          if ( this->ActivePXDMFDimsForSpace[o] == 0 ) {
              if ( dataarrays[o] ) {
                  if ( this->UseInterpolation && pointdata ) {
                      int pn = ( *cellGrids ) [o]->GetNumberOfPoints();
                      tmpval = 0;

                      for ( int pp = 0; pp < pn; ++pp ) {
                          tmpval += dataarrays[o]->GetValue ( ( *cellGrids ) [o]->GetPointId ( pp ) * nb_Components + Comp ) * ( *cellweights ) [o][pp];
                        }

                      val *= tmpval;
                    }
                  else {
                      val *= dataarrays[o]->GetValue ( pointIdGrids->GetId ( o ) * nb_Components + Comp );
                    }
                }
              else {
                  //val *= 1; // no need to multiply by one
                }
            }
        }

      if ( val != 0. ) {
          if ( spacedata[0] >= 0 ) {
              vtkDoubleArray* dataarrays0 = dataarrays[spacedata[0]];
              double* data0;
              int inc0;

              if ( dataarrays0 ) {
                  data0 = dataarrays0->GetPointer ( 0 );
                  //nb_points1 = dataarrays0->GetNumberOfTuples();
                  inc0 = nb_Components;
                }
              else {

                  data0 = one;
                  inc0 = 0;
                }


              if ( spacedata[1] >= 0 ) {
                  vtkDoubleArray* dataarrays1 = dataarrays[spacedata[1]];
                  //int nb_points2;
                  double* data1;
                  int inc1;

                  if ( dataarrays1 ) {
                      data1 = dataarrays1->GetPointer ( 0 );
                      //nb_points2 = dataarrays1->GetNumberOfTuples();
                      inc1 = nb_Components;
                    }
                  else {
                      data1 = one;
                      //nb_points2 =1;
                      inc1 = 0;
                    }

                  if ( spacedata[2] >= 0 ) {
                      // tree dimension
                      vtkDoubleArray* dataarrays2 = dataarrays[spacedata[2]];

                      double* data2;
                      int inc2;

                      if ( dataarrays2 ) {
                          data2 = dataarrays2->GetPointer ( 0 );
                          inc2 = nb_Components;
                        }
                      else {
                          data2 = one;
                          inc2 = 0;
                        }

                      unsigned cpt = Comp;

                      for ( unsigned point1 = 0; int ( point1 ) < sizes[0]; ++point1 ) {
                          const double val_temp1 = val * data0[point1 * inc0 + Comp];

                          for ( unsigned point2 = 0; int ( point2 ) < sizes[1]; ++point2 ) {
                              const double val_temp2 = val_temp1 * dataarrays1->GetValue ( point2 * inc1 + Comp );
                              DAXPY ( sizes[2], val_temp2, data2 + Comp, inc2, ( double* ) field->GetPointer ( cpt ), nb_Components );
                              cpt = cpt + sizes[2] * nb_Components;
                            }
                        }



                    }
                  else {
                      // only two dimensions
                      unsigned cpt = Comp;

                      for ( unsigned point1 = 0; int ( point1 ) < sizes[0]; ++point1 ) {
                          const double val_temp2 = val * data0[point1 * inc0 + Comp];
                          DAXPY ( sizes[1], val_temp2, data1 + Comp, inc1, ( double* ) field->GetPointer ( cpt ), nb_Components );
                          cpt = cpt + sizes[1] * nb_Components;
                        }
                    }
                }
              else {
                  DAXPY ( sizes[0], val, &data0[Comp], inc0, ( double* ) field->GetPointer ( Comp ), nb_Components );
                }

            }
          else {
              * ( double* ) field->GetPointer ( Comp )  += val;
            }
        }
    }

};

//
vtkDataArray* vtkPXDMFDocumentBaseStructure::getArrayPoint ( const unsigned& name, const unsigned& mode, const unsigned& dim ) {
  vtkDataArray*& val = this->PointMap[dim][name][mode];

  if ( val != NULL ) {
      return val;
    }
  else {
      const std::string smode = to_string ( mode );
      const std::string cname = GetPointDataName ( name );
      std::string totalname = cname + "_" + smode;
      int index = -1;
      vtkDataArray* theArray = static_cast<vtkDataArray*> ( this->AllData[dim]->GetPointData()->GetAbstractArray ( totalname.c_str(), index ) );

      // if the mode is smaller than 10 we try to read the term with one zero padding (example myterm_03 )
      if ( index == -1 &&  mode < 10 ) {
          std::string totalname = cname + "_0" + smode;
          theArray = static_cast<vtkDataArray*> ( this->AllData[dim]->GetPointData()->GetAbstractArray ( totalname.c_str(), index ) );
        }

      if ( index == -1 ) return NULL;

      val = theArray;
      return  val;
    }
}

//
vtkDataArray* vtkPXDMFDocumentBaseStructure::getArrayCell ( const unsigned& name, const unsigned& mode, const  unsigned& dim ) {
  vtkDataArray*& val = this->CellMap[dim][name][mode];

  if ( val ) {
      return val;
    }
  else {
      const std::string smode = to_string ( mode );
      const std::string cname = GetCellDataName ( name );
      const std::string totalname = cname + "_" + smode;
      int index = -1;
      vtkDataArray* theArray = static_cast<vtkDataArray*> ( this->AllData[dim]->GetCellData()->GetAbstractArray ( totalname.c_str(), index ) );


      // if the mode is smaller than 10 we try to read the term with one zero padding (example myterm_03 )
      if ( index == -1 &&  mode < 10 ) {
          std::string totalname = cname + "_0" + smode;
          theArray = static_cast<vtkDataArray*> ( this->AllData[dim]->GetCellData()->GetAbstractArray ( totalname.c_str(), index ) );
        }


      if ( index == -1 ) return NULL;

      val = theArray;
      return  val;
    }
};
//----------------------------------------------------------------------------
vtkDoubleArray* GenerateCoordinates ( const double& origin, const double& spacing, const int& extent0, const int& extent1 ) {
  vtkDoubleArray* res =  vtkDoubleArray::New();

  //std::cout << "extent " << extent0 << " " << extent1 <<  std::endl;
  if ( extent1 - extent0 ) {
      res->SetNumberOfValues ( extent1 - extent0 );
      double* d0 = res->GetPointer ( 0 );

      for ( int i = 0; i < extent1 - extent0; i++ ) {
          d0[i] = origin + i * spacing;
        }
    }
  else {
      res->SetNumberOfValues ( 1 );
      * ( res->GetPointer ( 0 ) ) = 0;
    }

  return res;
}
//-----------------------------------------------------------------------------
vtkDataSet* vtkPXDMFDocumentBaseStructure::GenerateSpace_RectilinearGrid ( vtkstd::vector<int>& spacedata, vtkSmartPointer<vtkIdList>& pointIdGrids, vtkSmartPointer<vtkIdList>& cellIdGrids ) {
  vtkRectilinearGrid* udata = vtkRectilinearGrid::New();

  vtkImageData* IMDATAS[3];
  vtkRectilinearGrid* RGDATAS[3];

  if ( spacedata[0] >= 0 ) {

      IMDATAS[0] = vtkImageData::SafeDownCast ( this->AllData[spacedata[0]] );
      RGDATAS[0] = vtkRectilinearGrid::SafeDownCast ( this->AllData[spacedata[0]] );

      if ( IMDATAS[0] == 0 && RGDATAS[0] == 0 ) {
          std::cout << "invalid conversion from vtkDataSet* to vtkImageData* or vtkRectilinearGrid* dimension 1" << std::endl;
          std::cout << "GetClassName :  " << this->AllData[spacedata[0]]->GetClassName() << std::endl;
        }



      vtkDataArray* XCoordinates0;
      vtkDataArray* YCoordinates0;
      vtkDataArray* ZCoordinates0;

      if ( IMDATAS[0] ) {
          int extent0[6];
          double spacing0[3];
          double origin0[3];
          IMDATAS[0]->GetExtent ( extent0 );
          IMDATAS[0]->GetSpacing ( spacing0 );
          IMDATAS[0]->GetOrigin ( origin0 );
          XCoordinates0 = GenerateCoordinates ( origin0[0], spacing0[0], extent0[0], extent0[1] );
          YCoordinates0 = GenerateCoordinates ( origin0[1], spacing0[1], extent0[2], extent0[3] );
          ZCoordinates0 = GenerateCoordinates ( origin0[2], spacing0[2], extent0[4], extent0[5] );
        }

      if ( RGDATAS[0] ) {
          XCoordinates0 = RGDATAS[0]->GetXCoordinates();
          YCoordinates0 = RGDATAS[0]->GetYCoordinates();
          ZCoordinates0 = RGDATAS[0]->GetZCoordinates();
        }

      if ( spacedata[1] >= 0 ) {
          IMDATAS[1] =  vtkImageData::SafeDownCast ( this->AllData[spacedata[1]] );
          RGDATAS[1] =  vtkRectilinearGrid::SafeDownCast ( this->AllData[spacedata[1]] );

          if ( IMDATAS[1] == 0 && RGDATAS[1] == 0 ) {
              std::cout << "invalid conversion from vtkDataSet* to vtkImageData* or vtkRectilinearGrid* dimension 2" << std::endl;
              std::cout << "GetClassName :  " << this->AllData[spacedata[1]]->GetClassName() << std::endl;
            }


          vtkDataArray* XCoordinates1;
          vtkDataArray* YCoordinates1;

          //vtkDataArray* ZCoordinates1;
          if ( IMDATAS[1] ) {
              int extent1[6];
              double spacing1[3];
              double origin1[3];
              IMDATAS[1]->GetExtent ( extent1 );
              IMDATAS[1]->GetSpacing ( spacing1 );
              IMDATAS[1]->GetOrigin ( origin1 );
              XCoordinates1 = GenerateCoordinates ( origin1[0], spacing1[0], extent1[0], extent1[1] );
              YCoordinates1 = GenerateCoordinates ( origin1[1], spacing1[1], extent1[2], extent1[3] );
            }

          if ( RGDATAS[1] ) {
              XCoordinates1 = RGDATAS[1]->GetXCoordinates();
              YCoordinates1 = RGDATAS[1]->GetYCoordinates();
            }

          if ( spacedata[2] >= 0 ) {
              IMDATAS[2] =  vtkImageData::SafeDownCast ( this->AllData[spacedata[2]] );
              RGDATAS[2] =  vtkRectilinearGrid::SafeDownCast ( this->AllData[spacedata[2]] );

              if ( IMDATAS[2] == 0 && RGDATAS[2] == 0 ) {
                  std::cout << "invalid conversion from vtkDataSet* to vtkImageData* or vtkRectilinearGrid*  dimension 3" << std::endl;
                  std::cout << "GetClassName :  " << this->AllData[spacedata[1]]->GetClassName() << std::endl;
                }

              vtkDataArray* XCoordinates2;

              if ( IMDATAS[2] ) {
                  int extent2[6];
                  double spacing2[3];
                  double origin2[3];
                  IMDATAS[2]->GetExtent ( extent2 );
                  IMDATAS[2]->GetSpacing ( spacing2 );
                  IMDATAS[2]->GetOrigin ( origin2 );
                  XCoordinates2 = GenerateCoordinates ( origin2[0], spacing2[0], extent2[0], extent2[1] );
                }

              if ( RGDATAS[2] ) {
                  XCoordinates2 = RGDATAS[2]->GetXCoordinates();
                }

              udata->SetDimensions ( XCoordinates0->GetSize(), XCoordinates1->GetSize(), XCoordinates2->GetSize() );
              udata->SetXCoordinates ( XCoordinates0 );
              udata->SetYCoordinates ( XCoordinates1 );
              udata->SetZCoordinates ( XCoordinates2 );

            }
          else {
              if ( YCoordinates0->GetSize() == 1 ) {
                  ///case IMDATAS[0] 1 space dim
                  if ( YCoordinates1->GetSize() == 1 ) {
                      ///case IMDATAS[1] 1 space dim

                      udata->SetDimensions ( XCoordinates0->GetSize(), XCoordinates1->GetSize(), 1 );
                      udata->SetXCoordinates ( XCoordinates0 );
                      udata->SetYCoordinates ( XCoordinates1 );
                    }
                  else {
                      ///case IMDATAS[1] 2 space dim
                      udata->SetDimensions ( XCoordinates0->GetSize(), XCoordinates1->GetSize(), YCoordinates1->GetSize() );
                      udata->SetXCoordinates ( XCoordinates0 );
                      udata->SetYCoordinates ( XCoordinates1 );
                      udata->SetZCoordinates ( YCoordinates1 );
                    }
                }
              else {
                  //case IMDATAS[0] 2 space dim
                  udata->SetDimensions ( XCoordinates0->GetSize(), YCoordinates0->GetSize(), XCoordinates1->GetSize() );
                  udata->SetXCoordinates ( XCoordinates0 );
                  udata->SetYCoordinates ( YCoordinates0 );
                  udata->SetZCoordinates ( XCoordinates1 );
                }
            }
        }
      else {
          //std::cout << "1 grid case"  << std::endl;
          udata->SetDimensions ( XCoordinates0->GetSize(), YCoordinates0->GetSize(), ZCoordinates0->GetSize() );
          udata->SetXCoordinates ( XCoordinates0 );
          udata->SetYCoordinates ( YCoordinates0 );
          udata->SetZCoordinates ( ZCoordinates0 );
        }



    }
  else {
      /// In the case we have no space dims (one node and one cell is created)
      /// in any way I think this case never arrives because when we have non space dims
      /// a unstructured mesh is created by default
      //udata->SetExtent(0, 0, 0, 0, 0, 0);
      //udata->SetSpacing(1,1,1);
      //udata->SetOrigin(0,0,0);

    }

  return udata;
};
//----------------------------------------------------------------------------
vtkDataSet* vtkPXDMFDocumentBaseStructure::GenerateSpace_StructuredGrid ( vtkstd::vector<int>& spacedata, vtkSmartPointer<vtkIdList>& pointIdGrids, vtkSmartPointer<vtkIdList>& cellIdGrids ) {
  vtkStructuredGrid* udata = vtkStructuredGrid::New();
  std::cout << "not developed yet sorry " << std::endl;
  return udata;
}
//----------------------------------------------------------------------------
vtkDataSet* vtkPXDMFDocumentBaseStructure::GenerateSpace_PolydataGrid ( vtkstd::vector<int>& spacedata, vtkSmartPointer<vtkIdList>& pointIdGrids, vtkSmartPointer<vtkIdList>& cellIdGrids ) {
  vtkPolyData* udata = vtkPolyData::New();

  //int cpt =0;
  if ( spacedata[0] >= 0 ) {
      if ( this->LastActivePXDMFDimsForSpace != this->ActivePXDMFDimsForSpace || this->forceMeshUpdate ) {
          this->forceMeshUpdate = false;

          if ( vtkPolyData::SafeDownCast ( this->AllData[spacedata[0]] ) == 0 ) {
              std::cout << "invalid conversion from vtkDataSet* to vtkPolyData* dimension 1" << std::endl;
            }

          this->LastActivePXDMFDimsForSpace = this->ActivePXDMFDimsForSpace;
          udata->CopyStructure ( this->AllData[spacedata[0]] );
        }
      else {
          //in the case we dont change the space dims
          udata->CopyStructure ( vtkPolyData::SafeDownCast ( this->AllData[spacedata[0]] ) );
        }
    }

  return udata;
};
//----------------------------------------------------------------------------
vtkDataSet* vtkPXDMFDocumentBaseStructure::GenerateSpace_UnstructuredGrid ( vtkstd::vector<int>& spacedata, vtkSmartPointer<vtkIdList>& pointIdGrids, vtkSmartPointer<vtkIdList>& cellIdGrids ) {
  vtkUnstructuredGrid* udata = vtkUnstructuredGrid::New();

  int cpt = 0;

  if ( spacedata[0] >= 0 ) {
      if ( this->LastActivePXDMFDimsForSpace != this->ActivePXDMFDimsForSpace || this->forceMeshUpdate ) {
          this->forceMeshUpdate = false;
          vtkSmartPointer<vtkIdList> ptIds0 = vtkSmartPointer<vtkIdList>::New();
          vtkSmartPointer<vtkIdList> ptIds1 = vtkSmartPointer<vtkIdList>::New();
          vtkSmartPointer<vtkIdList> ptIds2 = vtkSmartPointer<vtkIdList>::New();
          vtkIdType ptIds[27];///void InsertPoint(vtkIdType id, const float x[3])
          vtkDataSet* grids[3];
          this->Points = vtkSmartPointer<vtkPoints>::Take ( vtkPoints::New ( VTK_DOUBLE ) );
          this->Cells = vtkSmartPointer<vtkCellArray> ::New();
          this->CellLocationsArray = vtkSmartPointer<vtkIdTypeArray>::New();
          this->CellTypesArray = vtkSmartPointer<vtkUnsignedCharArray>::New();
          grids[0] = vtkDataSet::SafeDownCast ( this->AllData[spacedata[0]] );

          if ( grids[0] == 0 ) {
              std::cout << "invalid conversion from vtkDataSet* to vtkUnstructuredGrid* dimension 1" << std::endl;
            }

          if ( spacedata[1] >= 0 ) {
              grids[1] =  vtkDataSet::SafeDownCast ( this->AllData[spacedata[1]] );

              if ( grids[1] == 0 ) {
                  std::cout << "invalid conversion from vtkDataSet* to vtkUnstructuredGrid* dimension 2" << std::endl;
                }

              if ( spacedata[2] >= 0 ) {
                  grids[2] =  vtkDataSet::SafeDownCast ( this->AllData[spacedata[2]] );

                  if ( grids[2] == 0 ) {
                      std::cout << "invalid conversion from vtkDataSet* to vtkUnstructuredGrid* dimension 3" << std::endl;
                    }

                  ///Case with tree Grids in the space
                  /// allocation of space for the points
                  unsigned nb_points0 = grids[0]->GetNumberOfPoints();
                  unsigned nb_points1 = grids[1]->GetNumberOfPoints();
                  unsigned nb_points2 = grids[2]->GetNumberOfPoints();
                  this->Points->SetNumberOfPoints ( nb_points0 * nb_points1 * nb_points2 );

                  /// Points generation
                  for ( unsigned i = 0; i < nb_points0; ++i ) {
                      const double px = grids[0]->GetPoint ( i ) [0];

                      for ( unsigned j = 0; j < nb_points1; ++j ) {
                          const double py = grids[1]->GetPoint ( j ) [0];

                          for ( unsigned k = 0; k < nb_points2; ++k )
                            this->Points->SetPoint ( cpt++, px, py, grids[2]->GetPoint ( k ) [0] );
                        }

                    }

                  /// allocation of space for the cells
                  unsigned nb_cells0 = grids[0]->GetNumberOfCells();
                  unsigned nb_cells1 = grids[1]->GetNumberOfCells();
                  unsigned nb_cells2 = grids[2]->GetNumberOfCells();

                  for ( unsigned i = 0; i < nb_cells0; ++i ) {
                      grids[0]->GetCellPoints ( i, ptIds0 );
                      int  cell1 = grids[0]->GetCellType ( i );

                      switch ( cell1 ) {
                        //case VTK_POLY_VERTEX :
                        //  break;
                        case VTK_LINE :
                        case VTK_POLY_LINE : {
                          for ( unsigned j = 0; j < nb_cells1; ++j ) {
                              grids[1]->GetCellPoints ( j, ptIds1 );
                              int  cell2 = grids[1]->GetCellType ( j );

                              switch ( cell2 ) {
                                  //case VTK_POLY_VERTEX :
                                  //    break;
                                case VTK_LINE :
                                case VTK_POLY_LINE : {

                                  for ( unsigned k = 0; k < nb_cells2; ++k ) {
                                      grids[2]->GetCellPoints ( k, ptIds2 );
                                      int cell3 = grids[2]->GetCellType ( k );

                                      switch ( cell3 ) {
                                        case VTK_POLY_VERTEX :
                                          ptIds[0] = nb_points2 * nb_points1 * ptIds0->GetId ( 0 ) + nb_points2 * ptIds1->GetId ( 0 ) + ptIds2->GetId ( 0 );
                                          ptIds[1] = nb_points2 * nb_points1 * ptIds0->GetId ( 1 ) + nb_points2 * ptIds1->GetId ( 0 ) + ptIds2->GetId ( 0 );
                                          ptIds[2] = nb_points2 * nb_points1 * ptIds0->GetId ( 1 ) + nb_points2 * ptIds1->GetId ( 1 ) + ptIds2->GetId ( 0 );
                                          ptIds[3] = nb_points2 * nb_points1 * ptIds0->GetId ( 0 ) + nb_points2 * ptIds1->GetId ( 1 ) + ptIds2->GetId ( 0 );
                                          this->Cells->InsertNextCell ( 4, &ptIds[0] );
                                          this->CellLocationsArray->InsertNextValue ( this->Cells->GetInsertLocation ( 4 ) );
                                          this->CellTypesArray->InsertNextValue ( VTK_QUAD );
                                          break;
                                        case VTK_LINE :
                                        case VTK_POLY_LINE :
                                          ptIds[0] = nb_points2 * nb_points1 * ptIds0->GetId ( 0 ) + nb_points2 * ptIds1->GetId ( 0 ) + ptIds2->GetId ( 0 );
                                          ptIds[1] = nb_points2 * nb_points1 * ptIds0->GetId ( 1 ) + nb_points2 * ptIds1->GetId ( 0 ) + ptIds2->GetId ( 0 );
                                          ptIds[2] = nb_points2 * nb_points1 * ptIds0->GetId ( 1 ) + nb_points2 * ptIds1->GetId ( 1 ) + ptIds2->GetId ( 0 );
                                          ptIds[3] = nb_points2 * nb_points1 * ptIds0->GetId ( 0 ) + nb_points2 * ptIds1->GetId ( 1 ) + ptIds2->GetId ( 0 );
                                          ptIds[4] = nb_points2 * nb_points1 * ptIds0->GetId ( 0 ) + nb_points2 * ptIds1->GetId ( 0 ) + ptIds2->GetId ( 1 );
                                          ptIds[5] = nb_points2 * nb_points1 * ptIds0->GetId ( 1 ) + nb_points2 * ptIds1->GetId ( 0 ) + ptIds2->GetId ( 1 );
                                          ptIds[6] = nb_points2 * nb_points1 * ptIds0->GetId ( 1 ) + nb_points2 * ptIds1->GetId ( 1 ) + ptIds2->GetId ( 1 );
                                          ptIds[7] = nb_points2 * nb_points1 * ptIds0->GetId ( 0 ) + nb_points2 * ptIds1->GetId ( 1 ) + ptIds2->GetId ( 1 );
                                          this->Cells->InsertNextCell ( 8, &ptIds[0] );
                                          this->CellLocationsArray->InsertNextValue ( this->Cells->GetInsertLocation ( 8 ) );
                                          this->CellTypesArray->InsertNextValue ( VTK_HEXAHEDRON );
                                          break;
                                        default:
                                          std::cout << "1)Reconstruction of types " << cell1 << " X " << cell2 << " X " << cell3 << " not valid or not implemented (please fill  a bug report)" << std::endl;
                                        }
                                    }

                                  break;
                                }
                                default:
                                  std::cout << "2)Reconstruction of types " << cell1 << " X " << cell2 << " not valid or not implemented (please fill  a bug report)" << std::endl;
                                }
                            }

                          break;
                        }
                        case VTK_QUADRATIC_EDGE : { 
                          for ( unsigned j = 0; j < nb_cells1; ++j ) {
                              grids[1]->GetCellPoints ( j, ptIds1 );
                              int  cell2 = grids[1]->GetCellType ( j );
                              switch ( cell1 ) {
                                case VTK_QUADRATIC_EDGE : {
                                  for ( unsigned k = 0; k < nb_cells2; ++k ) {
                                    grids[2]->GetCellPoints ( k, ptIds2 );
                                    int cell3 = grids[2]->GetCellType ( k );

                                    switch ( cell3 ) {
                                      case VTK_QUADRATIC_EDGE : {       
                                        
                                        
                                        ptIds[0 ] = nb_points2 * nb_points1 * ptIds0->GetId ( 0 ) + nb_points2 * ptIds1->GetId ( 0 ) + ptIds2->GetId ( 0 );
                                        ptIds[1 ] = nb_points2 * nb_points1 * ptIds0->GetId ( 1 ) + nb_points2 * ptIds1->GetId ( 0 ) + ptIds2->GetId ( 0 );
                                        ptIds[2 ] = nb_points2 * nb_points1 * ptIds0->GetId ( 1 ) + nb_points2 * ptIds1->GetId ( 1 ) + ptIds2->GetId ( 0 );
                                        ptIds[3 ] = nb_points2 * nb_points1 * ptIds0->GetId ( 0 ) + nb_points2 * ptIds1->GetId ( 1 ) + ptIds2->GetId ( 0 );
                                        
                                        ptIds[4 ] = nb_points2 * nb_points1 * ptIds0->GetId ( 0 ) + nb_points2 * ptIds1->GetId ( 0 ) + ptIds2->GetId ( 1 );
                                        ptIds[5 ] = nb_points2 * nb_points1 * ptIds0->GetId ( 1 ) + nb_points2 * ptIds1->GetId ( 0 ) + ptIds2->GetId ( 1 );
                                        ptIds[6 ] = nb_points2 * nb_points1 * ptIds0->GetId ( 1 ) + nb_points2 * ptIds1->GetId ( 1 ) + ptIds2->GetId ( 1 );
                                        ptIds[7 ] = nb_points2 * nb_points1 * ptIds0->GetId ( 0 ) + nb_points2 * ptIds1->GetId ( 1 ) + ptIds2->GetId ( 1 );
                                        
                                        ptIds[8 ] = nb_points2 * nb_points1 * ptIds0->GetId ( 2 ) + nb_points2 * ptIds1->GetId ( 0 ) + ptIds2->GetId ( 0 );
                                        ptIds[9 ] = nb_points2 * nb_points1 * ptIds0->GetId ( 1 ) + nb_points2 * ptIds1->GetId ( 2 ) + ptIds2->GetId ( 0 );
                                        ptIds[10] = nb_points2 * nb_points1 * ptIds0->GetId ( 2 ) + nb_points2 * ptIds1->GetId ( 1 ) + ptIds2->GetId ( 0 );
                                        ptIds[11] = nb_points2 * nb_points1 * ptIds0->GetId ( 0 ) + nb_points2 * ptIds1->GetId ( 2 ) + ptIds2->GetId ( 0 );
                                        
                                        ptIds[12] = nb_points2 * nb_points1 * ptIds0->GetId ( 2 ) + nb_points2 * ptIds1->GetId ( 0 ) + ptIds2->GetId ( 1 );
                                        ptIds[13] = nb_points2 * nb_points1 * ptIds0->GetId ( 1 ) + nb_points2 * ptIds1->GetId ( 2 ) + ptIds2->GetId ( 1 );
                                        ptIds[14] = nb_points2 * nb_points1 * ptIds0->GetId ( 2 ) + nb_points2 * ptIds1->GetId ( 1 ) + ptIds2->GetId ( 1 );
                                        ptIds[15] = nb_points2 * nb_points1 * ptIds0->GetId ( 0 ) + nb_points2 * ptIds1->GetId ( 2 ) + ptIds2->GetId ( 1 );
                                        
                                        ptIds[16] = nb_points2 * nb_points1 * ptIds0->GetId ( 0 ) + nb_points2 * ptIds1->GetId ( 0 ) + ptIds2->GetId ( 2 );
                                        ptIds[17] = nb_points2 * nb_points1 * ptIds0->GetId ( 1 ) + nb_points2 * ptIds1->GetId ( 0 ) + ptIds2->GetId ( 2 );
                                        ptIds[18] = nb_points2 * nb_points1 * ptIds0->GetId ( 1 ) + nb_points2 * ptIds1->GetId ( 1 ) + ptIds2->GetId ( 2 );
                                        ptIds[19] = nb_points2 * nb_points1 * ptIds0->GetId ( 0 ) + nb_points2 * ptIds1->GetId ( 1 ) + ptIds2->GetId ( 2 );
                                        
                                        ptIds[20] = nb_points2 * nb_points1 * ptIds0->GetId ( 0 ) + nb_points2 * ptIds1->GetId ( 2 ) + ptIds2->GetId ( 2 );
                                        ptIds[21] = nb_points2 * nb_points1 * ptIds0->GetId ( 1 ) + nb_points2 * ptIds1->GetId ( 2 ) + ptIds2->GetId ( 2 );
                                        ptIds[22] = nb_points2 * nb_points1 * ptIds0->GetId ( 2 ) + nb_points2 * ptIds1->GetId ( 0 ) + ptIds2->GetId ( 2 );
                                        ptIds[23] = nb_points2 * nb_points1 * ptIds0->GetId ( 2 ) + nb_points2 * ptIds1->GetId ( 1 ) + ptIds2->GetId ( 2 );
                                        ptIds[24] = nb_points2 * nb_points1 * ptIds0->GetId ( 2 ) + nb_points2 * ptIds1->GetId ( 2 ) + ptIds2->GetId ( 0 );
                                        ptIds[25] = nb_points2 * nb_points1 * ptIds0->GetId ( 2 ) + nb_points2 * ptIds1->GetId ( 2 ) + ptIds2->GetId ( 1 );
                                        
                                        ptIds[26] = nb_points2 * nb_points1 * ptIds0->GetId ( 2 ) + nb_points2 * ptIds1->GetId ( 2 ) + ptIds2->GetId ( 2 );

                                        this->Cells->InsertNextCell ( 27, &ptIds[0] );
                                        this->CellLocationsArray->InsertNextValue ( this->Cells->GetInsertLocation ( 27 ) );
                                        this->CellTypesArray->InsertNextValue ( VTK_TRIQUADRATIC_HEXAHEDRON );
                                        
                                        break;
                                      }
                                      default:
                                        std::cout << "2.1)Reconstruction of types " << cell1 << " X " << cell2 << " X " << cell3 << " not valid or not implemented (please fill  a bug report)" << std::endl;
                                    }
                                  }
                                  break;
                                }
                                default:
                                  std::cout << "2.2 )Reconstruction of types " << cell1 << " X " << cell2 << " not valid or not implemented (please fill  a bug report)" << std::endl;
                              }
                          }   
                          break;
                        }
                        default:
                          std::cout << "3)Reconstruction of types " << cell1 << " not valid or not implemented (please fill  a bug report)" << std::endl;
                        }
                    }
                }
              else {
                  ///Case with only two Grids in the space
                  /// allocation of space for the points
                  unsigned nb_points0 = grids[0]->GetNumberOfPoints();
                  unsigned nb_points1 = grids[1]->GetNumberOfPoints();
                  this->Points->SetNumberOfPoints ( nb_points0 * nb_points1 );

                  /// Points generation
                  if ( !vtkPXDMFDocumentBaseStructure::QUITE )
                    this->SetProgressText ( "Points generation ..." );

                  if ( this->GetNumberOfDimsInEachPXDMFDim()->GetValue ( spacedata[0] ) == 1 ) {
                      if ( this->GetNumberOfDimsInEachPXDMFDim()->GetValue ( spacedata[1] ) == 1 ) {
                          ///case when the two Grids are unidimensional
                          for ( unsigned i = 0; i < nb_points0; ++i ) {
                              const double px = grids[0]->GetPoint ( i ) [0];

                              for ( unsigned j = 0; j < nb_points1; ++j ) {
                                  this->Points->SetPoint ( cpt++, px , grids[1]->GetPoint ( j ) [0], 0 );
                                }

                            }
                        }
                      else {
                          ///case when the second grid is bidimensional
                          for ( unsigned i = 0; i < nb_points0; ++i ) {
                              const double px = grids[0]->GetPoint ( i ) [0];

                              for ( unsigned j = 0; j < nb_points1; ++j )
                                this->Points->SetPoint ( cpt++, px, grids[1]->GetPoint ( j ) [0], grids[1]->GetPoint ( j ) [1] );

                            }
                        }
                    }
                  else {
                      ///case when the first grid is bidimensional
                      for ( unsigned i = 0; i < nb_points0; ++i ) {
                          const double px = grids[0]->GetPoint ( i ) [0];
                          const double py = grids[0]->GetPoint ( i ) [1];

                          for ( unsigned j = 0; j < nb_points1; ++j )
                            this->Points->SetPoint ( cpt++, px, py, grids[1]->GetPoint ( j ) [0] );
                        }
                    }

                  /// allocation of space for the cells
                  unsigned nb_cells0 = grids[0]->GetNumberOfCells();
                  unsigned nb_cells1 = grids[1]->GetNumberOfCells();

                  unsigned cell_points_cpt_1 = 0;
                  unsigned cell_points_cpt_2 = 0;

                  if ( !vtkPXDMFDocumentBaseStructure::QUITE )
                    this->SetProgressText ( "Cells generation ..." );

                  for ( unsigned i = 0; i < nb_cells0; ++i ) {
                      grids[0]->GetCellPoints ( i, ptIds0 );
                      int  cell1 = grids[0]->GetCellType ( i );

                      switch ( cell1 ) {
                        case VTK_VERTEX:
                        case VTK_POLY_VERTEX : {
                          for ( unsigned j = 0; j < nb_cells1; ++j ) {
                              grids[1]->GetCellPoints ( j, ptIds1 );
                              int  cell2 = grids[1]->GetCellType ( j );

                              switch ( cell2 ) {
                                case VTK_POLY_VERTEX :
                                  ptIds[0] = nb_points1 * ptIds0->GetId ( 0 ) + ptIds1->GetId ( 0 );
                                  this->Cells->InsertNextCell ( 1, &ptIds[0] );
                                  this->CellLocationsArray->InsertNextValue ( this->Cells->GetInsertLocation ( 1 ) );
                                  this->CellTypesArray->InsertNextValue ( VTK_VERTEX );
                                  break;
                                case VTK_LINE :
                                case VTK_POLY_LINE :
                                  ptIds[0] = nb_points1 * ptIds0->GetId ( 0 ) + ptIds1->GetId ( 0 );
                                  ptIds[1] = nb_points1 * ptIds0->GetId ( 0 ) + ptIds1->GetId ( 1 );
                                  this->Cells->InsertNextCell ( 2, &ptIds[0] );
                                  this->CellLocationsArray->InsertNextValue ( this->Cells->GetInsertLocation ( 2 ) );
                                  this->CellTypesArray->InsertNextValue ( VTK_LINE );
                                  break;
                                case VTK_TRIANGLE :
                                  ptIds[0] = nb_points1 * ptIds0->GetId ( 0 ) + ptIds1->GetId ( 0 );
                                  ptIds[1] = nb_points1 * ptIds0->GetId ( 0 ) + ptIds1->GetId ( 1 );
                                  ptIds[2] = nb_points1 * ptIds0->GetId ( 0 ) + ptIds1->GetId ( 2 );
                                  this->Cells->InsertNextCell ( 3, &ptIds[0] );
                                  this->CellLocationsArray->InsertNextValue ( this->Cells->GetInsertLocation ( 3 ) );
                                  this->CellTypesArray->InsertNextValue ( VTK_TRIANGLE );
                                  break;

                                case VTK_QUAD:
                                  ptIds[0] = nb_points1 * ptIds0->GetId ( 0 ) + ptIds1->GetId ( 1 );
                                  ptIds[1] = nb_points1 * ptIds0->GetId ( 0 ) + ptIds1->GetId ( 2 );
                                  ptIds[2] = nb_points1 * ptIds0->GetId ( 0 ) + ptIds1->GetId ( 3 );
                                  ptIds[3] = nb_points1 * ptIds0->GetId ( 0 ) + ptIds1->GetId ( 4 );
                                  this->Cells->InsertNextCell ( 4, &ptIds[0] );
                                  this->CellLocationsArray->InsertNextValue ( this->Cells->GetInsertLocation ( 4 ) );
                                  this->CellTypesArray->InsertNextValue ( VTK_QUAD );
                                  break;
                                default :
                                  std::cout << "'4)Reconstruction of types " << cell1 << " X " << cell2 << " not valid or not implemented (please fill  a bug report)" << std::endl;
                                }
                            }
                        }
                        break;
                        case VTK_LINE :
                        case VTK_POLY_LINE : {
                          for ( unsigned j = 0; j < nb_cells1; ++j ) {
                              grids[1]->GetCellPoints ( j, ptIds1 );
                              int  cell2 = grids[1]->GetCellType ( j );

                              switch ( cell2 ) {
                                case VTK_VERTEX :
                                case VTK_POLY_VERTEX :
                                  ptIds[0] = nb_points1 * ptIds0->GetId ( 0 ) + ptIds1->GetId ( 0 );
                                  ptIds[1] = nb_points1 * ptIds0->GetId ( 1 ) + ptIds1->GetId ( 0 );
                                  this->Cells->InsertNextCell ( 2, &ptIds[0] );
                                  this->CellLocationsArray->InsertNextValue ( this->Cells->GetInsertLocation ( 2 ) );
                                  this->CellTypesArray->InsertNextValue ( VTK_LINE );
                                  break;
                                case VTK_LINE :
                                case VTK_POLY_LINE :
                                  ptIds[0] = nb_points1 * ptIds0->GetId ( 0 ) + ptIds1->GetId ( 0 );
                                  ptIds[1] = nb_points1 * ptIds0->GetId ( 1 ) + ptIds1->GetId ( 0 );
                                  ptIds[2] = nb_points1 * ptIds0->GetId ( 1 ) + ptIds1->GetId ( 1 );
                                  ptIds[3] = nb_points1 * ptIds0->GetId ( 0 ) + ptIds1->GetId ( 1 );
                                  this->Cells->InsertNextCell ( 4, &ptIds[0] );
                                  this->CellLocationsArray->InsertNextValue ( this->Cells->GetInsertLocation ( 4 ) );
                                  this->CellTypesArray->InsertNextValue ( VTK_QUAD );
                                  cell_points_cpt_1 = cell_points_cpt_1 + 2;
                                  cell_points_cpt_2 = cell_points_cpt_2 + 2;
                                  break;
                                case VTK_PIXEL:
                                  ptIds[0] = nb_points1 * ptIds0->GetId ( 0 ) + ptIds1->GetId ( 1 );
                                  ptIds[1] = nb_points1 * ptIds0->GetId ( 0 ) + ptIds1->GetId ( 0 );
                                  ptIds[2] = nb_points1 * ptIds0->GetId ( 0 ) + ptIds1->GetId ( 2 );
                                  ptIds[3] = nb_points1 * ptIds0->GetId ( 0 ) + ptIds1->GetId ( 3 );
                                  ptIds[4] = nb_points1 * ptIds0->GetId ( 1 ) + ptIds1->GetId ( 1 );
                                  ptIds[5] = nb_points1 * ptIds0->GetId ( 1 ) + ptIds1->GetId ( 0 );
                                  ptIds[6] = nb_points1 * ptIds0->GetId ( 1 ) + ptIds1->GetId ( 2 );
                                  ptIds[7] = nb_points1 * ptIds0->GetId ( 1 ) + ptIds1->GetId ( 3 );
                                  this->Cells->InsertNextCell ( 8, &ptIds[0] );
                                  this->CellLocationsArray->InsertNextValue ( this->Cells->GetInsertLocation ( 8 ) );
                                  this->CellTypesArray->InsertNextValue ( VTK_HEXAHEDRON );
                                  break;
                                case VTK_TRIANGLE :
                                  ptIds[0] = nb_points1 * ptIds0->GetId ( 0 ) + ptIds1->GetId ( 0 );
                                  ptIds[1] = nb_points1 * ptIds0->GetId ( 0 ) + ptIds1->GetId ( 1 );
                                  ptIds[2] = nb_points1 * ptIds0->GetId ( 0 ) + ptIds1->GetId ( 2 );
                                  ptIds[3] = nb_points1 * ptIds0->GetId ( 1 ) + ptIds1->GetId ( 0 );
                                  ptIds[4] = nb_points1 * ptIds0->GetId ( 1 ) + ptIds1->GetId ( 1 );
                                  ptIds[5] = nb_points1 * ptIds0->GetId ( 1 ) + ptIds1->GetId ( 2 );
                                  this->Cells->InsertNextCell ( 6, &ptIds[0] );
                                  this->CellLocationsArray->InsertNextValue ( this->Cells->GetInsertLocation ( 6 ) );
                                  this->CellTypesArray->InsertNextValue ( VTK_WEDGE );

                                  break;
                                case VTK_QUAD :
                                  ptIds[0] = nb_points1 * ptIds0->GetId ( 0 ) + ptIds1->GetId ( 0 );
                                  ptIds[1] = nb_points1 * ptIds0->GetId ( 0 ) + ptIds1->GetId ( 1 );
                                  ptIds[2] = nb_points1 * ptIds0->GetId ( 0 ) + ptIds1->GetId ( 2 );
                                  ptIds[3] = nb_points1 * ptIds0->GetId ( 0 ) + ptIds1->GetId ( 3 );
                                  ptIds[4] = nb_points1 * ptIds0->GetId ( 1 ) + ptIds1->GetId ( 0 );
                                  ptIds[5] = nb_points1 * ptIds0->GetId ( 1 ) + ptIds1->GetId ( 1 );
                                  ptIds[6] = nb_points1 * ptIds0->GetId ( 1 ) + ptIds1->GetId ( 2 );
                                  ptIds[7] = nb_points1 * ptIds0->GetId ( 1 ) + ptIds1->GetId ( 3 );
                                  this->Cells->InsertNextCell ( 8, &ptIds[0] );
                                  this->CellLocationsArray->InsertNextValue ( this->Cells->GetInsertLocation ( 8 ) );
                                  this->CellTypesArray->InsertNextValue ( VTK_HEXAHEDRON );
                                  break;
                                default:
                                  std::cout << "5)Reconstruction of types " << cell1 << " X " << cell2 << " not valid or not implemented (please fill  a bug report)" << std::endl;
                                }
                            }
                        }
                        break;
                        case VTK_TRIANGLE : {
                          for ( unsigned j = 0; j < nb_cells1; ++j ) {
                              grids[1]->GetCellPoints ( j, ptIds1 );
                              int  cell2 = grids[1]->GetCellType ( j );

                              switch ( cell2 ) {
                                case VTK_VERTEX :
                                case VTK_POLY_VERTEX :
                                  ptIds[0] = nb_points1 * ptIds0->GetId ( 0 ) + ptIds1->GetId ( 0 );
                                  ptIds[1] = nb_points1 * ptIds0->GetId ( 1 ) + ptIds1->GetId ( 0 );
                                  ptIds[2] = nb_points1 * ptIds0->GetId ( 2 ) + ptIds1->GetId ( 0 );
                                  this->Cells->InsertNextCell ( 3, &ptIds[0] );
                                  this->CellLocationsArray->InsertNextValue ( this->Cells->GetInsertLocation ( 3 ) );
                                  this->CellTypesArray->InsertNextValue ( VTK_TRIANGLE );
                                  break;
                                case VTK_LINE :
                                case VTK_POLY_LINE :
                                  ptIds[0] = nb_points1 * ptIds0->GetId ( 0 ) + ptIds1->GetId ( 0 );
                                  ptIds[1] = nb_points1 * ptIds0->GetId ( 1 ) + ptIds1->GetId ( 0 );
                                  ptIds[2] = nb_points1 * ptIds0->GetId ( 2 ) + ptIds1->GetId ( 0 );
                                  ptIds[3] = nb_points1 * ptIds0->GetId ( 0 ) + ptIds1->GetId ( 1 );
                                  ptIds[4] = nb_points1 * ptIds0->GetId ( 1 ) + ptIds1->GetId ( 1 );
                                  ptIds[5] = nb_points1 * ptIds0->GetId ( 2 ) + ptIds1->GetId ( 1 );
                                  this->Cells->InsertNextCell ( 6, &ptIds[0] );
                                  this->CellLocationsArray->InsertNextValue ( this->Cells->GetInsertLocation ( 6 ) );
                                  this->CellTypesArray->InsertNextValue ( VTK_WEDGE );
                                  break;
                                default:
                                  std::cout << "6)Reconstruction of types " << cell1 << " X " << cell2  << " not valid or not implemented (please fill  a bug report)" << std::endl;
                                }

                            }
                        }
                        break;
                        case VTK_QUAD : {
                          for ( unsigned j = 0; j < nb_cells1; ++j ) {
                              grids[1]->GetCellPoints ( j, ptIds1 );
                              int  cell2 = grids[1]->GetCellType ( j );

                              switch ( cell2 ) {
                                case VTK_VERTEX :
                                case VTK_POLY_VERTEX :
                                  ptIds[0] = nb_points1 * ptIds0->GetId ( 0 ) + ptIds1->GetId ( 0 );
                                  ptIds[1] = nb_points1 * ptIds0->GetId ( 1 ) + ptIds1->GetId ( 0 );
                                  ptIds[2] = nb_points1 * ptIds0->GetId ( 2 ) + ptIds1->GetId ( 0 );
                                  ptIds[3] = nb_points1 * ptIds0->GetId ( 3 ) + ptIds1->GetId ( 0 );
                                  this->Cells->InsertNextCell ( 4, &ptIds[0] );
                                  this->CellLocationsArray->InsertNextValue ( this->Cells->GetInsertLocation ( 4 ) );
                                  this->CellTypesArray->InsertNextValue ( VTK_QUAD );
                                  break;
                                case VTK_LINE :
                                case VTK_POLY_LINE :
                                  ptIds[0] = nb_points1 * ptIds0->GetId ( 0 ) + ptIds1->GetId ( 0 );
                                  ptIds[1] = nb_points1 * ptIds0->GetId ( 1 ) + ptIds1->GetId ( 0 );
                                  ptIds[2] = nb_points1 * ptIds0->GetId ( 2 ) + ptIds1->GetId ( 0 );
                                  ptIds[3] = nb_points1 * ptIds0->GetId ( 3 ) + ptIds1->GetId ( 0 );
                                  ptIds[4] = nb_points1 * ptIds0->GetId ( 0 ) + ptIds1->GetId ( 1 );
                                  ptIds[5] = nb_points1 * ptIds0->GetId ( 1 ) + ptIds1->GetId ( 1 );
                                  ptIds[6] = nb_points1 * ptIds0->GetId ( 2 ) + ptIds1->GetId ( 1 );
                                  ptIds[7] = nb_points1 * ptIds0->GetId ( 3 ) + ptIds1->GetId ( 1 );
                                  this->Cells->InsertNextCell ( 8, &ptIds[0] );
                                  this->CellLocationsArray->InsertNextValue ( this->Cells->GetInsertLocation ( 8 ) );
                                  this->CellTypesArray->InsertNextValue ( VTK_HEXAHEDRON );
                                  break;
                                default:
                                  std::cout << "7)Reconstruction of types " << cell1 << " X " << cell2  << " not valid or not implemented (please fill  a bug report)" << std::endl;
                                }

                            }
                        }
                        break;
                        case VTK_QUADRATIC_EDGE : {
                          for ( unsigned j = 0; j < nb_cells1; ++j ) {
                              grids[1]->GetCellPoints ( j, ptIds1 );
                              int  cell2 = grids[1]->GetCellType ( j );

                              switch ( cell2 ) {
                                case VTK_QUADRATIC_EDGE :
                                  ptIds[0] = nb_points1 * ptIds0->GetId ( 0 ) + ptIds1->GetId ( 0 );
                                  ptIds[1] = nb_points1 * ptIds0->GetId ( 1 ) + ptIds1->GetId ( 0 );
                                  ptIds[2] = nb_points1 * ptIds0->GetId ( 1 ) + ptIds1->GetId ( 1 );
                                  ptIds[3] = nb_points1 * ptIds0->GetId ( 0 ) + ptIds1->GetId ( 1 );

                                  ptIds[4] = nb_points1 * ptIds0->GetId ( 2 ) + ptIds1->GetId ( 0 );
                                  ptIds[5] = nb_points1 * ptIds0->GetId ( 1 ) + ptIds1->GetId ( 2 );
                                  ptIds[6] = nb_points1 * ptIds0->GetId ( 2 ) + ptIds1->GetId ( 1 );
                                  ptIds[7] = nb_points1 * ptIds0->GetId ( 0 ) + ptIds1->GetId ( 2 );

                                  ptIds[8] = nb_points1 * ptIds0->GetId ( 2 ) + ptIds1->GetId ( 2 );

                                  this->Cells->InsertNextCell ( 9, &ptIds[0] );
                                  this->CellLocationsArray->InsertNextValue ( this->Cells->GetInsertLocation ( 9 ) );
                                  this->CellTypesArray->InsertNextValue ( VTK_BIQUADRATIC_QUAD );
                                  break;
                                default:
                                  std::cout << "7)Reconstruction of types " << cell1 << " X " << cell2  << " not valid or not implemented (please fill  a bug report)" << std::endl;
                                }

                            }
                        }
                        break;
                        case VTK_PIXEL: {
                          for ( unsigned j = 0; j < nb_cells1; ++j ) {
                              grids[1]->GetCellPoints ( j, ptIds1 );
                              int  cell2 = grids[1]->GetCellType ( j );

                              switch ( cell2 ) {
                                case VTK_POLY_VERTEX :
                                  ptIds[0] = nb_points1 * ptIds0->GetId ( 1 ) + ptIds1->GetId ( 0 );
                                  ptIds[1] = nb_points1 * ptIds0->GetId ( 0 ) + ptIds1->GetId ( 0 );
                                  ptIds[2] = nb_points1 * ptIds0->GetId ( 2 ) + ptIds1->GetId ( 0 );
                                  ptIds[3] = nb_points1 * ptIds0->GetId ( 3 ) + ptIds1->GetId ( 0 );
                                  this->Cells->InsertNextCell ( 4, &ptIds[0] );
                                  this->CellLocationsArray->InsertNextValue ( this->Cells->GetInsertLocation ( 4 ) );
                                  this->CellTypesArray->InsertNextValue ( VTK_QUAD );
                                  break;
                                case VTK_LINE :
                                case VTK_POLY_LINE :
                                  ptIds[0] = nb_points1 * ptIds0->GetId ( 1 ) + ptIds1->GetId ( 0 );
                                  ptIds[1] = nb_points1 * ptIds0->GetId ( 0 ) + ptIds1->GetId ( 0 );
                                  ptIds[2] = nb_points1 * ptIds0->GetId ( 2 ) + ptIds1->GetId ( 0 );
                                  ptIds[3] = nb_points1 * ptIds0->GetId ( 3 ) + ptIds1->GetId ( 0 );
                                  ptIds[4] = nb_points1 * ptIds0->GetId ( 1 ) + ptIds1->GetId ( 1 );
                                  ptIds[5] = nb_points1 * ptIds0->GetId ( 0 ) + ptIds1->GetId ( 1 );
                                  ptIds[6] = nb_points1 * ptIds0->GetId ( 2 ) + ptIds1->GetId ( 1 );
                                  ptIds[7] = nb_points1 * ptIds0->GetId ( 3 ) + ptIds1->GetId ( 1 );
                                  this->Cells->InsertNextCell ( 8, &ptIds[0] );
                                  this->CellLocationsArray->InsertNextValue ( this->Cells->GetInsertLocation ( 8 ) );
                                  this->CellTypesArray->InsertNextValue ( VTK_HEXAHEDRON );
                                  break;
                                default:
                                  std::cout << "8)Reconstruction of types " << cell1 << " X " << cell2 << " not valid or not implemented (please fill  a bug report)" << std::endl;
                                }
                            }

                          break;
                        } case VTK_BIQUADRATIC_QUAD: {
                          for ( unsigned j = 0; j < nb_cells1; ++j ) {
                              grids[1]->GetCellPoints ( j, ptIds1 );
                              int  cell2 = grids[1]->GetCellType ( j );

                              switch ( cell2 ) {
                                case VTK_QUADRATIC_EDGE :
                                  //http://www.vtk.org/doc/nightly/html/classvtkTriQuadraticHexahedron.html
                                  //bottom  points
                                  ptIds[0] = nb_points1 * ptIds0->GetId ( 0 ) + ptIds1->GetId ( 0 );
                                  ptIds[1] = nb_points1 * ptIds0->GetId ( 1 ) + ptIds1->GetId ( 0 );
                                  ptIds[2] = nb_points1 * ptIds0->GetId ( 2 ) + ptIds1->GetId ( 0 );
                                  ptIds[3] = nb_points1 * ptIds0->GetId ( 3 ) + ptIds1->GetId ( 0 );
                                  //top  points
                                  ptIds[4] = nb_points1 * ptIds0->GetId ( 0 ) + ptIds1->GetId ( 1 );
                                  ptIds[5] = nb_points1 * ptIds0->GetId ( 1 ) + ptIds1->GetId ( 1 );
                                  ptIds[6] = nb_points1 * ptIds0->GetId ( 2 ) + ptIds1->GetId ( 1 );
                                  ptIds[7] = nb_points1 * ptIds0->GetId ( 3 ) + ptIds1->GetId ( 1 );
                                  
                                  // bottom edges
                                  ptIds[8 ] = nb_points1 * ptIds0->GetId ( 4 ) + ptIds1->GetId ( 0 );
                                  ptIds[9 ] = nb_points1 * ptIds0->GetId ( 5 ) + ptIds1->GetId ( 0 );
                                  ptIds[10] = nb_points1 * ptIds0->GetId ( 6 ) + ptIds1->GetId ( 0 );
                                  ptIds[11] = nb_points1 * ptIds0->GetId ( 7 ) + ptIds1->GetId ( 0 );
                                  //
                                  // top edges
                                  ptIds[12] = nb_points1 * ptIds0->GetId ( 4 ) + ptIds1->GetId ( 1 );
                                  ptIds[13] = nb_points1 * ptIds0->GetId ( 5 ) + ptIds1->GetId ( 1 );
                                  ptIds[14] = nb_points1 * ptIds0->GetId ( 6 ) + ptIds1->GetId ( 1 );
                                  ptIds[15] = nb_points1 * ptIds0->GetId ( 7 ) + ptIds1->GetId ( 1 );
                                  //
                                  // middle points
                                  ptIds[16] = nb_points1 * ptIds0->GetId ( 0 ) + ptIds1->GetId ( 2 );
                                  ptIds[17] = nb_points1 * ptIds0->GetId ( 1 ) + ptIds1->GetId ( 2 );
                                  ptIds[18] = nb_points1 * ptIds0->GetId ( 2 ) + ptIds1->GetId ( 2 );
                                  ptIds[19] = nb_points1 * ptIds0->GetId ( 3 ) + ptIds1->GetId ( 2 );
                                  //
                                  // middle edges (vertical faces)
                                  ptIds[20] = nb_points1 * ptIds0->GetId ( 7 ) + ptIds1->GetId ( 2 );
                                  ptIds[21] = nb_points1 * ptIds0->GetId ( 5 ) + ptIds1->GetId ( 2 );
                                  ptIds[22] = nb_points1 * ptIds0->GetId ( 4 ) + ptIds1->GetId ( 2 );
                                  ptIds[23] = nb_points1 * ptIds0->GetId ( 6 ) + ptIds1->GetId ( 2 );
                                  //
                                  // bottom face
                                  ptIds[24] = nb_points1 * ptIds0->GetId ( 8 ) + ptIds1->GetId ( 0 );
                                  //
                                  // top face
                                  ptIds[25] = nb_points1 * ptIds0->GetId ( 8 ) + ptIds1->GetId ( 1 );
                                  //
                                  // cell Center
                                  ptIds[26] = nb_points1 * ptIds0->GetId ( 8 ) + ptIds1->GetId ( 2 );
                                  
                                  this->Cells->InsertNextCell ( 27, &ptIds[0] );
                                  this->CellLocationsArray->InsertNextValue ( this->Cells->GetInsertLocation ( 27 ) );
                                  this->CellTypesArray->InsertNextValue ( VTK_TRIQUADRATIC_HEXAHEDRON );
                                  break;
                                default:
                                  std::cout << "10)Reconstruction of types " << cell1 << " X " << cell2 << " not valid or not implemented (please fill  a bug report)" << std::endl;
                                }
                            }

                          break;
                        }
                        default:
                          std::cout << "9)Reconstruction of types " << cell1 << " not valid or not implemented (please fill  a bug report)" << std::endl;
                        }

                    }
                }
            }
          else {
              ///Case with only one grid in the space

              this->Points->ShallowCopy ( vtkUnstructuredGrid::SafeDownCast ( this->AllData[spacedata[0]] )->GetPoints() );
              this->CellTypesArray = vtkUnstructuredGrid::SafeDownCast ( this->AllData[spacedata[0]] )->GetCellTypesArray();
              this->CellLocationsArray = vtkUnstructuredGrid::SafeDownCast ( this->AllData[spacedata[0]] )->GetCellLocationsArray();
              this->Cells = vtkUnstructuredGrid::SafeDownCast ( this->AllData[spacedata[0]] )->GetCells();
            }

          udata->SetPoints ( this->Points );
          udata->SetCells ( this->CellTypesArray, this->CellLocationsArray, this->Cells );
          this->LastActivePXDMFDimsForSpace = this->ActivePXDMFDimsForSpace;
        }
      else {
          //in the case we dont change the space dims
          udata->SetPoints ( this->Points );
          udata->SetCells ( this->CellTypesArray, this->CellLocationsArray, this->Cells );
        }
    }
  else {
      /// In the case we have no space dims (one node and one cell is created)
      if ( this->LastActivePXDMFDimsForSpace != this->ActivePXDMFDimsForSpace ) {
          this->Points = vtkSmartPointer<vtkPoints>::Take ( vtkPoints::New ( VTK_DOUBLE ) );
          this->Points->SetNumberOfPoints ( 1 );
          this->Points->SetPoint ( 0, 0, 0, 0 );

          this->Cells = vtkSmartPointer<vtkCellArray> ::New();
          this->CellLocationsArray = vtkSmartPointer<vtkIdTypeArray>::New();
          this->CellTypesArray = vtkSmartPointer<vtkUnsignedCharArray>::New();
          vtkIdType ptIds[8];///void InsertPoint(vtkIdType id, const float x[3])
          ptIds[0] = 0;
          this->Cells->InsertNextCell ( 1, &ptIds[0] );
          this->CellLocationsArray->InsertNextValue ( this->Cells->GetInsertLocation ( 1 ) );
          this->CellTypesArray->InsertNextValue ( VTK_VERTEX );
          udata->SetPoints ( this->Points );
          udata->SetCells ( this->CellTypesArray, this->CellLocationsArray, this->Cells );
          this->LastActivePXDMFDimsForSpace = this->ActivePXDMFDimsForSpace;
        }
      else {
          //in the case we dont change the space dims
          udata->SetPoints ( this->Points );
          udata->SetCells ( this->CellTypesArray, this->CellLocationsArray, this->Cells );
        }
    }

  return udata;
};
//----------------------------------------------------------------------------
void vtkPXDMFDocumentBaseStructure::SetPosOfEachDimension ( const int grid, const int dim, const double& value ) {
  if ( this->PosOfEachDimension[grid][dim] != value ) {
      this->PosOfEachDimension[grid][dim] = value;
      this->Modified();
    }
};
//-----------------------------------
double  vtkPXDMFDocumentBaseStructure::GetPosOfEachDimension ( const int PXDMFdim, const int dim ) {
  return this->PosOfEachDimension[PXDMFdim][dim];
};
//----------------------------------------------------------------------------
vtkDataSet* vtkPXDMFDocumentBaseStructure::GenerateSpace_UniformGrid ( vtkstd::vector<int>& spacedata, vtkSmartPointer<vtkIdList>& pointIdsGrids, vtkSmartPointer<vtkIdList>& cellIdsGrids ) {
  vtkUniformGrid* udata = vtkUniformGrid::New();

  vtkImageData* grids[3];

  if ( spacedata[0] >= 0 ) {

      grids[0] = vtkImageData::SafeDownCast ( this->AllData[spacedata[0]] );

      if ( grids[0] == 0 ) {
          std::cout << "invalid conversion from vtkDataSet* to vtkImageData* dimension 1" << std::endl;
          std::cout << "GetClassName :  " << this->AllData[spacedata[0]]->GetClassName() << std::endl;
        }

      int extent0[6];
      grids[0]->GetExtent ( extent0 );
      double spacing0[3];
      grids[0]->GetSpacing ( spacing0 );
      double origin0[3];
      grids[0]->GetOrigin ( origin0 );

      if ( spacedata[1] >= 0 ) {
          grids[1] =  vtkImageData::SafeDownCast ( this->AllData[spacedata[1]] );

          if ( grids[1] == 0 ) {
              std::cout << "invalid conversion from vtkDataSet* to vtkImageData* dimension 2" << std::endl;
            }

          int extent1[6];
          grids[1]->GetExtent ( extent1 );
          double spacing1[3];
          grids[1]->GetSpacing ( spacing1 );
          double origin1[3];
          grids[1]->GetOrigin ( origin1 );

          if ( spacedata[2] >= 0 ) {
              grids[2] =  vtkImageData::SafeDownCast ( this->AllData[spacedata[2]] );

              if ( grids[2] == 0 ) {
                  std::cout << "invalid conversion from vtkDataSet* to vtkImageData* dimension 3" << std::endl;
                }

              int extent2[6];
              grids[2]->GetExtent ( extent2 );
              double spacing2[3];
              grids[2]->GetSpacing ( spacing2 );
              double origin2[3];
              grids[2]->GetOrigin ( origin2 );

              //std::cout << "3 grids case"  << std::endl;
              udata->SetExtent ( extent0[0], extent0[1] , extent1[0], extent1[1], extent2[0], extent2[1] );
              udata->SetSpacing ( spacing0[0], spacing1[0], spacing2[0] );
              udata->SetOrigin ( origin0[0], origin1[0], origin2[0] );

            }
          else {
              if ( extent0[3] == 0 && extent0[5] == 0 ) {
                  ///case grids[0] 1 space dim
                  if ( extent1[3] == 0 && extent1[5] == 0 ) {
                      ///case grids[1] 1 space dim
                      udata->SetExtent ( extent0[0], extent0[1] , extent1[0], extent1[1], 0, 0 );
                      udata->SetSpacing ( spacing0[0], spacing1[0], 0 );
                      udata->SetOrigin ( origin0[0], origin1[0], 0 );
                    }
                  else {
                      ///case grids[1] 2 space dim
                      udata->SetExtent ( extent0[0], extent0[1] , extent1[0], extent1[1], extent1[2], extent1[3] );
                      udata->SetSpacing ( spacing0[0], spacing1[0], spacing1[1] );
                      udata->SetOrigin ( origin0[0], origin1[0], origin1[1] );
                    }
                }
              else {
                  /// case grids[0] 2 space dim
                  udata->SetExtent ( extent0[0], extent0[1], extent0[2], extent0[3] , extent1[0], extent1[1] );
                  udata->SetSpacing ( spacing0[0], spacing0[1], spacing1[0] );
                  udata->SetOrigin ( origin0[0], origin0[1], origin1[1] );
                }
            }
        }
      else {
          //std::cout << "1 grid case"  << std::endl;
          udata->SetExtent ( extent0 );
          udata->SetSpacing ( spacing0 );
          udata->SetOrigin ( origin0 );
        }



    }
  else {
      /// In the case we have no space dims (one node and one cell is created)
      udata->SetExtent ( 0, 0, 0, 0, 0, 0 );
      udata->SetSpacing ( 1, 1, 1 );
      udata->SetOrigin ( 0, 0, 0 );
    }

  return udata;
};
//----------------------------------------------------------------------------
void vtkPXDMFDocumentBaseStructure::construct_cell_fields ( vtkIdList* cellIdsGrids, vtkstd::vector<int> &spacedata ) {

  vtkPXDMFDocumentBaseStructure::TCelldata_name::iterator it = this->CellFieldData.begin();
  vtkPXDMFDocumentBaseStructure::TCelldata_name::iterator fit = this->CellFieldData.end();
  std::vector<double> vals;
  ///alocation and zero setting
  unsigned cpt = 0;

  for ( ; it != fit; ++it, ++cpt ) {
      unsigned mode = 0;
      vtkDataArray* temp_data = NULL;
      std::string name;

      for ( ; mode < this->GetNumberOfModesOfCellDataByNumber ( cpt ); ++mode ) {
          name = std::string ( it->first + "_" + to_string ( mode ) );

          for ( unsigned i = 0; i < ( unsigned ) this->GetNumberOfPXDMFDims(); ++i ) {
              temp_data = getArrayCell ( cpt, mode, i );;

              if ( temp_data  > 0 ) break;
            }

          if ( temp_data > 0 ) break;
        }

      if ( mode >= this->GetNumberOfModesOfCellDataByNumber ( cpt ) ) {
          it->second = NULL;
          continue;
        }
      else

        if ( !temp_data ) {
            std::cout << "Error:  field " << name << " not found" << std::endl;
            exit ( 1 );
          }

      unsigned nb_total = 1;

      if ( spacedata[0] >= 0 ) {
          nb_total = this->AllData[spacedata[0]]->GetNumberOfCells();

          if ( spacedata[1] >= 0 ) {
              nb_total *= this->AllData[spacedata[1]]->GetNumberOfCells();

              if ( spacedata[2] >= 0 )
                nb_total *= this->AllData[spacedata[2]]->GetNumberOfCells();
            }
        }
      else {
          /// when no space dims are selected only one cell is created
          nb_total = 1;
        }

      const unsigned nb_component = temp_data->GetNumberOfComponents();

      if ( strcmp ( temp_data->GetClassName(), "vtkFloatArray" ) == 0 ) {
          vtkSmartPointer<vtkFloatArray> data = vtkSmartPointer<vtkFloatArray>::New();
          data->SetNumberOfComponents ( nb_component );
          data->SetNumberOfTuples ( nb_total );
          data->SetName ( it->first.c_str() );
          std::fill ( data->GetPointer ( 0 ), data->GetPointer ( 0 ) + nb_total * nb_component , 0.0 );
          it->second = data;
        }
      else {
          vtkSmartPointer<vtkDoubleArray> data = vtkSmartPointer<vtkDoubleArray>::New();
          data->SetNumberOfComponents ( nb_component );
          data->SetNumberOfTuples ( nb_total );
          data->SetName ( it->first.c_str() );
          std::fill ( data->GetPointer ( 0 ), data->GetPointer ( 0 ) + nb_total * nb_component , 0.0 );
          it->second = data ;
        }
    }

  it = this->CellFieldData.begin();
  cpt = 0;

  for ( ; it != fit; ++it, ++cpt ) {
      if ( it->second == NULL ) continue;

      //this->SetProgressText(("Building cell field : " + it->first).c_str());
      int nb_modes = this->GetNumberOfModesOfCellDataByNumber ( cpt );
      this->reconstruction ( it, nb_modes, cellIdsGrids, spacedata, cpt, 0 );

    }
};
//--vtkFloatArray-----------------------------------------------------------------
void vtkPXDMFDocumentBaseStructure::reconstruction ( vtkIdList* ptIdsGrids, const vtkstd::vector<int>& spacedata, const unsigned fieldnumber  , unsigned& mode,  vtkFloatArray* field, const unsigned pointdata,  std::vector<vtkCell*>* cellGrids, std::vector<std::vector<double> >* cellweights ) {

  if ( field == 0 )  return;

  std::vector<vtkFloatArray*>& dataarrays = this->fdataarrays;

  for ( unsigned i = 0; int ( i ) < this->NumberOfPXDMFDims; ++i ) {
      if ( pointdata ) {
          dataarrays[i] = static_cast<vtkFloatArray*> ( getArrayPoint ( fieldnumber, mode, i ) );;
        }
      else {
          dataarrays[i] = static_cast<vtkFloatArray*> ( getArrayCell ( fieldnumber, mode, i ) );;
        }

      if ( dataarrays[i] == 0 ) {
          return;
        }
    }

  unsigned nb_Components = field->GetNumberOfComponents();
  float val = 1;
  float tmpval = 0;

  for ( unsigned Comp = 0; Comp < nb_Components; ++Comp ) {
      val = 1;

      for ( unsigned o = 0; int ( o ) < this->NumberOfPXDMFDims; ++o )
        if ( this->GetActivePXDMFDimsForSpace() [o] == 0 ) {
            if ( this->UseInterpolation && pointdata && cellGrids ) {
                int pn = ( *cellGrids ) [o]->GetNumberOfPoints();
                tmpval = 0;

                for ( int pp = 0; pp < pn; ++pp ) {
                    tmpval += dataarrays[o]->GetValue ( ( *cellGrids ) [o]->GetPointId ( pp ) * nb_Components + Comp ) * ( *cellweights ) [o][pp];
                  }

                val *= tmpval;
              }
            else {

                val *= dataarrays[o]->GetValue ( ptIdsGrids->GetId ( o ) * nb_Components + Comp );
              }
          }

      if ( val != 0. ) {
          if ( spacedata[0] >= 0 ) {
              int nb_points1 = dataarrays[spacedata[0]]->GetNumberOfTuples();

              if ( spacedata[1] >= 0 ) {
                  int nb_points2 = dataarrays[spacedata[1]]->GetNumberOfTuples();

                  if ( spacedata[2] >= 0 ) {
                      // tree dimension
                      int nb_points3 = dataarrays[spacedata[2]]->GetNumberOfTuples();
                      unsigned cpt = Comp;

                      for ( unsigned point1 = 0; int ( point1 ) < nb_points1; ++point1 ) {
                          const float val_temp1 = val * dataarrays[spacedata[0]]->GetValue ( point1 * nb_Components + Comp );

                          for ( unsigned point2 = 0; int ( point2 ) < nb_points2; ++point2 ) {
                              const float val_temp2 = val_temp1 * dataarrays[spacedata[1]]->GetValue ( point2 * nb_Components + Comp );
                              SAXPY ( nb_points3, val_temp2, ( float* ) dataarrays[spacedata[2]]->GetPointer ( 0 + Comp ), nb_Components, ( float* ) field->GetPointer ( cpt ), nb_Components );
                              cpt = cpt + nb_points3 * nb_Components;
                            }
                        }
                    }
                  else {
                      // two dimension
                      unsigned cpt = Comp;

                      for ( unsigned point1 = 0; int ( point1 ) < nb_points1; ++point1 ) {
                          const float val_temp2 = val * dataarrays[spacedata[0]]->GetValue ( point1 * nb_Components + Comp );

                          SAXPY ( nb_points2, val_temp2, ( float* ) dataarrays[spacedata[1]]->GetPointer ( Comp ), nb_Components, ( float* ) field->GetPointer ( cpt ), nb_Components );
                          cpt = cpt + nb_points2 * nb_Components;
                        }
                    }
                }
              else {
                  SAXPY ( nb_points1, val, ( float* ) dataarrays[spacedata[0]]->GetPointer ( Comp ), nb_Components, ( float* ) field->GetPointer ( Comp ), nb_Components );
                }
            }
          else {
              * ( float* ) field->GetPointer ( Comp )  += val;
            }
        }
    }
}


void vtkPXDMFDocumentBaseStructure::reconstruction ( std::map <std::string, vtkSmartPointer <vtkDataArray > >::iterator& it, const  int& nb_modes, vtkIdList* pointIdGrids, const vtkstd::vector<int> &spacedata, unsigned fieldnumber, const unsigned pointdata,  std::vector<vtkCell*>* cellGrids, std::vector<std::vector<double> >* cellweights ) {

  if ( it->second.GetPointer()->IsA ( "vtkFloatArray" ) ) {
      for ( unsigned mode = 0; int ( mode ) < nb_modes; ++mode ) {
          reconstruction ( pointIdGrids, spacedata, fieldnumber, mode, static_cast<vtkFloatArray*> ( it->second.GetPointer() ), pointdata, cellGrids, cellweights );
        }
    }
  else {
      if ( it->second.GetPointer()->IsA ( "vtkDoubleArray" ) ) {
          for ( unsigned mode = 0; int ( mode ) < nb_modes; ++mode ) {
              reconstruction ( pointIdGrids, spacedata, fieldnumber, mode, static_cast<vtkDoubleArray*> ( it->second.GetPointer() ), pointdata, cellGrids, cellweights );
            }
        }

    }
};
vtkDataObject* vtkPXDMFDocumentBaseStructure::GenerateSpace() {

  const double findCellToll = 1e-15;

  vtkstd::vector<int> spacedata;
  spacedata.resize ( 3 );

  for ( unsigned i = 0; i < 3; ++i ) spacedata[i] = -1;

  int cpt = 0;

  for ( unsigned i = 0; i < this->AllData.size(); ++i ) {
      if ( this->GetActivePXDMFDimsForSpace() [i] ) {
          spacedata[cpt] = i;
          ++cpt;
        }
    }

  vtkFieldData* fd = vtkFieldData::New();
  vtkDoubleArray* newarraytime ;

  cpt = 0;

  int cptSpaceDims = 0;
  this->pointfound = true;
  this->cellfound = true;
  std::vector<std::string> names ;
  names.push_back ( std::string ( "X" ) );
  names.push_back ( std::string ( "Y" ) );
  names.push_back ( std::string ( "Z" ) );
    
  for ( unsigned i = 0; i < this->AllData.size(); ++i ) {
      if ( this->GetActivePXDMFDimForTime() [i] ) {
          if ( this->haveTime ) {
              newarraytime = vtkDoubleArray::New();
              newarraytime->SetName ( this->NamesOfEachDimension[i][0].c_str() );
              
              newarraytime->GetInformation()->Set ( vtkPXDMFDocumentBaseStructure::NAME(),this->NamesOfEachDimension[i][0].c_str() );
              newarraytime->GetInformation()->Set ( vtkPXDMFDocumentBaseStructure::UNIT(),this->UnitsOfEachDimension[i][0].c_str() );
              double bound[2];
              bound[0] = this->MinOfEachDimension[i][0];
              bound[1] = this->MaxOfEachDimension[i][0];
              newarraytime->GetInformation()->Set (
                vtkDataArray::COMPONENT_RANGE (),
                bound , 2 );
            }

          ++cpt;
          continue;
        }

      if ( this->GetActivePXDMFDimsForSpace() [i] ) {
          cpt += this->GetNumberOfDimsInEachPXDMFDim()->GetValue ( i );

          /// puting the names  of the space dimension in the fielddata
          for ( unsigned j = 0; int ( j ) < this->GetNumberOfDimsInEachPXDMFDim()->GetValue ( i ); ++j, ++cptSpaceDims ) {
              std::string name;

              vtkStringArray* TitleArray = vtkStringArray::New();
              TitleArray->SetName ( ("AxisTitleFor" + names[cptSpaceDims]).c_str() );
              TitleArray->InsertNextValue ( ( this->NamesOfEachDimension[i][j] + " " + this->UnitsOfEachDimension[i][j] ).c_str() );
              TitleArray->GetInformation()->Set (vtkPXDMFDocumentBaseStructure::NAME(),( this->NamesOfEachDimension[i][j] ).c_str() );
              TitleArray->GetInformation()->Set (vtkPXDMFDocumentBaseStructure::UNIT(),( this->UnitsOfEachDimension[i][j] ).c_str() );
              fd->AddArray ( TitleArray );
              TitleArray->Delete();
            }

          // we treat only the space dims
          continue;
        }

      vtkDataSet* grid = this->AllData[i];

      double x[3];
      x[0] = 0;
      x[1] = 0;
      x[2] = 0;

      double px[3];
      px[0] = 0;
      px[1] = 0;
      px[2] = 0;

      int cpt2 = cpt;

      for ( unsigned j = 0; int ( j ) < this->GetNumberOfDimsInEachPXDMFDim()->GetValue ( i ); ++j, ++cpt2 ) {
          x[j] = this->PosOfEachDimension[i][j];
          //std::cout << "Base this->PosOfEachDimension[i][j] " <<this->PosOfEachDimension[i][j] << std::endl;
        }

      vtkIdType point = grid->FindPoint ( x ) ;

      if ( point < 0 ) {
          std::cout << "Warning point (" << x[0] << "," << x[1] << "," << x[2] << ") of dimension " << i << " not found. Using point 0;" << std::endl;
          this->pointfound = false;
          point = 0;
        }
      else {
          cpt2 = 0 * cpt;

          for ( unsigned j = 0; int ( j ) < this->GetNumberOfDimsInEachPXDMFDim()->GetValue ( i ); ++j, ++cpt2 ) {
              px[j] = grid->GetPoint ( point ) [j];

            }
        }



      this->pointIdGrids->SetId ( i, point );

      int subId = 0;
      double pcoords[3];
      double weights[8];
      vtkIdType cellid = grid->FindCell ( x, this->cellGrids[i], this->cellIdGrids->GetId ( i ), findCellToll, subId, pcoords, weights );
      double dist;

      if ( cellid >= 0 ) {
          if ( grid->GetCell ( cellid )->EvaluatePosition ( x, NULL, subId, pcoords, dist, weights ) != 1 ) {
              cellid = -1;
            }
        }

      if ( this->UseInterpolation && cellid == -1 ) {
          this->pointfound = false;
        }

      /// puting all the parametric information in the field data
      /// puts the
      cpt2 = 0;

      for ( unsigned j = 0; int ( j ) < this->GetNumberOfDimsInEachPXDMFDim()->GetValue ( i ); ++j, ++cpt2 ) {
          vtkDoubleArray* newarray = vtkDoubleArray::New();
          newarray->SetName ( this->NamesOfEachDimension[i][j].c_str() );
          vtkInformation* info = newarray->GetInformation();

          if ( !info ) {
              std::cout << "infomation not initialized" << std::endl;
              return NULL;
            }

          info->Set ( vtkPXDMFDocumentBaseStructure::NAME(), this->NamesOfEachDimension[i][j].c_str() );
          info->Set ( vtkPXDMFDocumentBaseStructure::UNIT(), this->UnitsOfEachDimension[i][j].c_str() );


          double data[4];
          data[0] = this->MinOfEachDimension[i][j];
          data[1] = this->MaxOfEachDimension[i][j];
          data[2] = i;
          data[3] = j;
          info->Set (
            vtkPXDMFDocumentBaseStructure::BOUND_MIN_MAX_PXDMFDim_Dim(),
            data, 4 );


          if ( this->UseInterpolation && cellid != -1 ) {
              newarray->InsertNextValue ( x[j] );
              this->PosOfEachDimension[i][j] = x[j];
            }
          else {
              newarray->InsertNextValue ( px[j] );
              this->PosOfEachDimension[i][j] = px[j];
            }

          fd->AddArray ( newarray );
          newarray->Delete();
        }


      if ( cellid < 0 ) {
          cellid = grid->FindCell ( px, NULL, 0, findCellToll, subId, pcoords, weights );

          if ( cellid < 0 ) {
              std::cout << "Warning cell of dimension " << i << " not found. Using cell 0;" << std::endl;
              std::cout << "loof of pos" << px[0] << " " << px[1] << " " << px[2] << " not found. Using cell 0;" << std::endl;
              this->cellfound = false;
              weights[0] = 1;

              for ( int i = 1; i < 8; ++i ) weights[i] = 0;

              cellid = 0;
            }
        }

      //std::cout <<"*dim " << i << "cellid " << cellid << std::endl;
      this->cellIdGrids->SetId ( i, cellid );

      if ( cellid >= 0 ) {
          this->cellGrids[i] = grid->GetCell ( cellid );
        }
      else {
          this->cellGrids[i] = 0;
        }

      this->cellweights[i].resize ( this->cellGrids[i]->GetNumberOfPoints() );

      for ( int ii = 0; ii < int ( this->cellweights[i].size() ); ++ii ) {
          this->cellweights[i][ii] = weights[ii];
        }

      for ( int ii = 0; ii < 3; ++ii ) {
          this->cellpcoords[i][ii] = pcoords[ii];
        }

      cpt += this->GetNumberOfDimsInEachPXDMFDim()->GetValue ( i );
    }

  // Add a field data  with a flag to know is the points and the cells are found.
  vtkCharArray* pointFlag = vtkCharArray::New();
  pointFlag->SetName ( "vtkValidPointMask" );
  pointFlag->InsertNextValue ( this->pointfound );
  fd->AddArray ( pointFlag );
  pointFlag->Delete();

  vtkCharArray* cellFlag = vtkCharArray::New();
  cellFlag->SetName ( "vtkValidCellMask" );
  cellFlag->InsertNextValue ( this->cellfound );
  fd->AddArray ( cellFlag );
  cellFlag->Delete();


  if ( this->haveTime ) {
      unsigned i = 0;

      for ( ; i < this->AllData.size(); ++i ) if ( this->GetActivePXDMFDimForTime() [i] ) break;

      vtkDataSet* timeGrid = this->AllData[i];
      double x[3];
      x[0] = this->Time;
      x[1] = 0;
      x[2] = 0;

      double px[3];
      px[0] = 0;
      px[1] = 0;
      px[2] = 0;

      vtkIdType timepoint = timeGrid->FindPoint ( x );

      if ( timepoint < 0 ) {

          std::cout << "Warning time point not found. Using point 0;" << std::endl;
          px[0] = timeGrid->GetPoint ( 0 ) [0];
          px[1] = timeGrid->GetPoint ( 0 ) [1];
          px[2] = timeGrid->GetPoint ( 0 ) [2];
          x[0] = px[0];
          x[1] = px[1];
          x[2] = px[2];
          timepoint = 0;
        }
      else {
          px[0] = timeGrid->GetPoint ( timepoint ) [0];
          px[1] = timeGrid->GetPoint ( timepoint ) [1];
          px[2] = timeGrid->GetPoint ( timepoint ) [2];
        }

      //std::cout << "Time : "<<  x[0] << std::endl;

      this->pointIdGrids->SetId ( i, timepoint );

      //vtkCell *cell = NULL;
      //vtkGenericCell *gencell = NULL;
      //vtkIdType cellId;
      //double tol2= 0;
      int subId;
      double pcoords[3];
      double weights[8];
      //std::cout << "x " << x[0] << " " << x[1] << " "<< x[2] << " " << std::endl;
      //std::cout << "tol " <<  fabs(this->Time/10000) << " " << std::endl;
      //vtkIdType timecell = timeGrid->FindCell(x, NULL, 0, fabs(this->Time/10000), subId, pcoords, weights);
      vtkIdType timecell = timeGrid->FindCell ( x,  this->cellGrids[i], this->cellIdGrids->GetId ( i ), fabs ( this->Time / 10000 ), subId, pcoords, weights );

      if ( timecell < 0 ) {
          //std::cout << "px " << px[0] << " " << px[1] << " "<<px[2] << " " << std::endl;
          //timecell = timeGrid->FindCell(px, NULL, 0, fabs(this->Time/10000), subId, pcoords, weights);
          timecell = timeGrid->FindCell ( px, this->cellGrids[i], this->cellIdGrids->GetId ( i ), fabs ( this->Time / 10000 ), subId, pcoords, weights );

          //std::cout << "second " << std::endl;
          if ( timecell < 0 ) {
              std::cout << "Warning time cell not found. Using cell 0" << std::endl;
              timecell = 0;
            }
        }

      this->cellIdGrids->SetId ( i, timecell );
      this->cellGrids[i] = timeGrid->GetCell ( timecell );

      this->cellweights[i].resize ( this->cellGrids[i]->GetNumberOfPoints() );

      for ( int ii = 0; ii < int ( this->cellweights[i].size() ); ++ii ) {
          this->cellweights[i][ii] = weights[ii];
        }

      for ( int ii = 0; ii < 3; ++ii ) {
          this->cellpcoords[i][ii] = pcoords[ii];
        }

      newarraytime->InsertNextValue ( x[0] );
      fd->AddArray ( newarraytime );
      newarraytime->Delete();

      this->cellIdGrids->SetId ( i, timecell );
    }

  vtkDataSet* udata = 0;

  // for the tipe of mesh read
  // http://www.paraview.org/Wiki/ParaView/Users_Guide/VTK_Data_Model
  //std::cout << " this->VtkOutputType " << this->VtkOutputType<< std::endl;
  switch ( this->VtkOutputType ) {
    case VTK_UNIFORM_GRID: {
      udata = this->GenerateSpace_UniformGrid ( spacedata, this->pointIdGrids, this->cellIdGrids );
      break;
    }
    case VTK_RECTILINEAR_GRID: {
      udata = this->GenerateSpace_RectilinearGrid ( spacedata, this->pointIdGrids, this->cellIdGrids );
      break;
    }
    case VTK_STRUCTURED_GRID: {
      udata = this->GenerateSpace_StructuredGrid ( spacedata, this->pointIdGrids, this->cellIdGrids );
      break;
    }
    case VTK_UNSTRUCTURED_GRID: {
      udata = this->GenerateSpace_UnstructuredGrid ( spacedata, this->pointIdGrids, this->cellIdGrids );
      break;
    }
    case VTK_POLY_DATA: {
      udata = this->GenerateSpace_PolydataGrid ( spacedata, this->pointIdGrids, this->cellIdGrids );
      break;
    }
    }

  if ( udata ) {
      // adding meta data

      udata->SetFieldData ( fd );
      fd->Delete();

      // adding point data
      construct_point_fields ( this->pointIdGrids, spacedata, &this->cellGrids,  &this->cellweights );

      vtkPXDMFDocumentBaseStructure::TPointdata_name::iterator it = this->PointFieldData.begin();
      vtkPXDMFDocumentBaseStructure::TPointdata_name::iterator fit = this->PointFieldData.end();

      for ( ; it != fit; ++it ) {
          if ( it->second ) {
              //it->second->
              switch ( it->second->GetNumberOfComponents() ) {
                case 1 : {
                  udata->GetPointData()->SetActiveScalars ( it->first.c_str() );
                  udata->GetPointData()->SetScalars ( it->second );
                  break;
                }
                case 3 : {
                  udata->GetPointData()->SetActiveVectors ( it->first.c_str() );
                  udata->GetPointData()->SetVectors ( it->second );
                  break;
                }
                default :
                  break;
                }
            }
        }

      if ( this->UseInterpolation && this->ComputeDerivatives ) {
          std::map <std::string, vtkSmartPointer <vtkDataArray > >::iterator  itD = this->PointFieldDerivativeData.begin();
          std::map <std::string, vtkSmartPointer <vtkDataArray > >::iterator  fitD = this->PointFieldDerivativeData.end();

          for ( ; itD != fitD; ++itD ) {
              //std::cout << "+";
              if ( itD->second ) {
                  //std::cout << "working on  " << itD->first << std::endl;;
                  switch ( itD->second->GetNumberOfComponents() ) {
                    case 1 : {
                      udata->GetPointData()->SetActiveScalars ( itD->first.c_str() );
                      udata->GetPointData()->SetScalars ( itD->second );
                      break;
                    }
                    case 3 : {
                      udata->GetPointData()->SetActiveVectors ( itD->first.c_str() );
                      udata->GetPointData()->SetVectors ( itD->second );
                      break;
                    }
                    case 9 : {
                      udata->GetPointData()->SetActiveTensors ( itD->first.c_str() );
                      udata->GetPointData()->SetTensors ( itD->second );
                      break;
                    }
                    default :
                      udata->GetPointData()->SetActiveScalars ( itD->first.c_str() );
                      udata->GetPointData()->SetScalars ( itD->second );
                      break;
                    }
                }
            }
        }

      //std::cout << std::endl;

      // adding cell data
      construct_cell_fields ( this->cellIdGrids, spacedata );

      vtkPXDMFDocumentBaseStructure::TCelldata_name::iterator it_cel = this->CellFieldData.begin();
      vtkPXDMFDocumentBaseStructure::TCelldata_name::iterator fit_cel = this->CellFieldData.end();

      for ( ; it_cel != fit_cel; ++it_cel ) {
          if ( it_cel->second ) {
              switch ( it_cel->second->GetNumberOfComponents() ) {
                case 1 : {
                  udata->GetCellData()->SetActiveScalars ( it_cel->first.c_str() );
                  udata->GetCellData()->SetScalars ( it_cel->second );
                  break;
                }
                case 3 : {
                  udata->GetCellData()->SetActiveVectors ( it_cel->first.c_str() );
                  udata->GetCellData()->SetVectors ( it_cel->second );
                  break;
                }
                default :
                  break;
                }
            }
        }

      return udata;

    }

  fd->Delete();
  std::cout << "Incorrect output type" << std::endl;
  vtkErrorMacro ( "Failed to generate data." );
  return 0;
}
//
void vtkPXDMFDocumentBaseStructure::construct_point_fields ( vtkIdList* pointIdGrids, vtkstd::vector<int> spacedata,  std::vector<vtkCell*>* cellGrids, std::vector<std::vector<double> >* cellweights ) {

  vtkPXDMFDocumentBaseStructure::TPointdata_name::iterator it = this->PointFieldData.begin();
  vtkPXDMFDocumentBaseStructure::TPointdata_name::iterator fit = this->PointFieldData.end();
  std::vector<double> vals;
  ///alocation and zero setting
  unsigned  cpt = 0;

  for ( ; it != fit; ++it, ++cpt ) {
      unsigned mode = 0;
      vtkDataArray* temp_data = NULL;

      //std::string name;
      //for (; mode < this->Internal->NumberOfModesInEachPointField[it->first]; ++mode ) {
      for ( ; mode < this->GetNumberOfModesOfPointDataByNumber ( cpt ); ++mode ) {
          //const std::string name = std::string(it->first+"_"+to_string(mode));
          //std::cout << "searching for mode " << it->first << std::endl;
          for ( unsigned i = 0; i < this->GetNumberOfPXDMFDims(); ++i ) {
              temp_data = getArrayPoint ( cpt, mode, i );

              if ( temp_data  > 0 ) break;
            }

          if ( temp_data  > 0 ) break;

        }

      //if (mode >= this->Internal->NumberOfModesInEachPointField[it->first]) {
      if ( mode >= this->GetNumberOfModesOfPointDataByNumber ( cpt ) ) {
          //std::cout << "no modes for " << it->first << std::endl;
          it->second = NULL;
          continue;
        }

      if ( !temp_data ) {
          std::cout << "Error:  field " << it->first << "_" << mode << " not found" << std::endl;
        } //else {

      //std::cout << "OK:  field " << name << " found" << std::endl;
      //}
      unsigned nb_total;

      if ( spacedata[0] >= 0 ) {
          nb_total = this->AllData[spacedata[0]]->GetNumberOfPoints();

          if ( spacedata[1] >= 0 ) {
              nb_total *= this->AllData[spacedata[1]]->GetNumberOfPoints();

              if ( spacedata[2] >= 0 )
                nb_total *= this->AllData[spacedata[2]]->GetNumberOfPoints();
            }
        }
      else {
          /// when on space dims non dims are selected only one cell is created
          nb_total = 1;
        }

      unsigned nb_component = temp_data->GetNumberOfComponents();

      if ( strcmp ( temp_data->GetClassName(), "vtkFloatArray" ) == 0 ) {
          it->second = vtkSmartPointer<vtkFloatArray>::New();
          vtkFloatArray* data = static_cast<vtkFloatArray* > ( it->second.GetPointer() );
          data->SetNumberOfComponents ( nb_component );
          data->SetNumberOfTuples ( nb_total );
          data->SetName ( it->first.c_str() );
          std::fill ( data->GetPointer ( 0 ), data->GetPointer ( 0 ) + nb_total * nb_component , 0.0 );
        }
      else {
          it->second = vtkSmartPointer<vtkDoubleArray>::New();
          vtkDoubleArray* data = static_cast<vtkDoubleArray* > ( it->second.GetPointer() );
          //vtkSmartPointer<vtkDoubleArray>& data = it->second;
          data->SetNumberOfComponents ( nb_component );
          data->SetNumberOfTuples ( nb_total );
          data->SetName ( it->first.c_str() );
          std::fill ( data->GetPointer ( 0 ), data->GetPointer ( 0 ) + nb_total * nb_component , 0.0 );
        }
    }



  if ( this->ComputeDerivatives ) {
      this->PointFieldDerivativeData.clear();
    }

  it = this->PointFieldData.begin();
  cpt = 0;

  /// in the case a uniform mesh is used the field is constructed using the index (z,y,x) for the 3D or (y,x) for the 2D case
  if ( this->VtkOutputType == VTK_UNIFORM_GRID || this->VtkOutputType == VTK_RECTILINEAR_GRID ) {
      if ( spacedata[1] >= 0 ) {
          if ( spacedata[2] >= 0 ) {
              std::swap ( spacedata[0], spacedata[2] );
            }
          else {
              std::swap ( spacedata[0], spacedata[1] );
            }
        }
    }

  for ( ; it != fit; ++it, ++cpt ) {

      if ( it->second == NULL ) continue;

      if ( !vtkPXDMFDocumentBaseStructure::QUITE )
        this->SetProgressText ( ( "Building point field : " + it->first ).c_str() );

      //int nb_modes = this->Internal->NumberOfModesInEachPointField[it->first];
      int nb_modes = this->GetNumberOfModesOfPointDataByNumber ( cpt );

      this->reconstruction ( it, nb_modes, pointIdGrids, spacedata, cpt, 1, cellGrids, cellweights );

      bool FloatArray ;

      if ( it->second->IsA ( "vtkFloatArray" ) ) {
          FloatArray = true;
        }
      else {
          FloatArray = false;
        }

      std::map <std::string, vtkSmartPointer <vtkDataArray > > PointFieldDerivativeDataLocal;

      /// the derivative field in order, without the space dimension
      vtkstd::vector<std::string> PointFieldDerivativeDataLocal_name;
      vtkstd::vector<vtkSmartPointer <vtkDataArray > > PointFieldDerivativeDataLocal_field;

      PointFieldDerivativeDataLocal.clear();

      if ( this->UseInterpolation && this->ComputeDerivatives ) {

          ///building the fields and the names for the derivatives

          for ( unsigned o = 0; int ( o ) < this->NumberOfPXDMFDims; ++o ) {
              /// We canculate the derivatives only for non spacial coordinates
              if ( this->GetActivePXDMFDimsForSpace() [o] == 0 ) {
                  /// construction of the name
                  std::string name = "d" + it->first + "_d";

                  for ( int p = 0; p < GetNumberOfDimsInEachPXDMFDim()->GetValue ( o ); ++p ) {
                      name += this->GetNamesOfDimension ( o, p ) ;
                    }

                  //std::cout <<name  << std::endl;
                  if ( FloatArray ) {
                      //if(!this->Internal->PointFieldDerivativeData[name]){
                      vtkSmartPointer<vtkFloatArray> data = vtkSmartPointer<vtkFloatArray>::New();
                      data->SetNumberOfComponents ( it->second->GetNumberOfComponents() * 3 );
                      data->SetNumberOfTuples ( it->second->GetNumberOfTuples() );
                      data->SetName ( name.c_str() );

                      //for (unsigned point1= 0; point1 < data->GetNumberOfComponents()*data->GetNumberOfTuples(); ++point1) {
                      //    data->SetValue(point1, 0);
                      //}

                      std::fill ( data->GetPointer ( 0 ), data->GetPointer ( 0 ) + data->GetNumberOfComponents() *data->GetNumberOfTuples(), 0.0 );

                      PointFieldDerivativeDataLocal[name] = data;
                      PointFieldDerivativeDataLocal_name.push_back ( name );
                      PointFieldDerivativeDataLocal_field.push_back ( data );
                      //} else {
                      //  vtkDataArray*  datap = this->Internal->PointFieldDerivativeData[name].GetPointer();
                      //  vtkFloatArray* data =  static_cast<vtkFloatArray *>(datap);
                      //  std::fill( data->GetPointer(0), data->GetPointer(0) + data->GetNumberOfComponents()*data->GetNumberOfTuples(), 0.0 );
                      //}


                    }
                  else {
                      if ( !this->PointFieldDerivativeData[name] ) {
                          vtkSmartPointer<vtkDoubleArray> data = vtkSmartPointer<vtkDoubleArray>::New();;
                          data->SetNumberOfComponents ( it->second->GetNumberOfComponents() * 3 );
                          data->SetNumberOfTuples ( it->second->GetNumberOfTuples() );
                          data->SetName ( name.c_str() );
                          //for (unsigned point1= 0; point1< data->GetNumberOfComponents()*data->GetNumberOfTuples(); ++point1) {
                          //    data->SetValue(point1, 0);
                          //}

                          std::fill ( data->GetPointer ( 0 ), data->GetPointer ( 0 ) + data->GetNumberOfComponents() *data->GetNumberOfTuples(), 0.0 );


                          PointFieldDerivativeDataLocal[name] = data;
                          PointFieldDerivativeDataLocal_name.push_back ( name );
                          PointFieldDerivativeDataLocal_field.push_back ( data );
                        }
                      else {
                          //std::cout << "recycling " << name << std::endl;
                          vtkDataArray*  datap = this->PointFieldDerivativeData[name].GetPointer();
                          vtkDoubleArray* data =  static_cast<vtkDoubleArray*> ( datap );
                          std::fill ( data->GetPointer ( 0 ), data->GetPointer ( 0 ) + data->GetNumberOfComponents() *data->GetNumberOfTuples(), 0.0 );

                          PointFieldDerivativeDataLocal_name.push_back ( name );
                          PointFieldDerivativeDataLocal_field.push_back ( data );
                        }

                    }

                }
            }

          if ( PointFieldDerivativeDataLocal_name.size() != 0 ) { // In the case all the dimension are in space, so no derivation fields to computes
              if ( !vtkPXDMFDocumentBaseStructure::QUITE )
                this->SetProgressText ( ( "Building Point Field Derivative of " + it->first ).c_str() );

              for ( unsigned mode = 0; int ( mode ) < nb_modes; ++mode ) {
                  this->UpdateProgress ( double ( mode ) / nb_modes );

                  if ( getArrayPoint ( cpt, mode, unsigned ( 0 ) ) > 0 ) {
                      ;
                      this->reconstructionOfDerivatives ( pointIdGrids, spacedata, cpt, mode,  PointFieldDerivativeDataLocal_field, FloatArray, cellGrids, cellweights );
                    }
                }

            }
        }


      std::map< std::string, vtkSmartPointer< vtkDataArray > >::iterator itD = PointFieldDerivativeDataLocal.begin();
      std::map< std::string, vtkSmartPointer< vtkDataArray > >::iterator fitD = PointFieldDerivativeDataLocal.end();

      for ( ; itD != fitD; ++itD ) {
          this->PointFieldDerivativeData[itD->first] = itD->second;
        }


    }

  //std::cout << std::endl;
};

void vtkPXDMFDocumentBaseStructure::reconstructionOfDerivatives ( vtkIdList* pointIdGrids, vtkstd::vector<int> &spacedata, const unsigned fieldnumber , unsigned& mode, vtkstd::vector< vtkSmartPointer <vtkDataArray > >&  fields, const bool FloatArray,  std::vector<vtkCell*>* cellGrids, std::vector<std::vector<double> >* cellweights ) {
  //int index;
  int subId;
  double cellCenter[3];

  // number of component of the field before derivaton
  unsigned nb_Components = fields[0]->GetNumberOfComponents() / 3;

  if ( FloatArray ) {
      vtkstd::vector<vtkstd::vector<double> > derivatives;
      derivatives.resize ( this->NumberOfPXDMFDims );

      /// we reserve the space to hold the derivatives
      /// we compute the derivative of the mode in each dimension
      /// and for each component
      /// for each dimension
      for ( unsigned o = 0; o < this->NumberOfPXDMFDims; ++o ) {
          /// the mode for this dimension
          vtkFloatArray* data = vtkFloatArray::SafeDownCast ( getArrayPoint ( fieldnumber, mode, o ) );
          //vtkFloatArray* data = vtkFloatArray::SafeDownCast(getArrayPoint(name,mode,o ));

          //vtkFloatArray* data = vtkFloatArray::SafeDownCast(this->Internal->AllData[o]->GetPointData()->GetArray(std::string(name+"_"+to_string(mode)).c_str(), index));
          /// no need to calculate the derivative of a spacial dimension
          if ( this->GetActivePXDMFDimsForSpace() [o] == 0 ) {
              /// allocation for storing the derivation of the mode for the current dim
              derivatives[o].resize ( fields[0]->GetNumberOfComponents() );
              subId = ( *cellGrids ) [o]->GetParametricCenter ( cellCenter );
              /// number of points of the element of the
              int numpoints = ( *cellGrids ) [o]->GetNumberOfPoints();
              /// allocation for the value extraction
              vtkstd::vector<double> values ( numpoints );

              /// we recover the values of the mode on all the nodes of the element (for only one component) and then calculate the derivative
              for ( unsigned Comp = 0; Comp < nb_Components; ++Comp ) {
                  for ( int i = 0; i < numpoints; i++ ) {
                      values[i] = double ( data->GetValue ( ( *cellGrids ) [o]->GetPointId ( i ) * nb_Components + Comp ) );
                    }

                  /// Calculation of the derivative
                  ( *cellGrids ) [o]->Derivatives ( subId, &this->cellpcoords[o].front(), &values[0], 1, &derivatives[o].front() + Comp * 3 );
                  //(*cellGrids)[o]->Derivatives(subId, cellCenter, &values[0], 1, derivatives[o].data()+Comp*3);
                }
            }

        }

      /// casting of the vtkDataArray to vtkFloatArrays
      vtkstd::vector< vtkFloatArray* >  floatfields;
      floatfields.resize ( fields.size() );

      for ( unsigned i = 0; i < fields.size() ; ++i ) {
          floatfields[i] =  vtkFloatArray::SafeDownCast ( fields[i].GetPointer() );
        }

      this->reconstructionOfDerivativesFloat ( pointIdGrids, spacedata, fieldnumber, mode, floatfields, derivatives, cellGrids, cellweights );
      return;

    }
  else {
      vtkstd::vector<vtkstd::vector<double> > derivatives;
      derivatives.resize ( this->NumberOfPXDMFDims );

      // we reserve the space to hold the derivatives
      //
      for ( unsigned o = 0; o < this->NumberOfPXDMFDims; ++o ) {
          if ( this->GetActivePXDMFDimsForSpace() [o] == 0 ) {
              derivatives[o].resize ( fields[0]->GetNumberOfComponents() );
              subId = ( *cellGrids ) [o]->GetParametricCenter ( cellCenter );
              int numpoints = ( *cellGrids ) [o]->GetNumberOfPoints();
              vtkstd::vector<double> values ( numpoints );

              for ( unsigned Comp = 0; Comp < nb_Components; ++Comp ) {
                  vtkDoubleArray* data = vtkDoubleArray::SafeDownCast ( getArrayPoint ( fieldnumber, mode, o ) );

                  //vtkDoubleArray* data = vtkDoubleArray::SafeDownCast(getArrayPoint(name,mode,o ));
                  //vtkDoubleArray* data = vtkDoubleArray::SafeDownCast(this->Internal->AllData[o]->GetPointData()->GetArray(std::string(name+"_"+to_string(mode)).c_str(), index));
                  for ( int i = 0; i < numpoints; i++ ) {
                      values[i] = static_cast<double> ( data->GetValue ( ( *cellGrids ) [o]->GetPointId ( i ) * nb_Components + Comp ) );
                    }

                  ( *cellGrids ) [o]->Derivatives ( subId, &this->cellpcoords[o].front(), &values[0], 1, &derivatives[o].front() + Comp * 3 );
                  //cout.precision(16);
                  //(*cellGrids)[o]->Derivatives(subId, cellCenter, &values[0], 1, derivatives[o].data()+Comp*3);
                }
            }

        }

      vtkstd::vector< vtkDoubleArray* >  doublefields;
      doublefields.resize ( fields.size() );

      for ( unsigned i = 0; i < fields.size() ; ++i ) {
          doublefields[i] =  vtkDoubleArray::SafeDownCast ( fields[i].GetPointer() );
        }

      this->reconstructionOfDerivativesDouble ( pointIdGrids, spacedata, fieldnumber, mode, doublefields, derivatives, cellGrids, cellweights );
      return;
    }
};
//
void vtkPXDMFDocumentBaseStructure::reconstructionOfDerivativesFloat ( vtkIdList* pointIdGrids, vtkstd::vector<int> spacedata , unsigned fieldnumber , unsigned& mode,
    vtkstd::vector< vtkFloatArray* >&  fields, vtkstd::vector<vtkstd::vector<double> >& derivatives,  std::vector<vtkCell*>* cellGrids , std::vector<std::vector<double> >* cellweights ) {
  /// in the case a uniform mesh is used the field is constructed using the index (z,y,x) for the 3D or (y,x) for the 2D case

  if ( this->VtkOutputType == VTK_UNIFORM_GRID || this->VtkOutputType == VTK_RECTILINEAR_GRID ) {
      if ( spacedata[1] >= 0 ) {
          if ( spacedata[2] >= 0 ) {
              std::swap ( spacedata[0], spacedata[2] );
            }
          else {
              std::swap ( spacedata[0], spacedata[1] );
            }
        }
    }


  std::vector<vtkFloatArray*> dataarrays;
  dataarrays.resize ( this->NumberOfPXDMFDims );

  for ( unsigned i = 0; int ( i ) < this->NumberOfPXDMFDims; ++i ) {
      //int index;
      dataarrays[i] = static_cast<vtkFloatArray*> ( getArrayPoint ( fieldnumber, mode, i ) );

      //dataarrays[i] = vtkFloatArray::SafeDownCast(getArrayPoint(name,mode,i));;
      //dataarrays[i] = vtkFloatArray::SafeDownCast(this->Internal->AllData[i]->GetPointData()->GetArray(std::string(name+"_"+to_string(mode)).c_str(), index));
      if ( dataarrays[i] == 0 ) {
          return;
        }
    }

  unsigned nb_Components = fields[0]->GetNumberOfComponents() / 3;
  float val = 1;
  float tmpval = 0;

  for ( unsigned current_field = 0; current_field < fields.size(); ++current_field ) {                /// the derivative dimension
      for ( unsigned current_field_comp = 0; current_field_comp < 3; ++current_field_comp ) {         /// the derivative component

          for ( unsigned Comp = 0; Comp < nb_Components; ++Comp ) {                                   /// for each component of the original field

              val = 1;
              unsigned cpt_to_aling_current_field_with_PXDMFDims = 0;

              for ( unsigned o = 0; int ( o ) < this->NumberOfPXDMFDims; ++o ) {                    /// for each dimension
                  if ( this->GetActivePXDMFDimsForSpace() [o] == 0 ) {                                /// if dim is not spacial

                      if ( o - cpt_to_aling_current_field_with_PXDMFDims == current_field ) { // we use the derivative calculated
                          val *=  float ( derivatives[o][current_field_comp + Comp * 3] );
                          //std::cout << "using derivative field " <<  o << "  derivative component " << current_field_comp << "   and component " <<  Comp << "   val : " << derivatives[o][current_field_comp+Comp*3] << std::endl;
                        }
                      else {
                          //std::cout << "using normal mode for dim " << o << std::endl;
                          if ( this->UseInterpolation ) {
                              int pn = ( *cellGrids ) [o]->GetNumberOfPoints();
                              tmpval = 0;

                              for ( int pp = 0; pp < pn; ++pp ) {
                                  tmpval += dataarrays[o]->GetValue ( ( *cellGrids ) [o]->GetPointId ( pp ) * nb_Components + Comp ) * ( *cellweights ) [o][pp];
                                }

                              val *= tmpval;
                            }
                          else {
                              val *= dataarrays[o]->GetValue ( pointIdGrids->GetId ( o ) * nb_Components + Comp );
                            }
                        }
                    }
                  else {
                      cpt_to_aling_current_field_with_PXDMFDims += 1;

                    }
                }

              if ( val != 0. ) {
                  if ( spacedata[0] >= 0 ) {
                      int nb_points1 = dataarrays[spacedata[0]]->GetNumberOfTuples();

                      if ( spacedata[1] >= 0 ) {
                          int nb_points2 = dataarrays[spacedata[1]]->GetNumberOfTuples();

                          if ( spacedata[2] >= 0 ) {
                              // tree dimension
                              int nb_points3 = dataarrays[spacedata[2]]->GetNumberOfTuples();
                              unsigned cpt = current_field_comp + Comp * 3;

                              for ( unsigned point1 = 0; int ( point1 ) < nb_points1; ++point1 ) {
                                  const float val_temp1 = val * dataarrays[spacedata[0]]->GetValue ( point1 * nb_Components + Comp );

                                  for ( unsigned point2 = 0; int ( point2 ) < nb_points2; ++point2 ) {
                                      const float val_temp2 = val_temp1 * dataarrays[spacedata[1]]->GetValue ( point2 * nb_Components + Comp );
                                      SAXPY ( nb_points3, val_temp2, dataarrays[spacedata[2]]->GetPointer ( 0 + Comp ), nb_Components, fields[current_field]->GetPointer ( cpt ), nb_Components * 3 );
                                      cpt = cpt + nb_points3 * nb_Components * 3;
                                    }
                                }
                            }
                          else {
                              // only two dimension
                              unsigned cpt = current_field_comp + Comp * 3;

                              for ( unsigned point1 = 0; int ( point1 ) < nb_points1; ++point1 ) {
                                  //const double val_temp2 = val*dataarrays[spacedata[0]]->GetTuple1(point1*nb_Components+Comp);
                                  const float val_temp2 = val * dataarrays[spacedata[0]]->GetValue ( point1 * nb_Components + Comp );

                                  SAXPY ( nb_points2, val_temp2, dataarrays[spacedata[1]]->GetPointer ( Comp ), nb_Components, fields[current_field]->GetPointer ( cpt ), nb_Components * 3 );
                                  cpt = cpt + nb_points2 * nb_Components * 3;
                                }
                            }
                        }
                      else {
                          SAXPY ( nb_points1, val, dataarrays[spacedata[0]]->GetPointer ( Comp ), nb_Components, fields[current_field]->GetPointer ( current_field_comp + Comp * 3 ), nb_Components * 3 );
                        }
                    }
                  else {
                      * ( float* ) fields[current_field]->GetPointer ( current_field_comp + Comp * 3 )  += val;
                    }
                }
            }
        }
    }
}
//
//
void vtkPXDMFDocumentBaseStructure::reconstructionOfDerivativesDouble ( vtkIdList* pointIdsGrids, vtkstd::vector<int> spacedata, unsigned fieldnumber  , unsigned& mode,
    vtkstd::vector< vtkDoubleArray* >&  fields, vtkstd::vector<vtkstd::vector<double> >& derivatives,  std::vector<vtkCell*>* cellGrids , std::vector<std::vector<double> >* cellweights ) {
  /// in the case a uniform mesh is used the field is constructed using the index (z,y,x) for the 3D or (y,x) for the 2D case
  if ( this->VtkOutputType == VTK_UNIFORM_GRID || this->VtkOutputType == VTK_RECTILINEAR_GRID ) {
      if ( spacedata[1] >= 0 ) {
          if ( spacedata[2] >= 0 ) {
              std::swap ( spacedata[0], spacedata[2] );
            }
          else {
              std::swap ( spacedata[0], spacedata[1] );
            }
        }
    }

  std::vector<vtkDoubleArray*> dataarrays;
  dataarrays.resize ( this->NumberOfPXDMFDims );

  for ( unsigned i = 0; int ( i ) < this->NumberOfPXDMFDims; ++i ) {
      //int index;
      dataarrays[i] = vtkDoubleArray::SafeDownCast ( getArrayPoint ( fieldnumber, mode, i ) );;

      //dataarrays[i] = vtkDoubleArray::SafeDownCast(getArrayPoint(name,mode,i));;
      //dataarrays[i] = vtkDoubleArray::SafeDownCast(this->Internal->AllData[i]->GetPointData()->GetArray(std::string(name+"_"+to_string(mode)).c_str(), index));
      if ( dataarrays[i] == 0 ) {
          return;
        }
    }

  unsigned nb_Components = fields[0]->GetNumberOfComponents() / 3;
  double val = 1;
  double tmpval = 0;

  for ( unsigned current_field = 0; current_field < fields.size(); ++current_field ) {        // the derivative dimension
      for ( unsigned current_field_comp = 0; current_field_comp < 3; ++current_field_comp ) {         // the derivative component

          for ( unsigned Comp = 0; Comp < nb_Components; ++Comp ) {                                   // for each componenet
              val = 1;
              unsigned cpt_to_aling_current_field_with_PXDMFDims = 0;

              for ( unsigned o = 0; int ( o ) < this->NumberOfPXDMFDims; ++o ) {
                  if ( this->GetActivePXDMFDimsForSpace() [o] == 0 ) {

                      //std::cout << "current_field " << current_field << std::endl;
                      if ( o - cpt_to_aling_current_field_with_PXDMFDims == current_field ) { // we use the derivative calculated
                          val *=  derivatives[o][current_field_comp + Comp * 3];
                          //std::cout << "using derivative field " <<  o << "  derivative component " << current_field_comp << "   and component " <<  Comp << "   val : " << derivatives[o][current_field_comp+Comp*3] << std::endl;
                        }
                      else {
                          //std::cout << "using normal mode for dim " << o << std::endl;
                          if ( this->UseInterpolation ) {
                              int pn = ( *cellGrids ) [o]->GetNumberOfPoints();
                              tmpval = 0;

                              for ( int pp = 0; pp < pn; ++pp ) {
                                  tmpval += dataarrays[o]->GetValue ( ( *cellGrids ) [o]->GetPointId ( pp ) * nb_Components + Comp ) * ( *cellweights ) [o][pp];
                                }

                              val *= tmpval;
                            }
                          else {
                              val *= dataarrays[o]->GetValue ( pointIdsGrids->GetId ( o ) * nb_Components + Comp );
                            }
                        }
                    }
                  else {
                      cpt_to_aling_current_field_with_PXDMFDims += 1;

                    }
                }

              if ( val != 0. ) {
                  if ( spacedata[0] >= 0 ) {
                      int nb_points1 = dataarrays[spacedata[0]]->GetNumberOfTuples();

                      if ( spacedata[1] >= 0 ) {
                          int nb_points2 = dataarrays[spacedata[1]]->GetNumberOfTuples();

                          if ( spacedata[2] >= 0 ) {
                              // tree dimension
                              int nb_points3 = dataarrays[spacedata[2]]->GetNumberOfTuples();
                              unsigned cpt = current_field_comp + Comp * 3;

                              for ( unsigned point1 = 0; int ( point1 ) < nb_points1; ++point1 ) {
                                  const double val_temp1 = val * dataarrays[spacedata[0]]->GetValue ( point1 * nb_Components + Comp );

                                  for ( unsigned point2 = 0; int ( point2 ) < nb_points2; ++point2 ) {
                                      const double val_temp2 = val_temp1 * dataarrays[spacedata[1]]->GetValue ( point2 * nb_Components + Comp );
                                      DAXPY ( nb_points3, val_temp2, ( double* ) dataarrays[spacedata[2]]->GetPointer ( Comp ), nb_Components, ( double* ) fields[current_field]->GetPointer ( cpt ), nb_Components * 3 );
                                      cpt = cpt + nb_points3 * nb_Components * 3;
                                    }
                                }
                            }
                          else {
                              // only two dimension
                              unsigned cpt = current_field_comp + Comp * 3;

                              for ( unsigned point1 = 0; int ( point1 ) < nb_points1; ++point1 ) {
                                  //const double val_temp2 = val*dataarrays[spacedata[0]]->GetTuple1(point1*nb_Components+Comp);
                                  const double val_temp2 = val * dataarrays[spacedata[0]]->GetValue ( point1 * nb_Components + Comp );

                                  DAXPY ( nb_points2, val_temp2, ( double* ) dataarrays[spacedata[1]]->GetPointer ( Comp ), nb_Components, ( double* ) fields[current_field]->GetPointer ( cpt ), nb_Components * 3 );
                                  cpt = cpt + nb_points2 * nb_Components * 3;
                                }
                            }
                        }
                      else {
                          DAXPY ( nb_points1, val, ( double* ) dataarrays[spacedata[0]]->GetPointer ( Comp ), nb_Components, ( double* ) fields[current_field]->GetPointer ( current_field_comp + Comp * 3 ), nb_Components * 3 );
                        }
                    }
                  else {
                      * ( double* ) fields[current_field]->GetPointer ( current_field_comp + Comp * 3 )  += val;
                    }
                }
            }
        }
    }
}
//
vtkstd::vector<vtkstd::vector<double> >&  vtkPXDMFDocumentBaseStructure::GetPosOfEachDimension() {
  return PosOfEachDimension;
};


void addtime ( vtkstd::vector<double>& time_steps, const double& time, unsigned& cpt ) {
  for ( unsigned i = 0;; ++i ) {
      if ( i == cpt ) {
          time_steps[i] = time;
          ++cpt;
          break;
        }

      if ( fabs ( time_steps[i] - time ) < 1e-15 )  break;
    }
};
//
void vtkPXDMFDocumentBaseStructure::UpdateInternalData() {
  this->NumberOfModesInEachPointFieldByNumber.resize ( this->PointFieldData.size() );
  //std::cout << "this->Internal->PointFieldData.size() : " << this->Internal->PointFieldData.size() << std::endl;
  unsigned  cpt = 0;
  vtkPXDMFDocumentBaseStructure::TPointdata_name::iterator it = this->PointFieldData.begin();
  vtkPXDMFDocumentBaseStructure::TPointdata_name::iterator fit = this->PointFieldData.end();

  for ( ; it != fit; ++it, ++cpt ) {
      this->NumberOfModesInEachPointFieldByNumber[cpt] = this->NumberOfModesInEachPointField[it->first];
      //std::cout << cpt << " this->Internal->NumberOfModesInEachPointField[it->first] " << this->Internal->NumberOfModesInEachPointField[it->first] << std::endl;
    }

  this->NumberOfModesInEachCellFieldByNumber.resize ( this->CellFieldData.size() );
  cpt = 0;
  vtkPXDMFDocumentBaseStructure::TCelldata_name::iterator itc = this->CellFieldData.begin();
  vtkPXDMFDocumentBaseStructure::TCelldata_name::iterator fitc = this->CellFieldData.end();

  for ( ; itc != fitc; ++itc, ++cpt ) {
      this->NumberOfModesInEachCellFieldByNumber[cpt] = this->NumberOfModesInEachCellField[itc->first];
    }
}

int vtkPXDMFDocumentBaseStructure::GetNumberOfVisualizationSpaceArrays() {
  return this->GetNumberOfPXDMFDims();
};
//
const char* vtkPXDMFDocumentBaseStructure::GetVisualizationSpaceArrayName ( int index ) {
  if ( PXDMFDimsArrayNames.empty() ) {
      for ( int j = 0; j < this->GetNumberOfPXDMFDims(); ++j ) {
          std::string dimNames;
          dimNames = dimNames + this->GetNamesOfDimension ( j, 0 );

          for ( unsigned i = 1; int ( i ) < this->GetNumberOfDimsInEachPXDMFDim()->GetValue ( j ); ++i ) {
              dimNames = dimNames + ",";
              dimNames = dimNames + this->GetNamesOfDimension ( j, i );
            }

          PXDMFDimsArrayNames[j] = std::string ( std::string ( "Dim " ) + to_string ( j ) + std::string ( " size : " ) + to_string ( this->GetNumberOfDimsInEachPXDMFDim()->GetValue ( j ) ) + std::string ( " ( " ) + dimNames + std::string ( " ) " ) );
        }

    };

  if ( PXDMFDimsArrayNames.find ( index ) == PXDMFDimsArrayNames.end() )
    return NULL;
  else {
      return PXDMFDimsArrayNames[index].c_str();
    }
};
//
int vtkPXDMFDocumentBaseStructure::GetVisualizationSpaceArrayStatus ( const char* name ) {
  for ( unsigned i = 0; int ( i ) < this->GetNumberOfPXDMFDims(); ++i ) {
      if ( strcmp ( name, GetVisualizationSpaceArrayName ( i ) ) == 0 ) {
          return this->GetActivePXDMFDimsForSpace() [i];
        }
    }

  return 0;
};
//
void vtkPXDMFDocumentBaseStructure::SetVisualizationSpaceStatus ( const char* name, int status ) {
  if ( status == 0 ) {
      for ( unsigned i = 0; int ( i ) < this->GetNumberOfPXDMFDims(); ++i ) {
          if ( strcmp ( name, GetVisualizationSpaceArrayName ( i ) ) == 0 ) {
              this->GetActivePXDMFDimsForSpace() [i] = ( status != 0 );
            }
        }
    }
  else {
      int used_dims = 0 ;

      for ( unsigned i = 0; i < this->GetActivePXDMFDimsForSpace().size(); ++i ) {
          if ( this->GetActivePXDMFDimsForSpace() [i] )
            used_dims += this->GetNumberOfDimsInEachPXDMFDim()->GetValue ( i );
        }

      for ( unsigned i = 0; int ( i ) < this->GetNumberOfPXDMFDims(); ++i ) {
          //std::cout << "working on Dim  " << i << std::endl;
          if ( ( strcmp ( name, GetVisualizationSpaceArrayName ( i ) ) == 0 ) && ( this->GetActivePXDMFDimsForSpace() [i] != ( status != 0 ) ) ) {
              this->GetActivePXDMFDimsForSpace() [i] = ( status != 0 );
              this->Modified();
              //std::cout << "Dim  " << i << " " << (status!=0) << std::endl;
              break;
            }
        }
    }

  this->Modified();
  return;
};
