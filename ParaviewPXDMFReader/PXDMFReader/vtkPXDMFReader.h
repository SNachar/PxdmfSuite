/*=========================================================================

  Program:   PXDMFReader Plugin
  Module:    PXDMFReader.h

  Copyright (c) GeM, Ecole Centrale Nantes.
  All rights reserved.
  Copyright: See COPYING file that comes with this distribution


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkPXDMFReader 
// .SECTION Description

#ifndef __vtkPXDMFReader_h
#define __vtkPXDMFReader_h

#include "vtkPXDMFDocument.h"

//BTX
class vtkPXDMFDocument;
class XdmfGrid;
class vtkIdList;
class vtkXdmfDomain;
class vtkXdmfDocument;
class vtkCellArray;
class vtkUnsignedCharArray;
class vtkStringArray;

class XdmfArraySelection;
//ETX

//----------------------------------------------------------------------------
class VTK_EXPORT vtkPXDMFReader : public vtkPXDMFDocument {
private:
    vtkPXDMFReader(const vtkPXDMFReader& ); // Not implemented.
    void operator=( const vtkPXDMFReader&); // Not implemented
public:
    static vtkPXDMFReader* New();
    //
    vtkTypeMacro(vtkPXDMFReader, vtkPXDMFDocument);
    //
    void PrintSelf(ostream& os, vtkIndent indent);
    //
    // Description:
    // Determine if the file can be read with this reader.
    virtual int CanReadFile(const char* candidate_filename);
    
    //    
    vtkSetMacro(GUIMaxNbModes, unsigned);
    vtkGetMacro(GUIMaxNbModes, unsigned);
    
    unsigned GUIMaxNbModes;   // maximal number of mode showed int the GUI.
//----------------------------------------------------------------------------- 
    // Description:
    // Get information about point-based arrays. As is typical with readers this
    // in only valid after the filename is set and UpdateInformation() has been
    // called.
    int GetNumberOfPointArrays();
    const char* GetPointArrayName(int index);
    void SetPointArrayStatus(const char* name, int status);
    int GetPointArrayStatus(const char* name);
    
//BTX
    std::map<int,std::string> PointArrayNames;
//ETX
    
//-----------------------------------------------------------------------------    
    // Description:
    // Get information about cell-based arrays.  As is typical with readers this
    // in only valid after the filename is set and UpdateInformation() has been
    // called.
    int GetNumberOfCellArrays();
    const char* GetCellArrayName(int index);
    void SetCellArrayStatus(const char* name, int status);
    int GetCellArrayStatus(const char* name);
    
//BTX
    std::map<int,std::string> CellArrayNames;
//ETX
    
//-----------------------------------------------------------------------------   
    
    // Description:
    // Get/Set information about grids. As is typical with readers this is valid
    // only after the filename as been set and UpdateInformation() has been
    // called.
    //int GetNumberOfGrids();
    //const char* GetGridName(int index);
    //void SetGridStatus(const char* gridname, int status);
    //int GetGridStatus(const char* gridname);
    
//-----------------------------------------------------------------------------   
//BTX
    std::map<int,std::string> PXDMFDimsArrayNames;
//ETX
    
   
    // Description:
    // Get information about  PXDMF Spaces. As is typical with readers this
    // in only valid after the filename is set and PrepareDocument  has been
    // called.
    int GetNumberOfVisualizationTimeArrays();
    const char* GetVisualizationTimeArrayName(int index);  // Returns the name of point array at the give index. Returns NULL if index is invalid.

    // Description:
    // Get/Set the point array status.
    int GetVisualizationTimeArrayStatus(const char* name);
    void SetVisualizationTimeStatus(const char* name, int status);    
//-----------------------------------------------------------------------------
    /// Get information about PXDMF Spaces.
    vtkIntArray* GetPXDMFDimsDataInfo();
    vtkStringArray* GetPXDMFDimsNamesDataInfo();
    vtkStringArray* GetPXDMFDimsUnitsDataInfo();
    vtkDoubleArray* GetPXDMFDimsMinRangeDataInfo();
    vtkDoubleArray* GetPXDMFDimsMaxRangeDataInfo();
    vtkDoubleArray* GetPXDMFDimsPosDataInfo();
    void SetPXDMFDimsPosDataInfo(int id , double value);
    void SetPXDMFDimsPosDataInfoByName(const char* DimName, double value);
  
    vtkSetMacro(ContinuousUpdate, bool);
    vtkGetMacro(ContinuousUpdate, bool);
    vtkBooleanMacro(ContinuousUpdate, bool);
    bool ContinuousUpdate;
  
//-----------------------------------------------------------------------------
    virtual void SetRefresh (bool _arg);
protected:
    vtkPXDMFReader();
    ~vtkPXDMFReader();

//BTX

    // Until RequestInformation() is called, the active domain is not set
    // correctly. If SetGridStatus() etc. are called before that happens, then we
    // have no place to save the user choices. So we cache them in these temporary
    // caches. These are passed on to the actual vtkXdmfArraySelection instances
    // used by the active vtkXdmfDomain in RequestInformation().
    // Note that these are only used until the first domain is setup, once that
    // happens, the information set in these is passed to the domain and  these
    // are cleared an no longer used, until the active domain becomes invalid
    // again.
    XdmfArraySelection* PointArraysCache;
    XdmfArraySelection* CellArraysCache;
    //vtkXdmfArraySelection* GridsCache;
    //vtkXdmfArraySelection* SetsCache;
    
//ETX
private:
    
    bool PointDataShowAllModes;
    bool CellDataShowAllModes;
    // Description:
    // Prepares the XdmfDocument.
    virtual bool PrepareDocument();

    
    int TimeFileDataUpdate;
//BTX
    vtkSmartPointer<vtkStringArray> vtkPXDMFDimsNamesData;
    vtkSmartPointer<vtkStringArray> vtkPXDMFDimsUnitsData;
    vtkSmartPointer<vtkDoubleArray> vtkPXDMFDimsMinRangeData;
    vtkSmartPointer<vtkDoubleArray> vtkPXDMFDimsMaxRangeData;
    vtkSmartPointer<vtkDoubleArray> vtkPXDMFDimsPosData;

//ETX
};



#endif

