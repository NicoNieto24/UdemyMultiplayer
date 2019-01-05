// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FPSAIGuard.generated.h"

class UPawnSensingComponent;

UENUM(BlueprintType)
enum class EAIState : uint8
{
	Idle,
	Suspicious,
	Alerted
};

UCLASS()
class FPSGAME_API AFPSAIGuard : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AFPSAIGuard();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UPawnSensingComponent* PawnSensingComp;

	FRotator OriginalRotation;

	FTimerHandle TimerHandle_ResetOrientation;

	EAIState GuardState;

	UPROPERTY(EditInstanceOnly, Category = "AI")
	uint8 bPatrol : 1;

	UPROPERTY(EditInstanceOnly, Category = "AI", meta = (EditCondition = bPatrol))
	TArray<AActor*> PatrolPoints;

	AActor* CurrentPatrolPoint;

	int32 CurrentPatrolIndex;

	UFUNCTION()
	void OnSeenPawn(APawn* SeenPawn);

	UFUNCTION()
	void OnNoiseHeard(APawn* NoiseInstigator, const FVector& Location, float Volume);
	
	UFUNCTION()
	void ResetOrientation();

	UFUNCTION()
	void SetGuardState(EAIState NewState);

	UFUNCTION(BlueprintImplementableEvent, Category = "AI")
	void OnStateChange(EAIState NewState);

	void MoveToNextPatrolPoint();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
