/*=========================================================================

  Program:   PXDMFReader Plugin
  Module:    vtkOptimi.h

  Copyright (c) GeM, Ecole Centrale Nantes.
  All rights reserved.
  Copyright: See COPYING file that comes with this distribution


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME Optimi - calculate the optimal value in funciton of the fixed dimensions
// .SECTION Description
// 
//
// .SECTION See Also
// vtk vtk vtk

#ifndef __vtkOptimi_h
#define __vtkOptimi_h
// Std Includes 
#include <map>
#include <vector>

// Vtk Includes 
#include "vtkPassInputTypeAlgorithm.h"
#include <vtkMutexLock.h>

class vtkAmoebaMinimizer;
class vtkMultiThreader;
class vtkThreadUserData;

static void  vtkFunctionToMinimize(void *arg);

/// public vtkPointSetAlgorithm
class VTK_EXPORT vtkOptimi : public vtkPassInputTypeAlgorithm {
public:
    static vtkOptimi *New();
    vtkTypeMacro(vtkOptimi,vtkPassInputTypeAlgorithm);
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
   
    vtkSetMacro(PrintValues,bool);
    vtkGetMacro(PrintValues,bool);
    vtkBooleanMacro(PrintValues, bool);

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
    vtkOptimi();
    ~vtkOptimi();

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
    vtkOptimi(const vtkOptimi&);  // Not implemented.
    void operator=(const vtkOptimi&);  // Not implemented.



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
    volatile bool running;
// minimizer setup    
    int MaxNumberOfIterations;
    double ContractionRatio;
    double ExpansionRatio;
    double Tolerance;
    double ParameterTolerance;
    bool Maximize;
    vtkSimpleMutexLock* myMutex1;
    vtkSimpleMutexLock* myMutex2;
    vtkSimpleMutexLock* myMutex3;
    vtkSimpleMutexLock* myMutex4;
    bool main_0;
    bool opti_0;
    void barrier(const bool& main);
    bool PrintValues;

};

#endif
