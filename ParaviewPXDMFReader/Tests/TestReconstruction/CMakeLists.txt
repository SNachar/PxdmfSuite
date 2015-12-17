
ADD_EXECUTABLE(testreconstructionbin TestReconstruction)

TARGET_LINK_LIBRARIES(testreconstructionbin PXDMFReader )

ADD_TEST(PLUGINPXDMF-Reconstruction ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/testreconstructionbin ${TEST_DIRECTORY})


