// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TestingGroundGameMode.h"
#include "InfinateTerrainGameMode.generated.h"

/**
 * 
 */
UCLASS()
class TESTINGGROUND_API AInfinateTerrainGameMode : public ATestingGroundGameMode
{
	GENERATED_BODY()
	
public:
	AInfinateTerrainGameMode();

	UFUNCTION(BlueprintCallable, Category = "BoundsPool")
		void PopulateBoundsVolumePool();

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pool")
		class UActorPool* NavMeshBoundsVolumePool;

private:

	    void AddToPool(class ANavMeshBoundsVolume* VolumeToAdd);
	
};
