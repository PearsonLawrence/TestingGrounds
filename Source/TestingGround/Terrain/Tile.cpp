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
		return;
	}
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


template<class T>
void ATile::RandomlyPlaceActors(TSubclassOf<T> ToSpawn, int32 MinSpawn, int32 MaxSpawn, float Radius, float MinScale, float MaxScale)
{

	int32 NumberToSpawn = FMath::RandRange(MinSpawn, MaxSpawn);
	for (size_t i = 0; i < NumberToSpawn; i++)
	{
		FSpawnPosition SpawnPosition;
		SpawnPosition.Scale = FMath::RandRange(MinScale, MaxScale);
		bool found = FindEmptyLocation(SpawnPosition.Location, Radius * SpawnPosition.Scale);
		if (found)
		{
			SpawnPosition.Rotation = FMath::RandRange(-180.0f, 180.0f);
			PlaceActor(ToSpawn, SpawnPosition);
		}

	}
}

void ATile::PlaceActors(TSubclassOf<AActor> ToSpawn, int32 MinSpawn, int32 MaxSpawn, float Radius, float MinScale, float MaxScale)
{

	RandomlyPlaceActors(ToSpawn, MinSpawn, MaxSpawn, Radius, MinScale, MaxScale);
}

void ATile::PlaceAIPawns(TSubclassOf<APawn> ToSpawn, int32 MinSpawn, int32 MaxSpawn, float Radius)
{
	RandomlyPlaceActors(ToSpawn, MinSpawn, MaxSpawn, Radius, 1, 1);

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

void ATile::PlaceActor(TSubclassOf<AActor> toSpawn, FSpawnPosition SpawnPosition) // AActor
{

	AActor*  Spawned = GetWorld()->SpawnActor<AActor>(toSpawn);
	if (Spawned == nullptr)
	{
		return;
	}
	Spawned->SetActorRelativeLocation(SpawnPosition.Location);

	Spawned->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));

	Spawned->SetActorRotation(FRotator(0, SpawnPosition.Rotation, 0));

	Spawned->SetActorScale3D(FVector(SpawnPosition.Scale));
}

void ATile::PlaceActor(TSubclassOf<APawn> toSpawn, FSpawnPosition SpawnPosition) // APawn
{
	APawn*  Spawned = GetWorld()->SpawnActor<APawn>(toSpawn);
	if (Spawned == nullptr)
	{
		return;
	}
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

