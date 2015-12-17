
add_SERVER_MANAGER_SOURCES( 
      vtkOptimi.h
      vtkOptimi.cxx
     ) 

#add_GUI_RESOURCE_FILES( 
#    OptimiGUI.xml
#    )
  
add_SERVER_MANAGER_XML( 
    Optimi.xml
    )

add_to_COPY_ONLY_FILES(
    Optimi.png
    )