// Fill out your copyright notice in the Description page of Project Settings.

#include "TankPlayerController.h"
#include "TankAimingComponent.h"
#include "BattleTank.h"
#include "Engine/World.h"
#include "Tank.h"

void ATankPlayerController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);
	if (InPawn)
	{
		auto PossessedTank = Cast<ATank>(InPawn);
		if (!ensure(PossessedTank)) { return; }


		PossessedTank->OnDeath.AddUniqueDynamic(this, &ATankPlayerController::OnPossessedTankDeath);
	}
}

void ATankPlayerController::OnPossessedTankDeath()
{
	StartSpectatingOnly();
}

void ATankPlayerController::BeginPlay()
{
	Super::BeginPlay();
	auto AimingComponent = GetPawn()->FindComponentByClass<UTankAimingComponent>();
	if (!ensure(AimingComponent)) { return; }
	FoundAimingComponent(AimingComponent);
}

void ATankPlayerController::Tick(float DeltaTime)
{
	Super::Tick( DeltaTime );
	AimTowardsCrosshair();
}

void ATankPlayerController::AimTowardsCrosshair()
{
	if (!GetPawn()) { return; }
	auto AimingComponent = GetPawn()->FindComponentByClass<UTankAimingComponent>();
	if (!ensure(AimingComponent)) { return; }

	FVector HitLocation; // out parameter
	bool bGotHitLocation = GetSightRayHitLocation(HitLocation);
	if (bGotHitLocation)  // Has "side-effect, is going to line trace
	{
		AimingComponent->AimAt(HitLocation);
	}
}

// Get world location through crosshair through crosshair, true if hits landscape
bool ATankPlayerController::GetSightRayHitLocation(FVector& HitLocation) const
{
	// Find the crosshair position in pixel coordinates
	int32 ViewportSizeX, ViewportSizeY;
	GetViewportSize(ViewportSizeX, ViewportSizeY);
	auto ScreenLocation = FVector2D(ViewportSizeX * CrosshairXLocation, ViewportSizeY * CrosshairYLocation);

	// De-project the screen postion
	FVector LookDirection;
	if (GetLookDirection(ScreenLocation, LookDirection))
	{
		// Line-trace along that direction
		return GetLookVectorHitLocation(LookDirection, HitLocation);
	}

	return false;
}

bool ATankPlayerController::GetLookVectorHitLocation(FVector LookDirection, FVector& HitLocation) const
{
	FHitResult HitResult;
	auto StartLocation = PlayerCameraManager->GetCameraLocation();
	auto EndLocation = StartLocation + (LookDirection * LineTraceRange);
	if (GetWorld()->LineTraceSingleByChannel(
				HitResult,
				StartLocation,
				EndLocation,
				ECollisionChannel::ECC_Visibility)				
		)
	{
		HitLocation = HitResult.Location;
		return true;
	}
	HitLocation = FVector(0);
	return false;
}

bool ATankPlayerController::GetLookDirection(FVector2D ScreenLocation, FVector& LookDirection) const
{
	FVector CameraWorldLocation;
	return DeprojectScreenPositionToWorld(
		ScreenLocation.X,
		ScreenLocation.Y,
		CameraWorldLocation,
		LookDirection
	);
}


