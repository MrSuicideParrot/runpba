
#########################

add_library(attestation_ns INTERFACE)
add_library(attestation_ns_interface INTERFACE)

target_sources(attestation_ns
    INTERFACE
        attesation.c  

)


target_include_directories(attestation_ns_interface
    INTERFACE
        .
)

target_link_libraries(attestation_ns
    INTERFACE
        qcbor
        attestation_ns_interface
        tfm_t_cose_defs
)