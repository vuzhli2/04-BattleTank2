// Copyright Umber Ltd.

#pragma once
#include "Components/ActorComponent.h"
#include "Runtime/Engine/Classes/Components/StaticMeshComponent.h"
#include "TankAimingComponent.generated.h"

//Enum
UENUM()
enum class EFiringState : uint8
{
	Reloading,
	Aiming, 
	Locked,
	OutOfAmmo
};

//Forward Declaration
class UTankBarrel;
class UTankTurret;
class AProjectile;

// Holds barrel's properties
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BATTLETANK_API UTankAimingComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Setup")
	void Initialise(UTankBarrel * BarrelToSet, UTankTurret * TurretToSet);

	void AimAt(FVector HitLocation);

	UFUNCTION(BlueprintCallable, Category = "Firing")
		void Fire();

	EFiringState GetFiringState() const;

	UFUNCTION(BlueprintCallable, Category = "Firing")
	int32 GetRoundsLeft() const;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "State")
	EFiringState FiringState = EFiringState::Reloading;

private:
	// Sets default values for this component's properties
	UTankAimingComponent();

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	void MoveBarrelTowards(FVector AimDirection);

	bool IsBarrelMoving();

	UTankBarrel * Barrel = nullptr;
	UTankTurret * Turret = nullptr;

	UPROPERTY(EditAnywhere, Category = "Firing")
		float LaunchSpeed = 4000;

	UPROPERTY(EditAnywhere, Category = "Setup")
		TSubclassOf<AProjectile> ProjectileBlueprint;

	UPROPERTY(EditAnywhere, Category = "Firing")
		float ReloadTimeInSeconds = 1;

	UPROPERTY(EditAnywhere, Category = "Firing")
		int32 RoundsLeft = 3;

	double LastFireTime = 0;

	FVector AimDirection;
};

