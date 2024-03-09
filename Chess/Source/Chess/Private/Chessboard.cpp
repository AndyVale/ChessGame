// Fill out your copyright notice in the Description page of Project Settings.


#include "Chessboard.h"
#include "ChessPiece.h"

// Sets default values
AChessboard::AChessboard()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	BoardSize = 8;
	SquareSize = 100;
}

FVector2D* AChessboard::GetXYFormPiece(AChessPiece* p)
{
	if (p)
	{
		switch (p->PieceColor)
		{
		case WHITE:
			if (WhitePieces.Find(p))
				return WhitePieces.Find(p);
			break;
		case BLACK:
			if (BlackPieces.Find(p))
				return BlackPieces.Find(p);
			break;
		case NAC:
			break;
		}
	}
	UE_LOG(LogTemp, Error, TEXT("GetXYFormPiece: Piece not found"));
	return nullptr;
}

AChessPiece* AChessboard::GetPieceFromXY(FVector2D xy)
{
	if (XY_Square.Find(xy) && (*XY_Square.Find(xy)))
		return (*XY_Square.Find(xy))->getPiece();
	UE_LOG(LogTemp, Error, TEXT("GetPieceFromXY: XY is not a valid position"));
	return nullptr;
}

bool AChessboard::SetPieceFromXY(FVector2D xy, AChessPiece* p)
{
	//Remove old xy
	if (XY_Square.Find(xy) && (*XY_Square.Find(xy)))
	{
		if (p) {
			switch (p->PieceColor)
			{
			case WHITE:
				WhitePieces.Remove(p);
				WhitePieces.Add(p, xy);
				break;
			case BLACK:
				BlackPieces.Remove(p);
				BlackPieces.Add(p, xy);
				break;
			case NAC:
				UE_LOG(LogTemp, Error, TEXT("SetPieceFromXY:Color of piece NAC"));
				return false;
				break;
			}
		}
		(*XY_Square.Find(xy))->setPiece(p);
		return true;
	}
	return false;
}

ChessColor AChessboard::GetPieceColorFromXY(FVector2D xy)
{
	if (XY_Square.Find(xy) && (*XY_Square.Find(xy)) && (*XY_Square.Find(xy))->getPiece())
	{
		return  (*XY_Square.Find(xy))->getPiece()->PieceColor;
	}
	return NAC;
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

void AChessboard::ShowFeasibleMoves(AChessPiece* Piece)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("ShowFeasibleMoves"));
	FVector2D* xy = GetXYFormPiece(Piece);
	//TODO: Check control (you must protect the king in this situation)
	TArray<FVector2D> moves;
	if (xy)
	{
		moves = Piece->GetFeasibleMoves(xy, this);
		moves.Add(*xy); //Adding clicked square to gameplay 
	}
	//TODO: Check foreach moves if make the 'same-color' King under attack
	for (auto pos : moves)
	{
		if ((*XY_Square.Find(pos))) {
			(*XY_Square.Find(pos))->SetAsSelected(true);
		}
	}
}

void AChessboard::CancelFeasibleMoves()
{
	for (int32 y = 0; y < BoardSize; y++)
	{
		for (int32 x = 0; x < BoardSize; x++)
		{
			if ((*XY_Square.Find(FVector2D(x, y)))->IsSelected())
				(*XY_Square.Find(FVector2D(x, y)))->ResetSelectedAndSetColor((x + y) % 2 == 0);
		}
	}
}

void AChessboard::RestoreBoardColors()
{
	for (int32 y = 0; y < BoardSize; y++)
	{
		for (int32 x = 0; x < BoardSize; x++)
		{
			(*XY_Square.Find(FVector2D(x, y)))->ResetSelectedAndSetColor((x + y) % 2 == 0);
		}
	}
}

void AChessboard::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	//NormalizedCellPadding = FMath::RoundToDouble(((TileSize + CellPadding) / TileSize) * 100) / 100;
}

AChessPiece* AChessboard::SpawnStarterPieceByXYPosition(const int32 InX, const int32 InY)
{
	//true is intended as "whiteColor", false as "blackColor"
	FVector Location = AChessboard::GetRelativeLocationByXYPosition(InX, InY) + FVector(0, 0, 20);
	FRotator Rotation = FRotator(0, 180, 0);
	AChessPiece* Piece = nullptr;

	if (InY == 1 || InY == 6)
	{
		Piece = GetWorld()->SpawnActor<AChessPiece>(Pawn, Location, Rotation);
		if (Piece)
			if (InY == 1)
				Piece->SetColorAndMaterial(WHITE);//WHITE pieces are in Y=1
			else
				Piece->SetColorAndMaterial(BLACK);//BLACK pieces are in Y=6
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
			if (InY == 0)
				WhiteKing = Piece;
			else
				BlackKing = Piece;//BLACK pieces are in Y=6
			break;
		default:
			Piece = nullptr;
		}
		if (Piece)
			if (InY == 0)
				Piece->SetColorAndMaterial(WHITE);//WHITE pieces are in Y=1
			else
				Piece->SetColorAndMaterial(BLACK);//BLACK pieces are in Y=6
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
			ASquare* Sqr = GetWorld()->SpawnActor<ASquare>(SquareClass, Location, FRotator::ZeroRotator);
			AChessPiece* Pce = SpawnStarterPieceByXYPosition(x, y);
			if ((x + y) % 2 == 0)
			{
				Sqr->SetColor(true);
			}
			else
			{
				Sqr->SetColor(false);
			}
			const float TileScale = SquareSize / 100;
			Sqr->SetActorScale3D(FVector(TileScale, TileScale, 0.2));
			FVector2D pos = FVector2D(x, y);
			Sqr->setPiece(Pce);
			XY_Square.Add(pos, Sqr);
			if (Pce != nullptr) {
				if (Pce->PieceColor == WHITE)
					WhitePieces.Add(Pce, FVector2D(x, y));
				if (Pce->PieceColor == BLACK)
					BlackPieces.Add(Pce, FVector2D(x, y));
			}
		}
	}
}


FVector2D AChessboard::GetPositionFromHit(const FHitResult& Hit)
{
	return FVector2D();
}

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

void AChessboard::MakeAMove(FVector2D oldPosition, FVector2D newPosition)//TODO: NON FUNZIONA LA MANGIATA E OGNI SPESSO SI BUGGA
{
	ASquare** OldSquare = XY_Square.Find(oldPosition);//Using a ** to avoid nullpointer exception
	ASquare** NewSquare = XY_Square.Find(newPosition);

	if (!OldSquare || !*OldSquare || !NewSquare || !*NewSquare)
	{
		UE_LOG(LogTemp, Error, TEXT("MakeAMove:Impossible move, square missingn"));
		return;
	}

	AChessPiece* OldPiece = GetPieceFromXY(oldPosition);
	if (!OldPiece)
	{
		UE_LOG(LogTemp, Error, TEXT("MakeAMove:Impossible move, no pieces in old position"));
		return;
	}

	AChessPiece* NewPiece = GetPieceFromXY(newPosition);
	if (NewPiece)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Eat"));
		//Note: IS important to destroy and setlocation BEFORE changin the maps. Because of the change of the keys of the maps.
		//In the others if this is not a problem because there is not a newPiece
		if (NewPiece->PieceColor == WHITE)
		{
			WhitePieces.Remove(NewPiece);
		}
		else {
			BlackPieces.Remove(NewPiece);
		}
		(NewPiece)->Destroy();
		(OldPiece)->SetActorLocation(GetRelativeLocationByXYPosition(newPosition[0], newPosition[1]));
		SetPieceFromXY(oldPosition, nullptr);
		SetPieceFromXY(newPosition, OldPiece);
		return;
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Move"));
		SetPieceFromXY(oldPosition, nullptr);
		SetPieceFromXY(newPosition, OldPiece);
		//Piece_XY.Add(OldPiece, newPosition);
		(OldPiece)->SetActorLocation(GetRelativeLocationByXYPosition(newPosition[0], newPosition[1]));
		return;
	}
}

bool AChessboard::CheckControl(ChessColor C)
{
	AChessPiece* ActualKing;
	FVector2D KingPosition;
	TMap<AChessPiece*, FVector2D> ActualEnemyPices;
	TArray<FVector2D> SpottedSquares;
	if (C == WHITE)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("CONTROL WHITE!"));
		ActualKing = WhiteKing;
		ActualEnemyPices = BlackPieces;
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("CONTROL BLACK!"));
		ActualKing = BlackKing;
		ActualEnemyPices = WhitePieces;
	}
	if (!ActualKing) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("KING DIED"));
		return true;
	}
	KingPosition = *GetXYFormPiece(ActualKing);
	for (auto& Piece_XY : ActualEnemyPices)
	{
		if (Piece_XY.Key->GetFeasibleMoves(&Piece_XY.Value, this).Contains(KingPosition))
		{
			if (C == WHITE)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("CHECK FOR WHITE!"));
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("CHECK FOR BLACK!"));
			}
			(*XY_Square.Find(KingPosition))->SetDangerColor();
			return true;
		}
	}
	int tmp = KingPosition[0] + KingPosition[1];//tmp variable for casting purpose
	(*XY_Square.Find(KingPosition))->SetColor((tmp % 2) == 0);
	return false;
}

bool AChessboard::MakeASafeMove(FVector2D oldPosition, FVector2D newPosition)
{
	if (XY_Square.Find(newPosition) && (*XY_Square.Find(newPosition))->IsSelected())
	{
		if (GetPieceFromXY(newPosition) != GetPieceFromXY(oldPosition))
		{
			MakeAMove(oldPosition, newPosition);
			return true;
		}
	}
	return false;
}
