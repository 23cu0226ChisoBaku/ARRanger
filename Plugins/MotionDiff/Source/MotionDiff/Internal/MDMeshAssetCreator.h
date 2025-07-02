#pragma once

#ifndef _MD_INTERNAL_MESH_ASSET_CREATOR_
#define _MD_INTERNAL_MESH_ASSET_CREATOR_

#include "ProceduralMeshComponent.h"
#include "MeshDescription.h"
#include "StaticMeshAttributes.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "UObject/SavePackage.h"

void CreateStaticMeshFromPMC(UProceduralMeshComponent* PMC, const FString& PackagePath, const FString& AssetName)
{
// 1. 创建 Asset Package
	FString FullPackageName = PackagePath / AssetName;
	UPackage* Package = CreatePackage(*FullPackageName);
	Package->FullyLoad();

	// 2. 创建 StaticMesh 对象
	UStaticMesh* NewMesh = NewObject<UStaticMesh>(Package, *AssetName, RF_Public | RF_Standalone);
	NewMesh->InitResources();

  if (NewMesh->GetNumSourceModels() == 0)
  {
    NewMesh->AddSourceModel();
  }
  
  FMeshDescription* MeshDescription = NewMesh->GetMeshDescription(0);
  if (MeshDescription == nullptr)
  {
    MeshDescription = NewMesh->CreateMeshDescription(0);
  }
	FStaticMeshAttributes Attributes(*MeshDescription);
	Attributes.Register();

	TArray<FVector> VertexBuffer;
	TArray<int32> IndexBuffer;

	// 3. 假设只处理 Section 0（你可以拓展到多个 Section）
	const FProcMeshSection* Section = PMC->GetProcMeshSection(0);
	if (!Section) return;

	TMap<int32, FVertexID> VertexIDMap;
	const TArray<FProcMeshVertex>& Vertices = Section->ProcVertexBuffer;

	// 4. 添加顶点
	for (int32 i = 0; i < Vertices.Num(); ++i)
	{
		const FVector& Pos = Vertices[i].Position;
		FVertexID VertexID = MeshDescription->CreateVertex();
		Attributes.GetVertexPositions()[VertexID] = FVector3f(Pos);
		VertexIDMap.Add(i, VertexID);
	}

	// 5. 创建 PolygonGroup
	FPolygonGroupID PolyGroupID = MeshDescription->CreatePolygonGroup();

	// 6. 添加三角形
	const TArray<uint32>& Indices = Section->ProcIndexBuffer;
	for (int32 i = 0; i < Indices.Num(); i += 3)
	{
		TArray<FVertexInstanceID> InstanceIDs;
		for (int j = 0; j < 3; ++j)
		{
			const int32 VertIndex = Indices[i + j];
			FVertexID VtxID = VertexIDMap[VertIndex];
			FVertexInstanceID InstID = MeshDescription->CreateVertexInstance(VtxID);

			// 设置 UV
			Attributes.GetVertexInstanceUVs()[InstID] = FVector2f(Vertices[VertIndex].UV0);

			// 设置法线
			Attributes.GetVertexInstanceNormals()[InstID] = FVector3f(Vertices[VertIndex].Normal);

			// 可选：设置切线、颜色等

			InstanceIDs.Add(InstID);
		}

		// 创建 Polygon（三角形）
		MeshDescription->CreatePolygon(PolyGroupID, InstanceIDs);
	}

	// 7. 绑定 MeshDescription 到 StaticMesh


  NewMesh->CommitMeshDescription(0);
	NewMesh->Build(false); // 不自动生成 collision
	NewMesh->MarkPackageDirty();

	// 8. 保存到磁盘
	FAssetRegistryModule::AssetCreated(NewMesh);
	Package->SetDirtyFlag(true);

  FSavePackageArgs saveArgs;
  saveArgs.TopLevelFlags = EObjectFlags::RF_Public | EObjectFlags::RF_Standalone;
  saveArgs.SaveFlags = SAVE_NoError;

	UPackage::SavePackage(Package, NewMesh, *FPackageName::LongPackageNameToFilename(FullPackageName, FPackageName::GetAssetPackageExtension()), saveArgs);

}

#endif