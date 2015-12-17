IF(PARAVIEW_BUILD_QT_GUI)

 add_to_QT4_WRAP_UI(   pqPointCellDataSelector.ui 
		       pqSpaceTimeSelector.ui
 )

 add_to_QT4_WRAP_CPP(  pqFixedDimensionControl.h 
		       QWidgetDetach.h 
		       QAllTreeWidget.h 
		       pqSpaceTimeSelector.h 
		       pqComputeDerivativesWidgetDecorator.h 
		       pqPointCellDataSelector.h
               pqRefreshButton.h
               pqCalculatorGlobalWidget.h
 )


 add_SOURCES(  QAllTreeWidget.cxx
              pqFixedDimensionControl.cxx
              pqSpaceTimeSelector.cxx
              pqComputeDerivativesWidgetDecorator.cxx
              pqPointCellDataSelector.cxx
              pqRefreshButton.cxx
              pqCalculatorGlobalWidget.cxx
 )

ENDIF(PARAVIEW_BUILD_QT_GUI)
