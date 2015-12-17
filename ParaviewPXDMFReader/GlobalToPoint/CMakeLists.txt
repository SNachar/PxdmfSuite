
add_SERVER_MANAGER_SOURCES( 
      vtkFieldDataToPoint.h
      vtkFieldDataToPoint.cxx
     ) 

#add_GUI_RESOURCE_FILES( 
#    GlobalToPointGUI.xml
#    )
  
add_SERVER_MANAGER_XML( 
    GlobalToPoint.xml
    )

add_to_COPY_ONLY_FILES(
    GlobalToPoint.png
    )

add_to_COPY_ONLY_FILES(GlobalToPoint.png)