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

  # Work around Debian bug #973361
  set(_assimp_fixed_include_dirs)
  get_target_property(_assimp_include_dirs assimp::assimp INTERFACE_INCLUDE_DIRECTORIES)
  if(NOT _assimp_include_dirs MATCHES "-NOTFOUND$")
      foreach(dir IN LISTS _assimp_include_dirs)
          if(dir STREQUAL "/usr/lib/include"
                  AND NOT EXISTS "/usr/lib/include"
                  AND EXISTS "/usr/include")
              set(dir "/usr/include")
          endif()
          list(APPEND _assimp_fixed_include_dirs "${dir}")
      endforeach()
  endif()
  set_property(TARGET assimp::assimp
      PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${_assimp_fixed_include_dirs}")
  unset(_assimp_fixed_include_dirs)
  unset(_assimp_include_dirs)

  set(WrapQt3DAssimp_FOUND TRUE)
elseif(assimp_FOUND AND assimp_LIBRARIES AND assimp_INCLUDE_DIRS)
  add_library(WrapQt3DAssimp::WrapQt3DAssimp INTERFACE IMPORTED)
  target_link_libraries(WrapQt3DAssimp::WrapQt3DAssimp INTERFACE ${assimp_LIBRARIES})
  target_include_directories(WrapQt3DAssimp::WrapQt3DAssimp INTERFACE ${assimp_INCLUDE_DIRS})
  set(WrapQt3DAssimp_FOUND TRUE)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(WrapQt3DAssimp DEFAULT_MSG WrapQt3DAssimp_FOUND)
