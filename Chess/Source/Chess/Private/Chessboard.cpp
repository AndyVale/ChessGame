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

TMap<AChessPiece*, FVector2D> AChessboard::GetPieces(ChessColor C)
{
	switch (C)
	{
	case WHITE:
		return WhitePieces;
	case BLACK:
		return BlackPieces;
	}
	return TMap<AChessPiece*, FVector2D>();
}

FVector2D* AChessboard::GetXYFromPiece(AChessPiece* p)
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
	if (GetSquareFromXY(xy))
		return GetSquareFromXY(xy)->getPiece();
	UE_LOG(LogTemp, Error, TEXT("GetPieceFromXY: XY is not a valid position"));
	return nullptr;
}

ASquare* AChessboard::GetSquareFromXY(FVector2D xy)
{
	if(XY_Square.Find(xy))
		return *XY_Square.Find(xy);
	return nullptr;
}

bool AChessboard::SetPieceFromXY(FVector2D xy, AChessPiece* p)
{//Before calling this method make shure that p and xy are not already in a "pair xy-piece"
	if (GetSquareFromXY(xy))
	{
		if (p) {
			switch (p->PieceColor)
			{
			case WHITE:
				WhitePieces.Add(p, xy);
				break;
			case BLACK:
				BlackPieces.Add(p, xy);
				break;
			case NAC:
				UE_LOG(LogTemp, Error, TEXT("SetPieceFromXY:Color of piece NAC"));
				return false;
				break;
			}
		}
		GetSquareFromXY(xy)->setPiece(p);
		return true;
	}
	return false;
}

ChessColor AChessboard::GetPieceColorFromXY(FVector2D xy)
{
	if (GetPieceFromXY(xy))
	{
		return  GetPieceFromXY(xy)->PieceColor;
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

TArray<FVector2D> AChessboard::GetFeasibleMoves(AChessPiece* Piece, bool showMoves)
{
	FVector2D* xy = GetXYFromPiece(Piece);
	TArray<FVector2D> moves;
	if (xy && Piece)
	{
		moves = Piece->GetPieceMoves(xy, this);
	}

	FilterMovesAvoidCheck(Piece, moves);

	if(showMoves)
	{
		moves.Add(*xy); //Adding clicked square to enhance gameplay 
		for (auto pos : moves)
		{
			ASquare* square = GetSquareFromXY(pos);
			if (square && !square->IsDanger()) {
				square->SetAsSelected();
			}
		}
	}

	return moves;
}

void AChessboard::CancelFeasibleMoves()
{
	for (int32 y = 0; y < BoardSize; y++)
	{
		for (int32 x = 0; x < BoardSize; x++)
		{
			ASquare* square = GetSquareFromXY((FVector2D(x, y)));
			if (square && !square->IsDanger() && square->IsSelected())
			{
				square->SetSquareColor((x + y) % 2 == 0);
			}
		}
	}
}

void AChessboard::RestoreBoardColors()
{
	for (int32 y = 0; y < BoardSize; y++)
	{
		for (int32 x = 0; x < BoardSize; x++)
		{
			ASquare* square = GetSquareFromXY((FVector2D(x, y)));
			if(square)
				square->SetSquareColor((x + y) % 2 == 0);
		}
	}
}

void AChessboard::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

AChessPiece* AChessboard::SpawnStarterPieceByXYPosition(const int32 InX, const int32 InY)
{
	FVector Location = AChessboard::GetRelativeLocationByXYPosition(InX, InY) + FVector(0, 0, 20);
	FRotator Rotation = FRotator(0, 180, 0);
	AChessPiece* Piece = nullptr;

	if (InY == 1 || InY == 6)
	{
		Piece = GetWorld()->SpawnActor<AChessPiece>(Pawn, Location, Rotation);
		//Players[i]->Sign = i == CurrentPlayer ? ESign::X : ESign::O;
		if (Piece)
			Piece->SetColorAndMaterial(InY == 1 ? WHITE : BLACK);//WHITE pieces are in Y=1
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
			Piece->SetColorAndMaterial(InY == 0 ? WHITE : BLACK);//WHITE pieces are in Y=0
	}
	return Piece;
}

void AChessboard::GenerateField()
{
	const float TileScale = SquareSize / 100;

	for (int32 y = 0; y < BoardSize; y++)
	{
		for (int32 x = 0; x < BoardSize; x++)
		{
			FVector Location = AChessboard::GetRelativeLocationByXYPosition(x, y);
			ASquare* Sqr = GetWorld()->SpawnActor<ASquare>(SquareClass, Location, FRotator::ZeroRotator);
			AChessPiece* Pce = SpawnStarterPieceByXYPosition(x, y);
			(x + y) % 2 == 0 ? Sqr->SetSquareColor(true) : Sqr->SetSquareColor(false);
			Sqr->SetActorScale3D(FVector(TileScale, TileScale, 0.2));
			FVector2D pos = FVector2D(x, y);
			Sqr->setPiece(Pce);
			XY_Square.Add(pos, Sqr);
			if (Pce != nullptr && Pce->PieceColor != NAC) {
				Pce->PieceColor == WHITE ? WhitePieces.Add(Pce, pos) : BlackPieces.Add(Pce, pos);
			}
		}
	}
}

void AChessboard::RemovePiece(AChessPiece* p)
{
	if (p)
	{
		FVector2D* xy = GetXYFromPiece(p);
		if (xy && GetSquareFromXY(*xy))
		{
			GetSquareFromXY(*xy)->setPiece(nullptr);
			switch (p->PieceColor)
			{
			case WHITE:
				WhitePieces.Remove(p);
				break;
			case BLACK:
				BlackPieces.Remove(p);
				break;
			case NAC:
				UE_LOG(LogTemp, Error, TEXT("RemovePiece: Cannot remove NAC colored piece"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("RemovePiece: Cannot remove piece, square is null"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("RemovePiece: Cannot remove null piece"));
	}
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

AChessPiece* AChessboard::MakeAMove(FVector2D oldPosition, FVector2D newPosition, bool simulate)
{
	AChessPiece* eatingPiece = GetPieceFromXY(oldPosition);
	AChessPiece* eatenPiece = GetPieceFromXY(newPosition);

	if (!eatingPiece)
	{
		UE_LOG(LogTemp, Error, TEXT("MakeAMove:Impossible move, no pieces in old position"));
		return nullptr;
	}

	//making the move:
	RemovePiece(eatingPiece);
	if (eatenPiece)
	{
		RemovePiece(eatenPiece);//remove NewPiece
	}
	//SetPieceFromXY(oldPosition, nullptr);
	SetPieceFromXY(newPosition, eatingPiece);

	if (!simulate)//not a simulation, move the piece
	{
		if (eatenPiece)
		{
			(eatenPiece)->Destroy();
		}
		(eatingPiece)->SetActorLocation(GetRelativeLocationByXYPosition(newPosition[0], newPosition[1]));
	}
	return eatenPiece;
}

void AChessboard::RollbackMove(FVector2D oldPosition, FVector2D newPosition, AChessPiece* pieceToRestore)
{
	//NOTE: oldPosition MUST have no piece on it, this function is only usable to rollback a move (no "swap moves" are accepted in chess)
	AChessPiece* OldPiece = GetPieceFromXY(oldPosition);
	AChessPiece* NewPiece = GetPieceFromXY(newPosition);

	if (OldPiece)
	{
		UE_LOG(LogTemp, Error, TEXT("rollbackMove:Impossible rollback, pieces found in old position"));
		return;
	}
	if (!NewPiece)
	{
		UE_LOG(LogTemp, Error, TEXT("rollbackMove:Impossible rollback, no pieces in new position"));
		return;
	}

	SetPieceFromXY(oldPosition, NewPiece);
	SetPieceFromXY(newPosition, pieceToRestore);
}

bool AChessboard::CheckControl(ChessColor C)
{
	AChessPiece* ActualKing;
	FVector2D* KingPosition;
	TMap<AChessPiece*, FVector2D> ActualEnemyPices;
	TArray<FVector2D> SpottedSquares;

	if(C == NAC)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("CheckControl: Unexpected ChessColor NAC as input parameter"));
		return false;
	}

	ActualKing = C == WHITE ? WhiteKing : BlackKing;
	ActualEnemyPices = C == WHITE ? BlackPieces : WhitePieces;

	if (!ActualKing) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("KING DIED"));
		return true;
	}
	KingPosition = GetXYFromPiece(ActualKing);
	if(!KingPosition)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("KING NO MORE"));
		return true;
	}

	for (auto& Piece_XY : ActualEnemyPices)
	{
		auto pieceMoves = Piece_XY.Key->GetPieceMoves(&Piece_XY.Value, this);
		if (pieceMoves.Contains(*KingPosition))
		{
			return true;
		}
	}

	//int tmp = (*KingPosition)[0] + (*KingPosition)[1];//tmp variable for casting purpose
	//GetSquareFromXY(*KingPosition)->SetSquareColor((tmp % 2) == 0);
	return false;
}

bool AChessboard::MateControl(ChessColor C)
{
	AChessPiece* ActualKing;
	FVector2D* KingPosition;
	TMap<AChessPiece*, FVector2D> ActualOwnedPieces;

	if (C == NAC)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("MateControl: Unexpected ChessColor NAC as input parameter"));
		return false;
	}

	ActualKing = C == WHITE ? WhiteKing : BlackKing;
	ActualOwnedPieces = C == WHITE ? WhitePieces : BlackPieces;
	if (!ActualKing) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("KING DIED"));
		return true;
	}

	KingPosition = GetXYFromPiece(ActualKing);
	if (!KingPosition)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("KING NO MORE"));
		return true;
	}

	for (auto& Piece_XY : ActualOwnedPieces)
	{
		FVector2D oldPos = Piece_XY.Value;
		for (auto& newPos : GetFeasibleMoves(Piece_XY.Key, false)) {
			AChessPiece* tmp = MakeAMove(oldPos, newPos, true);
			if (!CheckControl(C)) {
				RollbackMove(oldPos, newPos, tmp);
				return false;
			}
			RollbackMove(oldPos, newPos, tmp);
		}
	}
	return true;
}

bool AChessboard::StallControl(ChessColor C)
{
	TMap<AChessPiece*, FVector2D> ActualOwnedPieces;

	if (C == NAC)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("MateControl: Unexpected ChessColor NAC as input parameter"));
		return false;
	}
	ActualOwnedPieces = C == WHITE ? WhitePieces : BlackPieces;

	for (auto& Piece_XY : ActualOwnedPieces)
	{
		if (GetFeasibleMoves(Piece_XY.Key, false).Num() > 0)
			return false;
	}
	return true;
}

FVector2D* AChessboard::GetKingPosition(ChessColor C)
{
	switch (C)
	{
	case WHITE:
		return GetXYFromPiece(WhiteKing);
	case BLACK:
		return GetXYFromPiece(BlackKing);
	}
	return nullptr;
}

bool AChessboard::MakeASafeMove(FVector2D oldPosition, FVector2D newPosition)
{
	if (GetSquareFromXY(newPosition) && GetSquareFromXY(newPosition)->IsSelected())
	{
		if (GetPieceFromXY(newPosition) != GetPieceFromXY(oldPosition))
		{
			MakeAMove(oldPosition, newPosition, false);
			return true;
		}
	}
	return false;
}

void AChessboard::FilterMovesAvoidCheck(AChessPiece* p, TArray<FVector2D>& moves)
{
	FVector2D oldPos =*GetXYFromPiece(p);
	FVector2D newPos;

	//note: using moves.Num() so the upper bound is automatically refreshed
	for (int i = 0; i < moves.Num(); i++) {
		newPos = moves[i];
		AChessPiece* tmp = MakeAMove(oldPos, newPos, true);
		if (oldPos != newPos && CheckControl(p->PieceColor)) {
			moves.Remove(newPos);
			i--;
		}
		RollbackMove(oldPos, newPos, tmp);
	}
}