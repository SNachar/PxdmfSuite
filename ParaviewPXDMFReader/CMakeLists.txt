cmake_minimum_required(VERSION 2.8)
  
if (POLICY CMP0017)
  cmake_policy(SET CMP0017 NEW)
endif()
if (POLICY CMP0003)
  cmake_policy(SET CMP0003 NEW)
endif()

# Locate ParaView build and then import CMake configuration, 
# macros etc. from it.
# ParaView_DIR               the directory of the Paraview build
# to configure and 
# ccmake -D ParaView_DIR=/dir/to/the/paraview/build/dir /dir/to/the/paraviewPXDMFReader/dir 
#
# For ParaviewSuperBuild add to the cmake command line of ParaView  Super Build the line
# so this plug in is compiled along Paraview:
#   -D PARAVIEW_EXTERNAL_PLUGIN_DIRS:string=$(PXDMFReader_src_dir)

# Configuration and options

Set(PXDMF_GENERATE_LATEX_DOCUMENTATION False CACHE BOOL "To generate the Documentation in PDF")
Set(Compile_PxdmfReconstruction True CACHE BOOL "To Compile the reconstruction filter")
Set(Compile_Optimi True CACHE BOOL "To Compile the Optimi filter")
Set(Compile_gmshReader True CACHE BOOL "To Compile the gmshReader reader ")

Set(Compile_Pxdmfwriter True CACHE BOOL "To Compile the PxdmfWriter")
Set(Compile_PXDMFReader True CACHE BOOL "To Compile the PXDMFReader")
Set(PXDMFReader_NO_BLAS False CACHE BOOL "To Compile the use of blas, use of c++ for loop" )
Set(Compile_SVTKReader False CACHE BOOL "To Compile the SVTKReader")
Set(Compile_GmshInterface False CACHE BOOL "To Compile the GmshInterface")

Set(Compile_Experimental True CACHE BOOL "To compile the experimental filters")

#------------------  end of the configuration ---------------------------------------------

PROJECT(PXDMFReader)
set(PXDMFReader_VERSION_MAJOR 2 ) 
set(PXDMFReader_VERSION_MINOR 0 )
set(PXDMFReader_VERSION_PATCH 0 )

#------------------------------------------------------------------------------------------
#-------------------  CMake setup  --------------------------------------------------------

Set(PXMDReader_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
include(CMakeMacros.txt)

#------------------------------------------------------------------------------------------
#-------------------  Blas configuration  -------------------------------------------------
#------------------------------------------------------------------------------------------

if(PXDMFReader_NO_BLAS)
  SET(CMAKE_CXX_FLAGS "-DNO_BLAS")
  SET(PXDMFReader_COMPILE_FLAGS "NO_BLAS")
else(PXDMFReader_NO_BLAS)
endif(PXDMFReader_NO_BLAS)

#------------------------------------------------------------------------------------------
#-------------------  Find and Use ParaView  ----------------------------------------------
#------------------------------------------------------------------------------------------
#INCLUDE_DIRECTORIES(c:/paraview/build/install/include/boost-1_50/) 
#INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})

IF (ParaView_SOURCE_DIR)
  INCLUDE_DIRECTORIES(${VTK_INCLUDE_DIRS})

  set(ParaView_OK TRUE)
ELSE (ParaView_SOURCE_DIR)
  FIND_PACKAGE(ParaView  QUIET PATHS ${ParaView_DIR} )
  IF (ParaView_FOUND)
    #INCLUDE_DIRECTORIES(/Big/fbordeu/builds/ParaView4_bin/VTK/Common/Core/)
    #INCLUDE_DIRECTORIES(/Big/fbordeu/builds/ParaView4_binDebug/VTK/ThirdParty/xdmf3/vtkxdmf3/)
    INCLUDE(${PARAVIEW_USE_FILE})
    INCLUDE_DIRECTORIES(${VTK_INCLUDE_DIRS})
    set(ParaView_OK TRUE)

    #FIND_PACKAGE(Xdmf PATHS ${ParaView_DIR}/VTK/ThirdParty/xdmf3/vtkxdmf3/  )
    #INCLUDE_DIRECTORIES(${ParaView_DIR}/VTK/IO/Xdmf3/ )
    #INCLUDE_DIRECTORIES(${ParaView_CMAKE_DIR}/../VTK/IO/Xdmf3/ )
    #INCLUDE_DIRECTORIES(${XDMF_vtkxdmf3_INCLUDE_DIRS}) 


  ELSE(ParaView_FOUND)
    message("ParaView Not Found. Compiling only with vtk ? ")
    set(ParaView_OK FALSE)
  ENDIF (ParaView_FOUND)
ENDIF (ParaView_SOURCE_DIR)

#------------------------------------------------------------------------------------------
#-------------------  Find packages  ------------------------------------------------------
#------------------------------------------------------------------------------------------

IF(PARAVIEW_BUILD_QT_GUI)
  IF(QT4_FOUND)
     INCLUDE(${QT_USE_FILE})
  ENDIF(QT4_FOUND)
ENDIF(PARAVIEW_BUILD_QT_GUI)

mark_as_advanced(CLEAR Boost_INCLUDE_DIR)
find_package(Boost REQUIRED)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
  mark_as_advanced(FORCE Boost_INCLUDE_DIR)
endif()

#------------------------------------------------------------------------------------------
#-----------------  Activation of filters  ------------------------------------------------
#------------------------------------------------------------------------------------------

add_subdirectory_path(About)
add_subdirectory_path(AnnotateFieldData)
add_subdirectory_path(ArrayCalculatorGlobal)
add_subdirectory_path(GlobalToPoint)
add_subdirectory_path(PXDMFReader)
add_subdirectory_path(TransformWithAxis)
add_subdirectory_path(Extras)
add_subdirectory_path(Qtsources)
add_subdirectory_path(Documentation)
add_subdirectory_path(PXDMFSync)
add_subdirectory_path(Settings)
add_subdirectory_path(Behaviors)
add_subdirectory_path(Reconstruction)
add_subdirectory_path(ApplyOnBlocks)

#-------------this are the experimental filters--------------

IF(Compile_Experimental)

# Only xml filters
  #add_subdirectory_path(DICOMReader)
  add_subdirectory_path(ArcLength)     

  #add_subdirectory_path(MostUsedActions)     
  #ADD_PARAVIEW_ACTION_GROUP(IFACES IFACE_SRCS
  #                        CLASS_NAME MostUsedActions
  #                        GROUP_NAME "ToolBar/Most Used Toolbar")
  #add_SOURCES(${IFACE_SRCS})
  #add_GUI_INTERFACES(${IFACES})


  add_subdirectory_path(AddZeros)

  IF(Compile_Optimi)
    add_subdirectory_path(Optimi)
  ENDIF(Compile_Optimi)

  IF(Compile_Pxdmfwriter)
    add_subdirectory_path(PXDMFWriter)
  ENDIF(Compile_Pxdmfwriter)

  IF(Compile_gmshReader)
    add_subdirectory_path(GmshReader)
    add_subdirectory_path(GmshWriter)
    add_subdirectory_path(MeshReader)
    add_subdirectory_path(RawWriter)
  ENDIF(Compile_gmshReader)

  IF(Compile_GmshInterface)
    add_subdirectory_path(GmshInterface)
  ENDIF(Compile_GmshInterface)

  

ENDIF(Compile_Experimental)

#------------------------------------------------------------------------------------------
#-----------------  GUI resources  --------------------------------------------------------
#------------------------------------------------------------------------------------------
IF(PARAVIEW_BUILD_QT_GUI)

  #--------  icons, splash screen, logos… ----------
   IF (PARAVIEW_QT_VERSION VERSION_GREATER "4")
    QT5_ADD_RESOURCES(RCS_SRCS pqPXDMFReader.qrc)
  ELSE ()
    QT4_ADD_RESOURCES(RCS_SRCS pqPXDMFReader.qrc)
  ENDIF ()

  #--------  QT Wrap CPP ---------------------------
   IF (PARAVIEW_QT_VERSION VERSION_GREATER "4")
    QT5_WRAP_CPP(MOC_SRCS ${to_QT4_WRAP_CPP_files})
  ELSE ()
    QT4_WRAP_CPP(MOC_SRCS  ${to_QT4_WRAP_CPP_files})
  ENDIF ()
  

  #--------  QT Wrap UI ---------------------------
   IF (PARAVIEW_QT_VERSION VERSION_GREATER "4")
    QT5_WRAP_UI(UI_SRCS  ${to_QT4_WRAP_UI_files})
  ELSE ()
    QT4_WRAP_UI(UI_SRCS  ${to_QT4_WRAP_UI_files})
  ENDIF ()
  #----------------------------------------------------------------------------------
  #-------------- Splash Screen and Menus of the plugin ---------------------------------------
  #----------- (for now this does not work in the subdirectory) ---------------------
  
  ADD_PARAVIEW_AUTO_START(IFACES_Splash IFACE_SRCS_Splash
			   CLASS_NAME pqAboutGEMStarter 
			    STARTUP onStartup 
			    SHUTDOWN onShutdown)
				
  add_GUI_INTERFACES(${IFACES_Splash})
  add_SOURCES(${IFACE_SRCS_Splash} )

  ADD_PARAVIEW_ACTION_GROUP(IFACE_Help IFACE_SRCS_Help
                            CLASS_NAME PXDMFAboutHelpMenuActions
                            GROUP_NAME "MenuBar/Help")

  add_GUI_INTERFACES(${IFACE_Help})
  add_SOURCES(${IFACE_SRCS_Help})

  #-----------------------------------------------------------
  #-- html files generation and image copy
  #-----------------------------------------------------------

  COFIG_TO_DOC(${CONFIG_TO_DOC_FILES})
  COPY_TO_DOC(${COPY_ONLY_FILES})

  #------------------------------------------------------------------------------------------
  #-----------------  Custom Widgets definition  --------------------------------------------
  #------------------------------------------------------------------------------------------
  add_paraview_property_widget(IFACESX3 IFACE_SRCSX3 TYPE "CalculatorGlobal" CLASS_NAME "pqCalculatorGlobalWidget")
  add_GUI_INTERFACES(${IFACESX3})
  add_SOURCES(${IFACE_SRCSX3})


  add_paraview_property_widget(IFACESX2 IFACE_SRCSX2 TYPE "SpaceTimeSelector" CLASS_NAME "pqSpaceTimeSelector")
  add_GUI_INTERFACES(${IFACESX2})
  add_SOURCES(${IFACE_SRCSX2})
  
  add_paraview_property_widget(IFACESX0 IFACE_SRCSX0 TYPE "PointCellDataSelector" CLASS_NAME "pqPointCellDataSelector")
  add_GUI_INTERFACES(${IFACESX0})
  add_SOURCES(${IFACE_SRCSX0})


  add_paraview_property_widget(IFACESX0 IFACE_SRCSX0 TYPE "RefreshButton" CLASS_NAME "pqRefreshButton")
  add_GUI_INTERFACES(${IFACESX0})
  add_SOURCES(${IFACE_SRCSX0})


  add_paraview_property_widget_decorator(IFACESX1 IFACE_SRCSX1 TYPE "ComputeDerivatives_decorator" CLASS_NAME pqComputeDerivativesWidgetDecorator)
  add_GUI_INTERFACES(${IFACESX1})
  add_SOURCES(${IFACE_SRCSX1})

  ADD_PARAVIEW_DOCK_WINDOW(
    OUTIFACES
    OUTSRCS
    CLASS_NAME pqPXDMFSync
    DOCK_AREA Bottom )
  add_GUI_INTERFACES(${OUTIFACES})
  add_SOURCES(${OUTSRCS})

#-----------------------------------------------------------
#--------- tex file generation
#-----------------------------------------------------------
IF (PXDMF_GENERATE_LATEX_DOCUMENTATION )
  FIND_PACKAGE(LATEX)
  configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/Documentation/PXDMFReader.tex.in"
    "${CMAKE_CURRENT_BINARY_DIR}/doc/PXDMFReader${PXDMFReader_VERSION_MAJOR}.${PXDMFReader_VERSION_MINOR}.tex"
    @ONLY)
  
  add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/doc/PXDMFReader${PXDMFReader_VERSION_MAJOR}.${PXDMFReader_VERSION_MINOR}.pdf
                   COMMAND ${PDFLATEX_COMPILER} --shell-escape -interaction=batchmode -halt-on-error -file-line-error ${CMAKE_CURRENT_BINARY_DIR}/doc/PXDMFReader${PXDMFReader_VERSION_MAJOR}.${PXDMFReader_VERSION_MINOR}.tex
                   COMMAND ${PDFLATEX_COMPILER} --shell-escape -interaction=batchmode -halt-on-error -file-line-error ${CMAKE_CURRENT_BINARY_DIR}/doc/PXDMFReader${PXDMFReader_VERSION_MAJOR}.${PXDMFReader_VERSION_MINOR}.tex
                   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc/
                   DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/doc/PXDMFReader${PXDMFReader_VERSION_MAJOR}.${PXDMFReader_VERSION_MINOR}.tex
                   COMMENT "Building Documentation" )

  file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/pqDocResource.qrc "<RCC>\n<qresource prefix=\"/\">\n<file alias=\"PXDMFReader.pdf\">doc/PXDMFReader${PXDMFReader_VERSION_MAJOR}.${PXDMFReader_VERSION_MINOR}.pdf</file> \n</qresource>\n</RCC>\n")

  IF (PARAVIEW_QT_VERSION VERSION_GREATER "4")
    QT5_ADD_RESOURCES(RCS_SRCS2 ${CMAKE_CURRENT_BINARY_DIR}/pqDocResource.qrc)
  ELSE ()
    QT4_ADD_RESOURCES(RCS_SRCS2 ${CMAKE_CURRENT_BINARY_DIR}/pqDocResource.qrc)
  ENDIF ()
    


  add_SOURCES(${RCS_SRCS2})
ENDIF (PXDMF_GENERATE_LATEX_DOCUMENTATION )

ENDIF(PARAVIEW_BUILD_QT_GUI)
#-----------------------------------------------------------
#--------- Plugin definition -------------------------------
#-----------------------------------------------------------
IF (ParaView_OK)
 #compiling for paraview
 ADD_PARAVIEW_PLUGIN(
  PXDMFReader                                                                                   #<-- Name for the plugin 
  "${PXDMFReader_VERSION_MAJOR}.${PXDMFReader_VERSION_MINOR}.${PXDMFReader_VERSION_PATCH}"      #<-- Version number
  DOCUMENTATION_DIR "${CMAKE_CURRENT_BINARY_DIR}/doc"                                           #<-- Doc folder
  GUI_INTERFACES  ${GUI_INTERFACES_files}                                                       #<-- GUI files
  SOURCES 
      ${RCS_SRCS}                                                                               #<-- RESOURCES
      ${MOC_SRCS}                                                                               #<-- QT moc
      ${UI_SRCS}                                                                                #<-- QT GUI
      ${SOURCES_files}                                                                          #<-- CPP
  SERVER_MANAGER_XML 
      ${SERVER_MANAGER_XML_files} 
  SERVER_MANAGER_SOURCES 
      ${SERVER_MANAGER_SOURCES_files}
#  GUI_RESOURCE_FILES 
#      ${GUI_RESOURCE_FILES_files}
  PYTHON_MODULES 
      ReaderSync.py
  REQUIRED_ON_SERVER
  REQUIRED_ON_CLIENT
 )

 #Add dependencies to the doc if the doc is generated
 IF (PXDMF_GENERATE_LATEX_DOCUMENTATION )
    add_dependencies(PXDMFReader ${CMAKE_CURRENT_BINARY_DIR}/doc/PXDMFReader${PXDMFReader_VERSION_MAJOR}.${PXDMFReader_VERSION_MINOR}.pdf)
 ENDIF (PXDMF_GENERATE_LATEX_DOCUMENTATION )

 #-----------------------------------------------------------
 #--------- Python Wrapping ---------------------------------
 #-----------------------------------------------------------
 IF (VTK_WRAP_PYTHON )
  OPTION ( PXDMFREADER_WRAP_PYTHON "Wrap vtkPXDMFReader to use from Python" 
    ${VTK_WRAP_PYTHON} )

 ENDIF (VTK_WRAP_PYTHON )

 IF ( PXDMFREADER_WRAP_PYTHON )
 #  find_package(PythonLibs)
 #  include_directories(${PYTHON_INCLUDE_DIRS})
 #
 #  INCLUDE ( ${VTK_CMAKE_DIR}/vtkWrapPython.cmake )
 #  VTK_WRAP_PYTHON3 ( ${PROJECT_NAME}Python vtkPXDMFReaderPYTHON_SRCS "${HDRS}" )
 #  ADD_LIBRARY ( ${PROJECT_NAME}PythonD ${vtkPXDMFReaderPYTHON_SRCS} )
 #  ADD_LIBRARY ( ${PROJECT_NAME}Python MODULE ${PROJECT_NAME}PythonInit.cxx )
  
 #  TARGET_LINK_LIBRARIES ( ${PROJECT_NAME}PythonD ${PROJECT_NAME} 
 #   ${PXDMFREADER_LINK_LIBRARIES} )
 #  FOREACH ( c ${VTK_LINK_LIBRARIES} )
 #   TARGET_LINK_LIBRARIES ( ${PROJECT_NAME}PythonD ${c}PythonD )
 #  ENDFOREACH ( c )
 #  TARGET_LINK_LIBRARIES ( ${PROJECT_NAME}Python ${PROJECT_NAME}PythonD )
 ENDIF ( PXDMFREADER_WRAP_PYTHON )

 IF(PXDMFReader_NO_BLAS)
 ELSE(PXDMFReader_NO_BLAS)
  #-----------------------------------------------------------
  #--------- Blas Interface ----------------------------------
  #-----------------------------------------------------------
  if(NOT BLAS_blas_LIBRARY)
    IF(APPLE)
      #FIND_LIBRARY(CBLAS NAMES vecLib ) 
      FIND_LIBRARY(CBLAS NAMES Accelerate )
      set(USE_CBLAS 1)
      set(BLAS_FOUND 1)
    ELSE(APPLE)
      find_package(BLAS)
      message("BLAS_VENDOR : ${BLAS_VENDOR}")
    ENDIF(APPLE)

    if(NOT BLAS_FOUND) 
      set(BLAS_blas_LIBRARY  "${CMAKE_CURRENT_SOURCE_DIR}/../../blas/blas.lib" ) 
      message("setting BLAS_blas_LIBRARY  to  ${BLAS_blas_LIBRARY}" )
    endif(NOT BLAS_FOUND)
  endif(NOT BLAS_blas_LIBRARY)

  #-----------------------------------------------------------
  #--------- C Blas Interface --------------------------------
  #-----------------------------------------------------------
  # to activate cblas put -DUSE_CBLAS=ON in the cmake command line
  IF(USE_CBLAS)
    ADD_DEFINITIONS(-DUSE_CBLAS)
  ENDIF(USE_CBLAS)

  #-----------------------------------------------------------
  #--------- Linking libraries blas -------------------------------
  #-----------------------------------------------------------
  IF(APPLE)
    FIND_LIBRARY(CBLAS NAMES Accelerate )
    TARGET_LINK_LIBRARIES(PXDMFReader LINK_PRIVATE ${CBLAS})
  ELSE(APPLE)
    TARGET_LINK_LIBRARIES(PXDMFReader LINK_PRIVATE ${BLAS_blas_LIBRARY} )
  ENDIF(APPLE)
 ENDIF(PXDMFReader_NO_BLAS)

 #-----------------------------------------------------------
 #--------- Linking libraries -------------------------------
 #-----------------------------------------------------------

 TARGET_LINK_LIBRARIES(PXDMFReader LINK_PRIVATE ${vtkCommonExecutionModel_LIBRARIES} )

 if(VTKXDMF_LIBRARIES)
  TARGET_LINK_LIBRARIES(PXDMFReader LINK_PRIVATE ${VTKXDMF_LIBRARIES} )
 endif(VTKXDMF_LIBRARIES)
 TARGET_LINK_LIBRARIES(PXDMFReader LINK_PRIVATE vtkIOXdmf2 ${vtkhdf5_LIBRARIES})
 TARGET_LINK_LIBRARIES(PXDMFReader LINK_PRIVATE ${vtkIOXdmf2_LIBRARIES})
 TARGET_LINK_LIBRARIES(PXDMFReader LINK_PRIVATE ${vtkxdmf2_LIBRARIES})

 TARGET_LINK_LIBRARIES(PXDMFReader LINK_PRIVATE vtkIOXdmf3 ${vtkhdf5_LIBRARIES})
 TARGET_LINK_LIBRARIES(PXDMFReader LINK_PRIVATE ${vtkIOXdmf3_LIBRARIES})
 TARGET_LINK_LIBRARIES(PXDMFReader LINK_PRIVATE ${vtkxdmf3_LIBRARIES})


ELSE(ParaView_OK)
  #compiling only for vtk 
  set_source_files_properties(
    vtkXdmfDataArray
    vtkXdmfHeavyData
    vtkXdmfReaderInternal
    PROPERTIES WRAP_EXCLUDE false
    )
  vtk_module_library(PXDMFReader ${SOURCES_files}  ${SERVER_MANAGER_SOURCES_files})
ENDIF(ParaView_OK)


IF(Compile_gmshReader)
  #message('Linking to zlib')
  find_package(ZLIB)
  target_link_libraries(PXDMFReader LINK_PRIVATE ${ZLIB_LIBRARIES})
  target_link_libraries(PXDMFReader LINK_PRIVATE ${vtkzlib_LIBRARIES})
  target_link_libraries(PXDMFReader ${VTK_ZLIB_LIBRARIES})
ENDIF(Compile_gmshReader)

#TARGET_LINK_LIBRARIES(PXDMFReader LINK_PRIVATE ${Boost_IOSTREAMS_LIBRARY} )

#-----------------------------------------------------------
#--------- Tests -------------------------------------------
#-----------------------------------------------------------

OPTION ( PXDMFREADER_TEST "compile test for the plugin"  False)

IF ( PXDMFREADER_TEST )
    ENABLE_TESTING()
    set(TEST_DIRECTORY  ${CMAKE_CURRENT_SOURCE_DIR}/Tests/test_data/ )
    add_subdirectory(Tests )
ENDIF(PXDMFREADER_TEST )








