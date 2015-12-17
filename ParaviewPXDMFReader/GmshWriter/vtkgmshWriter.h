/*=========================================================================

  Program:   PXDMFReader Plugin
  Module:    vtkgmshWriter.h

  Copyright (c) GeM, Ecole Centrale Nantes.
  All rights reserved.
  Copyright: See COPYING file that comes with this distribution


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

// .NAME vtkgmshWriter - 
// .SECTION Description


#ifndef _vtkgmshWriter_h
#define _vtkgmshWriter_h

#include "vtkWriter.h"

class VTK_EXPORT vtkgmshWriter : public vtkWriter {
public:
  static vtkgmshWriter *New();
  vtkTypeMacro(vtkgmshWriter,vtkWriter);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Set or get the file name of the xdmf file.
  vtkSetStringMacro(FileName);

  virtual void WriteData();
  virtual int FillInputPortInformation(int port, vtkInformation *info);

protected:
  vtkgmshWriter();
  ~vtkgmshWriter();

  ofstream *OpenFile();
  void CloseFile(ofstream *fp);
private:
  vtkgmshWriter(const vtkgmshWriter&); // Not implemented
  void operator=(const vtkgmshWriter&); // Not implemented

  char *  FileName;

};

#endif /* _vtkXdmfWriter_h */
