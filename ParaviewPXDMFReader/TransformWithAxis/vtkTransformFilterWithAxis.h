/*=========================================================================

  Program:   PXDMFReader Plugin
  Module:    vtkTransformFilterWithAxis.h

  Copyright (c) GeM, Ecole Centrale Nantes.
  All rights reserved.
  Copyright: See COPYING file that comes with this distribution


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkTransformFilterWithAxis - transform points and associated normals and vectors
// .SECTION Description
// vtkTransformFilterWithAxis is a filter to transform point coordinates, and
// associated point normals and vectors. Other point data is passed
// through the filter.
//
// An alternative method of transformation is to use vtkActor's methods
// to scale, rotate, and translate objects. The difference between the
// two methods is that vtkActor's transformation simply effects where
// objects are rendered (via the graphics pipeline), whereas
// vtkTransformFilterWithAxis actually modifies point coordinates in the
// visualization pipeline. This is necessary for some objects
// (e.g., vtkProbeFilter) that require point coordinates as input.

// .SECTION See Also
// vtkAbstractTransform vtkTransformPolyDataFilter vtkActor

#ifndef __vtkTransformFilterWithAxis_h
#define __vtkTransformFilterWithAxis_h

#include "vtkFiltersGeneralModule.h" // For export macro
#include "vtkPointSetAlgorithm.h"


#include <iostream>

class vtkAbstractTransform;

class VTK_EXPORT vtkTransformFilterWithAxis : public vtkPointSetAlgorithm
{
public:
    static vtkTransformFilterWithAxis *New();
    vtkTypeMacro(vtkTransformFilterWithAxis,vtkPointSetAlgorithm);
    void PrintSelf(ostream& os, vtkIndent indent);

    // Description:
    // Return the MTime also considering the transform.
    unsigned long GetMTime();

    // Description:
    // Specify the transform object used to transform points.
    virtual void SetTransform(vtkAbstractTransform*);
    vtkGetObjectMacro(Transform,vtkAbstractTransform);

    // Description:
    // This flag tells the transform filter to apply the tranformation to the
    // axis too
    vtkSetMacro(MoveRotateScaleAxis, int);
    vtkGetMacro(MoveRotateScaleAxis, int);
    vtkBooleanMacro(MoveRotateScaleAxis, int);
    
    vtkSetStringMacro(TitleNameX);
    vtkSetStringMacro(TitleNameY);
    vtkSetStringMacro(TitleNameZ);
    
    vtkSetStringMacro(UnitNameX);
    vtkSetStringMacro(UnitNameY);
    vtkSetStringMacro(UnitNameZ);
    
    vtkSetMacro(LabelRange, int);
    vtkGetMacro(LabelRange, int);
    vtkBooleanMacro(LabelRange, int);

    virtual int FillInputPortInformation(int port, vtkInformation *info);

protected:
    vtkTransformFilterWithAxis();
    ~vtkTransformFilterWithAxis();

    int RequestDataObject(vtkInformation *request,
                          vtkInformationVector **inputVector,
                          vtkInformationVector *outputVector);
    int RequestData(vtkInformation *,
                    vtkInformationVector **,
                    vtkInformationVector *);
    vtkAbstractTransform *Transform;
    int MoveRotateScaleAxis;
    int LabelRange;

    char* TitleNameX;
    char* UnitNameX;
    char* TitleNameY;
    char* UnitNameY;
    char* TitleNameZ;
    char* UnitNameZ;

private:
    vtkTransformFilterWithAxis(const vtkTransformFilterWithAxis&);  // Not implemented.
    void operator=(const vtkTransformFilterWithAxis&);  // Not implemented.
};

#endif
