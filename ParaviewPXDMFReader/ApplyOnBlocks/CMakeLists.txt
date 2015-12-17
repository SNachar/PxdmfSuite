
add_SERVER_MANAGER_SOURCES( 
      vtkApplyOnBlocksFilter.h
      vtkApplyOnBlocksFilter.cxx
      vtkSIMetaFilterProxy.cpp
      vtkSIMetaFilterProxy.h
     ) 

# add_GUI_RESOURCE_FILES( 
#     ApplyOnBlocksGUI.xml
#     )
  
add_SERVER_MANAGER_XML( 
    ApplyOnBlocks.xml
    )