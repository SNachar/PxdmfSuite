
add_SERVER_MANAGER_SOURCES( 
      vtkFieldDataToTextConvertor.cxx
      vtkFieldDataToTextConvertor.h
     ) 

#add_GUI_RESOURCE_FILES( 
#    AnnotateFieldDataGUI.xml
#    )
  
add_SERVER_MANAGER_XML( 
    AnnotateFieldData.xml
    )

add_to_COPY_ONLY_FILES(
    AnnotateFieldData.png
    )