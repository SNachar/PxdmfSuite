/*=========================================================================

  Program:   PXDMFReader Plugin
  Module:    vtkmeshReader.h

  Copyright (c) GeM, Ecole Centrale Nantes.
  All rights reserved.
  Copyright: See COPYING file that comes with this distribution


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkmesheader 
// .SECTION Description

#ifndef __vtkmeshReader_h
#define __vtkmeshReader_h

//vtk Includes
#include "vtkUnstructuredGridAlgorithm.h"

class vtkIntArray;
class vtkFloatArray;

class VTK_EXPORT vtkmeshReader : public vtkUnstructuredGridAlgorithm {
public:
    static vtkmeshReader* New();
    vtkTypeMacro(vtkmeshReader, vtkUnstructuredGridAlgorithm);  
    void PrintSelf(ostream& os, vtkIndent indent);
 
    // Description:
    // Specify file name of AVS UCD datafile to read
    vtkSetStringMacro(FileName);
    vtkGetStringMacro(FileName);

    // Description:
    // Get the total number of cells.
    vtkGetMacro(NumberOfCells,int);
  
    // Description:
    // Get the total number of nodes.
    vtkGetMacro(NumberOfNodes,int);
    void ReadFile(vtkUnstructuredGrid *output); 
    
protected:
    vtkmeshReader();
    ~vtkmeshReader(); 
    int RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
    int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
    
   char *FileName;
   int NumberOfNodes;
   int NumberOfCells;
   
   ifstream *FileStream;
   int Dimension;
   int meshVersionFormatted;
private:

    void ReadGeometry(vtkUnstructuredGrid *output);
    void ReadXYZCoords(vtkFloatArray *coords, vtkIntArray *ntag);
    void ReadASCIICellTopology(vtkIntArray *material, vtkUnstructuredGrid *output, int ,int);
    
    vtkmeshReader(const vtkmeshReader& ); // Not implemented.
    void operator=( const vtkmeshReader&); // Not implemented
};
  
#endif

  