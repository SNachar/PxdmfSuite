


add_SERVER_MANAGER_SOURCES( 
      vtkgmshWriter.h
      vtkgmshWriter.cxx
     ) 

#add_GUI_RESOURCE_FILES( 
#    gmshWriterGUI.xml
#    )
  
add_SERVER_MANAGER_XML( 
    gmshWriter.xml
    )