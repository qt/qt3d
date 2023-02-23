# special case begin
if(NOT QT_FEATURE_system_zlib)
    find_package(Qt6 COMPONENTS ZlibPrivate)
elseif(NOT TARGET WrapZLIB::WrapZLIB)
    qt_find_package(WrapZLIB PROVIDED_TARGETS WrapZLIB::WrapZLIB)
endif()
# special case end

function(qt3d_extend_target_for_assimp target)
    set(assimpDir ${PROJECT_SOURCE_DIR}/src/3rdparty/assimp)
    qt_internal_extend_target(${target} CONDITION QT_FEATURE_qt3d_system_assimp AND (NOT CMAKE_CROSSCOMPILING OR NOT host_build)
        LIBRARIES
            WrapQt3DAssimp::WrapQt3DAssimp
    )

    qt_internal_extend_target(${target} CONDITION NOT QT_FEATURE_qt3d_system_assimp OR (CMAKE_CROSSCOMPILING AND host_build)
        SOURCES
            ${assimpDir}/config.h
            ${assimpDir}/revision.h
            ${assimpDir}/src/code/CApi/CInterfaceIOWrapper.cpp ${assimpDir}/src/code/CApi/CInterfaceIOWrapper.h
            ${assimpDir}/src/code/AssetLib/Collada/ColladaHelper.cpp ${assimpDir}/src/code/AssetLib/Collada/ColladaHelper.h
            ${assimpDir}/src/code/AssetLib/Collada/ColladaLoader.cpp ${assimpDir}/src/code/AssetLib/Collada/ColladaLoader.h
            ${assimpDir}/src/code/AssetLib/Collada/ColladaParser.cpp ${assimpDir}/src/code/AssetLib/Collada/ColladaParser.h
            ${assimpDir}/src/code/Common/Assimp.cpp
            ${assimpDir}/src/code/Common/AssertHandler.cpp ${assimpDir}/src/code/Common/AssertHandler.h
            ${assimpDir}/src/code/Common/BaseImporter.cpp
            ${assimpDir}/src/code/Common/BaseProcess.cpp ${assimpDir}/src/code/Common/BaseProcess.h
            ${assimpDir}/src/code/Common/Bitmap.cpp
            ${assimpDir}/src/code/Common/CreateAnimMesh.cpp
            ${assimpDir}/src/code/Common/DefaultIOStream.cpp
            ${assimpDir}/src/code/Common/DefaultIOSystem.cpp
            ${assimpDir}/src/code/Common/DefaultLogger.cpp
            ${assimpDir}/src/code/Common/DefaultProgressHandler.h
            ${assimpDir}/src/code/Common/Exceptional.cpp
            ${assimpDir}/src/code/Common/FileLogStream.h
            ${assimpDir}/src/code/Common/IFF.h
            ${assimpDir}/src/code/Common/Importer.cpp ${assimpDir}/src/code/Common/Importer.h
            ${assimpDir}/src/code/Common/ImporterRegistry.cpp
            ${assimpDir}/src/code/Common/PolyTools.h
            ${assimpDir}/src/code/Common/PostStepRegistry.cpp
            ${assimpDir}/src/code/Common/RemoveComments.cpp
            ${assimpDir}/src/code/Common/SGSpatialSort.cpp
            ${assimpDir}/src/code/Common/SceneCombiner.cpp
            ${assimpDir}/src/code/Common/ScenePreprocessor.cpp ${assimpDir}/src/code/Common/ScenePreprocessor.h
            ${assimpDir}/src/code/Common/ScenePrivate.h
            ${assimpDir}/src/code/Common/SkeletonMeshBuilder.cpp
            ${assimpDir}/src/code/Common/SpatialSort.cpp
            ${assimpDir}/src/code/Common/StandardShapes.cpp
            ${assimpDir}/src/code/Common/StdOStreamLogStream.h
            ${assimpDir}/src/code/Common/Subdivision.cpp
            ${assimpDir}/src/code/Common/TargetAnimation.cpp ${assimpDir}/src/code/Common/TargetAnimation.h
            ${assimpDir}/src/code/Common/Version.cpp
            ${assimpDir}/src/code/Common/VertexTriangleAdjacency.cpp ${assimpDir}/src/code/Common/VertexTriangleAdjacency.h
            ${assimpDir}/src/code/Common/ZipArchiveIOSystem.cpp
            ${assimpDir}/src/code/Common/material.cpp
            ${assimpDir}/src/code/Common/scene.cpp
            ${assimpDir}/src/code/Common/simd.cpp ${assimpDir}/src/code/Common/simd.h
            ${assimpDir}/src/code/AssetLib/FBX/FBXAnimation.cpp
            ${assimpDir}/src/code/AssetLib/FBX/FBXBinaryTokenizer.cpp
            ${assimpDir}/src/code/AssetLib/FBX/FBXCommon.h
            ${assimpDir}/src/code/AssetLib/FBX/FBXCompileConfig.h
            ${assimpDir}/src/code/AssetLib/FBX/FBXConverter.cpp ${assimpDir}/src/code/AssetLib/FBX/FBXConverter.h
            ${assimpDir}/src/code/AssetLib/FBX/FBXDeformer.cpp
            ${assimpDir}/src/code/AssetLib/FBX/FBXDocument.cpp ${assimpDir}/src/code/AssetLib/FBX/FBXDocument.h
            ${assimpDir}/src/code/AssetLib/FBX/FBXDocumentUtil.cpp
            ${assimpDir}/src/code/AssetLib/FBX/FBXImportSettings.h
            ${assimpDir}/src/code/AssetLib/FBX/FBXImporter.cpp ${assimpDir}/src/code/AssetLib/FBX/FBXImporter.h
            ${assimpDir}/src/code/AssetLib/FBX/FBXMaterial.cpp
            ${assimpDir}/src/code/AssetLib/FBX/FBXMeshGeometry.cpp ${assimpDir}/src/code/AssetLib/FBX/FBXMeshGeometry.h
            ${assimpDir}/src/code/AssetLib/FBX/FBXModel.cpp
            ${assimpDir}/src/code/AssetLib/FBX/FBXNodeAttribute.cpp
            ${assimpDir}/src/code/AssetLib/FBX/FBXParser.cpp ${assimpDir}/src/code/AssetLib/FBX/FBXParser.h
            ${assimpDir}/src/code/AssetLib/FBX/FBXProperties.cpp ${assimpDir}/src/code/AssetLib/FBX/FBXProperties.h
            ${assimpDir}/src/code/AssetLib/FBX/FBXTokenizer.cpp ${assimpDir}/src/code/AssetLib/FBX/FBXTokenizer.h
            ${assimpDir}/src/code/AssetLib/FBX/FBXUtil.cpp ${assimpDir}/src/code/AssetLib/FBX/FBXUtil.h
            ${assimpDir}/src/code/Material/MaterialSystem.cpp ${assimpDir}/src/code/Material/MaterialSystem.h
            ${assimpDir}/src/code/AssetLib/Obj/ObjFileData.h
            ${assimpDir}/src/code/AssetLib/Obj/ObjFileImporter.cpp ${assimpDir}/src/code/AssetLib/Obj/ObjFileImporter.h
            ${assimpDir}/src/code/AssetLib/Obj/ObjFileMtlImporter.cpp ${assimpDir}/src/code/AssetLib/Obj/ObjFileMtlImporter.h
            ${assimpDir}/src/code/AssetLib/Obj/ObjFileParser.cpp ${assimpDir}/src/code/AssetLib/Obj/ObjFileParser.h
            ${assimpDir}/src/code/AssetLib/Obj/ObjTools.h
            ${assimpDir}/src/code/PostProcessing/ArmaturePopulate.cpp ${assimpDir}/src/code/PostProcessing/ArmaturePopulate.h
            ${assimpDir}/src/code/PostProcessing/CalcTangentsProcess.cpp ${assimpDir}/src/code/PostProcessing/CalcTangentsProcess.h
            ${assimpDir}/src/code/PostProcessing/ComputeUVMappingProcess.cpp ${assimpDir}/src/code/PostProcessing/ComputeUVMappingProcess.h
            ${assimpDir}/src/code/PostProcessing/ConvertToLHProcess.cpp ${assimpDir}/src/code/PostProcessing/ConvertToLHProcess.h
            ${assimpDir}/src/code/PostProcessing/DeboneProcess.cpp ${assimpDir}/src/code/PostProcessing/DeboneProcess.h
            ${assimpDir}/src/code/PostProcessing/DropFaceNormalsProcess.cpp ${assimpDir}/src/code/PostProcessing/DropFaceNormalsProcess.h
            ${assimpDir}/src/code/PostProcessing/EmbedTexturesProcess.cpp ${assimpDir}/src/code/PostProcessing/EmbedTexturesProcess.h
            ${assimpDir}/src/code/PostProcessing/FindDegenerates.cpp ${assimpDir}/src/code/PostProcessing/FindDegenerates.h
            ${assimpDir}/src/code/PostProcessing/FindInstancesProcess.cpp ${assimpDir}/src/code/PostProcessing/FindInstancesProcess.h
            ${assimpDir}/src/code/PostProcessing/FindInvalidDataProcess.cpp ${assimpDir}/src/code/PostProcessing/FindInvalidDataProcess.h
            ${assimpDir}/src/code/PostProcessing/FixNormalsStep.cpp ${assimpDir}/src/code/PostProcessing/FixNormalsStep.h
            ${assimpDir}/src/code/PostProcessing/GenBoundingBoxesProcess.cpp ${assimpDir}/src/code/PostProcessing/GenBoundingBoxesProcess.h
            ${assimpDir}/src/code/PostProcessing/GenFaceNormalsProcess.cpp ${assimpDir}/src/code/PostProcessing/GenFaceNormalsProcess.h
            ${assimpDir}/src/code/PostProcessing/GenVertexNormalsProcess.cpp ${assimpDir}/src/code/PostProcessing/GenVertexNormalsProcess.h
            ${assimpDir}/src/code/PostProcessing/ImproveCacheLocality.cpp ${assimpDir}/src/code/PostProcessing/ImproveCacheLocality.h
            ${assimpDir}/src/code/PostProcessing/JoinVerticesProcess.cpp ${assimpDir}/src/code/PostProcessing/JoinVerticesProcess.h
            ${assimpDir}/src/code/PostProcessing/MakeVerboseFormat.cpp ${assimpDir}/src/code/PostProcessing/MakeVerboseFormat.h
            ${assimpDir}/src/code/PostProcessing/OptimizeGraph.cpp ${assimpDir}/src/code/PostProcessing/OptimizeGraph.h
            ${assimpDir}/src/code/PostProcessing/OptimizeMeshes.cpp ${assimpDir}/src/code/PostProcessing/OptimizeMeshes.h
            ${assimpDir}/src/code/PostProcessing/PretransformVertices.cpp ${assimpDir}/src/code/PostProcessing/PretransformVertices.h
            ${assimpDir}/src/code/PostProcessing/ProcessHelper.cpp ${assimpDir}/src/code/PostProcessing/ProcessHelper.h
            ${assimpDir}/src/code/PostProcessing/RemoveRedundantMaterials.cpp ${assimpDir}/src/code/PostProcessing/RemoveRedundantMaterials.h
            ${assimpDir}/src/code/PostProcessing/RemoveVCProcess.cpp ${assimpDir}/src/code/PostProcessing/RemoveVCProcess.h
            ${assimpDir}/src/code/PostProcessing/ScaleProcess.cpp ${assimpDir}/src/code/PostProcessing/ScaleProcess.h
            ${assimpDir}/src/code/PostProcessing/SortByPTypeProcess.cpp ${assimpDir}/src/code/PostProcessing/SortByPTypeProcess.h
            ${assimpDir}/src/code/PostProcessing/SplitByBoneCountProcess.cpp ${assimpDir}/src/code/PostProcessing/SplitByBoneCountProcess.h
            ${assimpDir}/src/code/PostProcessing/SplitLargeMeshes.cpp ${assimpDir}/src/code/PostProcessing/SplitLargeMeshes.h
            ${assimpDir}/src/code/PostProcessing/TextureTransform.cpp ${assimpDir}/src/code/PostProcessing/TextureTransform.h
            ${assimpDir}/src/code/PostProcessing/TriangulateProcess.cpp ${assimpDir}/src/code/PostProcessing/TriangulateProcess.h
            ${assimpDir}/src/code/PostProcessing/ValidateDataStructure.cpp ${assimpDir}/src/code/PostProcessing/ValidateDataStructure.h
            ${assimpDir}/src/code/AssetLib/STL/STLLoader.cpp ${assimpDir}/src/code/AssetLib/STL/STLLoader.h
            ${assimpDir}/src/code/AssetLib/glTF/glTFAsset.h ${assimpDir}/src/code/AssetLib/glTF/glTFAsset.inl
            ${assimpDir}/src/code/AssetLib/glTF/glTFAssetWriter.h ${assimpDir}/src/code/AssetLib/glTF/glTFAssetWriter.inl
            ${assimpDir}/src/code/AssetLib/glTF/glTFCommon.cpp ${assimpDir}/src/code/AssetLib/glTF/glTFCommon.h
            ${assimpDir}/src/code/AssetLib/glTF/glTFImporter.cpp ${assimpDir}/src/code/AssetLib/glTF/glTFImporter.h
            ${assimpDir}/src/code/AssetLib/glTF2/glTF2Asset.h ${assimpDir}/src/code/AssetLib/glTF2/glTF2Asset.inl
            ${assimpDir}/src/code/AssetLib/glTF2/glTF2AssetWriter.h ${assimpDir}/src/code/AssetLib/glTF2/glTF2AssetWriter.inl
            ${assimpDir}/src/code/AssetLib/glTF2/glTF2Importer.cpp ${assimpDir}/src/code/AssetLib/glTF2/glTF2Importer.h
            ${assimpDir}/src/contrib/pugixml/contrib/foreach.hpp
            ${assimpDir}/src/contrib/pugixml/src/pugixml.hpp
            ${assimpDir}/src/contrib/poly2tri/poly2tri/common/shapes.cc ${assimpDir}/src/contrib/poly2tri/poly2tri/common/shapes.h
            ${assimpDir}/src/contrib/poly2tri/poly2tri/common/utils.h
            ${assimpDir}/src/contrib/poly2tri/poly2tri/sweep/advancing_front.cc ${assimpDir}/src/contrib/poly2tri/poly2tri/sweep/advancing_front.h
            ${assimpDir}/src/contrib/poly2tri/poly2tri/sweep/cdt.cc ${assimpDir}/src/contrib/poly2tri/poly2tri/sweep/cdt.h
            ${assimpDir}/src/contrib/poly2tri/poly2tri/sweep/sweep.cc ${assimpDir}/src/contrib/poly2tri/poly2tri/sweep/sweep.h
            ${assimpDir}/src/contrib/poly2tri/poly2tri/sweep/sweep_context.cc ${assimpDir}/src/contrib/poly2tri/poly2tri/sweep/sweep_context.h
            ${assimpDir}/src/contrib/unzip/crypt.h
            ${assimpDir}/src/contrib/unzip/ioapi.c ${assimpDir}/src/contrib/unzip/ioapi.h
            ${assimpDir}/src/contrib/unzip/unzip.c
            ${assimpDir}/src/include/assimp/BaseImporter.h
            ${assimpDir}/src/include/assimp/Bitmap.h
            ${assimpDir}/src/include/assimp/BlobIOSystem.h
            ${assimpDir}/src/include/assimp/ByteSwapper.h
            ${assimpDir}/src/include/assimp/Compiler/poppack1.h
            ${assimpDir}/src/include/assimp/Compiler/pstdint.h
            ${assimpDir}/src/include/assimp/Compiler/pushpack1.h
            ${assimpDir}/src/include/assimp/CreateAnimMesh.h
            ${assimpDir}/src/include/assimp/DefaultIOStream.h
            ${assimpDir}/src/include/assimp/DefaultIOSystem.h
            ${assimpDir}/src/include/assimp/DefaultLogger.hpp
            ${assimpDir}/src/include/assimp/Defines.h
            ${assimpDir}/src/include/assimp/Exceptional.h
            ${assimpDir}/src/include/assimp/Exporter.hpp
            ${assimpDir}/src/include/assimp/GenericProperty.h
            ${assimpDir}/src/include/assimp/Hash.h
            ${assimpDir}/src/include/assimp/IOStream.hpp
            ${assimpDir}/src/include/assimp/IOStreamBuffer.h
            ${assimpDir}/src/include/assimp/IOSystem.hpp
            ${assimpDir}/src/include/assimp/Importer.hpp
            ${assimpDir}/src/include/assimp/LineSplitter.h
            ${assimpDir}/src/include/assimp/LogAux.h
            ${assimpDir}/src/include/assimp/LogStream.hpp
            ${assimpDir}/src/include/assimp/Logger.hpp
            ${assimpDir}/src/include/assimp/MathFunctions.h
            ${assimpDir}/src/include/assimp/MemoryIOWrapper.h
            ${assimpDir}/src/include/assimp/NullLogger.hpp
            ${assimpDir}/src/include/assimp/ParsingUtils.h
            ${assimpDir}/src/include/assimp/Profiler.h
            ${assimpDir}/src/include/assimp/ProgressHandler.hpp
            ${assimpDir}/src/include/assimp/RemoveComments.h
            ${assimpDir}/src/include/assimp/SGSpatialSort.h
            ${assimpDir}/src/include/assimp/SceneCombiner.h
            ${assimpDir}/src/include/assimp/SkeletonMeshBuilder.h
            ${assimpDir}/src/include/assimp/SmoothingGroups.h ${assimpDir}/src/include/assimp/SmoothingGroups.inl
            ${assimpDir}/src/include/assimp/SpatialSort.h
            ${assimpDir}/src/include/assimp/StandardShapes.h
            ${assimpDir}/src/include/assimp/StreamReader.h
            ${assimpDir}/src/include/assimp/StreamWriter.h
            ${assimpDir}/src/include/assimp/StringComparison.h
            ${assimpDir}/src/include/assimp/StringUtils.h
            ${assimpDir}/src/include/assimp/Subdivision.h
            ${assimpDir}/src/include/assimp/TinyFormatter.h
            ${assimpDir}/src/include/assimp/Vertex.h
            ${assimpDir}/src/include/assimp/XMLTools.h
            ${assimpDir}/src/include/assimp/ZipArchiveIOSystem.h
            ${assimpDir}/src/include/assimp/ai_assert.h
            ${assimpDir}/src/include/assimp/anim.h
            ${assimpDir}/src/include/assimp/camera.h
            ${assimpDir}/src/include/assimp/cexport.h
            ${assimpDir}/src/include/assimp/cfileio.h
            ${assimpDir}/src/include/assimp/cimport.h
            ${assimpDir}/src/include/assimp/color4.h ${assimpDir}/src/include/assimp/color4.inl
            ${assimpDir}/src/include/assimp/defs.h
            ${assimpDir}/src/include/assimp/fast_atof.h
            ${assimpDir}/src/include/assimp/importerdesc.h
            ${assimpDir}/src/include/assimp/light.h
            ${assimpDir}/src/include/assimp/material.h ${assimpDir}/src/include/assimp/material.inl
            ${assimpDir}/src/include/assimp/matrix3x3.h ${assimpDir}/src/include/assimp/matrix3x3.inl
            ${assimpDir}/src/include/assimp/matrix4x4.h ${assimpDir}/src/include/assimp/matrix4x4.inl
            ${assimpDir}/src/include/assimp/mesh.h
            ${assimpDir}/src/include/assimp/metadata.h
            ${assimpDir}/src/include/assimp/pbrmaterial.h
            ${assimpDir}/src/include/assimp/postprocess.h
            ${assimpDir}/src/include/assimp/qnan.h
            ${assimpDir}/src/include/assimp/quaternion.h ${assimpDir}/src/include/assimp/quaternion.inl
            ${assimpDir}/src/include/assimp/scene.h
            ${assimpDir}/src/include/assimp/texture.h
            ${assimpDir}/src/include/assimp/types.h
            ${assimpDir}/src/include/assimp/vector2.h ${assimpDir}/src/include/assimp/vector2.inl
            ${assimpDir}/src/include/assimp/vector3.h ${assimpDir}/src/include/assimp/vector3.inl
            ${assimpDir}/src/include/assimp/version.h
            ${assimpDir}/unzip/unzip.h
        DEFINES
            ASSIMP_BUILD_BOOST_WORKAROUND
            ASSIMP_BUILD_NO_3DS_IMPORTER
            ASSIMP_BUILD_NO_3D_IMPORTER
            ASSIMP_BUILD_NO_3MF_IMPORTER
            ASSIMP_BUILD_NO_AC_IMPORTER
            ASSIMP_BUILD_NO_AMF_IMPORTER
            ASSIMP_BUILD_NO_ASE_IMPORTER
            ASSIMP_BUILD_NO_ASSBIN_IMPORTER
            ASSIMP_BUILD_NO_B3D_IMPORTER
            ASSIMP_BUILD_NO_BLEND_IMPORTER
            ASSIMP_BUILD_NO_BVH_IMPORTER
            ASSIMP_BUILD_NO_C4D_IMPORTER
            ASSIMP_BUILD_NO_COB_IMPORTER
            ASSIMP_BUILD_NO_COMPRESSED_IFC
            ASSIMP_BUILD_NO_CSM_IMPORTER
            ASSIMP_BUILD_NO_DXF_IMPORTER
            ASSIMP_BUILD_NO_EXPORT
            ASSIMP_BUILD_NO_HMP_IMPORTER
            ASSIMP_BUILD_NO_IFC_IMPORTER
            ASSIMP_BUILD_NO_IRRMESH_IMPORTER
            ASSIMP_BUILD_NO_IRR_IMPORTER
            ASSIMP_BUILD_NO_LIMITBONEWEIGHTS_PROCESS
            ASSIMP_BUILD_NO_LWO_IMPORTER
            ASSIMP_BUILD_NO_LWS_IMPORTER
            ASSIMP_BUILD_NO_M3D_IMPORTER
            ASSIMP_BUILD_NO_MD2_IMPORTER
            ASSIMP_BUILD_NO_MD3_IMPORTER
            ASSIMP_BUILD_NO_MD5_IMPORTER
            ASSIMP_BUILD_NO_MDC_IMPORTER
            ASSIMP_BUILD_NO_MDL_IMPORTER
            ASSIMP_BUILD_NO_MMD_IMPORTER
            ASSIMP_BUILD_NO_MS3D_IMPORTER
            ASSIMP_BUILD_NO_NDO_IMPORTER
            ASSIMP_BUILD_NO_NFF_IMPORTER
            ASSIMP_BUILD_NO_OFF_IMPORTER
            ASSIMP_BUILD_NO_OGRE_IMPORTER
            ASSIMP_BUILD_NO_OPENGEX_IMPORTER
            ASSIMP_BUILD_NO_OWN_ZLIB
            ASSIMP_BUILD_NO_PLY_IMPORTER
            ASSIMP_BUILD_NO_Q3BSP_IMPORTER
            ASSIMP_BUILD_NO_Q3D_IMPORTER
            ASSIMP_BUILD_NO_RAW_IMPORTER
            ASSIMP_BUILD_NO_SIB_IMPORTER
            ASSIMP_BUILD_NO_SMD_IMPORTER
            ASSIMP_BUILD_NO_STEP_IMPORTER
            ASSIMP_BUILD_NO_TERRAGEN_IMPORTER
            ASSIMP_BUILD_NO_X3D_IMPORTER
            ASSIMP_BUILD_NO_XGL_IMPORTER
            ASSIMP_BUILD_NO_X_IMPORTER
            ASSIMP_BUILD_NO_BLEND_IMPORTER
            NOUNCRYPT
            RAPIDJSON_HAS_STDSTRING=1
            RAPIDJSON_NOMEMBERITERATORCLASS
            ASSIMP_BUILD_NO_LIMITBONEWEIGHTS_PROCESS
        INCLUDE_DIRECTORIES
            ${assimpDir}
            ${assimpDir}/..
            ${assimpDir}/contrib/unzip
            ${assimpDir}/src
            ${assimpDir}/src/code
            ${assimpDir}/src/code/Blender
            ${assimpDir}/src/code/CApi
            ${assimpDir}/src/code/Collada
            ${assimpDir}/src/code/Common
            ${assimpDir}/src/code/FBX
            ${assimpDir}/src/code/Material
            ${assimpDir}/src/code/Obj
            ${assimpDir}/src/code/PostProcessing
            ${assimpDir}/src/code/glTF
            ${assimpDir}/src/code/glTF2
            ${assimpDir}/src/contrib/irrXML
            ${assimpDir}/src/contrib/openddlparser/include
            ${assimpDir}/src/contrib/poly2tri
            ${assimpDir}/src/contrib/pugixml/src
            ${assimpDir}/src/contrib/pugixml/contrib
            ${assimpDir}/src/contrib/rapidjson/include
            ${assimpDir}/src/contrib/utf8cpp/source
            ${assimpDir}/src/include
            ${assimpDir}/src/include/assimp/Compiler
            ${assimpDir}/unzip
        NO_UNITY_BUILD
    )

    qt_internal_extend_target(${target} CONDITION CMAKE_BUILD_TYPE STREQUAL Debug AND (CMAKE_CROSSCOMPILING OR NOT QT_FEATURE_qt3d_system_assimp) AND (host_build OR NOT QT_FEATURE_qt3d_system_assimp)
        DEFINES
            _DEBUG
    )

    qt_internal_extend_target(${target} CONDITION WIN32 AND (CMAKE_CROSSCOMPILING OR NOT QT_FEATURE_qt3d_system_assimp) AND (host_build OR NOT QT_FEATURE_qt3d_system_assimp)
        DEFINES
            _CRT_SECURE_NO_WARNINGS
    )

    qt_internal_extend_target(${target} CONDITION QT_FEATURE_system_zlib AND NOT QT_FEATURE_qt3d_system_assimp AND (NOT CMAKE_CROSSCOMPILING OR NOT host_build)
        LIBRARIES
            WrapZLIB::WrapZLIB
    )

    qt_internal_extend_target(${target} CONDITION (CMAKE_CROSSCOMPILING OR NOT QT_FEATURE_qt3d_system_assimp) AND (CMAKE_CROSSCOMPILING OR NOT QT_FEATURE_system_zlib) AND (host_build OR NOT QT_FEATURE_qt3d_system_assimp) AND (host_build OR NOT QT_FEATURE_system_zlib)
        LIBRARIES
            Qt::ZlibPrivate
    )

    # special case begin
    qt_internal_extend_target(${target} CONDITION ICC AND NOT QT_FEATURE_qt3d_system_assimp AND (NOT CMAKE_CROSSCOMPILING OR NOT host_build)
        COMPILE_OPTIONS
            "-wd310" "-wd68" "-wd858"
    )

    qt_internal_extend_target(${target} CONDITION (GCC OR CLANG) AND NOT QT_FEATURE_qt3d_system_assimp AND (NOT CMAKE_CROSSCOMPILING OR NOT host_build)
        COMPILE_OPTIONS
            "-Wno-ignored-qualifiers"
            "-Wno-unused-parameter"
            "-Wno-unused-variable"
            "-Wno-deprecated-declarations"
            "-Wno-unused-function"
            "-Wno-reorder"
    )

    qt_internal_extend_target(${target} CONDITION MSVC AND NOT QT_FEATURE_qt3d_system_assimp AND (NOT CMAKE_CROSSCOMPILING OR NOT host_build)
        COMPILE_OPTIONS
            "-wd4100"
            "-wd4189"
            "-wd4267"
            "-wd4996"
            "-wd4828"
    )

    qt_internal_extend_target(${target} CONDITION CLANG AND NOT QT_FEATURE_qt3d_system_assimp AND (NOT CMAKE_CROSSCOMPILING OR NOT host_build)
        COMPILE_OPTIONS
            "-Wno-unused-private-field"
    )
    # special case end

    qt_internal_extend_target(${target} CONDITION MSVC AND (CMAKE_CROSSCOMPILING OR NOT QT_FEATURE_qt3d_system_assimp) AND (host_build OR NOT QT_FEATURE_qt3d_system_assimp)
        DEFINES
            _CRT_SECURE_NO_WARNINGS
            _SCL_SECURE_NO_WARNINGS
        COMPILE_OPTIONS
            /bigobj
    )

    # Silence warnings in 3rdparty code
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        set_source_files_properties(${assimpDir}/src/code/AssetLib/Collada/ColladaParser.cpp PROPERTIES COMPILE_FLAGS "-Wno-strict-aliasing")
        set_source_files_properties(${assimpDir}/src/code/Common/ZipArchiveIOSystem.cpp PROPERTIES COMPILE_FLAGS "-Wno-cast-function-type")
    endif()
    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        set_source_files_properties(${assimpDir}/src/code/Common/Assimp.cpp PROPERTIES COMPILE_FLAGS "-Wno-ordered-compare-function-pointers")
    endif()

endfunction()
