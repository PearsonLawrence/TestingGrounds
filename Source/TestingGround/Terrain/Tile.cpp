// Fill out your copyright notice in the Description page of Project Settings.

#include "Tile.h"
#include "Engine/World.h"
#include "Runtime/Engine/Classes/AI/Navigation/NavigationSystem.h"
#include "DrawDebugHelpers.h"
// Sets default values
ATile::ATile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	MinSpawningExtent = FVector(0, -1700, 0);
	MaxSpawningExtent = FVector(3000, 1700, 0);
	NavigationBoundsOffset = FVector(1500, 0, 0);
}

void ATile::SetPool(UActorPool * Inpool)
{
	//UE_LOG(LogTemp, Warning, TEXT("[%s] Setting Pool %s"), *(this->GetName()), *(Inpool->GetName()));
	Pool = Inpool;
	PositionNavMeshBoundsVolume();
}

void ATile::PositionNavMeshBoundsVolume()
{
	NavMeshBoundsVolume = Pool->Checkout();
	if (NavMeshBoundsVolume == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Not Enough Actors In Pool"));
		return;
	}
	UE_LOG(LogTemp, Error, TEXT("Checked Out"));
	NavMeshBoundsVolume->SetActorLocation(GetActorLocation() + NavigationBoundsOffset);
	GetWorld()->GetNavigationSystem()->Build();
}
// Called when the game starts or when spawned
void ATile::BeginPlay()
{
	Super::BeginPlay();


}

void ATile::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Pool->Return(NavMeshBoundsVolume);
	//UE_LOG(LogTemp, Warning, TEXT("[%s] End Play."), *GetName());
}

// Called every frame
void ATile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}

void ATile::PlaceActors(TSubclassOf<AActor> ToSpawn, int32 MinSpawn, int32 MaxSpawn, float Radius, float MinScale, float MaxScale)
{
	TArray<FSpawnPosition> SpawnPositons = RandomSpawnPositions(MinSpawn, MaxSpawn, Radius, MinScale, MaxScale);

	for (FSpawnPosition SpawnPosition : SpawnPositons)
	{
		PlaceActor(ToSpawn, SpawnPosition);
	
	}
}

void ATile::PlaceAIPawns(TSubclassOf<APawn> ToSpawn, int32 MinSpawn, int32 MaxSpawn, float Radius)
{
	TArray<FSpawnPosition> SpawnPositons = RandomSpawnPositions(MinSpawn, MaxSpawn, Radius, 1, 1);

	for (FSpawnPosition SpawnPosition : SpawnPositons)
	{
		PlaceAIPawn(ToSpawn, SpawnPosition);

	}

}


TArray<FSpawnPosition> ATile::RandomSpawnPositions(int32 MinSpawn, int32 MaxSpawn, float Radius, float MinScale, float MaxScale)
{
	TArray<FSpawnPosition> SpawnPositions;
	int32 NumberToSpawn = FMath::RandRange(MinSpawn, MaxSpawn);
	for (size_t i = 0; i < NumberToSpawn; i++)
	{
		FSpawnPosition SpawnPosition;
		SpawnPosition.Scale = FMath::RandRange(MinScale, MaxScale);
		bool found = FindEmptyLocation(SpawnPosition.Location, Radius * SpawnPosition.Scale);
		if (found)
		{
			SpawnPosition.Rotation = FMath::RandRange(-180.0f, 180.0f);
			SpawnPositions.Add(SpawnPosition);
		}

	}
	return SpawnPositions;
}

bool ATile::FindEmptyLocation(FVector& OutLocation, float Radius)
{
	FBox Bounds(MinSpawningExtent, MaxSpawningExtent);

	const int MAX_ATTEMPTS = 100;
	for (size_t i = 0; i < MAX_ATTEMPTS; i++)
	{
		 FVector CandidatePoint = FMath::RandPointInBox(Bounds);
		 if (CanSpawnAtLocation(CandidatePoint, Radius))
		 {
			 OutLocation = CandidatePoint;
			 return true;
		 }
	}
	return false;
}

void ATile::PlaceActor(TSubclassOf<AActor> toSpawn, FSpawnPosition SpawnPosition)
{

	AActor*  Spawned = GetWorld()->SpawnActor<AActor>(toSpawn);
	
	Spawned->SetActorRelativeLocation(SpawnPosition.Location);

	Spawned->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));

	Spawned->SetActorRotation(FRotator(0, SpawnPosition.Rotation, 0));

	Spawned->SetActorScale3D(FVector(SpawnPosition.Scale));
}

void ATile::PlaceAIPawn(TSubclassOf<AActor> toSpawn, FSpawnPosition SpawnPosition)
{
	APawn*  Spawned = GetWorld()->SpawnActor<APawn>(toSpawn);

	Spawned->SetActorRelativeLocation(SpawnPosition.Location);

	Spawned->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));

	Spawned->SetActorRotation(FRotator(0, SpawnPosition.Rotation, 0));

	Spawned->SpawnDefaultController();

	Spawned->Tags.Add(FName("Enemy"));
}

bool ATile::CanSpawnAtLocation(FVector Location, float Radius)
{

	FHitResult HitResult;
	FVector GlobalLocation = ActorToWorld().TransformPosition(Location);
    bool HasHit = GetWorld()->SweepSingleByChannel(
		HitResult,
		GlobalLocation,
		GlobalLocation,
		FQuat::Identity, 
		ECollisionChannel::ECC_GameTraceChannel2,
		FCollisionShape::MakeSphere(Radius)
	);
	FColor ResultColor = HasHit? FColor::Red : FColor::Green;

	//DrawDebugCapsule(GetWorld(), GlobalLocation, 0, Radius, FQuat::Identity, ResultColor, true, 100);
	return !HasHit;
}

