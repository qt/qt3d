# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

qt_commandline_option(qt3d-assimp TYPE enum VALUES qt system no)
qt_commandline_option(qt3d-render TYPE boolean)
qt_commandline_option(qt3d-input TYPE boolean)
qt_commandline_option(qt3d-logic TYPE boolean)
qt_commandline_option(qt3d-extras TYPE boolean)
qt_commandline_option(qt3d-animation TYPE boolean)
qt_commandline_option(qt3d-simd TYPE optionalString VALUES no sse2 avx2)
