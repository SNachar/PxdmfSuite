/*=========================================================================

  Program:   PXDMFReader Plugin
  Module:    vtkrawWriter.cxx

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
#include "vtkImageData.h"

// PXDMFReader Pluing Includes
#include "vtkrawWriter.h"


template<typename T>
std::string to_string( const T & Value ) {
    std::ostringstream oss;
    oss << Value;
    return oss.str();
}

std::string to_string(int n);
std::string to_string(unsigned val );

//==============================================================================
vtkStandardNewMacro(vtkrawWriter);
//----------------------------------------------------------------------------
vtkrawWriter::vtkrawWriter(){
  this->FileName = NULL;
  
};
//----------------------------------------------------------------------------
vtkrawWriter::~vtkrawWriter()
{
  this->SetFileName(0);
}
//----------------------------------------------------------------------------
void vtkrawWriter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
//------------------------------------------------------------------------------
ofstream *vtkrawWriter::OpenFile(){
    ofstream *fptr;
  
    if ( !this->FileName ){
      vtkErrorMacro(<< "No FileName specified! Can't write!");
      return NULL;
    }
    
    vtkDebugMacro(<<"Opening file for writing...");
    
#ifdef _WIN32
      fptr = new ofstream(this->FileName, ios::out | ios::binary);
#else
      fptr = new ofstream(this->FileName, ios::out);
#endif

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
void vtkrawWriter::CloseFile(ofstream *fp){
  
  vtkDebugMacro(<<"Closing file\n");

  if ( fp != NULL ){
      fp->close();
      delete fp;
    }
    
}
//------------------------------------------------------------------------------

void vtkrawWriter::WriteData(){
  
  ofstream *fp;
  //char str[1024];
  //vtkImageData *input= vtkImageData::SafeDownCast(this->GetInput());
  
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
  *fp << "NO DATA \n" ;
 
  
  this->CloseFile(fp);
   //unsigned char *cptr =  static_cast<vtkUnsignedCharArray *>(data)->GetPointer(0);
   //fp->write(reinterpret_cast<char *>(cptr),  (sizeof(unsigned char))*((num-1)/8+1));
  
}
//
void vtkrawWriter::SetWriteHeader(int wh){
  this-> writeHeader = wh;
};
//
// void vtkrawWrite::SetFileName (const char* _arg) {
//   
//   std::string heavyfile = this->GetFileName() ;
//   heavyfile = heavyfile.substr(0, heavyfile.find_last_of(".")) + ".txt";
// headerFilename
// 
//   this->SetHeavyDataFileName(heavyfile.c_str()) ;
//   this->SetHeavyDataGroupName("PXDMFSolution");
//   std::cout << " - -" << this->GetHeavyDataFileName() << " GetHeavyDataFileName " << std::endl;
// } 

//----------------------------------------------------------------------------
int vtkrawWriter::FillInputPortInformation(int, vtkInformation *info){
  
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkImageData");
  return 1;
}

