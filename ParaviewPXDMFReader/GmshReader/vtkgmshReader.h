/*=========================================================================

  Program:   PXDMFReader Plugin
  Module:    vtkgmshReader.h

  Copyright (c) GeM, Ecole Centrale Nantes.
  All rights reserved.
  Copyright: See COPYING file that comes with this distribution


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkgmshReader 
// .SECTION Description
//  Reader to load gmsh files version legacy and version 2 .msh
//  And also .pos files.


#ifndef __vtkgmshReader_h
#define __vtkgmshReader_h

//std includes
#include <map>

//vtk Includes
#include "vtkAlgorithm.h"
#include "vtkCellType.h"

class vtkIntArray;
class vtkFloatArray;
class vtkDoubleArray;
class vtkMultiBlockDataSet;
class vtkUnstructuredGrid;
class vtkMultiProcessController;

class VTK_EXPORT vtkgmshReader : public vtkAlgorithm {
public:
  static vtkgmshReader* New();
  vtkTypeMacro(vtkgmshReader, vtkAlgorithm);  
  void PrintSelf(ostream& os, vtkIndent indent);
 
  // Description:
  // Specify file name of .msh/.pos datafile to read
  vtkSetStringMacro(FileName);
  vtkGetStringMacro(FileName);

  // Description:
  // Get the total number of cells.
  vtkGetMacro(NumberOfCells,int);
  
  // Description:
  // Get the total number of nodes.
  vtkGetMacro(NumberOfNodes,int);
  vtkDataObject* GetData();
  int GetOutputType();
  int ProcessRequest(vtkInformation *request, vtkInformationVector **inputVector, vtkInformationVector *outputVector);
  int RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
  int RequestDataObject(vtkInformationVector *outputVector);
  int FillOutputPortInformation(int, vtkInformation *info);

  vtkUnstructuredGrid* ReadMeshFile();
  vtkUnstructuredGrid* ReadPosFile();
  vtkUnstructuredGrid* ReadPosGZFile();
  vtkMultiBlockDataSet* ReadMainPosFile();
protected:
  vtkgmshReader();
  ~vtkgmshReader(); 
  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
    
  char *FileName;
  int NumberOfNodes;
  int NumberOfCells;
   
  istream *FileStream;
  bool legacy;
  enum GmshFileType { POS ,POSGZ , MSH, MPOS};
  GmshFileType fileType;
  //bool posfile;
  
private:
 
  void ReadGeometry(vtkUnstructuredGrid *output);
  void ReadXYZCoords(vtkFloatArray *coords, vtkIntArray *nodesId);
  void ReadASCIICellTopology(vtkIntArray *material, vtkUnstructuredGrid *output, vtkIntArray *GeoTag,vtkIntArray* nodesId);
  vtkDoubleArray *ReadASCIIData();
  
  vtkgmshReader(const vtkgmshReader& ); // Not implemented.
  void operator=( const vtkgmshReader&); // Not implemented
  struct CellType{
    VTKCellType vtkType;
    int npoints;
    int ncomp;
    CellType(const VTKCellType &a,const  int &b,const  int&c):vtkType(a),npoints(b),ncomp(c){};
    CellType():vtkType(VTKCellType(0)){};
  };
  static std::map<std::string, CellType > posFileDic;
  static std::map<int, vtkgmshReader::CellType > mshFileDic;
  vtkMultiProcessController *Controller;
  bool IsParallel( );
};
  
#endif