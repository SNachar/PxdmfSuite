/*=========================================================================

  Program:   PXDMFReader Plugin
  Module:    vtkFieldDataToPoint.h

  Copyright (c) GeM, Ecole Centrale Nantes.
  All rights reserved.
  Copyright: See COPYING file that comes with this distribution


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkFieldDataToPoint
// .SECTION Description

#ifndef __vtkFieldDataToPoint_h
#define __vtkFieldDataToPoint_h

#include "vtkPointSetAlgorithm.h"

class VTK_EXPORT vtkFieldDataToPoint : public vtkPointSetAlgorithm //vtkTableAlgorithm

{
public:
  static vtkFieldDataToPoint* New();
  vtkTypeMacro(vtkFieldDataToPoint, vtkPointSetAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

    // Description:
  // Set the name of the variable to use as the X coordinate for the points.
  vtkSetStringMacro(XColumn);
  vtkGetStringMacro(XColumn);
  
    // Description:
  // Set the index of the variable to use as the X coordinate for the points.
  vtkSetClampMacro(XColumnIndex, int, 0, VTK_INT_MAX);
  vtkGetMacro(XColumnIndex, int);
  
    // Description:
  // Specify the component for the column specified using SetXColumn() to
  // use as the xcoordinate in case the column is a multi-component array.
  // Default is 0.
  vtkSetClampMacro(XComponent, int, 0, VTK_INT_MAX);
  vtkGetMacro(XComponent, int);
  
   // Description:
  // Set the name of the column to use as the Y coordinate for the points.
  // Default is 0.
  vtkSetStringMacro(YColumn);
  vtkGetStringMacro(YColumn);

  // Description:
  // Set the index of the column to use as the Y coordinate for the points.
  vtkSetClampMacro(YColumnIndex, int, 0, VTK_INT_MAX);
  vtkGetMacro(YColumnIndex, int);

  // Description:
  // Specify the component for the column specified using SetYColumn() to
  // use as the Ycoordinate in case the column is a multi-component array.
  vtkSetClampMacro(YComponent, int, 0, VTK_INT_MAX);
  vtkGetMacro(YComponent, int);

  // Description:
  // Set the name of the column to use as the Z coordinate for the points.
  // Default is 0.
  vtkSetStringMacro(ZColumn);
  vtkGetStringMacro(ZColumn);

  // Description:
  // Set the index of the column to use as the Z coordinate for the points.
  vtkSetClampMacro(ZColumnIndex, int, 0, VTK_INT_MAX);
  vtkGetMacro(ZColumnIndex, int);

  // Description:
  // Specify the component for the column specified using SetZColumn() to
  // use as the Zcoordinate in case the column is a multi-component array.
  vtkSetClampMacro(ZComponent, int, 0, VTK_INT_MAX);
  vtkGetMacro(ZComponent, int);
  
  
    // Description:
  // Specify whether the points of the polydata are 3D or 2D. If this is set to
  // true then the Z Column will be ignored and the z value of each point on the
  // polydata will be set to 0. By default this will be off.
  vtkSetMacro(Create2DPoints, bool);
  vtkGetMacro(Create2DPoints, bool);
  vtkBooleanMacro(Create2DPoints, bool);
  
  
  // Description:
  // Allow user to keep columns specified as X,Y,Z as Data arrays.
  // By default this will be off.
  vtkSetMacro(PreserveCoordinateAsDataArrays, bool);
  vtkGetMacro(PreserveCoordinateAsDataArrays, bool);
  vtkBooleanMacro(PreserveCoordinateAsDataArrays, bool);
  
  
 
    // Description:
  // Allow user to use the new point to probe the original field.
  // By default this will be on.
  vtkSetMacro(ExtractFields, bool);
  vtkGetMacro(ExtractFields, bool);
  vtkBooleanMacro(ExtractFields, bool);

// BTX
   virtual int FillInputPortInformation(int port, vtkInformation* info);
protected:
  vtkFieldDataToPoint();
  ~vtkFieldDataToPoint();


  virtual int RequestData(vtkInformation* request,
                          vtkInformationVector** inputVector,
                          vtkInformationVector* outputVector);
  virtual int RequestDataObject(vtkInformation *request,
                                     vtkInformationVector **inputVector,
                                     vtkInformationVector *outputVector);
  char* XColumn;
  char* YColumn;
  char* ZColumn;
  int XColumnIndex;
  int YColumnIndex;
  int ZColumnIndex;
  int XComponent;
  int YComponent;
  int ZComponent;
  bool Create2DPoints;
  bool PreserveCoordinateAsDataArrays;
  bool ExtractFields;
private:
  vtkFieldDataToPoint(const vtkFieldDataToPoint&); // Not implemented
  void operator=(const vtkFieldDataToPoint&); // Not implemented
//ETX
};

#endif

