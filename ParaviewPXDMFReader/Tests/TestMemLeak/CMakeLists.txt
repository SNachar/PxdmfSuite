
ADD_EXECUTABLE(testmemleakbin TestMemLeak)

TARGET_LINK_LIBRARIES(testmemleakbin PXDMFReader )

ADD_TEST(PLUGINPXDMF-MemLeak ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/testmemleakbin ${TEST_DIRECTORY})
ADD_TEST(PLUGINPXDMF-MemLeakValgrind /usr/bin/valgrind --tool=memcheck  --leak-check=full ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/testmemleakbin ${TEST_DIRECTORY})

