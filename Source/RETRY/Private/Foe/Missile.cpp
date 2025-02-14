#include "Foe/Missile.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "GameFramework/Character.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AMissile::AMissile()
{
	PrimaryActorTick.bCanEverTick = true;

	// Création de la collision
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	CollisionComponent->InitSphereRadius(30.0f);
	CollisionComponent->SetCollisionProfileName(TEXT("BlockAll"));
	RootComponent = CollisionComponent;

	// Création du Mesh
	MissileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MissileMesh"));
	MissileMesh->SetupAttachment(RootComponent);
	MissileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Création de la traînée
	TrailEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TrailEffect"));
	TrailEffect->SetupAttachment(MissileMesh);

	// Bind de la fonction de collision
	CollisionComponent->OnComponentHit.AddDynamic(this, &AMissile::OnHit);
}

// Called when the game starts or when spawned
void AMissile::BeginPlay()
{
	Super::BeginPlay();

	// Trouver le joueur cible
	TargetPlayer = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
}

// Called every frame
void AMissile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (TargetPlayer)
	{
		TrackTarget(DeltaTime);
	}

	// Avancer en ligne droite
	FVector Forward = GetActorForwardVector();
	SetActorLocation(GetActorLocation() + Forward * Speed * DeltaTime);
}

// Ajuste la direction vers la cible avec une rotation fluide
void AMissile::TrackTarget(float DeltaTime)
{
	if (!TargetPlayer) return;

	FVector TargetDirection = (TargetPlayer->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	FRotator CurrentRotation = GetActorRotation();
	FRotator TargetRotation = TargetDirection.Rotation();

	// Interpolation de la rotation
	FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, RotationSpeed);
	SetActorRotation(NewRotation);
}

// Gère l'explosion du missile
void AMissile::Explode()
{
	if (ExplosionEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());
	}

	Destroy();
}

void AMissile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Jouer l'effet d'explosion s'il existe
	if (ExplosionEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());
	}

	// Détruire le missile dans tous les cas
	Destroy();
}
