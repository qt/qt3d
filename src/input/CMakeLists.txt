# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from input.pro.

#####################################################################
## Qt::3DInput Module:
#####################################################################

qt_internal_add_module(3DInput
    PLUGIN_TYPES 3dinputdevices
    SOURCES
        backend/abstractactioninput.cpp backend/abstractactioninput_p.h
        backend/abstractaxisinput.cpp backend/abstractaxisinput_p.h
        backend/action.cpp backend/action_p.h
        backend/actioninput.cpp backend/actioninput_p.h
        backend/analogaxisinput.cpp backend/analogaxisinput_p.h
        backend/assignkeyboardfocusjob.cpp backend/assignkeyboardfocusjob_p.h
        backend/axis.cpp backend/axis_p.h
        backend/axisaccumulator.cpp backend/axisaccumulator_p.h
        backend/axisaccumulatorjob.cpp backend/axisaccumulatorjob_p.h
        backend/axissetting.cpp backend/axissetting_p.h
        backend/backendnode.cpp backend/backendnode_p.h
        backend/buttonaxisinput.cpp backend/buttonaxisinput_p.h
        backend/genericdevicebackendnode.cpp backend/genericdevicebackendnode_p.h
        backend/handle_types_p.h
        backend/inputbackendnodefunctor_p.h
        backend/inputchord.cpp backend/inputchord_p.h
        backend/inputhandler.cpp backend/inputhandler_p.h
        backend/inputmanagers_p.h
        backend/inputsequence.cpp backend/inputsequence_p.h
        backend/inputsettings.cpp backend/inputsettings_p.h
        backend/job_common_p.h
        backend/keyboarddevice.cpp backend/keyboarddevice_p.h
        backend/keyboardhandler.cpp backend/keyboardhandler_p.h
        backend/keyboardmousegenericdeviceintegration.cpp backend/keyboardmousegenericdeviceintegration_p.h
        backend/loadproxydevicejob.cpp backend/loadproxydevicejob_p.h
        backend/logicaldevice.cpp backend/logicaldevice_p.h
        backend/mousedevice.cpp backend/mousedevice_p.h
        backend/mousehandler.cpp backend/mousehandler_p.h
        backend/movingaverage.cpp backend/movingaverage_p.h
        backend/physicaldeviceproxy.cpp backend/physicaldeviceproxy_p.h
        backend/qabstractphysicaldevicebackendnode.cpp backend/qabstractphysicaldevicebackendnode_p.h
        backend/qabstractphysicaldevicebackendnode_p_p.h
        backend/updateaxisactionjob.cpp backend/updateaxisactionjob_p.h
        backend/utils_p.h
        frontend/qabstractactioninput.cpp frontend/qabstractactioninput.h frontend/qabstractactioninput_p.h
        frontend/qabstractaxisinput.cpp frontend/qabstractaxisinput.h frontend/qabstractaxisinput_p.h
        frontend/qabstractphysicaldevice.cpp frontend/qabstractphysicaldevice.h frontend/qabstractphysicaldevice_p.h
        frontend/qabstractphysicaldeviceproxy.cpp frontend/qabstractphysicaldeviceproxy_p.h
        frontend/qabstractphysicaldeviceproxy_p_p.h
        frontend/qaction.cpp frontend/qaction.h frontend/qaction_p.h
        frontend/qactioninput.cpp frontend/qactioninput.h frontend/qactioninput_p.h
        frontend/qanalogaxisinput.cpp frontend/qanalogaxisinput.h frontend/qanalogaxisinput_p.h
        frontend/qaxis.cpp frontend/qaxis.h frontend/qaxis_p.h
        frontend/qaxisaccumulator.cpp frontend/qaxisaccumulator.h frontend/qaxisaccumulator_p.h
        frontend/qaxissetting.cpp frontend/qaxissetting.h frontend/qaxissetting_p.h
        frontend/qbuttonaxisinput.cpp frontend/qbuttonaxisinput.h frontend/qbuttonaxisinput_p.h
        frontend/qgenericinputdevice.cpp frontend/qgenericinputdevice_p.h
        frontend/qinputaspect.cpp frontend/qinputaspect.h frontend/qinputaspect_p.h
        frontend/qinputchord.cpp frontend/qinputchord.h frontend/qinputchord_p.h
        frontend/qinputdeviceintegration.cpp frontend/qinputdeviceintegration_p.h
        frontend/qinputdeviceintegration_p_p.h
        frontend/qinputdeviceintegrationfactory.cpp frontend/qinputdeviceintegrationfactory_p.h
        frontend/qinputdeviceplugin.cpp frontend/qinputdeviceplugin_p.h
        frontend/qinputsequence.cpp frontend/qinputsequence.h frontend/qinputsequence_p.h
        frontend/qinputsettings.cpp frontend/qinputsettings.h frontend/qinputsettings_p.h
        frontend/qkeyboarddevice.cpp frontend/qkeyboarddevice.h frontend/qkeyboarddevice_p.h
        frontend/qkeyboardhandler.cpp frontend/qkeyboardhandler.h frontend/qkeyboardhandler_p.h
        frontend/qkeyevent.cpp frontend/qkeyevent.h
        frontend/qlogicaldevice.cpp frontend/qlogicaldevice.h frontend/qlogicaldevice_p.h
        frontend/qmousedevice.cpp frontend/qmousedevice.h frontend/qmousedevice_p.h
        frontend/qmouseevent.cpp frontend/qmouseevent.h
        frontend/qmousehandler.cpp frontend/qmousehandler.h frontend/qmousehandler_p.h
        qt3dinput_global.h qt3dinput_global_p.h
    DEFINES
        QT3DINPUT_LIBRARY
    INCLUDE_DIRECTORIES
        backend
        frontend
    LIBRARIES
        Qt::CorePrivate
        Qt::3DCorePrivate
    PUBLIC_LIBRARIES
        Qt::3DCore
        Qt::Core
        Qt::Gui
    PRIVATE_MODULE_INTERFACE
        Qt::CorePrivate
        Qt::3DCorePrivate
)

#### Keys ignored in scope 1:.:.:input.pro:<TRUE>:
# MODULE = "3DInput"

## Scopes:
#####################################################################

qt_internal_extend_target(3DInput CONDITION TARGET Qt::Gamepad
    SOURCES
        frontend/qgamepadinput.cpp frontend/qgamepadinput_p.h
    DEFINES
        HAVE_QGAMEPAD
    PUBLIC_LIBRARIES
        Qt::Gamepad
)
