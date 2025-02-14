#include "Foe/Foe.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Foe/Missile.h"

// Sets default values
AFoe::AFoe()
{
	PrimaryActorTick.bCanEverTick = true;

	Mesh1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh1"));
	Mesh1->SetupAttachment(RootComponent);
	Mesh2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh2"));
	Mesh2->SetupAttachment(Mesh1);
	
	SpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("SpawnPoint"));
	SpawnPoint->SetupAttachment(RootComponent);
}

void AFoe::BeginPlay()
{
	Super::BeginPlay();

	// Timer missiles
	GetWorldTimerManager().SetTimer(MissileSpawnTimer, this, &AFoe::SpawnMissile, MissileSpawnRate, true);
}

// Called every frame
void AFoe::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FRotator NewRotation = Mesh1->GetRelativeRotation();
	NewRotation.Yaw += RotationSpeed * DeltaTime;
	Mesh1->SetRelativeRotation(NewRotation);
}

void AFoe::SpawnMissile()
{
	if (!MissileClass || !GetWorld()) return;

	FVector SpawnLocation = SpawnPoint->GetComponentLocation();
	FRotator SpawnRotation = SpawnPoint->GetComponentRotation();

	GetWorld()->SpawnActor<AMissile>(MissileClass, SpawnLocation, SpawnRotation);
}
