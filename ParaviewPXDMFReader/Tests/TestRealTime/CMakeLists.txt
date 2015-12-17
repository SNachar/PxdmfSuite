
ADD_EXECUTABLE(testrealtimebin TestRealTime)

TARGET_LINK_LIBRARIES(testrealtimebin PXDMFReader )

ADD_TEST(NAME PLUGINPXDMF-RealTimeBigmesh COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/testrealtimebin  ${TEST_DIRECTORY}/SolDepOficial.pxdmf)
ADD_TEST(NAME PLUGINPXDMF-RealTimeManyField COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/testrealtimebin  ${TEST_DIRECTORY}/FeedBack.pxdmf)
ADD_TEST(NAME PLUGINPXDMF-RealTimeManyFieldValgrind COMMAND valgrind  --dump-before=Update --tool=callgrind --callgrind-out-file=ManyFields.callgrind.out ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/testrealtimebin  ${TEST_DIRECTORY}/FeedBack.pxdmf)

