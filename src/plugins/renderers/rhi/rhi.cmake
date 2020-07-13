function(qt3d_setup_rhi_renderer_target target)

    set(rhiplugindir ${PROJECT_SOURCE_DIR}/src/plugins/renderers/rhi)
    target_link_libraries(${target}
        PRIVATE
            RhiRendererLib
    )

endfunction()
