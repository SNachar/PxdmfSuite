/*=========================================================================

  Program:   PXDMFReader Plugin
  Module:    TestMemLeak.cpp

  Copyright (c) GeM, Ecole Centrale Nantes.
  All rights reserved.
  Copyright: See COPYING file that comes with this distribution


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME TestMemLeak 
// .SECTION Description

#include "vtkPXDMFDocument.h"

int main( int argc, char* argv[]) {
  
   std::string filename;
    if(argc > 1 ){
      filename = argv[1] ;
      filename  += "test_ascii_1DCoRectMesh_1DCoRectMesh.pxdmf";
    } else {
      std::cerr <<  "I need the path of the data folder" << std::endl;
      return 1;
    }
    vtkPXDMFDocument* reader =vtkPXDMFDocument::New();
    reader->SetFileName(filename.c_str());
    reader->UpdateInformation();
    reader->GetActivePXDMFDimsForSpace()[0] = 1;
    reader->Update();
    reader->GetOutput();
    
    reader->GetActivePXDMFDimsForSpace()[0] = 0;
    reader->GetActivePXDMFDimsForSpace()[1] = 0;
    reader->Update();
    reader->GetOutput();//->Print(std::cout);
    reader->Delete();
    
    return 0;
}