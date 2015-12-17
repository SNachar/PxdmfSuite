add_SERVER_MANAGER_XML(
  ArrayCalculatorGlobal.xml
)

add_SERVER_MANAGER_SOURCES(
     vtkArrayCalculatorGlobal.cxx
     vtkArrayCalculatorGlobal.h
     vtkPVArrayCalculatorGlobal.cxx
     vtkPVArrayCalculatorGlobal.h
)

#add_GUI_RESOURCE_FILES(
#     ArrayCalculatorGlobalGUI.xml
#)

add_to_COPY_ONLY_FILES(
     ArrayCalculatorGlobal.png
)