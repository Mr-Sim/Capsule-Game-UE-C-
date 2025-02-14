#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Missile.generated.h"

class UStaticMeshComponent;
class UNiagaraComponent;
class UNiagaraSystem;
class USphereComponent;
class ACharacter;

UCLASS()
class RETRY_API AMissile : public AActor
{
	GENERATED_BODY()
	
public:	
	AMissile();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* MissileMesh;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UNiagaraComponent* TrailEffect;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USphereComponent* CollisionComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UNiagaraSystem* ExplosionEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float Speed = 900.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float RotationSpeed = 0.5f;

	ACharacter* TargetPlayer;

	void TrackTarget(float DeltaTime);

	void Explode();

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
