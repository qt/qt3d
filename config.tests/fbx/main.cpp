// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include <fbxsdk.h>

int main(int, char **)
{
    FbxManager *manager = FbxManager::Create();
    FbxScene::Create(manager, "scene");
    manager->Destroy();

    return 0;
}

