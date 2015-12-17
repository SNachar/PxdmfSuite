/*=========================================================================

  Program:   gmshReader Plugin
  Module:    vtkgmshInterface.cxx

  Copyright (c) GeM, Ecole Centrale Nantes.
  All rights reserved.
  Copyright: See COPYING file that comes with this distribution


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

//c++ Includes
#include <iostream>
#include <fstream>
#include <cstdio>
#include <ext/stdio_filebuf.h>

//vtk Includes
#include <vtksys/SystemTools.hxx>
#include "vtkObjectFactory.h"
#include "vtkUnstructuredGrid.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkCellData.h"
#include "vtkPointData.h"
#include "vtkErrorCode.h"
#include "vtkType.h"
#include "vtkCellArray.h"
#include "vtkDoubleArray.h"

// plugin Includes
#include "vtkgmshInterface.h"
#include <GmshReader/vtkgmshReader.h>
#include <vtkPXDMFGeneralSettings.h>

#if defined(_WIN32) || defined(_WIN64)
  #include "io.h"
  //#define mktemp _mktemp
  #define unlink _unlink
#else
  #include <unistd.h>  // for the unlink
#endif



vtkStandardNewMacro(vtkgmshInterface);
//
vtkgmshInterface:: vtkgmshInterface() {
  this->GmshPath = NULL;
  this->Script = NULL;
  OutputDimension = 2;
  this->SetNumberOfInputPorts(0);
  this->SetGmshPath(vtkPXDMFGeneralSettings::GetInstance()->GetGmshPath()); 
};
//
vtkgmshInterface::~vtkgmshInterface() {
  this->SetScript(NULL);
};
//
void vtkgmshInterface::PrintSelf(ostream& os, vtkIndent indent) {
    
  this->Superclass::PrintSelf(os, indent);

  os << indent << "Script: " << (this->Script ? this->Script : "(none)") << "\n";
  os << indent << "OutputDimension: " << OutputDimension<< "\n";
};
//
int vtkgmshInterface::RequestInformation(
    vtkInformation *vtkNotUsed(request),
    vtkInformationVector **vtkNotUsed(inputVector),
    vtkInformationVector *vtkNotUsed(outputVector)){

    //if ( !this->Script ){
    //    vtkErrorMacro("No Script specified");
    //    return 0;
    //}
    
    //std::cout  << "end of RequestInformation\n";

    return 1;
}
//
int vtkgmshInterface::RequestData(vtkInformation *vtkNotUsed(request), vtkInformationVector **vtkNotUsed(inputVector), vtkInformationVector *outputVector){
  
  if ( this->Script ){
    vtkInformation *outInfo = outputVector->GetInformationObject(0);
    // get the ouptut
    vtkUnstructuredGrid *output = vtkUnstructuredGrid::SafeDownCast(
                                      outInfo->Get(vtkDataObject::DATA_OBJECT()));

    // the old mktemp solution
    //std::ofstream outfile;
    //char temp[23] = "temp_gmsh_fileXXXXXX";
    //std::string TempMFile;
    //TempMFile += mktemp(temp);
    //outfile.open((TempMFile + ".geo" ).c_str(), std::ios::out | std::ios::trunc );
    
    // the new mkstemp solution
    char temp[25] = "temp_gmsh_fileXXXXXX.geo";
    std::string TempMFile;
    int fd = mkstemps(temp,4);
    __gnu_cxx::stdio_filebuf<char> filebuf(fd, std::ios_base::out | std::ios_base::trunc); // 1
    std::ostream outfile(&filebuf); // 2
    
    TempMFile += temp;
    
    outfile << this->Script << std::endl;
    //outfile.close(); old for the mktemp solution
    
    
    //std::cout << "TempMFile " << TempMFile << std::endl;
    
    char dim[1024];
    sprintf(dim,"%s -%d \'%s\'",GmshPath,OutputDimension, TempMFile.c_str());
    {
    int res = system(dim);
    }
    std::cout <<"***" << dim << "***" <<std::endl;
    
    vtkgmshReader* thereader = vtkgmshReader::New();                                    // delete ok
    std::string  geoFile = TempMFile;
    geoFile.replace(TempMFile.length()-4,4,".msh" );
    thereader->SetFileName(geoFile.c_str());
    std::cout  << "file is " << thereader->GetFileName() << std::endl;
    if(thereader->RequestInformation((NULL),(NULL), (NULL))){
      vtkDataObject* space_data = thereader->ReadMeshFile();
      output->ShallowCopy(space_data);
      space_data->Delete();
      thereader->Delete();
      vtksys::SystemTools::RemoveFile(TempMFile);
      vtksys::SystemTools::RemoveFile(geoFile);
            
      
      //std::cout  << "end of RequestData\n";
      return 1;
    } else {
      thereader->Delete();
      std::cout  << "error in the generation of the mesh\n";
      return 0;
    }
    
  }
  
  std::cout  << "end of RequestData failure\n";
  return 0;
};



