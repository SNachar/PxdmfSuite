
add_subdirectory(PythonTests)

add_subdirectory(TestvtkPXDMFDocument)

add_subdirectory(TestMemLeak)

add_subdirectory(TestRealTime)

add_subdirectory(TestReconstruction)

add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} DEPENDS TestvtkPXDMFDocument_bin testmemleakbin testrealtimebin testreconstructionbin)







