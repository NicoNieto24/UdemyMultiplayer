// Fill out your copyright notice in the Description page of Project Settings.

#include "BP_BlackHole.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"

// Sets default values
ABP_BlackHole::ABP_BlackHole()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = MeshComp;

	InnerSphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("InnerSphereComp"));
	InnerSphereComponent->SetSphereRadius(100);
	InnerSphereComponent->SetupAttachment(MeshComp);

	InnerSphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ABP_BlackHole::OverlapInnerSphere);

	OutterSphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("OutterSphereComp"));
	OutterSphereComponent->SetSphereRadius(3000);
	OutterSphereComponent->SetupAttachment(MeshComp);
}

// Called when the game starts or when spawned
void ABP_BlackHole::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABP_BlackHole::OverlapInnerSphere(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor)
		OtherActor->Destroy();
}

// Called every frame
void ABP_BlackHole::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TArray<UPrimitiveComponent*> OverlappingComps;
	OutterSphereComponent->GetOverlappingComponents(OverlappingComps);

	for (int32 i = 0; i < OverlappingComps.Num(); i++)
	{
		UPrimitiveComponent* PrimComp = OverlappingComps[i];

		if (PrimComp && PrimComp->IsSimulatingPhysics())
		{
			const float SphereRadius = OutterSphereComponent->GetScaledSphereRadius();
			const float ForceStrenght = -2000.0f;

			PrimComp->AddRadialForce(GetActorLocation(), SphereRadius, ForceStrenght, ERadialImpulseFalloff::RIF_Constant, true);
		}
	}
}

