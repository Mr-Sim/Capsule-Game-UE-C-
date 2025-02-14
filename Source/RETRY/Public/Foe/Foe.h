#pragma once

#include "CoreMinimal.h"
#include "Missile.h"
#include "GameFramework/Actor.h"
#include "Foe.generated.h"

class UStaticMeshComponent;
class USceneComponent;
class AMissile;

UCLASS()
class RETRY_API AFoe : public AActor
{
	GENERATED_BODY()
	
public:	
	AFoe();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

private:
	// Mesh principal qui tourne
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* Mesh1;

	// Mesh enfant de Mesh1
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* Mesh2;

	// Point de spawn des missiles
	UPROPERTY(VisibleAnywhere, Category = "Components")
	USceneComponent* SpawnPoint;

	// Classe du missile à instancier
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Missile", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AMissile> MissileClass;

	// Intervalle de spawn des missiles
	UPROPERTY(EditDefaultsOnly, Category = "Missile")
	float MissileSpawnRate = 1.0f;

	// Timer pour le spawn des missiles
	FTimerHandle MissileSpawnTimer;
	
	// Rotation speed for Mesh1
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float RotationSpeed = 8.0f;

	// Fonction de spawn du missile
	void SpawnMissile();
};
