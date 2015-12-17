
IF(PARAVIEW_BUILD_QT_GUI)

add_SOURCES( AxesLabels.cxx
             AxesLabels.h
             PostFilters.cxx
             PostFilters.h)

add_to_QT4_WRAP_CPP(AxesLabels.h 
                    PostFilters.h)

ENDIF(PARAVIEW_BUILD_QT_GUI)