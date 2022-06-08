// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <assimp/DefaultLogger.hpp>
#include <assimp/anim.h>

int main(int , char **)
{
  Assimp::Importer importer;
  Assimp::DefaultLogger::create("AssimpLog.txt", Assimp::Logger::VERBOSE);

  // SET THIS TO REMOVE POINTS AND LINES -> HAVE ONLY TRIANGLES
  importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_LINE|aiPrimitiveType_POINT);

  // Check for new version by setting animation morph channels.
  aiAnimation anim;
  anim.mNumMorphMeshChannels = 0;

  // Check for something that was set in a recent version
  aiMaterial aiM;
  (void)aiM.GetName();

  return 0;
}
