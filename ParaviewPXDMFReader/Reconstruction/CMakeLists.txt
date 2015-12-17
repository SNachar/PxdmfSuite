
add_SERVER_MANAGER_SOURCES( 
     vtkReconstruction.h
     vtkReconstruction.cxx
     )

#add_GUI_RESOURCE_FILES( 
#     ReconstructionGUI.xml
#     )
  
add_SERVER_MANAGER_XML(  
     Reconstruction.xml
     )

add_to_COPY_ONLY_FILES(
    Reconstruction.png
    )

add_to_COPY_ONLY_FILES(
    Reconstruction.png
    )