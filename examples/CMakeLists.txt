cmake_minimum_required(VERSION 3.15)

# Test GaiaClient
add_executable(test_gaia test_gaia.cpp)
target_link_libraries(test_gaia PRIVATE starmap)

# Carta del Leone
add_executable(leo_chart leo_chart.cpp)
target_link_libraries(leo_chart PRIVATE starmap)
if(OpenMP_CXX_FOUND)
    target_link_libraries(leo_chart PRIVATE OpenMP::OpenMP_CXX)
else()
    target_link_libraries(leo_chart PRIVATE "/opt/homebrew/opt/libomp/lib/libomp.dylib")
endif()

# Generatore mappe configurabile JSON
add_executable(generate_chart generate_chart.cpp)
target_link_libraries(generate_chart PRIVATE starmap)
if(OpenMP_CXX_FOUND)
    target_link_libraries(generate_chart PRIVATE OpenMP::OpenMP_CXX)
else()
    target_link_libraries(generate_chart PRIVATE "/opt/homebrew/opt/libomp/lib/libomp.dylib")
endif()

# Esempio base
add_executable(example_basic basic_usage.cpp)
target_link_libraries(example_basic PRIVATE starmap)
if(OpenMP_CXX_FOUND)
    target_link_libraries(example_basic PRIVATE OpenMP::OpenMP_CXX)
else()
    target_link_libraries(example_basic PRIVATE "/opt/homebrew/opt/libomp/lib/libomp.dylib")
endif()

# Esempio configurazione JSON
add_executable(example_json json_config.cpp)
target_link_libraries(example_json PRIVATE starmap)
if(OpenMP_CXX_FOUND)
    target_link_libraries(example_json PRIVATE OpenMP::OpenMP_CXX)
else()
    target_link_libraries(example_json PRIVATE "/opt/homebrew/opt/libomp/lib/libomp.dylib")
endif()

# Esempio configurazione programmatica
add_executable(example_programmatic programmatic_config.cpp)
target_link_libraries(example_programmatic PRIVATE starmap)
if(OpenMP_CXX_FOUND)
    target_link_libraries(example_programmatic PRIVATE OpenMP::OpenMP_CXX)
else()
    target_link_libraries(example_programmatic PRIVATE "/opt/homebrew/opt/libomp/lib/libomp.dylib")
endif()

# Esempio carte occultazione
add_executable(occultation_chart occultation_chart.cpp)
target_link_libraries(occultation_chart PRIVATE starmap)
if(OpenMP_CXX_FOUND)
    target_link_libraries(occultation_chart PRIVATE OpenMP::OpenMP_CXX)
else()
    target_link_libraries(occultation_chart PRIVATE "/opt/homebrew/opt/libomp/lib/libomp.dylib")
endif()

# Test database SAO
add_executable(test_sao_database test_sao_database.cpp)
target_link_libraries(test_sao_database PRIVATE starmap)
if(OpenMP_CXX_FOUND)
    target_link_libraries(test_sao_database PRIVATE OpenMP::OpenMP_CXX)
else()
    target_link_libraries(test_sao_database PRIVATE "/opt/homebrew/opt/libomp/lib/libomp.dylib")
endif()

# Test carta di approccio completa
add_executable(approach_full_test approach_full_test.cpp)
target_link_libraries(approach_full_test PRIVATE starmap)
if(OpenMP_CXX_FOUND)
    target_link_libraries(approach_full_test PRIVATE OpenMP::OpenMP_CXX)
else()
    target_link_libraries(approach_full_test PRIVATE "/opt/homebrew/opt/libomp/lib/libomp.dylib")
endif()

# Installa esempi
install(TARGETS 
    example_basic 
    example_json 
    example_programmatic 
    leo_chart
    generate_chart
    occultation_chart
    test_sao_database
    approach_full_test
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}/examples
)

# Installa file di configurazione esempio
install(FILES 
    config_examples/orion.json
    config_examples/andromeda.json
    config_examples/custom.json
    DESTINATION ${CMAKE_INSTALL_DATADIR}/starmap/examples
)

# Installa esempi occultazione
install(FILES 
    occultation_examples/example_eros.json
    DESTINATION ${CMAKE_INSTALL_DATADIR}/starmap/examples/occultation
)
