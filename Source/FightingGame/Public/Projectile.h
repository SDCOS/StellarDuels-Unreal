// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"


class UProjectileMovementComponent;
class USphereComponent;

UCLASS(Blueprintable)
class FIGHTINGGAME_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override; //so all clients can see the projectiles

	// Sphere component for collision
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* CollisionComponent;

	// Sphere mesh for rendering
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components" /*, Replicated*/)
	UStaticMeshComponent* SphereMesh;

	// Movement component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components" /*, Replicated*/)
	UProjectileMovementComponent* ProjectileMovementComponent;

	// Damage value
	UPROPERTY(EditAnywhere, Category = "Damage")
	float Damage = 20.0f;

	// Function to handle collisions
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
