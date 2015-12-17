

add_SERVER_MANAGER_SOURCES( 
      vtkAddZeros.cxx
      vtkAddZeros.h
     ) 

#add_GUI_RESOURCE_FILES( 
#    vtkAddZerosGUI.xml
#    )
  
add_SERVER_MANAGER_XML( 
    vtkAddZeros.xml
    )

add_to_COPY_ONLY_FILES(
   AddZeros.png
   )