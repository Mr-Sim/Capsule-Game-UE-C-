#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Character.h"
#include "CapsuleCharacter.generated.h"

class UJumpComponent;
struct FTimeline;
class UDashComponent;
class USpringArmComponent;
class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;

UCLASS(config=Game)
class ACapsuleCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    ACapsuleCharacter();

protected:
    
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void Tick(float DeltaSeconds) override;
    virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
    

    // INPUT RESPONSES
    void Move(const FInputActionValue& Value);
    FVector2D GetCurrentMoveVector() const;
    void Look(const FInputActionValue& Value);
    void Stomp();

public:
    // Camera management
    void SetFirstPerson();
    void SetThirdPerson();
    void VelocityAnimation(float Force);
    
    /** Returns Mesh1P subobject **/
    USkeletalMeshComponent* GetMesh3P() const { return Mesh3P; }
    /** Returns FirstPersonCameraComponent subobject **/
    UCameraComponent* GetCameraComponent() const { return CameraComponent; }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Camera)
    float CameraLagSpeed;

protected:
    /** Mesh for third-person view */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Mesh, meta = (AllowPrivateAccess = "true"))
    USkeletalMeshComponent* Mesh3P;

    /** Camera components */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    UCameraComponent* CameraComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    USpringArmComponent* SpringArm;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
    UJumpComponent* JumpComponent;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Components, meta = (AllowPrivateAccess = "true"))
    UDashComponent* DashComponent;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* MoveAction;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* LookAction;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* StompAction;
    

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = State, meta = (AllowPrivateAccess = "true"))
    bool bIsFirstPerson;

    struct FEnhancedInputActionValueBinding* MoveActionBinding;
    

    // ANIMATION VARIABLES
    UPROPERTY(EditAnywhere)
    FTimeline FovTimeline;
    UPROPERTY(EditAnywhere)
    FTimeline SpringArmTimeline;
    UPROPERTY(EditAnywhere, Category = "Camera", meta = (AllowPrivateAccess = "true"))
    UCurveFloat* FovCurve;
    UPROPERTY(EditAnywhere, Category = "Camera", meta = (AllowPrivateAccess = "true"))
    UCurveFloat* SpringArmCurve;
    UPROPERTY(EditAnywhere, Category = "Camera", meta = (AllowPrivateAccess = "true"))
    float InitialFOV;
    UPROPERTY(EditAnywhere, Category = "Camera", meta = (AllowPrivateAccess = "true"))
    float TargetFov;
    UPROPERTY(EditAnywhere, Category = "Camera", meta = (AllowPrivateAccess = "true"))
    float InitialSpringArmTarget;
    UPROPERTY(EditAnywhere, Category = "Camera", meta = (AllowPrivateAccess = "true"))
    float TargetSpringArmTarget;

    UFUNCTION()
    void UpdateFov(float Value);
    UFUNCTION()
    void UpdateSpringArm(float Value);
    UFUNCTION()
    void ResetCamera();
    void CancelAllTimers();
};
