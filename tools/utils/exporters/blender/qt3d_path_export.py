# Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
# SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

# Required Blender information.
bl_info = {
           "name": "Qt3D Path Exporter",
           "author": "Sean Harmer <sean.harmer@kdab.com>, Robert Brock <robert.brock@kdab.com>",
           "version": (0, 2),
           "blender": (2, 80, 0),
           "location": "File > Export > Qt3D Path Exporter (.json)",
           "description": "Export path to json to use with Qt3D",
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
from bpy_extras.io_utils import ExportHelper
from bpy.props import (
        BoolProperty,
        FloatProperty,
        StringProperty,
        EnumProperty,
        )
from collections import defaultdict

def jsonBuilder():
    pathList = []
    name = ""

    obj = bpy.context.object
    curve = obj.data
    if not hasattr(curve, 'splines'):
        return pathList

    spline = curve.splines.active

    for point in spline.points:
            # add the stuff
            pathList.append({"position": str("%.0f, %.0f, %.0f" % (point.co[0] * -1,
                                                                   point.co[2],
                                                                   point.co[1]))})

    return pathList

class Qt3DPathDataConverter:

    def pathInfoToJson(self):
        # Pass 1 - collect data we need to produce the output in pass 2
        print(">>> About to enter Objects")

        jsonData = json.dumps({ "splines": jsonBuilder()}, indent=2, sort_keys=True, separators=(',', ': '))
        return jsonData


class Qt3DPathExporter(bpy.types.Operator, ExportHelper):
    """Qt3D Exporter"""
    bl_idname       = "export_scene.qt3d_td_path_exporter";
    bl_label        = "Qt3DPathExporter";
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

        converter = Qt3DPathDataConverter()
        fileContent = converter.pathInfoToJson()
        with open(self.userpath + ".json", '+w') as f:
            f.write(fileContent)

        return {'FINISHED'}

def createBlenderMenu(self, context):
    self.layout.operator(Qt3DPathExporter.bl_idname, text="Qt3D Path Export(.json)")

# Register against Blender
def register():
    bpy.utils.register_class(Qt3DPathExporter)
    if bpy.app.version < (2, 80, 0):
        bpy.types.INFO_MT_file_export.append(createBlenderMenu)
    else:
        bpy.types.TOPBAR_MT_file_export.append(createBlenderMenu)

def unregister():
    bpy.utils.unregister_class(Qt3DPathExporter)
    if bpy.app.version < (2, 80, 0):
        bpy.types.INFO_MT_file_export.remove(createBlenderMenu)
    else:
        bpy.types.TOPBAR_MT_file_export.remove(createBlenderMenu)

# Handle running the script from Blender's text editor.
if (__name__ == "__main__"):
   register();
   bpy.ops.export_scene.qt3d_path_exporter();
