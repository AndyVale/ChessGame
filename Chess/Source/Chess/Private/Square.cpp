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
	PieceOn = nullptr;
}

void ASquare::SetSquareColor(bool c)
{
	c ? SquareMesh->SetMaterial(0, WhiteMaterial) : SquareMesh->SetMaterial(0, BlackMaterial);
	bIsSelected = false;
	bIsInReplay = false;
}

void ASquare::SetAsSelected()
{
	SquareMesh->SetMaterial(0, SelectedMaterial);
	bIsSelected = true;
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

void ASquare::SetDangerColor()
{
	SquareMesh->SetMaterial(0, DangerMaterial);
}

bool ASquare::IsDanger()
{
	return (SquareMesh->GetMaterial(0) == DangerMaterial);
}

void ASquare::InReplay()
{
	SquareMesh->SetMaterial(0, ReplayMaterial);
	bIsInReplay = true;
}

/* Called every frame
void ASquare::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
*/
