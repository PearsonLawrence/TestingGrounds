// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "ActorPool.h"
#include "Tile.generated.h"

USTRUCT()
struct FSpawnPosition
{
	GENERATED_USTRUCT_BODY()
public:
	FVector Location;
	float Rotation;
	float Scale;
};

UCLASS()
class TESTINGGROUND_API ATile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	UPROPERTY(EditDefaultsOnly, Category = "Spawning")
	FVector MinSpawningExtent;
	UPROPERTY(EditDefaultsOnly, Category = "Spawning")
	FVector MaxSpawningExtent;
	UPROPERTY(EditDefaultsOnly, Category = "Navigation")
		FVector NavigationBoundsOffset;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	UFUNCTION(BlueprintCallable, Category = "Pool")
		void SetPool(UActorPool* Inpool);
	UFUNCTION(BlueprintCallable, Category = "Spawning")
		void PlaceActors(TSubclassOf<AActor> ToSpawn, int32 MinSpawn, int32 MaxSpawn, float Radius = 500, float MinScale = 1, float MaxScale = 1);


	UFUNCTION(BlueprintCallable, Category = "Spawning")
		void PlaceAIPawns(TSubclassOf<APawn> ToSpawn, int32 MinSpawn, int32 MaxSpawn, float Radius = 500);


private:

	TArray<FSpawnPosition> RandomSpawnPositions(int32 MinSpawn, int32 MaxSpawn, float Radius, float MinScale, float MaxScale);

	bool FindEmptyLocation(FVector& OutLocation,float Radius);

	template<class T>
	void RandomlyPlaceActors (TSubclassOf<T> ToSpawn, int32 MinSpawn, int32 MaxSpawn, float Radius = 500, float MinScale = 1, float MaxScale = 1);


	void PlaceActor(TSubclassOf<AActor> toSpawn, FSpawnPosition SpawnPosition);

	void PlaceActor(TSubclassOf<APawn> toSpawn, FSpawnPosition SpawnPosition);

	
	bool CanSpawnAtLocation(FVector Location, float Radius);

	

	UActorPool* Pool;
	void PositionNavMeshBoundsVolume();

	AActor* NavMeshBoundsVolume;
};

