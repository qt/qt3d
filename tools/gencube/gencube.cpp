/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

// Used to generate the cube vertex arrays for qglcube.cpp.

#include <stdio.h>

static float const normals[6 * 3] = {
    -1.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    1.0f, 0.0f, 0.0f,
    0.0f, -1.0f, 0.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, -1.0f
};

static float const vertices[8 * 3] = {
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, 0.5f,
    -0.5f, 0.5f, 0.5f,
    -0.5f, 0.5f, -0.5f,
    0.5f, -0.5f, -0.5f,
    0.5f, -0.5f, 0.5f,
    0.5f, 0.5f, 0.5f,
    0.5f, 0.5f, -0.5f
};

static float const texCoords[4 * 2] = {
    1.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f,
    0.0f, 0.0f
};

static unsigned short const indices[6 * 4] = {
    0, 1, 2, 3, 3, 2, 6, 7, 7, 6, 5, 4,
    4, 5, 1, 0, 5, 6, 2, 1, 7, 4, 0, 3
};

int main(int argc, char *argv[])
{
    float vert[6][6][3];
    float norm[6][6][3];
    float tcoords[6][6][2];
    for (int face = 0; face < 6; ++face) {
        norm[face][0][0] = normals[face * 3 + 0];
        norm[face][0][1] = normals[face * 3 + 1];
        norm[face][0][2] = normals[face * 3 + 2];
        norm[face][1][0] = normals[face * 3 + 0];
        norm[face][1][1] = normals[face * 3 + 1];
        norm[face][1][2] = normals[face * 3 + 2];
        norm[face][2][0] = normals[face * 3 + 0];
        norm[face][2][1] = normals[face * 3 + 1];
        norm[face][2][2] = normals[face * 3 + 2];
        norm[face][3][0] = normals[face * 3 + 0];
        norm[face][3][1] = normals[face * 3 + 1];
        norm[face][3][2] = normals[face * 3 + 2];
        norm[face][4][0] = normals[face * 3 + 0];
        norm[face][4][1] = normals[face * 3 + 1];
        norm[face][4][2] = normals[face * 3 + 2];
        norm[face][5][0] = normals[face * 3 + 0];
        norm[face][5][1] = normals[face * 3 + 1];
        norm[face][5][2] = normals[face * 3 + 2];

        vert[face][0][0] = vertices[indices[face * 4] * 3 + 0];
        vert[face][0][1] = vertices[indices[face * 4] * 3 + 1];
        vert[face][0][2] = vertices[indices[face * 4] * 3 + 2];
        vert[face][1][0] = vertices[indices[face * 4 + 1] * 3 + 0];
        vert[face][1][1] = vertices[indices[face * 4 + 1] * 3 + 1];
        vert[face][1][2] = vertices[indices[face * 4 + 1] * 3 + 2];
        vert[face][2][0] = vertices[indices[face * 4 + 2] * 3 + 0];
        vert[face][2][1] = vertices[indices[face * 4 + 2] * 3 + 1];
        vert[face][2][2] = vertices[indices[face * 4 + 2] * 3 + 2];
        vert[face][3][0] = vertices[indices[face * 4] * 3 + 0];
        vert[face][3][1] = vertices[indices[face * 4] * 3 + 1];
        vert[face][3][2] = vertices[indices[face * 4] * 3 + 2];
        vert[face][4][0] = vertices[indices[face * 4 + 2] * 3 + 0];
        vert[face][4][1] = vertices[indices[face * 4 + 2] * 3 + 1];
        vert[face][4][2] = vertices[indices[face * 4 + 2] * 3 + 2];
        vert[face][5][0] = vertices[indices[face * 4 + 3] * 3 + 0];
        vert[face][5][1] = vertices[indices[face * 4 + 3] * 3 + 1];
        vert[face][5][2] = vertices[indices[face * 4 + 3] * 3 + 2];

        tcoords[face][0][0] = texCoords[0];
        tcoords[face][0][1] = texCoords[1];
        tcoords[face][1][0] = texCoords[2];
        tcoords[face][1][1] = texCoords[3];
        tcoords[face][2][0] = texCoords[4];
        tcoords[face][2][1] = texCoords[5];
        tcoords[face][3][0] = texCoords[0];
        tcoords[face][3][1] = texCoords[1];
        tcoords[face][4][0] = texCoords[4];
        tcoords[face][4][1] = texCoords[5];
        tcoords[face][5][0] = texCoords[6];
        tcoords[face][5][1] = texCoords[7];
    }
    printf("#define QGL_CUBE_SIZE (6 * 6 * (3 + 3 + 2))\n");
    printf("static float const cubeVertices[QGL_CUBE_SIZE] = {\n");
    for (int face = 0; face < 6; ++face) {
        for (int index = 0; index < 6; ++index) {
            printf("    %.1ff, %.1ff, %.1ff, ",
                   vert[face][index][0], vert[face][index][1],
                   vert[face][index][2]);
            printf("%.1ff, %.1ff, %.1ff, ",
                   norm[face][index][0], norm[face][index][1],
                   norm[face][index][2]);
            printf("%.1ff, %.1ff,\n",
                   tcoords[face][index][0], tcoords[face][index][1]);
        }
        printf("\n");
    }
    printf("};\n\n");
    return 0;
}
