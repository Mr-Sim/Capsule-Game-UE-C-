#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CapsuleCharacter.generated.h"

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
    virtual void Tick(float DeltaSeconds) override;
    virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
    

    // INPUT RESPONSES
    void Move(const FInputActionValue& Value);
    void Look(const FInputActionValue& Value);
    void ChargeJump();
    void ReleaseJump();
    void Stomp();
    void ChargeDash();
    void ReleaseDash();

public:
    // Camera management
    void SetFirstPerson();
    void SetThirdPerson();

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

    /** Input actions */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* JumpAction;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* MoveAction;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* LookAction;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* DashAction;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* StompAction;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* ChargedJumpAction;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* ChargedDashAction;

private:
    /** Movement variables */
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
    bool bIsFirstPerson;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
    float JumpChargePower;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
    float MaxJumpForce;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
    float BaseJumpForce;
    float JumpForce;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
    float DashChargePower;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
    float MaxDashForce;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
    float BaseDashForce;
    float DashForce;
};
