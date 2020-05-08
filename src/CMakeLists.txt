# Generated from src.pro.
add_subdirectory(core)
add_subdirectory(render)
add_subdirectory(logic)
add_subdirectory(input)
add_subdirectory(animation)
add_subdirectory(extras)
add_subdirectory(doc)

if(TARGET Qt::Quick)
  # Quick3D libs
  add_subdirectory(quick3d/quick3d)
  add_subdirectory(quick3d/quick3drender)
  add_subdirectory(quick3d/quick3dinput)
  add_subdirectory(quick3d/quick3danimation)
  add_subdirectory(quick3d/quick3dextras)
  add_subdirectory(quick3d/quick3dscene2d)

#  # Quick3D imports
  add_subdirectory(quick3d/imports/core)
  add_subdirectory(quick3d/imports/render)
  add_subdirectory(quick3d/imports/scene3d)
  add_subdirectory(quick3d/imports/input)
  add_subdirectory(quick3d/imports/logic)
  add_subdirectory(quick3d/imports/animation)
  add_subdirectory(quick3d/imports/extras)
  add_subdirectory(quick3d/imports/scene2d)
endif()

## Plugins
add_subdirectory(plugins/sceneparsers)
add_subdirectory(plugins/geometryloaders)
add_subdirectory(plugins/renderers)

if(TARGET Qt::Quick)
  add_subdirectory(plugins/renderplugins)
endif()
