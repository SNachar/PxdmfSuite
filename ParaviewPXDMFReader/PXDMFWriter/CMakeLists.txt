
add_SERVER_MANAGER_SOURCES(

################# 
      vtkPXDMFWriter.h
      vtkPXDMFWriter.cxx
# to activate the new writer comment the previous lines and uncomment the next line
      vtkPXDMFWriter3.cxx
      vtkPXDMFWriter3.cxx
##################

     ) 
 
add_SERVER_MANAGER_XML( 
    PXDMFWriter.xml
    )