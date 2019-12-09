// Fill out your copyright notice in the Description page of Project Settings.
#include "Boid.h"

//using this so we can init the static mesh at the right time
void ABoid::Init(UStaticMesh* bodyRef, TArray<AActor*> a_boids, AActor* a_target, float a_avoidWeight, float a_seperationWeight, 
					float a_cohesionWeight, float a_alignmentWeight, float a_targetWeight, float a_maxSpeed, float a_minSpeed, 
					float a_maxForce, float a_boundsRadius, float a_viewRadius, float a_avoidRadius, float a_collisionCheckDistance, int a_numViewDirections, TEnumAsByte<ETraceTypeQuery> a_traceChannel, TArray<FVector> a_points)
{
	//set body
	body->SetStaticMesh(bodyRef);

	//copy params
	boids = a_boids;

	AActor* target = a_target;

	TEnumAsByte<ETraceTypeQuery> traceChannel = a_traceChannel;

	target = a_target;

	avoidWeight = a_avoidWeight;

	seperationWeight = a_seperationWeight;

	cohesionWeight = a_cohesionWeight;

	alignmentWeight = a_alignmentWeight;

	targetWeight = a_targetWeight;

	maxSpeed = a_maxSpeed;

	minSpeed = a_minSpeed;

	maxForce = a_maxForce;

	boundsRadius = a_boundsRadius;

	viewRadius = a_viewRadius;

	avoidRadius = a_avoidRadius;

	collisionCheckDistance = a_collisionCheckDistance;

	numViewDirections = a_numViewDirections;

	points = a_points;

	//setup
	position = GetActorLocation();

	FQuat newRotation;

	SetActorRotation(newRotation.MakeFromEuler(FVector(FMath::RandRange(0, 360), FMath::RandRange(0, 360), FMath::RandRange(0, 360))));

	direction = GetActorForwardVector();

	velocity = direction * maxSpeed;
}

// Sets default values
ABoid::ABoid()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//setup body
	body = CreateDefaultSubobject<UStaticMeshComponent>("Body");

	//set body as root
	RootComponent = body;

	numPerceivedFlockmates = 0;
}

// Called when the game starts or when spawned
void ABoid::BeginPlay()
{
	Super::BeginPlay();
}

void ABoid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetActorLocation(position);
	SetActorRotation(UKismetMathLibrary::FindLookAtRotation(position, position + direction));
}

bool ABoid::IsCloseToObject()
{
	//hit result
	FHitResult hit;

	TArray<AActor*> empty;

	if (UKismetSystemLibrary::SphereTraceSingle((UObject*)GetWorld(), position, position + direction * collisionCheckDistance, boundsRadius, traceChannel, false, boids, EDrawDebugTrace::None, hit, true))
	{
		return true;
	}

	return false;
}

FVector ABoid::GetAvoidDir()
{
	for (int i = 0; i < points.Num(); i++)
	{
		//rotate the point torwards forward
		FTransform newTransform = GetTransform();
		FVector viewDirection = UKismetMathLibrary::TransformDirection(newTransform, points[i]);

		FHitResult hit;

		TArray<AActor*> empty; 

		if (!UKismetSystemLibrary::SphereTraceSingle((UObject*)GetWorld(), position, position + viewDirection * collisionCheckDistance, boundsRadius, traceChannel, false, boids, EDrawDebugTrace::None, hit, true))
		{
			return viewDirection;
		}
	}

	return direction;
}

FVector ABoid::GetForceToDirection(FVector a_direction)
{
	FVector direction = (a_direction.GetSafeNormal() * maxSpeed) - velocity;
	return direction.GetClampedToMaxSize(maxForce);
}




