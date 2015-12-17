/*=========================================================================

  Program:   PXDMFReader Plugin
  Module:    TestReconstruction.cpp

  Copyright (c) GeM, Ecole Centrale Nantes.
  All rights reserved.
  Copyright: See COPYING file that comes with this distribution


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME TestReconstruction
// .SECTION Description

// STD Includes
#include  <cassert>

// VTK Includes
#include  "vtkXMLUnstructuredGridReader.h"
#include  "vtkDataObject.h"
#include  "vtkDataSet.h"
#include  "vtkPointData.h"
#include  "vtkDataArray.h"
#include  "vtkXMLMultiBlockDataReader.h"

// Plugin Includes 
#include  "vtkPXDMFDocument.h"
#include  "vtkReconstruction.h"

int main( int argc, char* argv[]) {
  
   std::string filename;
   std::string filename1;
   std::string filename2;
   
    if(argc > 1 ){
      filename = argv[1] ;
      filename  += "testvtm.vtm";
      
      filename1 = argv[1] ;
      filename1  += "testvtm_0_0.vtu";
      filename2 = argv[1] ;
      filename2  += "testvtm_1_0.vtu";      
    } else {
      std::cerr <<  "I need the path of the data folder" << std::endl;
      return 1;
    }
    
    vtkSmartPointer<vtkXMLUnstructuredGridReader> reader1 = vtkSmartPointer<vtkXMLUnstructuredGridReader>::New();
    reader1->SetFileName(filename1.c_str());
  
    vtkSmartPointer<vtkXMLUnstructuredGridReader> reader2 = vtkSmartPointer<vtkXMLUnstructuredGridReader>::New();
    reader2->SetFileName(filename2.c_str());
      
    vtkSmartPointer<vtkReconstruction> reconstruction= vtkSmartPointer<vtkReconstruction>::New();

    reconstruction->SetInputConnection(reader1->GetOutputPort());
  
    reconstruction->AddInputConnection (reader2->GetOutputPort());
    
    
    reconstruction->UpdateInformation();
    reconstruction->SetVisualizationSpaceStatus(reconstruction->GetVisualizationSpaceArrayName(0), true);
    reconstruction->SetVisualizationSpaceStatus(reconstruction->GetVisualizationSpaceArrayName(1), true);
    reconstruction->Update();
    
    std::cout << "number of point " << ((vtkDataSet*) reconstruction->GetOutputDataObject(0))->GetNumberOfPoints() << std::endl;
    int numberOfPointFields = ((vtkDataSet*) reconstruction->GetOutputDataObject(0))->GetPointData()->GetNumberOfArrays();
    std::cout << "number of fields " <<   numberOfPointFields  << std::endl;
    for (int i =0; i < numberOfPointFields ; ++i){
      std::cout << "  field " << i << " "  <<   ((vtkDataSet*) reconstruction->GetOutputDataObject(0))->GetPointData()->GetArray(i)->GetName() << std::endl;  
      
    } 
    assert(numberOfPointFields != 0);
    
    /// Using a multiBlock    
    std::cout << "***  Using a multiBlock  ***" << std::endl;
    std::cout << "Reading " << filename << std::endl;
    vtkSmartPointer<vtkXMLMultiBlockDataReader> reader = vtkSmartPointer<vtkXMLMultiBlockDataReader>::New();
    reader->SetFileName(filename.c_str());

    vtkSmartPointer<vtkReconstruction> reconstruction2= vtkSmartPointer<vtkReconstruction>::New();
    reconstruction2->SetInputConnection(reader->GetOutputPort());
    
    reconstruction2->UpdateInformation();
    reconstruction2->SetVisualizationSpaceStatus(reconstruction2->GetVisualizationSpaceArrayName(0), true);
    reconstruction2->SetVisualizationSpaceStatus(reconstruction2->GetVisualizationSpaceArrayName(1), true);
    reconstruction2->Update();
    
    std::cout << "number of point " << ((vtkDataSet*) reconstruction2->GetOutputDataObject(0))->GetNumberOfPoints() << std::endl;
    int numberOfPointFields2 = ((vtkDataSet*) reconstruction2->GetOutputDataObject(0))->GetPointData()->GetNumberOfArrays();
    std::cout << "number of fields " <<   numberOfPointFields2  << std::endl;
    for (int i =0; i < numberOfPointFields2 ; ++i){
      std::cout << "  field " << i << " "  <<   ((vtkDataSet*) reconstruction2->GetOutputDataObject(0))->GetPointData()->GetArray(i)->GetName() << std::endl;  
    } 
    assert(numberOfPointFields2 != 0);
    return 0;
}
