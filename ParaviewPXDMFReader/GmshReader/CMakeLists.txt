
add_SERVER_MANAGER_SOURCES( 
        vtkgmshReader.cxx
        vtkgmshReader.h
     ) 

add_SOURCES(
        gzstream.cxx
        gzstream.h  
)

add_SERVER_MANAGER_XML( 
       gmshReader.xml
    )