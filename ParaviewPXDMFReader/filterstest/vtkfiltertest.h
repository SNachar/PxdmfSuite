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

#include <map>
#include <vector>
#include <utility>

#include "vtkFiltersGeneralModule.h" // For export macro
#include "vtkPointSetAlgorithm.h"
#include "vtkPassInputTypeAlgorithm.h"

class vtkAmoebaMinimizer;
class vtkMultiThreader;
class vtkThreadUserData;

static void  vtkFunctionToMinimize(void *arg);

/// public vtkPointSetAlgorithm
class VTK_EXPORT vtkfiltertest : public vtkPassInputTypeAlgorithm {
public:
    static vtkfiltertest *New();
    vtkTypeMacro(vtkfiltertest,vtkPassInputTypeAlgorithm);
    void PrintSelf(ostream& os, vtkIndent indent);

    
    vtkSetMacro(MaxNumberOfIterations, int);
    vtkGetMacro(MaxNumberOfIterations, int);
    
    vtkSetMacro(ContractionRatio, double);
    vtkGetMacro(ContractionRatio, double);
    
    vtkSetMacro(ExpansionRatio, double);
    vtkGetMacro(ExpansionRatio, double);
    
    
    vtkSetMacro(Tolerance, double);
    vtkGetMacro(Tolerance, double);
    
    
    vtkSetMacro(ParameterTolerance, double);
    vtkGetMacro(ParameterTolerance, double);
    
    vtkSetMacro(Maximize,bool);
    vtkGetMacro(Maximize,bool);
    vtkBooleanMacro(Maximize, bool);
   
    virtual int FillInputPortInformation(int port, vtkInformation *info);

    //vtkSetMacro(AttributeMode,int);
    //vtkGetMacro(AttributeMode,int);
    
    int GetNumberOfOptimizationSpaceArrays();
    const char* GetOptimizationSpaceArrayName(int index); 

        // Description:
    // Get/Set the point array status.
    int GetOptimizationSpaceArrayStatus(const char* name);
    void SetOptimizationSpaceStatus(const char* name, int status);
    void Restart(int);
protected:
    vtkfiltertest();
    ~vtkfiltertest();

    virtual int RequestDataObject(vtkInformation *request,
                                  vtkInformationVector **inputVector,
                                  vtkInformationVector *outputVector);

    virtual int RequestData(vtkInformation *,
                            vtkInformationVector **,
                            vtkInformationVector *);
    virtual int RequestUpdateExtent(vtkInformation *request,
                                    vtkInformationVector **inputVector,
                                    vtkInformationVector *outputVector);

    virtual int RequestInformation(vtkInformation* request, 
                                   vtkInformationVector **inputVector,
                                   vtkInformationVector *vtkNotUsed(outputVector) );
private:
    vtkfiltertest(const vtkfiltertest&);  // Not implemented.
    void operator=(const vtkfiltertest&);  // Not implemented.



    int CurrentTimeIndex;
    
    vtkAmoebaMinimizer* minimizer;
    int NumberOfOptimizationSpaceArrays;
    std::vector<char> ActivePXDMFDimsForOptimization; 
    std::map<int,std::string> PXDMFDimsArrayNames;
    vtkDataArray *inScalars ;
    
    friend void  vtkFunctionToMinimize( void *arg );
    friend void*  vtkRunMinimization(void *arg);
    vtkMultiThreader* threader ;
    int threadID;
    bool needANewIteration;
    unsigned int maincpt;
    unsigned int minicpt;
    vtkInformation *inInfo;
    std::map<std::string,std::pair<int,int> > pos;
    bool running;
// minimizer setup    
    int MaxNumberOfIterations;
    double ContractionRatio;
    double ExpansionRatio;
    double Tolerance;
    double ParameterTolerance;
    bool Maximize;
};

#endif
