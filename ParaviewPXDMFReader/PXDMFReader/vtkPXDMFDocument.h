/*=========================================================================

  Program:   PXDMFReader Plugin
  Module:    vtkPXDMFDocument.h

  Copyright (c) GeM, Ecole Centrale Nantes.
  All rights reserved.
  Copyright: See COPYING file that comes with this distribution

  
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkPXDMFDocument 
// .SECTION Description

#ifndef __vtkPXDMFDocument_h
#define __vtkPXDMFDocument_h

// STD Includes
#include <map>

// Paraview/VTK Includes.
#define vtkstd std

// plugin Includes 
#include "vtkPXDMFDocumentBaseStructure.h"


class vtkImageData;
class vtkUnstructuredGrid;
class vtkStructuredGrid;
class vtkIdList;
class vtkInformationStringKey;
class vtkDoubleArray;
class vtkCell;
class vtkFloatArray;
class vtkIntArray;
class vtkRectilinearGrid;

//BTX
class FileDataAbstract;
//ETX



class VTK_EXPORT vtkPXDMFDocument: public vtkPXDMFDocumentBaseStructure {
public:

    // Description: (inheritance from vtkDataReader)
    // Specify file name of vtk data file to read.
    // After setting the FileName, UpdateInformation() must be called
    // to read the file
     vtkSetStringMacro(FileName);

    // Description: (inheritance from vtkPXDMFDocumentBaseStructure)
    // Get the number of PXDMF dimensions in the current domain
    //int GetNumberOfPXDMFDims() const;
    //void  setNumberOfPXDMFDims(int dims) ;
    
    // Description: (inheritance from vtkPXDMFDocumentBaseStructure)
    // Get the array containing the size of each PXDMF dimension
    //vtkIntArray* GetNumberOfDimsInEachPXDMFDim();

//BTX
    // Description: (inheritance from vtkPXDMFDocumentBaseStructure)
    // Get the name of the dimension dim in the PXDMF dimension PXDMFdim;
    //vtkstd::string GetNamesOfDimension(int PXDMFdim,int dim);

    // Description: (inheritance from vtkPXDMFDocumentBaseStructure)
    // The unit for each fixed dimension
    //vtkstd::string GetUnitsOfDimension(int PXDMFdim,int dim);

    // Description: (inheritance from vtkPXDMFDocumentBaseStructure)
    // Get the names of the available fields in the file  (Point or Cell)
    //vtkstd::vector<std::string> GetPointDataNames();
    //vtkstd::vector<std::string> GetCellDataNames();

    // Description: (inheritance from vtkPXDMFDocumentBaseStructure)
    // Get the number of modes available for each field  (Point or Cell)
    // unsigned& GetNumberOfModesOfPointData(vtkstd::string& _name);
    // unsigned& GetNumberOfModesOfPointDataByNumber(unsigned& nb);
    // unsigned& GetNumberOfModesOfCellData(vtkstd::string& _name);
    // unsigned& GetNumberOfModesOfCellDataByNumber(unsigned& nb);
    
    // Description: (inheritance from vtkPXDMFDocumentBaseStructure)
    // Get/Set the active dimensions for Space and Time
    // vtkstd::vector<char>& GetActivePXDMFDimsForSpace();
    // vtkstd::vector<char>& GetActivePXDMFDimForTime();
//ETX

    // Description: (inheritance from vtkPXDMFDocumentBaseStructure)
    // Get/Set the fixed dimensions value
    // When a dimension is free (not a space dimension neither a time dimension)
    // it must be set to fixed value
    // void SetPosOfEachDimension(const int PXDMFdim, const int dim, const double& value);
    // double GetPosOfEachDimension(const int PXDMFdim, const int dim);

    // Description:
    // Generate the reconstructed solution.
    // if no space is selected a point is created (0,0,0) with the values of the fields
    vtkDataObject* GenerateOutput(vtkInformation * Info, vtkInformationVector ** inputVector,
                                  vtkInformationVector *outputVector);
    // Description:
    // Get the output data object for a port on this algorithm.
    vtkDataSet* GetOutput();
    vtkDataSet *GetOutput(int idx);

    // Description:
    // Get the output as various concrete types. This method is typically used
    // when you know exactly what type of data is being generated.  Otherwise,
    // use the general GetOutput() method. If the wrong type is used NULL is
    // returned.  (You must also set the filename of the object prior to
    // getting the output.)
    vtkStructuredGrid *GetStructuredGridOutput();
    vtkUnstructuredGrid *GetUnstructuredGridOutput();
    vtkRectilinearGrid *GetRectilinearGridOutput();

    // Description:
    // This method can be used to find out the type of output expected without
    // needing to read the whole file.
    virtual int ReadOutputType();

    // Description:
    // Every time the SIL is updated a this will return a different value.
    //vtkGetMacro(SILUpdateStamp, int);

    // Description:
    // Set the active domain. Only one domain can be selected at a time. By
    // default the first domain in the datafile is chosen. Setting this to null
    // results in the domain being automatically chosen. Note that if the domain
    // name is changed, you should explicitly call UpdateInformation() before
    // accessing information about grids, data arrays etc.
    vtkSetStringMacro(DomainName);
    vtkGetStringMacro(DomainName);

    // Description:
    // Get/Set the stride used to skip points when reading structured datasets.
    // This affects all grids being read.
    // This function is not finish
    //vtkSetVector3Macro(Stride, int);
    void SetStride (int _arg1, int _arg2, int _arg3);
    void SetStride (int _arg[3]);
    vtkGetVector3Macro(Stride, int);
    
    vtkSetMacro(UseStride, bool);
    vtkGetMacro(UseStride, bool);
    vtkBooleanMacro(UseStride, bool);

    // Description: (inheritance from vtkPXDMFDocumentBaseStructure)
    // Get/Set the automatically computation of the derivatives
    // vtkSetMacro(ComputeDerivatives, bool);
    // vtkGetMacro(ComputeDerivatives, bool);
    // vtkBooleanMacro(ComputeDerivatives, bool);


    // Description: (inheritance from vtkPXDMFDocumentBaseStructure)
    // Get/Set the use of the interpolation for point data
    // vtkSetMacro(UseInterpolation, bool);
    // vtkGetMacro(UseInterpolation, bool);
    // vtkBooleanMacro(UseInterpolation, bool);

    // Description:
    // To Activate/Deactivate the modes, if mode is -1 all the mode are deactivated.
    void SetPointStatus(const char* arrayname, const int mode, const int status);
    void SetCellStatus(const char* arrayname, const int mode, const int status);

    //Description:
    // helper function to know if the algorithm found the point/cell
    bool PointFound();
    bool CellFound();

//BTX
    virtual bool PrepareDocument();
//ETX
// -------------------------------------------------------------------------------------------------------------------------------------
protected:
//BTX
    char* DomainName;
private:
    int Stride[3];                                                              /// Stride used to reduce the size of the models (only for Structured meshes)
    bool UseStride;                                                             /// Value is true if Stride can be used
//ETX
public:
// ---------------------------------------------------------- VTK interface ----------------------------------------------------------
    static vtkPXDMFDocument* New();
    vtkTypeMacro(vtkPXDMFDocument, vtkPXDMFDocumentBaseStructure);
    void PrintSelf(ostream& os, vtkIndent indent);

    virtual int RequestInformation(vtkInformation *info, vtkInformationVector **inputVector, vtkInformationVector *outputVector);
    int RequestDataObject(vtkInformationVector *outputVector);
    virtual int RequestData(vtkInformation *, vtkInformationVector **,vtkInformationVector* );
    virtual int FillOutputPortInformation(int port, vtkInformation *info);
    int ProcessRequest(vtkInformation *request, vtkInformationVector **inputVector, vtkInformationVector *outputVector);
    //virtual int ModifyRequest(vtkInformation * request, int when)      ;
// ---------------------------------------------------------- VTK interface ----------------------------------------------------------
    ~vtkPXDMFDocument();
protected:
    vtkPXDMFDocument();
    vtkPXDMFDocumentBaseStructure* Internal;
    FileDataAbstract* XDMFInternal;
    char *FileName;
private:
    vtkPXDMFDocument(const vtkPXDMFDocument&); // Not implemented.
    void operator=(const vtkPXDMFDocument&); // Not implemented
};

#endif
