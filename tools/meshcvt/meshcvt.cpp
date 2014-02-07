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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <QtGui/qvector3d.h>
#include "qarray.h"

static void meshError(const char *filename)
{
    qWarning("%s: does not contain valid mesh data\n", filename);
    exit(1);
}

static int numPatches = 0;
static int numVertices = 0;
static int *patches = 0;

int main(int argc, char *argv[])
{
    int depth = 4;
    int teapotAdjust = 0;
    int reversePatches = 0;
    char buffer[BUFSIZ];
    char *filename;
    char *name;
    QArray<QVector3D> vertices;

    // Validate the command-line arguments.
    if (argc < 3) {
        qWarning("Usage: %s [--teapot-adjust] [--reverse-patches] mesh-filename name [depth]\n", argv[0]);
        return 1;
    }
    if (!strcmp(argv[1], "--teapot-adjust")) {
        // Adjust the vertices to correctly size and position the teapot.
        ++argv;
        --argc;
        teapotAdjust = 1;
    }
    if (!strcmp(argv[1], "--reverse-patches")) {
        // Reverse the coordinate order of patches (fixes teacup).
        ++argv;
        --argc;
        reversePatches = 1;
    }
    filename = argv[1];
    name = argv[2];
    if (argc > 3)
        depth = atoi(argv[3]);

    // Read the mesh data from the input file and determine
    // if it is a Bezier patch mesh or a triangle mesh.
    FILE *file;
    if ((file = fopen(filename, "r")) == NULL) {
        perror(filename);
        return 1;
    }
    if (fscanf(file, "%i\n", &numPatches) != 1)
        meshError(filename);
    patches = new int [numPatches * 17];
    for (int patch = 0; patch < numPatches; ++patch) {
        int *p = patches + patch * 17;
        if (!fgets(buffer, sizeof(buffer), file))
            meshError(filename);
        if (sscanf(buffer, "%i, %i, %i, %i, %i, %i, %i, %i, "
                           "%i, %i, %i, %i, %i, %i, %i, %i, %i\n",
                   p, p + 1, p + 2, p + 3,
                   p + 4, p + 5, p + 6, p + 7,
                   p + 8, p + 9, p + 10, p + 11,
                   p + 12, p + 13, p + 14, p + 15, p + 16) != 17) {
            if (sscanf(buffer, "%i, %i, %i, %i, %i, %i, %i, %i, "
                               "%i, %i, %i, %i, %i, %i, %i, %i\n",
                       p, p + 1, p + 2, p + 3,
                       p + 4, p + 5, p + 6, p + 7,
                       p + 8, p + 9, p + 10, p + 11,
                       p + 12, p + 13, p + 14, p + 15) != 16) {
                meshError(filename);
            } else {
                p[16] = -1;     // Use the default minimum depth value.
            }
        }
        if (reversePatches) {
            int reversed[16];
            for (int i = 0; i < 16; ++i)
                reversed[i] = p[(i & 0x0C) + (3 - (i % 4))];
            for (int i = 0; i < 16; ++i)
                p[i] = reversed[i];
        }
        // The standard patch indices are 1-based, not 0-based.
        // Correct for that problem here.
        for (int offset = 0; offset < 16; ++offset)
            --(p[offset]);
    }
    if (fscanf(file, "%i\n", &numVertices) != 1)
        meshError(filename);
    for (int vertex = 0; vertex < numVertices; ++vertex) {
        float x, y, z, xnormal, ynormal, znormal;
        if (!fgets(buffer, sizeof(buffer), file))
            meshError(filename);
        if (sscanf(buffer, "%f, %f, %f, %f, %f, %f\n",
                   &x, &y, &z, &xnormal, &ynormal, &znormal) != 6) {
            if (sscanf(buffer, "%f, %f, %f\n", &x, &y, &z) != 3)
                meshError(filename);
        }
        if (teapotAdjust) {
            // Do the equivalent of the following transformation:
            // matrix.rotate(270.0f, 1.0f, 0.0f, 0.0f);
            // matrix.scale(0.5f, 0.5f, 0.5f);
            // matrix.translate(0.0f, 0.0f, -1.5f);
            z -= 1.5f;
            x *= 0.5f;
            y *= 0.5f;
            z *= 0.5f;
            float y2 = z;
            float z2 = -y;
            y = y2;
            z = z2;
        }
        vertices.append(QVector3D(x, y, z));
    }
    fclose(file);

    // Generate the output data.
    printf("// Generated from %s by meshcvt, depth = %d\n\n", filename, depth);
    printf("#define %sBezierVertexCount %d\n", name, numVertices);
    printf("#define %sPatchCount %d\n", name, numPatches);
    printf("#define %sDepth %d\n", name, depth);
    printf("static float const %sBezierVertexData[] = {\n", name);
    for (int vertex = 0; vertex < vertices.count(); ++vertex) {
        QVector3D v = vertices[vertex];
        printf("    %ff, %ff, %ff", v.x(), v.y(), v.z());
        if (vertex < (vertices.count() - 1))
            printf(",\n");
        else
            printf("\n");
    }
    printf("};\n\n");
    printf("static ushort const %sPatchData[] = {\n", name);
    for (int patch = 0; patch < numPatches; ++patch) {
        int *p = patches + patch * 17;
        printf("    %d, %d, %d, %d, %d, %d, %d, %d, "
                   "%d, %d, %d, %d, %d, %d, %d, %d",
               p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7],
               p[8], p[9], p[10], p[11], p[12], p[13], p[14], p[15]);
        if (patch < (numPatches - 1))
            printf(",\n");
        else
            printf("\n");
    }
    printf("};\n");
    printf("\n");
    printf("class %sPatches : public QGLBezierPatches\n{\n", name);
    printf("public:\n");
    printf("    %sPatches()\n", name);
    printf("    {\n");
    printf("        QVector3DArray positions;\n");
    printf("        for (int pindex = 0; pindex < %sPatchCount * 16; ++pindex) {\n", name);
    printf("            int vindex = %sPatchData[pindex];\n", name);
    printf("            positions.append(%sBezierVertexData[vindex * 3],\n", name);
    printf("                             %sBezierVertexData[vindex * 3 + 1],\n", name);
    printf("                             %sBezierVertexData[vindex * 3 + 2]);\n", name);
    printf("        }\n");
    printf("        setPositions(positions);\n");
    printf("        setSubdivisionDepth(%sDepth);\n", name);
    printf("    }\n");
    printf("};\n");

    // Dump some statistics to stderr.
    qWarning("Depth:               %d\n", depth);
    qWarning("Number of vertices:  %d\n", numVertices);
    qWarning("Number of patches:   %d\n", numPatches);
    int storage = numVertices * sizeof(float) * 3;
    storage += numPatches * sizeof(ushort);
    qWarning("Bezier storage:      %d bytes\n", storage);

    return 0;
}
