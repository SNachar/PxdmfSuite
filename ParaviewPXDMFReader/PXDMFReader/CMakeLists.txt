
IF(PARAVIEW_BUILD_QT_GUI)

ENDIF(PARAVIEW_BUILD_QT_GUI)



add_SERVER_MANAGER_SOURCES( 
        vtkPXDMFDocumentBaseStructure.cxx
        vtkPXDMFDocumentBaseStructure.h
     ) 
  
add_SERVER_MANAGER_XML( 
	PXDMFDocumentBaseStructure.xml

     )

if(Compile_PXDMFReader)
  add_SERVER_MANAGER_SOURCES( 

        vtkPXDMFDocument.cxx  
        vtkPXDMFDocument.h 
        vtkPXDMFReader.cxx
        vtkPXDMFReader.h
  )

  add_SOURCES(        FileDataAbstract.cxx
        FileDataAbstract.h
# 
        XDMF2Data.cxx
        XDMF2Data.h
# to activate the new reader comment the previous lines and uncomment the next lines
        XDMF3Data.cxx
        XDMF3Data.h
        )

  add_SERVER_MANAGER_XML( 
        PXDMFReader.xml
  )
endif(Compile_PXDMFReader)