add_library(arcsParser-core STATIC
    ResourcesParserInterpreter.h
    ResourcesParserInterpreter.cpp
    ResourcesParser.h
    ResourcesParser.cpp
    ResourceTypes.h
    ResourceTypes.cpp
    configuration.h
    ByteOrder.h
)

target_link_libraries(arcsParser-core)
