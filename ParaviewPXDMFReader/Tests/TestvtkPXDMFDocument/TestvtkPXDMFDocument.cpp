/*=========================================================================

  Program:   PXDMFReader Plugin
  Module:    test1.cpp

  Copyright (c) GeM, Ecole Centrale Nantes.
  All rights reserved.
  Copyright: See COPYING file that comes with this distribution


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME pqSpaceTimeSelector 
// .SECTION Description

// std Includes
#include "vector"

//VTK Includes
#include "vtkDataSet.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkLight.h"
#include "vtkDataSetMapper.h"
    
// Pluing Includes
#include "vtkPXDMFDocument.h"

int main( int argc, char* argv[]) {

    /// Space dims
    std::vector<int> SpaceDims;
    
    ///Full screen
    bool FullScreen = 1;
   
    /// Skip interactive stage;
    bool InteractiveStage = 0;
    
    SpaceDims.push_back(0);
    SpaceDims.push_back(1);
    
    std::string filename;
    if(argc > 1 ){
      filename = argv[1] ;
      filename  += "test_ascii_1DCoRectMesh_1DCoRectMesh.pxdmf";
    } else {
      std::cerr <<  "I need the path of the data folder" << std::endl;
      return 1;
    }

    vtkPXDMFDocument *reader= vtkPXDMFDocument::New();

    std::cout << "Reading ...";
    std::cout.flush();
    reader->SetFileName(filename.c_str());
    reader->UpdateInformation();

    std::cout << "OK " << std::endl;

    /// put x into the Space Visualization
    for (unsigned i =0; i < SpaceDims.size(); ++i) {
        reader->GetActivePXDMFDimsForSpace()[SpaceDims[i]] = 1;
    }

    reader->Update();

    vtkRenderer *ren1 = vtkRenderer::New();

    vtkRenderWindow *renWin = vtkRenderWindow::New();
    renWin->AddRenderer(ren1);

    // map to graphics library
    vtkDataSetMapper *map = vtkDataSetMapper::New();
    map->ImmediateModeRenderingOff();
    map->SetInputConnection(reader->GetOutputPort());

    // actor coordinates geometry, properties, transformation
    vtkActor *aSW = vtkActor::New();
    aSW->SetMapper(map);

    // an interactor
    vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::New();

    iren->SetRenderWindow(renWin);
    //iren->Initialize();

    // add the actor to the scene
    ren1->AddActor(aSW);
    //ren1->SetBackground(1,1,1); // Background color white
    ren1->SetBackground(10,10,10); // Background color black


    /// render an image (lights and cameras are created automatically)

    if(FullScreen)
      renWin->FullScreenOn();
    ren1->ResetCamera();

    vtkLight* headlight= ren1->MakeLight();
    headlight->SetColor(125,125,125);
    headlight->SetIntensity(0.01);

    ren1->ResetCamera();
    renWin->Render();
    
    if(InteractiveStage)
      iren->Start();

    // release memory and return
    map->Delete();
    ren1->Delete();
    renWin->Delete();
    iren->Delete();
    return EXIT_SUCCESS;
}
