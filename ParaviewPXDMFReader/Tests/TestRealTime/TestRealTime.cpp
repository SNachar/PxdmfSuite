/*=========================================================================

  Program:   PXDMFReader Plugin
  Module:    TestRealTime.cpp

  Copyright (c) GeM, Ecole Centrale Nantes.
  All rights reserved.
  Copyright: See COPYING file that comes with this distribution


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME TestRealTime
// .SECTION Description

#include "vtkPXDMFReader.h"
#include <vtkDataSet.h>
#include <vtkDoubleArray.h>

int main( int argc, char* argv[]) {
  
    std::string filename = argv[1] ;
    std::cout << "using file : " <<   filename  << std::endl;
    vtkPXDMFReader* reader = vtkPXDMFReader::New();
    vtkPXDMFReader::QUITE = true;
    
    reader->SetFileName(filename.c_str());
    std::cout << "Reading File ... ";std::cout.flush();
    reader->UpdateInformation();
    //reader->SetComputeDerivatives(true);;
    reader->Update();
    reader->GetOutput(); 
    std::cout << "Done " << std::endl;
    std::cout << "Reconstruction " << std::endl;
    double min = reader->GetPXDMFDimsMinRangeDataInfo()->GetValue(0);
    double max = reader->GetPXDMFDimsMaxRangeDataInfo()->GetValue(0);
    reader->GetActivePXDMFDimsForSpace()[1] = 1;
    for(unsigned i =0; i < 1000 ; ++i){
     // std::cout << "n : " << i << std::endl;
      float r = (float)rand()/(float)RAND_MAX;
      reader->SetPosOfEachDimension(0,0,r*(max-min)+min);
      //r = (float)rand()/(float)RAND_MAX;
      //reader->SetPosOfEachDimension(1,1,r);
      reader->Update();
      reader->GetOutput();      
    }
    
    reader->Delete();
    
}