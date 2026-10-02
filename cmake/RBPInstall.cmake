include(CMakePackageConfigHelpers)

install(TARGETS RBP EXPORT RBPTargets)
install(DIRECTORY include/rbp ${RBP_GENERATED_DIR}/include/rbp DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
set(RBP_CMAKE_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/RBP)
install(EXPORT RBPTargets NAMESPACE RBP:: DESTINATION ${RBP_CMAKE_INSTALL_DIR})
configure_package_config_file(cmake/RBPConfig.cmake.in RBPConfig.cmake INSTALL_DESTINATION ${RBP_CMAKE_INSTALL_DIR})
write_basic_package_version_file(RBPConfigVersion.cmake COMPATIBILITY SameMinorVersion)
install(FILES ${PROJECT_BINARY_DIR}/RBPConfig.cmake ${PROJECT_BINARY_DIR}/RBPConfigVersion.cmake
        DESTINATION ${RBP_CMAKE_INSTALL_DIR}
)
