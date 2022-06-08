# Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
# SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

# Required Blender information.
bl_info = {
           "name": "Qt3D Armature Exporter",
           "author": "Sean Harmer <sean.harmer@kdab.com>, Robert Brock <robert.brock@kdab.com>",
           "version": (0, 2),
           "blender": (2, 80, 0),
           "location": "File > Export > Qt3D Armature Exporter (.json)",
           "description": "Export Armature to json to use with Qt3D",
           "warning": "",
           "wiki_url": "",
           "tracker_url": "",
           "category": "Import-Export"
          }

import bpy
import os
import struct
import mathutils
import math
import json
from array import array
from bpy import context
from bpy_extras.io_utils import ExportHelper
from bpy.props import (
        BoolProperty,
        FloatProperty,
        StringProperty,
        EnumProperty,
        )
from collections import defaultdict

def jsonBuilder():
    bonesList = []
    name = ""
    boneParent = ""

    ob = bpy.context.object.data

    if not hasattr(ob, 'bones'):
        return bonesList

    for bone in ob.bones:

        #check parent exists
        if bone.parent:
            boneParent = bone.parent.name
        else:
            boneParent = ""

        #add the bones
        bonesList.append({"bone": bone.name,
                          "parent": boneParent,
                          "matrix": jsonMatrix(bone)
                          })

    return bonesList

def jsonMatrix(bone):
    matrix = []

    for i in range(0, 4):
        matrix.append(str("%.4f, %.4f, %.4f, %.4f" % (bone.matrix_local[i][0],
                                                      bone.matrix_local[i][1],
                                                      bone.matrix_local[i][2],
                                                      bone.matrix_local[i][3])))

    return matrix


class Qt3DMeshDataConverter:

    def boneInfoToJson(self):
        # Pass 1 - collect data we need to produce the output in pass 2
        print(">>> About to enter Objects")

        jsonData = json.dumps({ "bones": jsonBuilder()}, indent=2, sort_keys=True, separators=(',', ': '))
        return jsonData


class Qt3DArmatureExporter(bpy.types.Operator, ExportHelper):
    """Qt3D Exporter"""
    bl_idname       = "export_scene.qt3d_armature_exporter";
    bl_label        = "Qt3DArmatureExporter";
    bl_options      = {'PRESET'};

    filename_ext = ""
    use_filter_folder = True

    def __init__(self):
        pass

    def execute(self, context):
        print("In Execute" + bpy.context.scene.name)

        self.userpath = self.properties.filepath

        # unselect all
        bpy.ops.object.select_all(action='DESELECT')

        converter = Qt3DMeshDataConverter()
        fileContent = converter.boneInfoToJson()
        with open(self.userpath + ".json", '+w') as f:
            f.write(fileContent)

        return {'FINISHED'}

def createBlenderMenu(self, context):
    self.layout.operator(Qt3DArmatureExporter.bl_idname, text="Qt3D Armature Export(.json)")

# Register against Blender
def register():
    bpy.utils.register_class(Qt3DArmatureExporter)
    if bpy.app.version < (2, 80, 0):
        bpy.types.INFO_MT_file_export.append(createBlenderMenu)
    else:
        bpy.types.TOPBAR_MT_file_export.append(createBlenderMenu)

def unregister():
    bpy.utils.unregister_class(Qt3DArmatureExporter)
    if bpy.app.version < (2, 80, 0):
        bpy.types.INFO_MT_file_export.remove(createBlenderMenu)
    else:
        bpy.types.TOPBAR_MT_file_export.remove(createBlenderMenu)

# Handle running the script from Blender's text editor.
if (__name__ == "__main__"):
   register();
   bpy.ops.export_scene.qt3d_Armature_exporter();
