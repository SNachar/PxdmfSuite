/*=========================================================================

  Program:   PXDMFReader Plugin
  Module:    vtkgmshWriter.cxx

  Copyright (c) GeM, Ecole Centrale Nantes.
  All rights reserved.
  Copyright: See COPYING file that comes with this distribution


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// STD includes
#include "vector"
#include <sstream>
#include <map>
#include <cstdio>  // for the unlink
#ifdef _WIN32
#include <io.h>  // for the unlink
#define unlink _unlink
#else
#include <unistd.h>  // for the unlink
#endif

//VTK Includes
#include "vtkIdTypeArray.h"
#include "vtkSmartPointer.h"
#include "vtkPointSet.h"
#include "vtkGenericCell.h"
#include "vtkObjectFactory.h"
#include "vtkInformation.h"
#include "vtkFieldData.h"
#include <vtkStringArray.h>
#include "vtkCellData.h"
#include "vtkPointData.h"
#include "vtkErrorCode.h"
#include "vtkByteSwap.h"
#include <vtkFloatArray.h>
#include <vtkDoubleArray.h>
#include "vtkUnstructuredGrid.h"
#include "vtkCellArray.h"

// PXDMFReader Pluing Includes
#include "vtkgmshWriter.h"

template<typename T>
std::string to_string( const T & Value ) {
    std::ostringstream oss;
    oss << Value;
    return oss.str();
}

std::string to_string(int n);
std::string to_string(unsigned val );

//==============================================================================
vtkStandardNewMacro(vtkgmshWriter);
//----------------------------------------------------------------------------
vtkgmshWriter::vtkgmshWriter(){
  this->FileName = NULL;
  
};
//----------------------------------------------------------------------------
vtkgmshWriter::~vtkgmshWriter()
{
  this->SetFileName(0);
}
//----------------------------------------------------------------------------
void vtkgmshWriter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
//------------------------------------------------------------------------------
ofstream *vtkgmshWriter::OpenFile(){
    ofstream *fptr;
  
    if ( !this->FileName ){
      vtkErrorMacro(<< "No FileName specified! Can't write!");
      return NULL;
    }
    
    vtkDebugMacro(<<"Opening file for writing...");
    
    fptr = new ofstream(this->FileName, ios::out);

    if (fptr->fail())
    {
    vtkErrorMacro(<< "Unable to open file: "<< this->FileName);
    this->SetErrorCode(vtkErrorCode::CannotOpenFileError);
    delete fptr;
    return NULL;
    }

  return fptr; 
};
//------------------------------------------------------------------------------------
// Close a file.
void vtkgmshWriter::CloseFile(ofstream *fp){
  
  vtkDebugMacro(<<"Closing file\n");

  if ( fp != NULL ){
      fp->close();
      delete fp;
    }
    
}
//------------------------------------------------------------------------------------
template <class T>
void vtkWriteDataArray(ostream *fp, T *data, int fileType,
                       const char *format, int num, int numComp)
{
  int i, j, idx, sizeT;
  char str[1024];

  sizeT = sizeof(T);

  if ( fileType == VTK_ASCII )
    {
    for (j=0; j<num; j++)
      {
      for (i=0; i<numComp; i++)
        {
        idx = i + j*numComp;
        sprintf (str, format, *data++); *fp << str;
        if ( !((idx+1)%3) )
          {
          *fp << "\n";
          }
        }
      }
    }
  else
    {
    if (num*numComp > 0)
      {
      // need to byteswap ??
      switch (sizeT)
        {
        case 2:
          // no typecast needed here; method call takes void* data
          vtkByteSwap::SwapWrite2BERange(data, num*numComp, fp);
          break;
        case 4:
          // no typecast needed here; method call takes void* data
          vtkByteSwap::SwapWrite4BERange(data, num*numComp, fp);
          break;
        case 8:
          // no typecast needed here; method call takes void* data
          vtkByteSwap::SwapWrite8BERange(data, num*numComp, fp);
          break;
        default:
          fp->write(reinterpret_cast<char*>(data), sizeof(T) * (num*numComp));
          break;
        }
      }
    }
  *fp << "\n";
}


void vtkgmshWriter::WriteData(){
  
  ofstream *fp;
  char str[1024];
  vtkUnstructuredGrid *input= vtkUnstructuredGrid::SafeDownCast(this->GetInput());
  
  
  std::cout << "Opening files  " << this->FileName << " ---- " << std::endl;
  fp=this->OpenFile();

  if (!fp){
    std::cout << "error " << std::endl;
    vtkErrorMacro("Ran out of disk space; deleting file: "
                    << this->FileName);
      this->CloseFile(fp);
      unlink(this->FileName);
      return;
  }


    *fp << "$NOD\n";
    
    {
    /// write points 
    vtkPoints* data = input->GetPoints();
    int numPts=data->GetNumberOfPoints();
    
    *fp <<numPts <<  "\n";
    
     switch (data->GetDataType()) {
     case VTK_FLOAT:
       {
       float *s=static_cast<vtkFloatArray *>(data->GetData())->GetPointer(0);
       for(int i =0; i < numPts; ++i){
         *fp << i+1 << " ";
         for(int i =0; i < 3; ++i){
           sprintf (str, "%lg ", *(s++)); *fp << str;
         }
         *fp << "\n";
       }
       
    }
     break;
     case VTK_DOUBLE:{
       double *s=static_cast<vtkDoubleArray *>(data->GetData())->GetPointer(0);
       for(int i =0; i < numPts; ++i){
         *fp << i+1;
         for(int i =0; i < 3; ++i){
           sprintf (str, "%lg ", *(s++)); *fp << str;
         }
         *fp << "\n";
       }
      }
  break;
     }
    *fp <<   "$ENDNOD\n"  ;
    }
  
  /// element 
  int ncells, cellId;
  
  if ( input->GetCells() ){
    
    
    vtkDataArray* celldata =  input->GetCellData()->GetScalars("PhyTag");
    
    vtkFloatArray *floatdata =NULL;
    vtkDoubleArray *doubledata =NULL;
    if(celldata){
      floatdata = vtkFloatArray::SafeDownCast(celldata);
      if(!floatdata){
        doubledata = vtkDoubleArray::SafeDownCast(celldata);
      }
    } 
    
    
    *fp <<  "$ELM\n" ;
    ncells = input->GetCells()->GetNumberOfCells();
    *fp <<  ncells << "\n" ;
    
    for (cellId=0; cellId < ncells; cellId++) {
      *fp <<  cellId + 1 << " " ;    /// elm-number
      
      ///elm-type
      switch(input->GetCellType(cellId)){
        case VTK_LINE:
          *fp <<  "1 ";
          break;    
        case VTK_TRIANGLE:
          *fp <<  "2 ";
          break;          
        case VTK_QUAD:
          *fp <<  "3 ";
          break;    
        case VTK_TETRA:
          *fp <<  "4 ";
          break;
        case VTK_HEXAHEDRON:
          *fp <<  "5 ";
          break;
        case VTK_WEDGE:
          *fp <<  "6 ";
          break;    
        case VTK_PYRAMID:
          *fp <<  "7 ";
          break;    
        case VTK_QUADRATIC_EDGE:
          *fp <<  "8 ";
          break;   
        case VTK_QUADRATIC_TRIANGLE:
          *fp <<  "9 ";
          break;       
        case  VTK_BIQUADRATIC_QUAD:
          *fp <<  "10 ";
          break;     
        case   VTK_TRIQUADRATIC_HEXAHEDRON:
          *fp <<  "12 ";
          break;  
        case VTK_VERTEX:
          *fp <<  "15 ";
          break;
        case VTK_QUADRATIC_QUAD:
          *fp <<  "16 ";
          break; 
        default:
          std::cout << "Error : cell type " << input->GetCellType(cellId) << " not coded yet, sorry." << std::endl;
          *fp <<  "99 ";
          break; 
      }

     int tag = 0;
     
     if( floatdata)
       tag = floatdata->GetValue(cellId) ;
     else if(doubledata){
       tag = doubledata->GetValue(cellId) ;
     } 
     
     ///reg-phys 
     *fp << tag << " ";    
     ///reg-elem 
     *fp << tag  << " ";
     
     ///number-of-nodes 
     int nodes = input->GetCell(cellId)->GetNumberOfPoints();
     *fp << nodes << " ";
     ///node-number-list
     for(int i=0; i < nodes ; ++i)
       *fp << input->GetCell(cellId)->GetPointId(i)+1 << " ";
     *fp << "\n";
   }
    *fp <<   "$ENDELM\n"  ;
  }
  this->CloseFile(fp);
  
  
  ///return vtkXdmfWriter::Write();
}
//----------------------------------------------------------------------------
int vtkgmshWriter::FillInputPortInformation(int,
                                                  vtkInformation *info)
{
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkUnstructuredGrid");
  return 1;
}
