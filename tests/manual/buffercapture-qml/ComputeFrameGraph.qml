// Copyright (C) 2017 Juan José Casafranca.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import Qt3D.Core 2.0
import Qt3D.Render 2.9

Viewport {
    RenderSurfaceSelector {
        id: surfaceSelector

        // Compute Pass
        DispatchCompute {
            workGroupX: 50; workGroupY: 1; workGroupZ: 1
            BufferCapture {
                TechniqueFilter {
                    matchAll: [
                        FilterKey { name: "type"; value: "compute"}
                    ]
                }
            }
        }

        NoDraw { }
    }
}


