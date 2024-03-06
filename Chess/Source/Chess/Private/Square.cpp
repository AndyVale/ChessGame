// Fill out your copyright notice in the Description page of Project Settings.


#include "Square.h"

// Sets default values
ASquare::ASquare()
{
	PrimaryActorTick.bCanEverTick = false;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SquareMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	SetRootComponent(Scene);
	SquareMesh->SetupAttachment(Scene);
}

// Called to set Square color
void ASquare::setColor(bool c)
{
	if (c)
		SquareMesh->SetMaterial(0, WhiteMaterial);
	else
		SquareMesh->SetMaterial(0, BlackMaterial);
}

// Called when the game starts or when spawned
void ASquare::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ASquare::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

