// Fill out your copyright notice in the Description page of Project Settings.

#include "Chessboard.h"
#include "ChessPiece.h"
#include "CP_King.h"
#include <Chess_GameInstance.h>
#include "Chess_GameMode.h"
#include <Kismet/GameplayStatics.h>
#include "Components/TextRenderComponent.h"



// Sets default values
AChessboard::AChessboard()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	StackMoves = TArray<TSharedPtr<Chess_Move>>();
	StackUndoMoves = TArray<TSharedPtr<Chess_Move>>();
	PrimaryActorTick.bCanEverTick = false;
	//PositionValue = 0;
	WhiteMaterial = 0;
	BlackMaterial = 0;
	BoardSize = 8;
	SquareSize = 100;
	BlackKing = nullptr;
	WhiteKing = nullptr;
}

// Called when the game starts or when spawned
void AChessboard::BeginPlay()
{
	Super::BeginPlay();

	if (UChess_GameInstance* GameInstanceRef = Cast<UChess_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()))) {
		GameInstanceRef->OnResetEvent.AddDynamic(this, &AChessboard::ResetBoard);
	}
	if (AChess_GameMode* GameModeRef = Cast<AChess_GameMode>(UGameplayStatics::GetGameMode(GetWorld()))) {
		GameModeRef->OnReplayMove.AddDynamic(this, &AChessboard::ReplayMove);
		GameModeRef->OnTurnGoBack.AddUObject(this, &AChessboard::RemoveUndoneMoves);
	}
	if (SquareClass != nullptr)
	{
		GenerateField();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("SquareClass non trovata in GameField"));
	}
}

void AChessboard::ReplayMove(int32 moveNumber)
{
	PushAndPopUntilMove(moveNumber);
	RestoreBoardColors();
	if (!StackMoves.IsEmpty())
	{
		TSharedPtr<Chess_Move> moveInReplay = StackMoves.Top();
		ASquare* f = GetSquareFromXY(moveInReplay->From);
		ASquare* t = GetSquareFromXY(moveInReplay->To);
		if (moveInReplay->PlayerOnCheck != ChessColor::NAC)
		{
			FVector2D* kingPos = GetKingPosition(moveInReplay->PlayerOnCheck);
			if (kingPos && GetSquareFromXY(*kingPos))
			{
				GetSquareFromXY(*kingPos)->SetDangerColor();
			}
		}
		if (f && t)//Highlight squares
		{
			f->InReplay();
			t->InReplay();
		}
	}
}

void AChessboard::RemoveUndoneMoves(int32 moveNumber)
{
	PushAndPopUntilMove(moveNumber);//not used(?)
	StackUndoMoves.Empty();
}

TArray<AChessPiece*> AChessboard::GetPieces(ChessColor C)
{
	switch (C)
	{
	case WHITE:
		return WhitePieces;
	case BLACK:
		return BlackPieces;
	}
	return TArray<AChessPiece*>();
}

void AChessboard::ResetBoard()
{
	TArray<AActor*> ChessPieces, Squares;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AChessPiece::StaticClass(), ChessPieces);
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASquare::StaticClass(), Squares);
	for (AActor* Actor : ChessPieces)
	{
		Actor->Destroy();
	}
	for (AActor* Actor : Squares)
	{
		Actor->Destroy();
	}

	//	PositionValue = 0;
	WhiteMaterial = 0;
	BlackMaterial = 0;
	WhiteKing = BlackKing = nullptr;
	WhitePieces.Empty();
	BlackPieces.Empty();
	StackUndoMoves.Empty();
	StackMoves.Empty();
	XY_Square.Empty();
	GenerateField();
}

bool AChessboard::IsOnReplay()
{
	return StackUndoMoves.Num() != 0;
}

FVector2D* AChessboard::GetXYFromPiece(AChessPiece* p)
{
	if (p)
	{
		return &p->PiecePosition;
	}
	return nullptr;
	/*FVector2D xy;
	if (p)
	{
		switch (p->PieceColor)
		{
		case WHITE:
			if (WhitePieces.Find(p))
			{

				xy = (*WhitePieces.Find(p))->PiecePosition;
				if (xy != p->PiecePosition)
				{
					bool b = true;
				}
				return //WhitePieces.Find(p);
			}
			break;
		case BLACK:
			if (BlackPieces.Find(p))
			{

				xy = *BlackPieces.Find(p);
				if (xy != p->PiecePosition)
				{
					bool b = true;
				}
				return BlackPieces.Find(p);
			}
			break;
		case NAC:
			break;
		}
	}
	UE_LOG(LogTemp, Error, TEXT("GetXYFormPiece: Piece not found"));
	return nullptr;*/
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
	if (XY_Square.Find(xy))
		return *XY_Square.Find(xy);
	return nullptr;
}

bool AChessboard::SetPieceFromXY(FVector2D xy, AChessPiece* p)
{//Before calling this method make shure that p and xy are not already in a "pair xy-piece"
	if (GetSquareFromXY(xy))
	{
		if (p) {
			int32 oldNum = 0;
			switch (p->PieceColor)
			{
			case WHITE:
				oldNum = WhitePieces.Num();
				if (WhitePieces.Find(p) == INDEX_NONE)//Setting a new piece
				{
					WhitePieces.Add(p);
					WhiteMaterial += p->GetPieceValue();
				}
				if (ACP_King* possKing = Cast<ACP_King>(p))
				{
					WhiteKing = p;
				}
				break;
			case BLACK:
				oldNum = BlackPieces.Num();
				if (BlackPieces.Find(p) == INDEX_NONE)//Setting a new piece
				{
					BlackPieces.Add(p);
					BlackMaterial += p->GetPieceValue();
				}
				if (ACP_King* possKing = Cast<ACP_King>(p))
				{
					BlackKing = p;
				}
				break;
			case NAC:
				UE_LOG(LogTemp, Error, TEXT("SetPieceFromXY:Color of piece NAC"));
				return false;
				break;
			}
			p->ReferredBoard = this;
			p->PiecePosition = xy;
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

TArray<FVector2D> AChessboard::GetLegalSquares(AChessPiece* Piece, bool showMoves)
{
	TArray<FVector2D> xys = TArray<FVector2D>();
	for(Chess_Move& move : Piece->GetPieceLegalMoves())
	{
		xys.Add(move.To);
	}

	if (showMoves)
	{
		for (FVector2D& xy : xys)
		{
			ASquare* square = GetSquareFromXY(xy);
			if (square && !square->IsDanger()) {
				square->SetAsSelected();
			}
		}
	}
	return xys;
}

void AChessboard::CancelFeasibleSquares()
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
			if (square)
				square->SetSquareColor((x + y) % 2 == 0);
		}
	}
}

void AChessboard::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}
/**/
//STANDARD CHESSBOARD:
AChessPiece* AChessboard::SpawnStarterPieceByXYPosition(const int32 InX, const int32 InY)
{
	FVector Location = AChessboard::GetRelativeLocationByXYPosition(InX, InY);
	FRotator Rotation = FRotator(0, 0, 0);
	AChessPiece* Piece = nullptr;

	//spawn piece:

	if (InY == 1 || InY == 6)
	{
		Piece = GetWorld()->SpawnActor<AChessPiece>(Pawn, Location, Rotation);
		//Players[i]->Sign = i == CurrentPlayer ? ESign::X : ESign::O;
		if (Piece)
			Piece->SetColorAndMaterial(InY == 6 ? WHITE : BLACK);//WHITE pieces are in Y=6
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
		case 3:
			Piece = GetWorld()->SpawnActor<AChessPiece>(Queen, Location, Rotation);
			break;
		case 4:
			Piece = GetWorld()->SpawnActor<AChessPiece>(King, Location, Rotation);
			if (InY == 7)
				WhiteKing = Piece;
			else
				BlackKing = Piece;//BLACK pieces are in Y=6
			break;
		default:
			Piece = nullptr;
		}
		if (Piece)
		{
			Piece->SetColorAndMaterial(InY == 7 ? WHITE : BLACK);//WHITE pieces are in Y=7
		}
	}
	//updating material count:{
	//if (Piece)
	//{
	//	Piece->ReferredBoard = this;
	//	Piece->PiecePosition = FVector2D(InX, InY);
	//
	//	if (Piece->PieceColor == ChessColor::WHITE)
	//	{
	//		 += Piece->GetPieceValue();//+= Piece->GetCorrectedPieceValue();
	//	}
	//	else
	//	{
	//		PositionValue -= Piece->GetPieceValue();// -= Piece->GetCorrectedPieceValue();
	//	}
	//}
	return Piece;
}
/*
AChessPiece* AChessboard::SpawnStarterPieceByXYPosition(const int32 InX, const int32 InY)
{
	FVector Location = AChessboard::GetRelativeLocationByXYPosition(InX, InY);
	FRotator Rotation = FRotator(0, 0, 0);
	AChessPiece* Piece = nullptr;

	if (InX == 4 && InY == 4) {
		Piece = GetWorld()->SpawnActor<AChessPiece>(Pawn, Location, Rotation);
		Piece->SetColorAndMaterial(BLACK);
	}
	if (InX == 6 && InY == 7) {
		Piece = GetWorld()->SpawnActor<AChessPiece>(Rook, Location, Rotation);
		Piece->SetColorAndMaterial(BLACK);
	}
	if (InX == 4 && InY == 2) {
		Piece = GetWorld()->SpawnActor<AChessPiece>(Rook, Location, Rotation);
		Piece->SetColorAndMaterial(WHITE);
	}

	if (InX == 5 && InY == 2) {
		Piece = GetWorld()->SpawnActor<AChessPiece>(King, Location, Rotation);
		Piece->SetColorAndMaterial(WHITE);
		WhiteKing = Piece;
	}

	if (InX == 7 && InY == 0) {
		Piece = GetWorld()->SpawnActor<AChessPiece>(King, Location, Rotation);
		Piece->SetColorAndMaterial(BLACK);
		BlackKing = Piece;
	}


	//if (Piece)
	//{
	//	Piece->ReferredBoard = this;
	//	Piece->PiecePosition = FVector2D(InX, InY);
	//
	//	if (Piece->PieceColor == ChessColor::WHITE)
	//	{
	//		//PositionValue += Piece->GetCorrectedPieceValue();
	//		PositionValue += Piece->GetPieceValue();
	//	}
	//	else
	//	{
	//		//PositionValue -= Piece->GetCorrectedPieceValue();
	//		PositionValue -= Piece->GetPieceValue();
	//	}
	//}

	return Piece;
}	
*/
void AChessboard::GenerateField()
{
	const float TileScale = SquareSize / 100;
	const char Letters[] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H' };
	const char Numbers[] = { '8', '7', '6', '5', '4', '3', '2', '1' };
	for (int32 y = 0; y < BoardSize; y++)
	{
		for (int32 x = 0; x < BoardSize; x++)
		{
			//Spawn squares and pieces:
			FVector Location = AChessboard::GetRelativeLocationByXYPosition(x, y) - FVector(0, 0, 20);//slightly below pieces
			ASquare* Sqr = GetWorld()->SpawnActor<ASquare>(SquareClass, Location, FRotator::ZeroRotator);
			AChessPiece* Pce = SpawnStarterPieceByXYPosition(x, y);
			(x + y) % 2 == 0 ? Sqr->SetSquareColor(true) : Sqr->SetSquareColor(false);
			Sqr->SetActorScale3D(FVector(TileScale, TileScale, 0.2));
			FVector2D pos = FVector2D(x, y);
			//Sqr->setPiece(Pce);
			XY_Square.Add(pos, Sqr);
			if (Pce != nullptr && Pce->PieceColor != NAC) {
				//Pce->PieceColor == WHITE ? WhitePieces.Add(Pce) : BlackPieces.Add(Pce);
				SetPieceFromXY(FVector2D(x, y), Pce);
			}
			//Chessboard indications:
			if (y == 7 || x == 0)
			{
				if (y == 7)
				{	//Spawn a letter
					UTextRenderComponent* letterText = NewObject<UTextRenderComponent>(Sqr);
					letterText->SetWorldSize(10);
					// Letters Low-Left
					letterText->SetRelativeLocation(FVector(30, 30, 100));
					letterText->SetText(FText::FromString(FString(1, &Letters[x])));
					letterText->SetupAttachment(Sqr->GetRootComponent());
					letterText->RegisterComponent();
					//FVector tmp = Sqr->GetActorScale3D();
					letterText->SetWorldScale3D(FVector(2, 2, 5));

					letterText->SetRelativeRotation(FRotator(90, 90, 0));
					letterText->SetTextRenderColor(FColor(10, 10, 10));
					letterText->SetVisibility(true);
				}
				if (x == 0)
				{	//Spawn a number
					UTextRenderComponent* numberText = NewObject<UTextRenderComponent>(Sqr);
					numberText->SetWorldSize(10);

					// Numbers High-Left
					numberText->SetRelativeLocation(FVector(-30, 0, 100));
					numberText->SetText(FText::FromString(FString(1, &Numbers[y])));
					numberText->SetupAttachment(Sqr->GetRootComponent());
					numberText->RegisterComponent();

					//FVector tmp = Sqr->GetActorScale3D();
					numberText->SetWorldScale3D(FVector(2, 2, 5));

					numberText->SetRelativeRotation(FRotator(90, 90, 0));
					numberText->SetTextRenderColor(FColor(10, 10, 10));
					numberText->SetVisibility(true);
				}
			}
		}
	}
}


void AChessboard::RemovePiece(AChessPiece* p)
{
	if (p)//TODO: Quando gli passo la regina del pezzo promosso non trova nulla.
	{
		FVector2D* xy = GetXYFromPiece(p);
		if (xy && GetSquareFromXY(*xy))
		{
			GetSquareFromXY(*xy)->setPiece(nullptr);
			p->PiecePosition = FVector2D(-1, -1);
			switch (p->PieceColor)
			{
			case WHITE:
				if (ACP_King* possKing = Cast<ACP_King>(p))
				{
					WhiteKing = nullptr;
				}
				WhitePieces.Remove(p);
				WhiteMaterial -= p->GetPieceValue();
				break;
			case BLACK:
				if (ACP_King* possKing = Cast<ACP_King>(p))
				{
					BlackKing = nullptr;
				}
				BlackPieces.Remove(p);
				BlackMaterial -= p->GetPieceValue();
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
	return SquareSize * FVector(InX, InY, 0) + FVector(0, 0, 20);
}

FVector2D AChessboard::GetXYPositionByRelativeLocation(const FVector& Location) const
{
	const double x = Location[0] / SquareSize;
	const double y = Location[1] / SquareSize;
	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("x=%f,y=%f"), x, y));
	return FVector2D(x, y);
}

TSharedPtr<Chess_Move> AChessboard::PromoteLastMove(CP ChessPieceEnum)
{
	TSubclassOf<AChessPiece> selectedClass = nullptr;
	switch (ChessPieceEnum) {
	case CP::KNIGHT:
		selectedClass = Knight;
		break;
	case CP::ROOK:
		selectedClass = Rook;
		break;
	case CP::BISHOP:
		selectedClass = Bishop;
		break;
	case CP::QUEEN:
		selectedClass = Queen;
		break;
	default:
		UE_LOG(LogTemp, Error, TEXT("PromoteLastMove: Unexpected CP enum value"));
	}
	StackMoves.Top()->PromotePawn(false, selectedClass);
	return StackMoves.Top();
}

bool AChessboard::CheckControl(ChessColor C)
{
	AChessPiece* ActualKing;
	FVector2D* KingPosition;
	TArray<AChessPiece*> ActualEnemyPices;
	TArray<FVector2D> SpottedSquares;

	if (C == NAC)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("CheckControl: Unexpected ChessColor NAC as input parameter"));
		return false;
	}

	ActualKing = C == WHITE ? WhiteKing : BlackKing;
	ActualEnemyPices = C == WHITE ? BlackPieces : WhitePieces;

	if (!ActualKing) {//In simulated moves king's death is allowed
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("KING DIED"));
		return true;
	}
	KingPosition = GetXYFromPiece(ActualKing);
	if (!KingPosition)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("KING NO MORE"));
		return true;
	}

	for (auto& Piece_XY : ActualEnemyPices)
	{
		for(auto& pieceMove : Piece_XY->GetPieceMoves())
		{ 
			if (pieceMove.To == *KingPosition)
			{
				return true;
			}
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
	TArray<AChessPiece*>  ActualOwnedPieces;

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

	for (auto& Piece : ActualOwnedPieces)
	{
		FVector2D oldPos = Piece->PiecePosition;
		for (Chess_Move& tmpMove : Piece->GetPieceMoves()) {
			if (tmpMove.IsLegal())
			{
				return false;
			}
		}
	}
	return true;
}

bool AChessboard::StallControl(ChessColor C)
{
	TArray<AChessPiece*> ActualOwnedPieces;

	if (C == NAC)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("MateControl: Unexpected ChessColor NAC as input parameter"));
		return false;
	}
	ActualOwnedPieces = C == WHITE ? WhitePieces : BlackPieces;

	for (auto& Piece : ActualOwnedPieces)
	{
		if (Piece->GetPieceLegalMoves().Num() > 0)
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

bool AChessboard::MakeASafeMove(TSharedPtr<Chess_Move> move)
{
	FVector2D oldPosition = move->From;
	FVector2D newPosition = move->To;
	ASquare* selectedSquare = GetSquareFromXY(newPosition);
	if (selectedSquare && selectedSquare->IsSelected())
	{
		if (GetPieceFromXY(newPosition) != GetPieceFromXY(oldPosition))
		{
			MakeAMove(move, false);
			return true;
		}
	}
	return false;
}

void AChessboard::MakeAMove(TSharedPtr<Chess_Move> move, bool simulate)
{
	if (move == nullptr)
	{
		return;
	}
	float value = 0.0f;
	float delta = -(WhiteMaterial - BlackMaterial);
	move->MakeMove(simulate);
	delta += WhiteMaterial - BlackMaterial;
	if (!simulate)
	{
		/*if (move->GetMoveColor() == WHITE)
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("From %f %f To %f %f"), move->From.X, move->From.Y, move->To.X, move->To.Y));
		else
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, FString::Printf(TEXT("From %f %f To %f %f"), move->From.X, move->From.Y, move->To.X, move->To.Y));
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("Board value: %f"), PositionValue));
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Delta: %f"), delta));
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Move value: %f"), move->MoveValue));

		move->PieceFrom->GetPieceLegalMoves();
		if (CheckControl(WHITE))//AI win
		{
			if (MateControl(WHITE))
			{
				value = -MAX_flt + 1;
			}
		}

		if (CheckControl(BLACK))//AI lose
		{
			if (MateControl(BLACK))
			{
				value = MAX_flt - 1;
			}
		}*/

		float pv = WhiteMaterial - BlackMaterial;//white material - black material
		int32 tmp1 = move->From.X, tmp2 = move->From.Y, tmp3 = move->To.X, tmp4 = move->To.Y;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Il valore della mossa da %d, %d a %d, %d è %f:->%f"), tmp1, tmp2, tmp3, tmp4, value, pv));


		StackMoves.Push(move);
		//OnMove.Broadcast(move->ToString());

		if (move->bPromotionAfterMove) {
			UE_LOG(LogTemp, Error, TEXT("Spawn HUD"));
			if (AChess_GameMode* GameMode = Cast<AChess_GameMode>(GetWorld()->GetAuthGameMode())) {
				GameMode->ShowPromotionWidget(move->GetMoveColor());
			}
		}
	}
}

void AChessboard::RollbackMove(TSharedPtr<Chess_Move> move, bool simulate)
{
	move->RollbackMove(simulate);
	if (!simulate)
	{
		//StackMoves.Pop();
	}
}

void AChessboard::PushAndPopUntilMove(int32 moveNumber)
{
	int32 actualMoveNumber = StackMoves.Num();
	if (moveNumber <= actualMoveNumber)
	{
		for (int32 i = 0; i < actualMoveNumber - moveNumber; i++)
		{
			if (!StackMoves.IsEmpty())
			{
				TSharedPtr<Chess_Move> tmpMove = StackMoves.Pop();
				tmpMove->RollbackMove(false);
				StackUndoMoves.Push(tmpMove);
			}
			else
			{
				break;
			}
		}
	}
	else
	{
		for (int32 i = actualMoveNumber; i < moveNumber; i++)
		{
			if (!StackUndoMoves.IsEmpty())
			{
				TSharedPtr<Chess_Move> tmpMove = StackUndoMoves.Pop();
				tmpMove->MakeMove(false);
				StackMoves.Push(tmpMove);
			}
			else
			{
				break;
			}
		}
	}
}
