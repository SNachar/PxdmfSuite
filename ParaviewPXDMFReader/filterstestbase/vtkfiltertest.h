/*=========================================================================

  Program:   PXDMFReader Plugin
  Module:    filtertest.h

  Copyright (c) GeM, Ecole Centrale Nantes.
  All rights reserved.
  Copyright: See COPYING file that comes with this distribution

  
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME filtertest - do mome thing to the input to make an output
// .SECTION Description
// filtertest  is a filter to ..., and
//
// .SECTION See Also
// vtk vtk vtk

#ifndef __filtertest_h
#define __filtertest_h

#include "vtkFiltersGeneralModule.h" // For export macro
#include "vtkPointSetAlgorithm.h"

/// public vtkPointSetAlgorithm
class VTK_EXPORT vtkfiltertest : public vtkPointSetAlgorithm{
public:
  static vtkfiltertest *New();
  vtkTypeMacro(vtkfiltertest,vtkPointSetAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Return the MTime also considering the transform.
  //unsigned long GetMTime();

  // Description:
  // Specify the transform object used to transform points.
  // virtual void SetTransform(vtkAbstractTransform*);
  // vtkGetObjectMacro(Transform,vtkAbstractTransform);

  // Description:
  // This flag tells the transform filter to apply the tranformation to the 
  // axis too
  // vtkSetMacro(MoveRotateScaleAxis, int);
  // vtkGetMacro(MoveRotateScaleAxis, int);
  // vtkBooleanMacro(MoveRotateScaleAxis, int);
  
  // vtkSetMacro(TitleNameX, std::string);
  // vtkSetMacro(TitleNameY, std::string);
  //  vtkSetMacro(TitleNameZ, std::string);
   
  // vtkSetMacro(LabelRange, int);
  // vtkGetMacro(LabelRange, int);
  // vtkBooleanMacro(LabelRange, int);
  
  virtual int FillInputPortInformation(int port, vtkInformation *info);

protected:
  vtkfiltertest(); 
  ~vtkfiltertest();

  int RequestDataObject(vtkInformation *request,
                        vtkInformationVector **inputVector,
                        vtkInformationVector *outputVector);
  int RequestData(vtkInformation *,
                  vtkInformationVector **,
                  vtkInformationVector *);
  
private:
  vtkfiltertest(const vtkfiltertest&);  // Not implemented.
  void operator=(const vtkfiltertest&);  // Not implemented.
};

#endif
