set(SOURCES waterMeterCluster.cpp waterFlowMeasCluster.cpp)
idf_component_register(SRCS ${SOURCES}
                    INCLUDE_DIRS include 
                    REQUIRES cppgpio persistedValue zigbee periodicSoftTask)
                    