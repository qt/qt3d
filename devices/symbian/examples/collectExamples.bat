
xcopy /R /Y ..\..\..\examples\qt3d\basket\basket_unsigned.sis basket.sis
xcopy /R /Y ..\..\..\examples\qt3d\builder\builder_unsigned.sis builder.sis
xcopy /R /Y ..\..\..\examples\qt3d\cube\cube_unsigned.sis cube.sis
xcopy /R /Y ..\..\..\examples\qt3d\cylinder\cylinder_unsigned.sis cylinder.sis
xcopy /R /Y ..\..\..\examples\qt3d\geometry\geometry_unsigned.sis geometry.sis
xcopy /R /Y ..\..\..\examples\qt3d\graphicsview\graphicsview_unsigned.sis graphicsview.sis
xcopy /R /Y ..\..\..\examples\qt3d\nesting\nesting_unsigned.sis nesting.sis
xcopy /R /Y ..\..\..\examples\qt3d\solarsystem\solarsystem_unsigned.sis solarsystem.sis
xcopy /R /Y ..\..\..\examples\qt3d\tank\tank_unsigned.sis tank.sis
xcopy /R /Y ..\..\..\examples\qt3d\teapot\teapot_unsigned.sis teapot.sis

xcopy /R /Y ..\..\..\examples\quick3d\basket\basket_qml_unsigned.sis basket_qml.sis
xcopy /R /Y ..\..\..\examples\quick3d\cube\cube_qml_unsigned.sis cube_qml.sis
xcopy /R /Y ..\..\..\examples\quick3d\forest\forest_qml_unsigned.sis forest_qml.sis
xcopy /R /Y ..\..\..\examples\quick3d\lander\lander_unsigned.sis lander.sis
xcopy /R /Y ..\..\..\examples\quick3d\matrix_animation\matrix_animation_unsigned.sis matrix_animation.sis
xcopy /R /Y ..\..\..\examples\quick3d\monkeygod\monkeygod_unsigned.sis monkeygod.sis
xcopy /R /Y ..\..\..\examples\quick3d\moon\moon_unsigned.sis moon.sis
xcopy /R /Y ..\..\..\examples\quick3d\photoroom\photoroom_unsigned.sis photoroom.sis
xcopy /R /Y ..\..\..\examples\quick3d\shaders\shaders_unsigned.sis shaders.sis
xcopy /R /Y ..\..\..\examples\quick3d\sphere\sphere_unsigned.sis sphere.sis

xcopy /R /Y ..\..\..\demos\qt3d\cubehouse\cubehouse\cubehouse_unsigned.sis cubehouse.sis
xcopy /R /Y ..\..\..\demos\qt3d\pageflip\pageflip_unsigned.sis pageflip.sis
xcopy /R /Y ..\..\..\demos\qt3d\photobrowser3d\photobrowser3d_unsigned.sis photobrowser3d.sis
xcopy /R /Y ..\..\..\demos\qt3d\shapes\shapes_unsigned.sis shapes.sis
xcopy /R /Y ..\..\..\demos\qt3d\teaservice\teaservice_unsigned.sis teaservice.sis

xcopy /R /Y ..\..\..\demos\quick3d\model_viewer\model_viewer_unsigned.sis model_viewer.sis
xcopy /R /Y ..\..\..\demos\quick3d\robo_bounce\robo_bounce_unsigned.sis robo_bounce.sis
xcopy /R /Y ..\..\..\demos\quick3d\tea_service\tea_service_unsigned.sis tea_service.sis

createpackage.bat -u examples_template.pkg release-armv5

