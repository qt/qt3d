# AssImp expects this to be defined on debug builds
CONFIG(debug, debug|release) : DEFINES+=_DEBUG
CONFIG += exceptions rtti

CONFIG -= precompile_header

win32:DEFINES+=_CRT_SECURE_NO_WARNINGS

qtConfig(system-zlib):!if(cross_compile:host_build): \
    QMAKE_USE_PRIVATE += zlib
else: \
    QT_PRIVATE += zlib-private

DEFINES += \
    ASSIMP_BUILD_BOOST_WORKAROUND \
    ASSIMP_BUILD_NO_3DS_IMPORTER \
    ASSIMP_BUILD_NO_3D_IMPORTER \
    ASSIMP_BUILD_NO_3MF_IMPORTER \
    ASSIMP_BUILD_NO_AC_IMPORTER \
    ASSIMP_BUILD_NO_AMF_IMPORTER \
    ASSIMP_BUILD_NO_ASE_IMPORTER \
    ASSIMP_BUILD_NO_ASSBIN_IMPORTER \
    ASSIMP_BUILD_NO_B3D_IMPORTER \
    ASSIMP_BUILD_NO_BLEND_IMPORTER \
    ASSIMP_BUILD_NO_BVH_IMPORTER \
    ASSIMP_BUILD_NO_C4D_IMPORTER \
    ASSIMP_BUILD_NO_COB_IMPORTER \
    ASSIMP_BUILD_NO_COMPRESSED_IFC \
    ASSIMP_BUILD_NO_CSM_IMPORTER \
    ASSIMP_BUILD_NO_DXF_IMPORTER \
    ASSIMP_BUILD_NO_EXPORT \
    ASSIMP_BUILD_NO_HMP_IMPORTER \
    ASSIMP_BUILD_NO_IFC_IMPORTER \
    ASSIMP_BUILD_NO_IRRMESH_IMPORTER \
    ASSIMP_BUILD_NO_IRR_IMPORTER \
    ASSIMP_BUILD_NO_LIMITBONEWEIGHTS_PROCESS \
    ASSIMP_BUILD_NO_LWO_IMPORTER \
    ASSIMP_BUILD_NO_LWS_IMPORTER \
    ASSIMP_BUILD_NO_M3D_IMPORTER \
    ASSIMP_BUILD_NO_MD2_IMPORTER \
    ASSIMP_BUILD_NO_MD3_IMPORTER \
    ASSIMP_BUILD_NO_MD5_IMPORTER \
    ASSIMP_BUILD_NO_MDC_IMPORTER \
    ASSIMP_BUILD_NO_MDL_IMPORTER \
    ASSIMP_BUILD_NO_MMD_IMPORTER \
    ASSIMP_BUILD_NO_MS3D_IMPORTER \
    ASSIMP_BUILD_NO_NDO_IMPORTER \
    ASSIMP_BUILD_NO_NFF_IMPORTER \
    ASSIMP_BUILD_NO_OFF_IMPORTER \
    ASSIMP_BUILD_NO_OGRE_IMPORTER \
    ASSIMP_BUILD_NO_OPENGEX_IMPORTER \
    ASSIMP_BUILD_NO_OWN_ZLIB \
    ASSIMP_BUILD_NO_PLY_IMPORTER \
    ASSIMP_BUILD_NO_Q3BSP_IMPORTER \
    ASSIMP_BUILD_NO_Q3D_IMPORTER \
    ASSIMP_BUILD_NO_RAW_IMPORTER \
    ASSIMP_BUILD_NO_SIB_IMPORTER \
    ASSIMP_BUILD_NO_SMD_IMPORTER \
    ASSIMP_BUILD_NO_STEP_IMPORTER \
    ASSIMP_BUILD_NO_TERRAGEN_IMPORTER \
    ASSIMP_BUILD_NO_X3D_IMPORTER \
    ASSIMP_BUILD_NO_XGL_IMPORTER \
    ASSIMP_BUILD_NO_X_IMPORTER \
    ASSIMP_BUILD_NO_BLEND_IMPORTER \
    NOUNCRYPT \
    RAPIDJSON_HAS_STDSTRING=1 \
    RAPIDJSON_NOMEMBERITERATORCLASS \
    ASSIMP_BUILD_NO_LIMITBONEWEIGHTS_PROCESS

win32: DEFINES += WindowsStore

intel_icc: {
    # warning #310: old-style parameter list (anachronism)
    QMAKE_CFLAGS_WARN_ON += -wd310

    # warning #68: integer conversion resulted in a change of sign
    QMAKE_CFLAGS_WARN_ON += -wd68

    # warning #858: type qualifier on return type is meaningless
    QMAKE_CFLAGS_WARN_ON += -wd858

    QMAKE_CXXFLAGS_WARN_ON += $$QMAKE_CFLAGS_WARN_ON
} else:gcc|clang: {
    # Stop compiler complaining about ignored qualifiers on return types
    QMAKE_CFLAGS_WARN_ON += -Wno-ignored-qualifiers -Wno-unused-parameter -Wno-unused-variable -Wno-deprecated-declarations -Wno-unused-function
    QMAKE_CXXFLAGS_WARN_ON = $$QMAKE_CFLAGS_WARN_ON -Wno-reorder
} else:msvc {
    # Disabled Warnings:
    #   4100: 'identifier' : unreferenced formal parameter
    #   4189: 'identifier' : local variable is initialized but not referenced
    #   4267: coversion from 'size_t' to 'int', possible loss of data
    #   4996: Function call with parameters that may be unsafe
    #   4828: The file contains a character starting at offset 0x167b that
    #         is illegal in the current source character set (codepage 65001)
    QMAKE_CFLAGS_WARN_ON += -wd"4100" -wd"4189" -wd"4267" -wd"4996" -wd"4828"
    QMAKE_CXXFLAGS_WARN_ON = $$QMAKE_CFLAGS_WARN_ON
}

clang: {
    QMAKE_CFLAGS_WARN_ON += -Wno-unused-private-field
    QMAKE_CXXFLAGS_WARN_ON = $$QMAKE_CFLAGS_WARN_ON
}

# Prevents "catastrophic error: Too many segments for object format" for builds using Windows ICC
msvc: QMAKE_CXXFLAGS += /bigobj

CONFIG += warn_on

VPATH += \
    $$PWD/src \
    $$PWD/src/code \
    $$PWD/src/code/res

INCLUDEPATH += \
        $$PWD \
        $$PWD/.. \
        $$PWD/src \
        $$PWD/src/code \
        $$PWD/src/code/Common \
        $$PWD/src/code/AssetLib/Collada \
        $$PWD/src/code/Material \
        $$PWD/src/code/AssetLib/Obj \
        $$PWD/src/code/AssetLib/FBX \
        $$PWD/src/code/PostProcessing \
        $$PWD/src/code/AssetLib/STL \
        $$PWD/src/code/AssetLib/glTF \
        $$PWD/src/code/AssetLib/glTF2 \
        $$PWD/src/code/CApi \
        $$PWD/src/include \
        $$PWD/src/include/assimp/Compiler

HEADERS += \
    $$PWD/config.h \
    $$PWD/revision.h

HEADERS += \
    $$PWD/src/include/assimp/Compiler/pushpack1.h \
    $$PWD/src/include/assimp/Compiler/poppack1.h \
    $$PWD/src/include/assimp/Compiler/pstdint.h \
    $$PWD/src/include/assimp/anim.h \
    $$PWD/src/include/assimp/ai_assert.h \
    $$PWD/src/include/assimp/camera.h \
    $$PWD/src/include/assimp/color4.h \
    $$PWD/src/include/assimp/color4.inl \
    $$PWD/src/include/assimp/defs.h \
    $$PWD/src/include/assimp/Defines.h \
    $$PWD/src/include/assimp/cfileio.h \
    $$PWD/src/include/assimp/light.h \
    $$PWD/src/include/assimp/material.h \
    $$PWD/src/include/assimp/material.inl \
    $$PWD/src/include/assimp/matrix3x3.h \
    $$PWD/src/include/assimp/matrix3x3.inl \
    $$PWD/src/include/assimp/matrix4x4.h \
    $$PWD/src/include/assimp/matrix4x4.inl \
    $$PWD/src/include/assimp/mesh.h \
    $$PWD/src/include/assimp/pbrmaterial.h \
    $$PWD/src/include/assimp/postprocess.h \
    $$PWD/src/include/assimp/quaternion.h \
    $$PWD/src/include/assimp/quaternion.inl \
    $$PWD/src/include/assimp/scene.h \
    $$PWD/src/include/assimp/metadata.h \
    $$PWD/src/include/assimp/texture.h \
    $$PWD/src/include/assimp/types.h \
    $$PWD/src/include/assimp/vector2.h \
    $$PWD/src/include/assimp/vector2.inl \
    $$PWD/src/include/assimp/vector3.h \
    $$PWD/src/include/assimp/vector3.inl \
    $$PWD/src/include/assimp/version.h \
    $$PWD/src/include/assimp/cimport.h \
    $$PWD/src/include/assimp/importerdesc.h \
    $$PWD/src/include/assimp/Importer.hpp \
    $$PWD/src/include/assimp/DefaultLogger.hpp \
    $$PWD/src/include/assimp/ProgressHandler.hpp \
    $$PWD/src/include/assimp/IOStream.hpp \
    $$PWD/src/include/assimp/IOSystem.hpp \
    $$PWD/src/include/assimp/Logger.hpp \
    $$PWD/src/include/assimp/LogStream.hpp \
    $$PWD/src/include/assimp/NullLogger.hpp \
    $$PWD/src/include/assimp/cexport.h \
    $$PWD/src/include/assimp/Exporter.hpp \
    $$PWD/src/include/assimp/DefaultIOStream.h \
    $$PWD/src/include/assimp/DefaultIOSystem.h \
    $$PWD/src/include/assimp/SceneCombiner.h \
    $$PWD/src/include/assimp/fast_atof.h \
    $$PWD/src/include/assimp/qnan.h \
    $$PWD/src/include/assimp/BaseImporter.h \
    $$PWD/src/include/assimp/Hash.h \
    $$PWD/src/include/assimp/MemoryIOWrapper.h \
    $$PWD/src/include/assimp/ParsingUtils.h \
    $$PWD/src/include/assimp/StreamReader.h \
    $$PWD/src/include/assimp/StreamWriter.h \
    $$PWD/src/include/assimp/StringComparison.h \
    $$PWD/src/include/assimp/StringUtils.h \
    $$PWD/src/include/assimp/SGSpatialSort.h \
    $$PWD/src/include/assimp/GenericProperty.h \
    $$PWD/src/include/assimp/SpatialSort.h \
    $$PWD/src/include/assimp/SkeletonMeshBuilder.h \
    $$PWD/src/include/assimp/SmoothingGroups.h \
    $$PWD/src/include/assimp/SmoothingGroups.inl \
    $$PWD/src/include/assimp/StandardShapes.h \
    $$PWD/src/include/assimp/RemoveComments.h \
    $$PWD/src/include/assimp/Subdivision.h \
    $$PWD/src/include/assimp/Vertex.h \
    $$PWD/src/include/assimp/LineSplitter.h \
    $$PWD/src/include/assimp/TinyFormatter.h \
    $$PWD/src/include/assimp/Profiler.h \
    $$PWD/src/include/assimp/LogAux.h \
    $$PWD/src/include/assimp/Bitmap.h \
    $$PWD/src/include/assimp/XMLTools.h \
    $$PWD/src/include/assimp/IOStreamBuffer.h \
    $$PWD/src/include/assimp/CreateAnimMesh.h \
    $$PWD/src/include/assimp/BlobIOSystem.h \
    $$PWD/src/include/assimp/MathFunctions.h \
    $$PWD/src/include/assimp/Exceptional.h \
    $$PWD/src/include/assimp/ByteSwapper.h \
    $$PWD/src/include/assimp/DefaultLogger.hpp \
    $$PWD/src/include/assimp/LogStream.hpp \
    $$PWD/src/include/assimp/Logger.hpp \
    $$PWD/src/include/assimp/NullLogger.hpp \
    $$PWD/src/include/assimp/ZipArchiveIOSystem.h \
    $$PWD/src/code/Common/AssertHandler.h \
    $$PWD/src/code/Common/FileLogStream.h \
    $$PWD/src/code/Common/StdOStreamLogStream.h \
    $$PWD/src/code/Common/BaseProcess.h \
    $$PWD/src/code/Common/Importer.h \
    $$PWD/src/code/Common/ScenePrivate.h \
    $$PWD/src/code/Common/DefaultProgressHandler.h \
    $$PWD/src/code/CApi/CInterfaceIOWrapper.h \
    $$PWD/src/code/Common/IFF.h \
    $$PWD/src/code/Common/VertexTriangleAdjacency.h \
    $$PWD/src/code/Common/ScenePreprocessor.h \
    $$PWD/src/code/Common/TargetAnimation.h \
    $$PWD/src/code/Common/simd.h \
    $$PWD/src/code/AssetLib/Collada/ColladaHelper.h \
    $$PWD/src/code/AssetLib/Collada/ColladaLoader.h \
    $$PWD/src/code/AssetLib/Collada/ColladaParser.h \
    $$PWD/src/code/Material/MaterialSystem.h \
    $$PWD/src/code/AssetLib/Obj/ObjFileData.h \
    $$PWD/src/code/AssetLib/Obj/ObjFileImporter.h \
    $$PWD/src/code/AssetLib/Obj/ObjFileMtlImporter.h \
    $$PWD/src/code/AssetLib/Obj/ObjFileParser.h \
    $$PWD/src/code/AssetLib/Obj/ObjTools.h \
    $$PWD/src/code/AssetLib/FBX/FBXCompileConfig.h \
    $$PWD/src/code/AssetLib/FBX/FBXImporter.h \
    $$PWD/src/code/AssetLib/FBX/FBXParser.h \
    $$PWD/src/code/AssetLib/FBX/FBXTokenizer.h \
    $$PWD/src/code/AssetLib/FBX/FBXImportSettings.h \
    $$PWD/src/code/AssetLib/FBX/FBXConverter.h \
    $$PWD/src/code/AssetLib/FBX/FBXUtil.h \
    $$PWD/src/code/AssetLib/FBX/FBXDocument.h \
    $$PWD/src/code/AssetLib/FBX/FBXProperties.h \
    $$PWD/src/code/AssetLib/FBX/FBXMeshGeometry.h \
    $$PWD/src/code/AssetLib/FBX/FBXCommon.h \
    $$PWD/src/code/PostProcessing/ArmaturePopulate.h \
    $$PWD/src/code/PostProcessing/CalcTangentsProcess.h \
    $$PWD/src/code/PostProcessing/ComputeUVMappingProcess.h \
    $$PWD/src/code/PostProcessing/ConvertToLHProcess.h \
    $$PWD/src/code/PostProcessing/EmbedTexturesProcess.h \
    $$PWD/src/code/PostProcessing/FindDegenerates.h \
    $$PWD/src/code/PostProcessing/FindInstancesProcess.h \
    $$PWD/src/code/PostProcessing/FindInvalidDataProcess.h \
    $$PWD/src/code/PostProcessing/FixNormalsStep.h \
    $$PWD/src/code/PostProcessing/DropFaceNormalsProcess.h \
    $$PWD/src/code/PostProcessing/GenBoundingBoxesProcess.h \
    $$PWD/src/code/PostProcessing/GenFaceNormalsProcess.h \
    $$PWD/src/code/PostProcessing/GenVertexNormalsProcess.h \
    $$PWD/src/code/PostProcessing/PretransformVertices.h \
    $$PWD/src/code/PostProcessing/ImproveCacheLocality.h \
    $$PWD/src/code/PostProcessing/JoinVerticesProcess.h \
    $$PWD/src/code/PostProcessing/RemoveRedundantMaterials.h \
    $$PWD/src/code/PostProcessing/RemoveVCProcess.h \
    $$PWD/src/code/PostProcessing/SortByPTypeProcess.h \
    $$PWD/src/code/PostProcessing/SplitLargeMeshes.h \
    $$PWD/src/code/PostProcessing/SplitByBoneCountProcess.h \
    $$PWD/src/code/PostProcessing/TextureTransform.h \
    $$PWD/src/code/PostProcessing/TriangulateProcess.h \
    $$PWD/src/code/PostProcessing/ValidateDataStructure.h \
    $$PWD/src/code/PostProcessing/OptimizeGraph.h \
    $$PWD/src/code/PostProcessing/OptimizeMeshes.h \
    $$PWD/src/code/PostProcessing/DeboneProcess.h \
    $$PWD/src/code/PostProcessing/ProcessHelper.h \
    $$PWD/src/code/Common/PolyTools.h \
    $$PWD/src/code/PostProcessing/MakeVerboseFormat.h \
    $$PWD/src/code/PostProcessing/ScaleProcess.h \
    $$PWD/src/code/AssetLib/glTF/glTFAsset.h \
    $$PWD/src/code/AssetLib/glTF/glTFAsset.inl \
    $$PWD/src/code/AssetLib/glTF/glTFAssetWriter.inl \
    $$PWD/src/code/AssetLib/glTF/glTFAssetWriter.h \
    $$PWD/src/code/AssetLib/glTF/glTFImporter.h \
    $$PWD/src/code/AssetLib/glTF/glTFCommon.h \
    $$PWD/src/code/AssetLib/glTF2/glTF2AssetWriter.h \
    $$PWD/src/code/AssetLib/glTF2/glTF2Asset.h \
    $$PWD/src/code/AssetLib/glTF2/glTF2Asset.inl \
    $$PWD/src/code/AssetLib/glTF2/glTF2AssetWriter.inl \
    $$PWD/src/code/AssetLib/glTF2/glTF2Importer.h \
    $$PWD/src/code/AssetLib/STL/STLLoader.h

SOURCES += \
    $$PWD/src/code/Common/Assimp.cpp \
    $$PWD/src/code/Common/AssertHandler.cpp \
    $$PWD/src/code/Common/DefaultLogger.cpp \
    $$PWD/src/code/Common/BaseImporter.cpp \
    $$PWD/src/code/Common/BaseProcess.cpp \
    $$PWD/src/code/Common/PostStepRegistry.cpp \
    $$PWD/src/code/Common/ImporterRegistry.cpp \
    $$PWD/src/code/Common/DefaultIOStream.cpp \
    $$PWD/src/code/Common/DefaultIOSystem.cpp \
    $$PWD/src/code/Common/Exceptional.cpp \
    $$PWD/src/code/CApi/CInterfaceIOWrapper.cpp \
    $$PWD/src/code/Common/Importer.cpp \
    $$PWD/src/code/Common/SGSpatialSort.cpp \
    $$PWD/src/code/Common/VertexTriangleAdjacency.cpp \
    $$PWD/src/code/Common/SpatialSort.cpp \
    $$PWD/src/code/Common/SceneCombiner.cpp \
    $$PWD/src/code/Common/ScenePreprocessor.cpp \
    $$PWD/src/code/Common/SkeletonMeshBuilder.cpp \
    $$PWD/src/code/Common/StandardShapes.cpp \
    $$PWD/src/code/Common/TargetAnimation.cpp \
    $$PWD/src/code/Common/RemoveComments.cpp \
    $$PWD/src/code/Common/Subdivision.cpp \
    $$PWD/src/code/Common/scene.cpp \
    $$PWD/src/code/Common/Bitmap.cpp \
    $$PWD/src/code/Common/Version.cpp \
    $$PWD/src/code/Common/CreateAnimMesh.cpp \
    $$PWD/src/code/Common/simd.cpp \
    $$PWD/src/code/Common/material.cpp \
    $$PWD/src/code/Common/ZipArchiveIOSystem.cpp \
    $$PWD/src/code/AssetLib/Collada/ColladaHelper.cpp \
    $$PWD/src/code/AssetLib/Collada/ColladaLoader.cpp \
    $$PWD/src/code/AssetLib/Collada/ColladaParser.cpp \
    $$PWD/src/code/Material/MaterialSystem.cpp \
    $$PWD/src/code/AssetLib/Obj/ObjFileImporter.cpp \
    $$PWD/src/code/AssetLib/Obj/ObjFileMtlImporter.cpp \
    $$PWD/src/code/AssetLib/Obj/ObjFileParser.cpp \
    $$PWD/src/code/AssetLib/FBX/FBXImporter.cpp \
    $$PWD/src/code/AssetLib/FBX/FBXParser.cpp \
    $$PWD/src/code/AssetLib/FBX/FBXTokenizer.cpp \
    $$PWD/src/code/AssetLib/FBX/FBXConverter.cpp \
    $$PWD/src/code/AssetLib/FBX/FBXUtil.cpp \
    $$PWD/src/code/AssetLib/FBX/FBXDocument.cpp \
    $$PWD/src/code/AssetLib/FBX/FBXProperties.cpp \
    $$PWD/src/code/AssetLib/FBX/FBXMeshGeometry.cpp \
    $$PWD/src/code/AssetLib/FBX/FBXMaterial.cpp \
    $$PWD/src/code/AssetLib/FBX/FBXModel.cpp \
    $$PWD/src/code/AssetLib/FBX/FBXAnimation.cpp \
    $$PWD/src/code/AssetLib/FBX/FBXNodeAttribute.cpp \
    $$PWD/src/code/AssetLib/FBX/FBXDeformer.cpp \
    $$PWD/src/code/AssetLib/FBX/FBXBinaryTokenizer.cpp \
    $$PWD/src/code/AssetLib/FBX/FBXDocumentUtil.cpp \
    $$PWD/src/code/PostProcessing/ArmaturePopulate.cpp \
    $$PWD/src/code/PostProcessing/CalcTangentsProcess.cpp \
    $$PWD/src/code/PostProcessing/ComputeUVMappingProcess.cpp \
    $$PWD/src/code/PostProcessing/ConvertToLHProcess.cpp \
    $$PWD/src/code/PostProcessing/EmbedTexturesProcess.cpp \
    $$PWD/src/code/PostProcessing/FindDegenerates.cpp \
    $$PWD/src/code/PostProcessing/FindInstancesProcess.cpp \
    $$PWD/src/code/PostProcessing/FindInvalidDataProcess.cpp \
    $$PWD/src/code/PostProcessing/FixNormalsStep.cpp \
    $$PWD/src/code/PostProcessing/DropFaceNormalsProcess.cpp \
    $$PWD/src/code/PostProcessing/GenBoundingBoxesProcess.cpp \
    $$PWD/src/code/PostProcessing/GenFaceNormalsProcess.cpp \
    $$PWD/src/code/PostProcessing/GenVertexNormalsProcess.cpp \
    $$PWD/src/code/PostProcessing/PretransformVertices.cpp \
    $$PWD/src/code/PostProcessing/ImproveCacheLocality.cpp \
    $$PWD/src/code/PostProcessing/JoinVerticesProcess.cpp \
    $$PWD/src/code/PostProcessing/RemoveRedundantMaterials.cpp \
    $$PWD/src/code/PostProcessing/RemoveVCProcess.cpp \
    $$PWD/src/code/PostProcessing/SortByPTypeProcess.cpp \
    $$PWD/src/code/PostProcessing/SplitLargeMeshes.cpp \
    $$PWD/src/code/PostProcessing/SplitByBoneCountProcess.cpp \
    $$PWD/src/code/PostProcessing/TextureTransform.cpp \
    $$PWD/src/code/PostProcessing/TriangulateProcess.cpp \
    $$PWD/src/code/PostProcessing/ValidateDataStructure.cpp \
    $$PWD/src/code/PostProcessing/OptimizeGraph.cpp \
    $$PWD/src/code/PostProcessing/OptimizeMeshes.cpp \
    $$PWD/src/code/PostProcessing/DeboneProcess.cpp \
    $$PWD/src/code/PostProcessing/ProcessHelper.cpp \
    $$PWD/src/code/PostProcessing/MakeVerboseFormat.cpp \
    $$PWD/src/code/PostProcessing/ScaleProcess.cpp \
    $$PWD/src/code/AssetLib/glTF/glTFImporter.cpp \
    $$PWD/src/code/AssetLib/glTF/glTFCommon.cpp \
    $$PWD/src/code/AssetLib/glTF2/glTF2Importer.cpp \
    $$PWD/src/code/AssetLib/STL/STLLoader.cpp

# pugixml (needed for DAE/Collada support)
HEADERS += \
    $$PWD/src/contrib/pugixml/src/pugixml.hpp \
    $$PWD/src/contrib/pugixml/contrib/foreach.hpp

SOURCES += $$PWD/src/contrib/pugixml/src/pugixml.cpp

VPATH += $$PWD/src/contrib/pugixml/src
INCLUDEPATH += $$PWD/src/contrib/pugixml/src \
        $PWD/src/contrib/pugixml/contrib

msvc: DEFINES += _SCL_SECURE_NO_WARNINGS _CRT_SECURE_NO_WARNINGS

# rapidjson (needed for GLTF/GLTF2)
VPATH += $$PWD/src/contrib/rapidjson/include
INCLUDEPATH += $$PWD/src/contrib/rapidjson/include

# utf8cpp
VPATH += $$PWD/src/contrib/utf8cpp/source
INCLUDEPATH += $$PWD/src/contrib/utf8cpp/source

# poly2tri (blender tessellator)
VPATH += $$PWD/src/contrib/poly2tri
INCLUDEPATH += $$PWD/src/contrib/poly2tri

HEADERS += \
    $$PWD/src/contrib/poly2tri/poly2tri/common/shapes.h \
    $$PWD/src/contrib/poly2tri/poly2tri/common/utils.h \
    $$PWD/src/contrib/poly2tri/poly2tri/sweep/advancing_front.h \
    $$PWD/src/contrib/poly2tri/poly2tri/sweep/cdt.h \
    $$PWD/src/contrib/poly2tri/poly2tri/sweep/sweep.h \
    $$PWD/src/contrib/poly2tri/poly2tri/sweep/sweep_context.h

SOURCES += \
    $$PWD/src/contrib/poly2tri/poly2tri/common/shapes.cc \
    $$PWD/src/contrib/poly2tri/poly2tri/sweep/advancing_front.cc \
    $$PWD/src/contrib/poly2tri/poly2tri/sweep/cdt.cc \
    $$PWD/src/contrib/poly2tri/poly2tri/sweep/sweep.cc \
    $$PWD/src/contrib/poly2tri/poly2tri/sweep/sweep_context.cc

# unzip (collada loader)
VPATH += $$PWD/unzip $$PWD/contrib/unzip
INCLUDEPATH += $$PWD/unzip $$PWD/contrib/unzip

HEADERS += \
    $$PWD/src/contrib/unzip/crypt.h \
    $$PWD/src/contrib/unzip/ioapi.h \
    $$PWD/unzip/unzip.h
SOURCES += \
    $$PWD/src/contrib/unzip/ioapi.c \
    $$PWD/src/contrib/unzip/unzip.c
