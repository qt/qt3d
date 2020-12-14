if(TARGET WrapQt3DAssimp::WrapQt3DAssimp)
  set(WrapQt3DAssimp_FOUND TRUE)
  return()
endif()

set(WrapQt3DAssimp_FOUND FALSE)

find_package(assimp ${WrapQt3DAssimp_FIND_VERSION} CONFIG QUIET)
if (assimp_FOUND AND TARGET assimp::assimp)
  add_library(WrapQt3DAssimp::WrapQt3DAssimp INTERFACE IMPORTED)
  target_link_libraries(WrapQt3DAssimp::WrapQt3DAssimp INTERFACE assimp::assimp)

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

  set(WrapQt3DAssimp_FOUND TRUE)
elseif(assimp_FOUND AND assimp_LIBRARIES AND assimp_INCLUDE_DIRS)
  add_library(WrapQt3DAssimp::WrapQt3DAssimp INTERFACE IMPORTED)
  target_link_libraries(WrapQt3DAssimp::WrapQt3DAssimp INTERFACE ${assimp_LIBRARIES})
  target_include_directories(WrapQt3DAssimp::WrapQt3DAssimp INTERFACE ${assimp_INCLUDE_DIRS})
  set(WrapQt3DAssimp_FOUND TRUE)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(WrapQt3DAssimp DEFAULT_MSG WrapQt3DAssimp_FOUND)
