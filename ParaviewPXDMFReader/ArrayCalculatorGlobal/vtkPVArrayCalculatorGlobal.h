/*=========================================================================

  Program:   PXDMFReader Plugin
  Module:    vtkPVArrayCalculatorGlobal.h

  Copyright (c) GeM, Ecole Centrale Nantes.
  All rights reserved.
  Copyright: See COPYING file that comes with this distribution


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkPVArrayCalculatorGlobal - perform mathematical operations on data 
//  in field data arrays
//
// .SECTION Description
//  vtkPVArrayCalculatorGlobal performs operations on vectors or scalars in field
//  data arrays.  It uses vtkFunctionParser to do the parsing and to
//  evaluate the function for each entry in the input arrays.  The arrays
//  used in a given function must be all in point data or all in cell data.
//  The resulting array will be stored as a field data array.  The result
//  array can either be stored in a new array or it can overwrite an 
//  existing array.
//
// .SECTION See Also
//  vtkArrayCalculatorGlobal vtkArrayCalculator vtkFunctionParser

#ifndef __vtkPVArrayCalculatorGlobal_h
#define __vtkPVArrayCalculatorGlobal_h

#include "vtkArrayCalculatorGlobal.h"

class vtkDataObject;
class vtkDataSetAttributes;
class vtkFieldData;

class VTK_EXPORT vtkPVArrayCalculatorGlobal : public vtkArrayCalculatorGlobal
{
public:
  vtkTypeMacro( vtkPVArrayCalculatorGlobal,vtkArrayCalculatorGlobal );
  void   PrintSelf( ostream & os, vtkIndent indent );

  static vtkPVArrayCalculatorGlobal * New();

protected:
  vtkPVArrayCalculatorGlobal();
  ~vtkPVArrayCalculatorGlobal();
 
  virtual int RequestData( vtkInformation *, vtkInformationVector **, 
                           vtkInformationVector *);
  
  // Description:
  // This function updates the (scalar and vector arrays / variables) names 
  // to make them consistent with those of the upstream calculator(s). This
  // addresses the scenarios where the user modifies the name of a calculator
  // whose output is the input of a (some) subsequent calculator(s) or the user
  // changes the input of a downstream calculator. Argument inDataAttrs refers
  // to the attributes of the input dataset. This function should be called by 
  // RequestData() only.
  void    UpdateArrayAndVariableNames( vtkDataObject        * theInputObj, 
                                       vtkDataSetAttributes * inDataAttrs,
                                       vtkFieldData * inDataAttrsGlobal );
private:
  vtkPVArrayCalculatorGlobal( const vtkPVArrayCalculatorGlobal & ); // Not implemented.
  void operator = ( const vtkPVArrayCalculatorGlobal & );     // Not implemented.
};

#endif
