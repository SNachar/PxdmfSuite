/*=========================================================================

  Program:   PXDMFReader Plugin
  Module:    vtkFieldDataToTextConvertor.cxx

  Copyright (c) GeM, Ecole Centrale Nantes.
  All rights reserved.
  Copyright: See COPYING file that comes with this distribution


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkFieldDataToTextConvertor
// .SECTION Description

#ifndef __vtkFieldDataToTextConvertor_h
#define __vtkFieldDataToTextConvertor_h

//vtk Includes
#include "vtkTableAlgorithm.h"

class VTK_EXPORT vtkFieldDataToTextConvertor : public vtkTableAlgorithm
{
public:
  static vtkFieldDataToTextConvertor* New();
  vtkTypeMacro(vtkFieldDataToTextConvertor, vtkTableAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Get/Set the format in which the to display the
  // input update time. Use printf formatting.
  // Default is "%f %s".
  // Default is "val unit".
  vtkSetStringMacro(Format);
  vtkGetStringMacro(Format);

// BTX
protected:
  vtkFieldDataToTextConvertor();
  ~vtkFieldDataToTextConvertor();

  virtual int FillInputPortInformation(int port, vtkInformation* info);
  virtual int RequestData(vtkInformation* request,
                          vtkInformationVector** inputVector,
                          vtkInformationVector* outputVector);

  virtual int RequestInformation(vtkInformation *request,
                                 vtkInformationVector **inputVector,
                                 vtkInformationVector *outputVector);

  char  *Format;

private:
  vtkFieldDataToTextConvertor(const vtkFieldDataToTextConvertor&); // Not implemented
  void operator=(const vtkFieldDataToTextConvertor&); // Not implemented
//ETX
};

#endif

