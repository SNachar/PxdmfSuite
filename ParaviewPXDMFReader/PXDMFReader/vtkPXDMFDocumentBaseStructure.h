/*=========================================================================

  Program:   PXDMFReader Plugin
  Module:    vtkPXDMFDocumentBaseStructure.h

  Copyright (c) GeM, Ecole Centrale Nantes.
  All rights reserved.
  Copyright: See COPYING file that comes with this distribution

  
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkPXDMFDocumentBaseStructure 
// .SECTION Description

#ifndef __vtkPXDMFDocumentBaseStructure_h
#define __vtkPXDMFDocumentBaseStructure_h

//std includes
#include <vector>
#include <string>
#include <map>

// Vtk Includes 
#define vtkstd std
#include "vtkSmartPointer.h"
#include "vtkAlgorithm.h"

class vtkFloatArray;
class vtkIntArray;
class vtkDataSet;
class vtkCell;
class vtkPoints;
class vtkCellArray;
class vtkIdTypeArray;
class vtkUnsignedCharArray;
class vtkIdList;
class vtkDoubleArray;
class vtkInformationDoubleKey;
class vtkInformationDoubleVectorKey;


class VTK_EXPORT vtkPXDMFDocumentBaseStructure : public vtkAlgorithm {
public:
  
  void SetNumberOfPXDMFDims(const int dims);
  int  GetNumberOfPXDMFDims() const ;
  
  vtkIntArray* GetNumberOfDimsInEachPXDMFDim();
  std::string GetNamesOfDimension(const int PGDdim, const int dim) const ;
  std::string GetUnitsOfDimension(const int PGDdim, const int dim);
  unsigned& GetNumberOfModesOfPointData(const vtkstd::string& _name) ;
  unsigned& GetNumberOfModesOfPointDataByNumber(unsigned& nb) ;
  unsigned& GetNumberOfModesOfCellData(const vtkstd::string& _name);
  unsigned& GetNumberOfModesOfCellDataByNumber(unsigned& nb) ;
  vtkstd::vector<char>& GetActivePXDMFDimsForSpace();
  vtkstd::vector<char>& GetActivePXDMFDimForTime();
  
  vtkstd::map<vtkstd::string, unsigned>&  GetNumberOfModesOfPointData();
  vtkstd::map<vtkstd::string, unsigned>& GetNumberOfModesOfCellData() ;
 
  /// description /////
  vtkSmartPointer<vtkIntArray> NumberOfDimsInEachGrid;                /// Number of Dimensions in each Grid
  vtkstd::vector<vtkDataSet* > AllData;                               /// Buffer to store the PGD dimensions 
  vtkstd::vector<vtkstd::vector<vtkstd::string> > NamesOfEachDimension; /// Names of the dimensions in each PGD Dimension
  vtkstd::vector<vtkstd::vector<vtkstd::string> > UnitsOfEachDimension; /// Units of the dimensions in each PGD Dimension

  vtkDataArray* getArrayPoint(const unsigned& name,const unsigned& mode,const unsigned& dim );
  vtkDataArray* getArrayCell(const unsigned& name,const unsigned& mode,const  unsigned& dim );
  
  /// Reconstruction ///
  
  vtkSetMacro(ComputeDerivatives, bool);
  vtkGetMacro(ComputeDerivatives, bool);
  vtkBooleanMacro(ComputeDerivatives, bool);
  
  vtkSetMacro(UseInterpolation, bool);
  vtkGetMacro(UseInterpolation, bool);
  vtkBooleanMacro(UseInterpolation, bool);
  
  vtkSetMacro(ExpandFields, bool);
  vtkGetMacro(ExpandFields, bool);
  vtkBooleanMacro(ExpandFields, bool);    
  
  vtkSetMacro(DoReconstruction, bool);
  vtkGetMacro(DoReconstruction, bool);
  vtkBooleanMacro(DoReconstruction, bool);
    

  
  vtkstd::vector<char> ActivePXDMFDimsForSpace;                         /// Dimensions to be used in the space representation
  vtkstd::vector<char> ActivePXDMFDimForTime;                           /// Dimension to be used in the time representation
  
  vtkstd::vector<char> LastActivePXDMFDimsForSpace;                     /// Last Time dimension used in the space representation
  int LastTimeDimension;                                                /// This variable store the time dimension used in the last reconstruction  

  vtkstd::vector<vtkstd::vector<double> >&  GetPosOfEachDimension();
    
  vtkstd::vector<double> TimeStepsVector;                               /// Vector used to store the time information. In the case a Unstructured meshes is used for the time, a sort must be done.
  double Time;                                                          /// This variable store the time asked by Paraview or vtk.
  
  void SetPosOfEachDimension(const int PGDdim, const int dim, const double& value) ;
  double  GetPosOfEachDimension(const int PXDMFdim, const int dim);
    
  std::vector<std::vector<double> > cellweights;                        /// to store the weight of the fixed and temporal dimension
  std::vector<std::vector<double> > cellpcoords;                        /// to store the parametric coordinate inside the element for fixed and temporal dimension
  std::vector<vtkCell*> cellGrids;                                        /// to store the cell of the fixed and temporal dimension
  bool forceMeshUpdate;
  
  vtkSmartPointer<vtkPoints>  Points;                                   /// Point generated for unstructured meshes
  vtkSmartPointer<vtkCellArray>  Cells;                                 /// Cells generated for unstructured meshes
  vtkSmartPointer<vtkIdTypeArray>  CellLocationsArray;                  /// Cells generated for unstructured meshes
  vtkSmartPointer<vtkUnsignedCharArray>  CellTypesArray ;               /// Cells generated for unstructured meshes
  
  void reconstruction(std::map <std::string, vtkSmartPointer <vtkDataArray > >::iterator &it,const  int& nb_modes, vtkIdList* ptIdsPGD,const vtkstd::vector<int> &spacedata, unsigned fieldnumber, const unsigned pointdata,  std::vector<vtkCell*>* cellPGD=NULL, std::vector<std::vector<double> >* cellweights=NULL);
  void reconstruction(vtkIdList* ptIdsPGD,const vtkstd::vector<int>& spacedata, const unsigned fieldnumber , unsigned& mode,  vtkDoubleArray* field, const unsigned pointdata, std::vector<vtkCell*>* cellPGD= NULL , std::vector<std::vector<double> >* cellweights= NULL);
  void reconstruction(vtkIdList* ptIdsPGD,const vtkstd::vector<int>& spacedata, const unsigned fieldnumber  , unsigned& mode,  vtkFloatArray* field, const unsigned pointdata,  std::vector<vtkCell*>* cellPGD= NULL, std::vector<std::vector<double> >* cellweights= NULL);
  
  void reconstructionOfDerivatives(vtkIdList* ptIdsPGD, std::vector< int >& spacedata, const unsigned fieldnumber, unsigned int& mode, std::vector< vtkSmartPointer< vtkDataArray > >& fields, const bool FloatArray, std::vector< vtkCell* >* cellPGD, std::vector< std::vector< double > >* cellweights);
  void reconstructionOfDerivativesFloat (vtkIdList* ptIdsPXDMF,vtkstd::vector<int> spacedata, const unsigned fieldnumber, unsigned& mode,  vtkstd::vector< vtkFloatArray * >&  fields, vtkstd::vector<vtkstd::vector<double> >& derivatives,  std::vector<vtkCell*>* cellPXDMF, std::vector<std::vector<double> >* cellweights);
  void reconstructionOfDerivativesDouble(vtkIdList* ptIdsPXDMF,vtkstd::vector<int> spacedata, const unsigned fieldnumber, unsigned& mode,  vtkstd::vector< vtkDoubleArray* >&  fields, vtkstd::vector<vtkstd::vector<double> >& derivatives,  std::vector<vtkCell*>* cellPXDMF, std::vector<std::vector<double> >* cellweights);
    
  vtkDataSet* GenerateSpace_UnstructuredGrid(vtkstd::vector<int>& spacedata, vtkSmartPointer<vtkIdList>& ptIdsPGD, vtkSmartPointer<vtkIdList>& cellIdsPGD);
  vtkDataSet* GenerateSpace_RectilinearGrid(vtkstd::vector<int>& spacedata, vtkSmartPointer<vtkIdList>& ptIdsPGD, vtkSmartPointer<vtkIdList>& cellIdsPGD  );
  vtkDataSet* GenerateSpace_StructuredGrid(vtkstd::vector<int>& spacedata, vtkSmartPointer<vtkIdList>& ptIdsPGD, vtkSmartPointer<vtkIdList>& cellIdsPGD  );
  vtkDataSet* GenerateSpace_UniformGrid(vtkstd::vector<int>& spacedata, vtkSmartPointer<vtkIdList>& ptIdsPGD, vtkSmartPointer<vtkIdList>& cellIdsPGD );
  vtkDataSet* GenerateSpace_PolydataGrid(vtkstd::vector<int>& spacedata, vtkSmartPointer<vtkIdList>& ptIdsPGD, vtkSmartPointer<vtkIdList>& cellIdsPGD);
  vtkDataObject * GenerateSpace();
  void construct_cell_fields( vtkIdList* cellIdsPGD,vtkstd::vector<int> &spacedata);
  void construct_point_fields( vtkIdList* ptIdsPGD, vtkstd::vector<int> spacedata,  std::vector<vtkCell*>* cellPGD, std::vector<std::vector<double> >* cellweights);

  static vtkInformationStringKey* UNIT();
  static vtkInformationStringKey* NAME();

  static vtkInformationDoubleVectorKey* BOUND_MIN_MAX_PXDMFDim_Dim();
  static vtkInformationDoubleVectorKey* UPDATE_FIXED_DIMENSIONS();

  vtkTypeMacro(vtkPXDMFDocumentBaseStructure, vtkAlgorithm);
  virtual int ReadOutputType();
protected:  

  bool UseInterpolation;                                                /// If interpolation is used for the reconstruction
  bool ComputeDerivatives; 
  bool ExpandFields;
  bool DoReconstruction;                                                      /// to deactivate the reconstruction

  /// If the sensitivity field must be computed
  bool haveTime;                                                        /// This variable is true if a time dimension is set
  vtkPXDMFDocumentBaseStructure();
private:
  unsigned NumberOfPXDMFDims;                                           /// Number of PGD Dimensions

  ///----------- ^^^^OK^^^^   vvvvv NOT OK vvvvv ----------------------------------------

public:
  
  std::vector<vtkDoubleArray*> ddataarrays;
  std::vector<vtkFloatArray*>  fdataarrays;

  vtkstd::map<vtkstd::string, vtkSmartPointer<vtkDataArray> > PointFieldData;     /// The Pointfield (reconstructed) by name
  vtkstd::map<vtkstd::string, unsigned> NumberOfModesInEachPointField;            /// Number of mode in each dimension Point fields  // TODO to clean
  std::vector<unsigned>                 NumberOfModesInEachPointFieldByNumber;    /// Number of mode in each dimension Point fields
  vtkstd::map<vtkstd::string, vtkSmartPointer<vtkDataArray> > PointFieldDerivativeData; /// The Pointfield Derivative(reconstructed) by name
  
  vtkstd::map<vtkstd::string, vtkSmartPointer<vtkDataArray> > CellFieldData;      /// The Cellfield (reconstructed) by name
  vtkstd::map<vtkstd::string, unsigned> NumberOfModesInEachCellField;             /// Number of mode in each dimension Cell fields // TODO to clean
  std::vector<unsigned>                 NumberOfModesInEachCellFieldByNumber;     /// Number of mode in each dimension Cell fields
  
  void UpdateInternalData();
 
  
//Type definition:
  typedef vtkstd::map<vtkstd::string,vtkSmartPointer<vtkDataArray> > TPointdata_name;
  typedef vtkstd::map<vtkstd::string,vtkSmartPointer<vtkDataArray> > TCelldata_name;


  int VtkOutputType;
  int update_extent[6];
  bool pointfound;
  bool cellfound;
  vtkSmartPointer<vtkIdList> pointIdGrids;
  vtkSmartPointer<vtkIdList> cellIdGrids;
  vtkstd::vector<vtkstd::vector<double> > MinOfEachDimension;           /// Min of the dimensions in each PXDMF Dimension (bounding box)
  vtkstd::vector<vtkstd::vector<double> > MaxOfEachDimension;           /// Max of the dimensions in each PXDMF Dimension (bounding box)
  vtkstd::vector<vtkstd::vector<double> > PosOfEachDimension;           /// Pos use to particularize the solution in each PXDMF Dimension



  ///    dim         field       mode
  std::vector< std::vector< std::vector< vtkDataArray* >  >  > PointMap;
  std::vector< std::vector< std::vector< vtkDataArray* >  >  > CellMap;
  
  std::string GetPointDataName(unsigned number);
  std::string GetCellDataName(unsigned number);
  //
  static bool QUITE ;
  
  int GetNumberOfVisualizationSpaceArrays();
  const char* GetVisualizationSpaceArrayName(int index);
  int GetVisualizationSpaceArrayStatus(const char* name);
  void SetVisualizationSpaceStatus(const char* name, int status);
  std::map<int,std::string> PXDMFDimsArrayNames;
      
      
};

void addtime(vtkstd::vector<double>& time_steps,const double& time, unsigned& cpt);

#endif
