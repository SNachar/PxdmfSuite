/*=========================================================================

  Program:   PXDMFReader Plugin
  Module:    vtkgmshInterface.h

  Copyright (c) GeM, Ecole Centrale Nantes.
  All rights reserved.
  Copyright: See COPYING file that comes with this distribution


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkgmshInterface 
// .SECTION Description

#ifndef __vtkgmshInterface_h
#define __vtkgmshInterface_h

//vtk Includes
#include "vtkUnstructuredGridAlgorithm.h"

class VTK_EXPORT vtkgmshInterface : public vtkUnstructuredGridAlgorithm {
public:
    static vtkgmshInterface* New();
    vtkTypeMacro(vtkgmshInterface, vtkUnstructuredGridAlgorithm);  
    void PrintSelf(ostream& os, vtkIndent indent);
 

    // Set the text of the python script to execute.
    vtkSetStringMacro(Script)
    vtkGetStringMacro(Script)
    
    vtkSetClampMacro(OutputDimension, int,1,3);
    
    vtkSetStringMacro(GmshPath)
    vtkGetStringMacro(GmshPath)

    
protected:
    vtkgmshInterface();
    ~vtkgmshInterface(); 
    int RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
    int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

private: 
    char * GmshPath;
    char *Script;
    int OutputDimension;
    vtkgmshInterface(const vtkgmshInterface& ); // Not implemented.
    void operator=( const vtkgmshInterface&); // Not implemented
};
  
#endif

  