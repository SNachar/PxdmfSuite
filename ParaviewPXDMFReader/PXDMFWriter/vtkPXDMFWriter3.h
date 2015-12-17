/*=========================================================================

  Program:   PXDMFReader Plugin
  Module:    vtkPXdmfWriter.h

  Copyright (c) GeM, Ecole Centrale Nantes.
  All rights reserved.
  Copyright: See COPYING file that comes with this distribution


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

// .NAME vtkPXdmfWriter - write eXtensible Data Model and Format files
// .SECTION Description
// vtkPXdmfWriter converts vtkDataObjects to PXDMF format. This is intended to
// replace vtkXdmfWriter, which is not up to date with the capabilities of the
// newer XDMF2 library. This writer understands VTK's composite data types and
// produces full trees in the output XDMF files.
//
// this file and the cxx one are almost a exact copy of vtkXdmf3Writer.h vtkXdmf3Writer.cxx

#ifndef _vtkPXdmfWriter3_h
#define _vtkPXdmfWriter3_h

// PXDMFReader Pluing Includes

//VTK Includes
#include "vtkSystemIncludes.h"
#include "vtkXdmf3Writer.h"

// STD includes



class VTK_EXPORT vtkPXDMFWriter3 : public vtkDataObjectAlgorithm {
public:
  static vtkPXDMFWriter3 *New();
  vtkTypeMacro(vtkPXDMFWriter3,vtkDataObjectAlgorithm);
  
  void SetASCII(bool ascii);
  
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Set the input data set.
  virtual void SetInputData(vtkDataObject* dobj);

  // Description:
  // Set or get the file name of the xdmf file.
  vtkSetStringMacro(FileName);
  vtkGetStringMacro(FileName);

  // Description:
  // Write data to output. Method executes subclasses WriteData() method, as
  // well as StartMethod() and EndMethod() methods.
  // Returns 1 on success and 0 on failure.
  virtual int Write();

  // Description:
  // Topology Geometry and Attribute arrays smaller than this are written in line into the XML.
  // Default is 100.
  vtkSetMacro(LightDataLimit, unsigned int);
  vtkGetMacro(LightDataLimit, unsigned int);

  //Description:
  //Controls whether writer automatically writes all input time steps, or
  //just the timestep that is currently on the input.
  //Default is OFF.
  vtkSetMacro(WriteAllTimeSteps, bool);
  vtkGetMacro(WriteAllTimeSteps, bool);
  vtkBooleanMacro(WriteAllTimeSteps, bool);

protected:
  vtkPXDMFWriter3();
  ~vtkPXDMFWriter3();

  //Overridden to set up automatic loop over time steps.
  virtual int RequestInformation(vtkInformation*,
                                 vtkInformationVector**,
                                 vtkInformationVector*);
  //Overridden to continue automatic loop over time steps.
  virtual int RequestUpdateExtent(vtkInformation*,
                                  vtkInformationVector**,
                                  vtkInformationVector*);
  //Write out the input data objects as XDMF and HDF output files.
  virtual int RequestData(vtkInformation*,
                          vtkInformationVector**,
                          vtkInformationVector*);

  char *FileName;
  unsigned int LightDataLimit;
  bool WriteAllTimeSteps;

private:
  vtkPXDMFWriter3(const vtkPXDMFWriter3&); // Not implemented
  void operator=(const vtkPXDMFWriter3&); // Not implemented

  class Internals;
  Internals *Internal;
  
  
  
};

#endif