// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSAIGuard.h"
#include "Perception/PawnSensingComponent.h"
#include "DrawDebugHelpers.h"
#include "FPSGameMode.h"
#include "AI/Navigation/NavigationSystem.h"

// Sets default values
AFPSAIGuard::AFPSAIGuard()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));

	PawnSensingComp->OnSeePawn.AddDynamic(this, &AFPSAIGuard::OnSeenPawn);
	PawnSensingComp->OnHearNoise.AddDynamic(this, &AFPSAIGuard::OnNoiseHeard);

	GuardState = EAIState::Idle;
}

// Called when the game starts or when spawned
void AFPSAIGuard::BeginPlay()
{
	Super::BeginPlay();

	OriginalRotation = GetActorRotation();

	if (bPatrol)
		MoveToNextPatrolPoint();
}

void AFPSAIGuard::OnSeenPawn(APawn * SeenPawn)
{
	if (SeenPawn == nullptr)
		return;

	DrawDebugSphere(GetWorld(), SeenPawn->GetActorLocation(), 32.0f, 12, FColor::Red, false, 10.0f);

	AFPSGameMode* GM = Cast<AFPSGameMode>(GetWorld()->GetAuthGameMode());
	if (GM)
		GM->CompleteMission(SeenPawn, false);

	SetGuardState(EAIState::Alerted);

	// Stop your movement
	if (bPatrol)
	{
		AController* Controller = GetController();
		if (Controller)
			Controller->StopMovement();
	}
}

void AFPSAIGuard::OnNoiseHeard(APawn* NoiseInstigator, const FVector& Location, float Volume)
{
	if (GuardState == EAIState::Alerted)
		return;

	DrawDebugSphere(GetWorld(), Location, 32.0f, 12, FColor::Green, false, 10.0f);

	FVector Direction = Location - GetActorLocation();
	Direction.Normalize();

	FRotator NewLookAt = FRotationMatrix::MakeFromX(Direction).Rotator();
	NewLookAt.Pitch = 0.0f;
	NewLookAt.Roll = 0.0f;

	SetActorRotation(NewLookAt);

	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_ResetOrientation);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_ResetOrientation, this, &AFPSAIGuard::ResetOrientation, 3.0f);

	SetGuardState(EAIState::Suspicious);

	// Stop your movement
	if (bPatrol)
	{
		AController* Controller = GetController();
		if (Controller)
			Controller->StopMovement();
	}
}

void AFPSAIGuard::ResetOrientation()
{
	if (GuardState == EAIState::Alerted)
		return;

	SetActorRotation(OriginalRotation);
	SetGuardState(EAIState::Idle);

	// Start your movement
	if (bPatrol)
		MoveToNextPatrolPoint();
}

void AFPSAIGuard::SetGuardState(EAIState NewState)
{
	if (GuardState == NewState)
		return;

	GuardState = NewState;
	OnStateChange(GuardState);
}

void AFPSAIGuard::MoveToNextPatrolPoint()
{
	if (PatrolPoints.Num() == 0)
		return;

	if (CurrentPatrolPoint == nullptr || CurrentPatrolPoint == PatrolPoints[PatrolPoints.Num() - 1])
		CurrentPatrolIndex = 0;
	else
		CurrentPatrolIndex++;

	CurrentPatrolPoint = PatrolPoints[CurrentPatrolIndex];

	UNavigationSystem::SimpleMoveToActor(GetController(), CurrentPatrolPoint);
}

// Called every frame
void AFPSAIGuard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Check distance with the patrol point, to know if the guard are closer to change for the next patrol point.
	if (CurrentPatrolPoint)
	{
		FVector Delta = GetActorLocation() - CurrentPatrolPoint->GetActorLocation();
		float DistanceToGoal = Delta.Size();

		if (DistanceToGoal < 100)
			MoveToNextPatrolPoint();
	}
}

