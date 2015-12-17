


add_SERVER_MANAGER_SOURCES( 
      vtkTransformFilterWithAxis.cxx
      vtkTransformFilterWithAxis.h
     ) 

#add_GUI_RESOURCE_FILES( 
#      TransformFilterWithAxisGUI.xml
#     )
  
add_SERVER_MANAGER_XML( 
    TransformFilterWithAxis.xml
    )

add_to_COPY_ONLY_FILES(
    TransformFilterWithAxis.png
    )