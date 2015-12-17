#----------------------------------------------------------------
# About configuration
#----------------------------------------------------------------
IF(PARAVIEW_BUILD_QT_GUI)

   configure_file(
    pqAboutGEMStarter.cxx.in 
    "${CMAKE_CURRENT_BINARY_DIR}/pqAboutGEMStarter.cxx"
    @ONLY)

   add_SOURCES("${CMAKE_CURRENT_BINARY_DIR}/pqAboutGEMStarter.cxx" 
            pqAboutGEM.cxx 
            PXDMFAboutHelpMenuActions.cxx
)

   #----------------------------------------------------
   #-------- Splash Screen of the plugin ---------------
   #----------------------------------------------------
   
   add_to_QT4_WRAP_CPP(pqAboutGEMStarter.h  
                    pqAboutGEM.h 
                    PXDMFAboutHelpMenuActions.h
                    )

   add_to_QT4_WRAP_UI( AboutPXDMF.ui )

ENDIF(PARAVIEW_BUILD_QT_GUI)