// Fill out your copyright notice in the Description page of Project Settings.


#include "Square.h"
#include "ChessPiece.h"

// Sets default values
ASquare::ASquare()
{
	PrimaryActorTick.bCanEverTick = false;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SquareMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	SetRootComponent(Scene);
	SquareMesh->SetupAttachment(Scene);
	bIsSelected = false;
	PieceOn = nullptr;
}

void ASquare::SetColor(bool c)
{
	if (c)
		SquareMesh->SetMaterial(0, WhiteMaterial);
	else
		SquareMesh->SetMaterial(0, BlackMaterial);
}

// Called to set Square color
void ASquare::ResetSelectedAndSetColor(bool c)
{
	bIsSelected = false;
	SetColor(c);
}

void ASquare::SetAsSelected(bool m)
{
	bIsSelected = true;
	if(m)
		SquareMesh->SetMaterial(0, SelectedMaterial);
}

void ASquare::setPiece(AChessPiece* p)
{
	PieceOn = p;
}

AChessPiece* ASquare::getPiece()
{
	return PieceOn;
}

bool ASquare::IsSelected()
{
	return bIsSelected;
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

