// Fill out your copyright notice in the Description page of Project Settings.


#include "Chessboard.h"

// Sets default values
AChessboard::AChessboard()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	BoardSize = 8;
	SquareSize = 100;
}

// Called when the game starts or when spawned
void AChessboard::BeginPlay()
{
	Super::BeginPlay();
	if (SquareClass != nullptr)
	{
		GenerateField();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("SquareClass non trovata in GameField"));
	}
}


void AChessboard::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	//NormalizedCellPadding = FMath::RoundToDouble(((TileSize + CellPadding) / TileSize) * 100) / 100;
}

AChessPiece* AChessboard::SpawnStarterPieceByXYPosition(const int32 InX, const int32 InY) const
{
	//true is intended as "whiteColor", false as "blackColor"
	FVector Location = AChessboard::GetRelativeLocationByXYPosition(InX, InY) + FVector(0,0,20);
	FRotator Rotation = FRotator(0, 180, 0);
	AChessPiece* Piece = nullptr;

	if (InY == 1 || InY == 6)
	{
		Piece = GetWorld()->SpawnActor<AChessPiece>(Pawn, Location, Rotation);
		if(Piece)
			Piece->SetColorAndMaterial(InY == 1);//White = True and white pieces are in Y=1
	}
	else if (InY == 0 || InY == 7)
	{
			switch (InX)
			{
			case 0:
			case 7:
				Piece = GetWorld()->SpawnActor<AChessPiece>(Rook, Location, Rotation);
				break;
			case 1:
			case 6:
				Piece = GetWorld()->SpawnActor<AChessPiece>(Knight, Location, Rotation);
				break;
			case 2:
			case 5:
				Piece = GetWorld()->SpawnActor<AChessPiece>(Bishop, Location, Rotation);
				break;
			case 4:
				Piece = GetWorld()->SpawnActor<AChessPiece>(Queen, Location, Rotation);
				break;
			case 3:
				Piece = GetWorld()->SpawnActor<AChessPiece>(King, Location, Rotation);
				break;
			}
			if (Piece)
				Piece->SetColorAndMaterial(InY == 0);//White = True and white pieces are in Y=0
	}
	
	return Piece;
}

void AChessboard::GenerateField()
{
	for (int32 y = 0; y < BoardSize; y++)
	{
		for (int32 x = 0; x < BoardSize; x++)
		{
			FVector Location = AChessboard::GetRelativeLocationByXYPosition(x, y);
			ASquare* Obj = GetWorld()->SpawnActor<ASquare>(SquareClass, Location, FRotator::ZeroRotator);
			AChessPiece* Pce = SpawnStarterPieceByXYPosition(x, y);
			if ((x + y) % 2 == 0)
			{
				Obj->setColor(true);
			}
			else
			{
				Obj->setColor(false);
			}
			const float TileScale = SquareSize / 100;
			Obj->SetActorScale3D(FVector(TileScale, TileScale, 0.2));
			SquareMap.Add(FVector2D(x, y), Obj);//TODO:SquareMap mi serve davvero?meeh
			PieceMap.Add(FVector2D(x, y), Pce);
		}
	}
}

/*
FVector2D AChessboard::GetPosition(const FHitResult& Hit)
{
	return FVector2D();
}

TArray<ASquare*>& AChessboard::GetTileArray()
{
	return TileArray;
}
*/
FVector AChessboard::GetRelativeLocationByXYPosition(const int32 InX, const int32 InY) const
{
	return SquareSize * FVector(InX, InY, 0);
}

FVector2D AChessboard::GetXYPositionByRelativeLocation(const FVector& Location) const
{
	const double x = Location[0] / SquareSize;
	const double y = Location[1] / SquareSize;
	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("x=%f,y=%f"), x, y));
	return FVector2D(x, y);
}



