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

#ifndef _vtkPXdmfWriter_h
#define _vtkPXdmfWriter_h

#include "vtkIOXdmf2Module.h" // For export macro
#include "vtkXdmfWriter.h"

class vtkExecutive;

class vtkCompositeDataSet;
class vtkDataArray;
class vtkDataSet;
class vtkDataObject;
class vtkFieldData;
class vtkInformation;
class vtkInformationVector;
class vtkXdmfWriterDomainMemoryHandler;
class XdmfArray;
class vtkPxdmfWriterDomainMemoryHandler;

namespace xdmf2{
  class XdmfDOM;
  class XdmfGrid;  
}


class VTK_EXPORT vtkPXDMFWriter : public vtkXdmfWriter{
public:
  static vtkPXDMFWriter *New();
  vtkTypeMacro(vtkPXDMFWriter,vtkXdmfWriter);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Set the input data set.
  //virtual void SetInputData(vtkDataObject* dobj);

  // Description:
  // Set or get the file name of the xdmf file.
  //vtkSetStringMacro(FileName);
  virtual void SetFileName (const char* _arg) ;
  //vtkGetStringMacro(FileName);

  // Description:
  // Set or get the file name of the hdf5 file.
  // Note that if the File name is not specified, then the group name is ignore
  //vtkSetStringMacro(HeavyDataFileName);
  //vtkGetStringMacro(HeavyDataFileName);

  // Description:
  // Set or get the group name into which data will be written
  // it may contain nested groups as in "/Proc0/Block0"
  //vtkSetStringMacro(HeavyDataGroupName);
  //vtkGetStringMacro(HeavyDataGroupName);

  // Description:
  // Write data to output. Method executes subclasses WriteData() method, as
  // well as StartMethod() and EndMethod() methods.
  // Returns 1 on success and 0 on failure.
 
  virtual int Write();

  // Description:
  // Topology Geometry and Attribute arrays smaller than this are written in line into the XML.
  // Default is 100.
  //vtkSetMacro(LightDataLimit, int);
  //vtkGetMacro(LightDataLimit, int);

  //Description:
  //Controls whether writer automatically writes all input time steps, or
  //just the timestep that is currently on the input.
  //Default is OFF.
  //vtkSetMacro(WriteAllTimeSteps, int);
  //vtkGetMacro(WriteAllTimeSteps, int);
  //vtkBooleanMacro(WriteAllTimeSteps, int);


  void SetASCII(bool ascii);
  
protected:
  vtkPXDMFWriter();
  ~vtkPXDMFWriter();

  //Choose composite executive by default for time.
  //virtual vtkExecutive* CreateDefaultExecutive();

  //Can take any one data object
  //virtual int FillInputPortInformation(int port, vtkInformation *info);

  //Overridden to ...
  //virtual int RequestInformation(vtkInformation*,
  //                               vtkInformationVector**,
  //                               vtkInformationVector*);
  //Overridden to ...
  //virtual int RequestUpdateExtent(vtkInformation*,
  //                                vtkInformationVector**,
  //                                vtkInformationVector*);
  //Overridden to ...
  //virtual int RequestData(vtkInformation*,
  //                        vtkInformationVector**,
  //                        vtkInformationVector*);

  //These do the work: recursively parse down input's structure all the way to arrays,
  //use XDMF lib to dump everything to file.

  //virtual void CreateTopology(vtkDataSet *ds, XdmfGrid *grid, vtkIdType PDims[3], vtkIdType CDims[3], vtkIdType &PRank, vtkIdType &CRank, void *staticdata);
  virtual int CreateTopology(vtkDataSet *ds, xdmf2::XdmfGrid *grid, vtkIdType PDims[3], vtkIdType CDims[3], vtkIdType &PRank, vtkIdType &CRank, void *staticdata);

  //virtual void CreateGeometry(vtkDataSet *ds, XdmfGrid *grid, void *staticdata);

  //virtual void WriteDataSet(vtkDataObject *dobj, XdmfGrid *grid);
  virtual int WriteCompositeDataSet(vtkCompositeDataSet *dobj, xdmf2::XdmfGrid *grid);
  virtual int WriteAtomicDataSet(vtkDataObject *dobj, xdmf2::XdmfGrid *grid);
  //virtual void WriteArrays(vtkFieldData* dsa, XdmfGrid *grid, int association,
  //                         vtkIdType rank, vtkIdType *dims, const char *name);
  //virtual void ConvertVToXArray(vtkDataArray *vda, XdmfArray *xda,
  //                              vtkIdType rank, vtkIdType *dims,
  //                              int AllocStrategy, const char *heavyprefix);

  //char *FileName;
  //char *HeavyDataFileName;
  //char *HeavyDataGroupName;

  //int LightDataLimit;

  //int WriteAllTimeSteps;
  //int NumberOfTimeSteps;
  //int CurrentTimeIndex;

  //int Piece;
  //int NumberOfPieces;

  //XdmfDOM *DOM;
  //XdmfGrid *TopTemporalGrid;

  //vtkPxdmfWriterDomainMemoryHandler *PDomainMemoryHandler;


private:
  vtkPXDMFWriter(const vtkPXDMFWriter&); // Not implemented
  void operator=(const vtkPXDMFWriter&); // Not implemented
  int internal_cpt;
};

#endif /* _vtkXdmfWriter_h */
