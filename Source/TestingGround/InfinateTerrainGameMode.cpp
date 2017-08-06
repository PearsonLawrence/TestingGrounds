// Fill out your copyright notice in the Description page of Project Settings.

#include "InfinateTerrainGameMode.h"
#include "AI/Navigation//NavMeshBoundsVolume.h"
#include "ActorPool.h"
#include "EngineUtils.h"


AInfinateTerrainGameMode::AInfinateTerrainGameMode()
{
	NavMeshBoundsVolumePool = CreateDefaultSubobject<UActorPool>(FName("NavMeshBoundsVolumePool"));
}

void AInfinateTerrainGameMode::PopulateBoundsVolumePool()
{
	auto VolumeIterator = TActorIterator<ANavMeshBoundsVolume>(GetWorld());

	while (VolumeIterator)
	{
		AddToPool(*VolumeIterator);
		++VolumeIterator;
	}
}

void AInfinateTerrainGameMode::AddToPool(ANavMeshBoundsVolume *VolumeToAdd)
{ 
	NavMeshBoundsVolumePool->Add(VolumeToAdd);
}
