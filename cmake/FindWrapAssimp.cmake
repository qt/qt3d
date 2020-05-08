if(TARGET WrapAssimp::WrapAssimp)
  set(WrapAssimp_FOUND TRUE)
  return()
endif()

set(WrapAssimp_FOUND FALSE)

find_package(assimp ${WrapAssimp_FIND_VERSION} CONFIG QUIET)
if (assimp_FOUND AND TARGET assimp::assimp)
  add_library(WrapAssimp::WrapAssimp INTERFACE IMPORTED)
  target_link_libraries(WrapAssimp::WrapAssimp INTERFACE assimp::assimp)

  # The Assimp configuration file is broken, and sets only the Release location, without advertising
  # that it's available via IMPORTED_CONFIGURATIONS. Thus when configuring with a different
  # configuration, you get a assimp::assimp-NOTFOUND at build time.
  # Make the release configuration be the default one.
  get_target_property(_assimp_library_location assimp::assimp IMPORTED_LOCATION)
  if(NOT _assimp_library_location)
      get_target_property(_assimp_library_location_release assimp::assimp IMPORTED_LOCATION_RELEASE)
      if(_assimp_library_location_release)
          set_target_properties(assimp::assimp PROPERTIES
                                IMPORTED_LOCATION "${_assimp_library_location_release}")
      endif()
  endif()

  set(WrapAssimp_FOUND TRUE)
elseif(assimp_FOUND AND assimp_LIBRARIES AND assimp_INCLUDE_DIRS)
  add_library(WrapAssimp::WrapAssimp INTERFACE IMPORTED)
  target_link_libraries(WrapAssimp::WrapAssimp INTERFACE ${assimp_LIBRARIES})
  target_include_directories(WrapAssimp::WrapAssimp INTERFACE ${assimp_INCLUDE_DIRS})
  set(WrapAssimp_FOUND TRUE)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(WrapAssimp DEFAULT_MSG WrapAssimp_FOUND)
