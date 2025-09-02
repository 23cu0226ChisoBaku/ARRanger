// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class FUECollectionsLibrary
{
  public:
  ///
  ///	@brief コレクションコンテナをランダムシャッフルする
  /// @param outCollection Unreal Engine コレクションタイプ
  ///
  template<typename UECollectionType>
  static void Shuffle(UECollectionType& outCollection);

  public:
    FUECollectionsLibrary() = delete;
    ~FUECollectionsLibrary() = delete;
};

template<typename UECollectionType>
void FUECollectionsLibrary::Shuffle(UECollectionType& OutCollection)
{
  const int32 collectionLastIndex = OutCollection.Num() - 1;
  for (int32 i = 0; i <= collectionLastIndex; ++i)
  {
    int32 index = FMath::RandRange(i, collectionLastIndex);
    if (i != index)
    {
      OutCollection.Swap(i, index);
    }
  }
}
