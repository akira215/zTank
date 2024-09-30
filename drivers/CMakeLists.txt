set(SOURCES volumetricMeter.cpp)
idf_component_register(SRCS ${SOURCES}
                    INCLUDE_DIRS include 
                    REQUIRES cppgpio persistedValue)
                    