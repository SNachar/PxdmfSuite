/*=========================================================================

  Program:   PXDMFReader Plugin
  Module:    vtkrawWriter.h

  Copyright (c) GeM, Ecole Centrale Nantes.
  All rights reserved.
  Copyright: See COPYING file that comes with this distribution


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

// .NAME vtkrawWriter - 
// .SECTION Description

#ifndef _vtkrawWriter_h
#define _vtkrawWriter_h

// Std Includes 
#include <string>

// Vtk Includes 
#include "vtkWriter.h"

class VTK_EXPORT vtkrawWriter : public vtkWriter {
public:
  static vtkrawWriter *New();
  vtkTypeMacro(vtkrawWriter,vtkWriter);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Set or get the file name of the xdmf file.
  vtkSetStringMacro(FileName);

  virtual void WriteData();
  virtual int FillInputPortInformation(int port, vtkInformation *info);
  
  void SetWriteHeader(int);
  
protected:
  vtkrawWriter();
  ~vtkrawWriter();


  ofstream *OpenFile();
  void CloseFile(ofstream *fp);
private:
  vtkrawWriter(const vtkrawWriter&); // Not implemented
  void operator=(const vtkrawWriter&); // Not implemented
  bool writeHeader;
  char * FileName;
  std::string headerFilename;
};

#endif 

