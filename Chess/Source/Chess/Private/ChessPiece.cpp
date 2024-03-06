// Fill out your copyright notice in the Description page of Project Settings.


#include "ChessPiece.h"

// Sets default values
AChessPiece::AChessPiece()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	ChessPieceMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	SetRootComponent(Scene);
	ChessPieceMesh->SetupAttachment(Scene);
}

void AChessPiece::SetColorAndMaterial(bool w)
{
	bIsWhite = w;
	if (w) {
		ChessPieceMesh->SetMaterial(0, WhiteMaterial);
	}
	else {
		ChessPieceMesh->SetMaterial(0, BlackMaterial);
	}
}

// Called when the game starts or when spawned
void AChessPiece::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AChessPiece::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

