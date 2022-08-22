# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

function(qt3d_setup_opengl_renderer_target target)

    set(glplugindir ${PROJECT_SOURCE_DIR}/src/plugins/renderers/opengl)
    target_link_libraries(${target}
        PRIVATE
            OpenGLRendererLib
    )

endfunction()
