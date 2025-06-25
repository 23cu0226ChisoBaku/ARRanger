// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class MLIBRARY_API FUECollectionsLibrary
{
  private:
    FUECollectionsLibrary() = delete;
    ~FUECollectionsLibrary() = delete;

  public:
    ///
    ///	@brief コレクションコンテナをランダムシャッフルする
    /// @param outCollection Unreal Engine コレクションタイプ
    ///
    template<typename UECollectionType>
    static void Shuffle(UECollectionType& outCollection)
    {
      const int32 collectionLastIndex = outCollection.Num() - 1;
      for (int32 i = 0; i <= collectionLastIndex; ++i)
      {
        int32 index = FMath::RandRange(i, collectionLastIndex);
        if (i != index)
        {
          outCollection.Swap(i, index);
        }
      }
    }
};
