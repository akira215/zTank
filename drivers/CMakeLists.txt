set(SOURCES waterPressureMeasCluster.cpp 
            waterFlowMeasCluster.cpp
            adsDriver.cpp)

idf_component_register(SRCS ${SOURCES}
                    INCLUDE_DIRS include 
                    REQUIRES cppgpio persistedValue zigbee periodicSoftTask
                            cppi2c ads1115)
                    