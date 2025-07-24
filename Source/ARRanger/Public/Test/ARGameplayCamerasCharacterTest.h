// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "AttackData.h"
#include "GameFramework/Character.h"
#include "ARRangerCharacter.h"

#include "ARGameplayCamerasCharacterTest.generated.h"

class UAnimMontage;
class UCameraComponent;
class USkeletalMesh;
class USpringArmComponent;
class UInputAction;

struct FInputActionValue;

UCLASS()
class ARRANGER_API AARGameplayCamerasCharacterTest : public ACharacter
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* MouseLookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* LockOnAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* SwitchTargetRightAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* SwitchTargetLeftAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* PunchAction;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* KickAction;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* TransformAction;

public:

	AARGameplayCamerasCharacterTest();	

protected:

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:

	void Move(const FInputActionValue& Value);

	void Look(const FInputActionValue& Value);

private:
	bool bIsLockedOn;

	bool isAbleToSwitchTarget;

	float DefaultArmLength;

	float DashArmLength; 

	float ArmLengthInterpSpeed; 

	void ToggleLockOn();

	void SwitchTargetRight();

	void SwitchTargetLeft();

	void SwitchTarget(bool isPressedRight);

	AActor* FindNearestEnemy(AActor* IgnoreActor = nullptr);

	void Punch();

	void Kick();

	void PlayAttackMontage(const FAttackData& Attack);

	void AttackHit(const FAttackData& Attack);

	void Transform();

public:

	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoLook(float Yaw, float Pitch);

	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpStart();
	
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpEnd();

	UFUNCTION(BlueprintCallable)
	void PunchHitNotify();

	UFUNCTION(BlueprintCallable)
	void KickHitNotify();

	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool IsInterrupted);

	UPROPERTY(EditAnywhere, Category = "PlayerMesh")
	USkeletalMesh* AttractiveMesh;

	UPROPERTY(EditAnywhere, Category = "PlayerMesh")
	USkeletalMesh* RepulsiveMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	AActor* LockedOnTarget;

	UPROPERTY(EditAnywhere, Category = "LockOn")
	float maxLockOnDistance;

	UPROPERTY(BlueprintReadWrite)
	bool isDashed;

	float dashStartThreshold;

	float dashEndThreshold;    

	UPROPERTY(EditAnywhere, Category = "Attack")
	FAttackData PunchData;

	UPROPERTY(EditAnywhere, Category = "Attack")
	FAttackData KickData;

	UPROPERTY(BlueprintReadOnly)
	bool isAttacked;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gravity")
	EGravityType CurrentGravityType;

public:
	virtual void Tick(float DeltaTime) override;

};

