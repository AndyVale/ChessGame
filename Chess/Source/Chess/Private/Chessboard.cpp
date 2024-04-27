// Fill out your copyright notice in the Description page of Project Settings.

#include "Chessboard.h"
#include "ChessPiece.h"
#include "CP_King.h"
#include "CP_Pawn.h"
#include <Chess_GameInstance.h>
#include "Chess_GameMode.h"
#include <Kismet/GameplayStatics.h>
#include "Components/TextRenderComponent.h"



// Sets default values
AChessboard::AChessboard()
{
	PrimaryActorTick.bCanEverTick = false;

	//initialize variables to default values
	StackMoves = TArray<TSharedPtr<Chess_Move>>();
	StackUndoMoves = TArray<TSharedPtr<Chess_Move>>();
	//PositionValue = 0;
	WhiteMaterial = 0;
	BlackMaterial = 0;
	BoardSize = 8;
	SquareSize = 100;
	BlackKing = nullptr;
	WhiteKing = nullptr;
	bCastleBlackLong = true;
	bCastleBlackShort = true;
	bCastleWhiteLong = true;
	bCastleWhiteShort = true;
}

// Called when the game starts or when spawned
void AChessboard::BeginPlay()
{
	Super::BeginPlay();
	//binding events to delegates:
	if (UChess_GameInstance* GameInstanceRef = Cast<UChess_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()))) {
		GameInstanceRef->OnResetEvent.AddDynamic(this, &AChessboard::ResetBoard);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("GameInstanceRef is null in Chessboard"));
	}

	if (AChess_GameMode* GameModeRef = Cast<AChess_GameMode>(UGameplayStatics::GetGameMode(GetWorld()))) {
		GameModeRef->OnReplayMove.AddDynamic(this, &AChessboard::ReplayMove);
		GameModeRef->OnTurnGoBack.AddUObject(this, &AChessboard::RemoveUndoneMoves);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("GameModeRef is null in Chessboard"));
	}
	
	//Spawn the chessboard
	if (SquareClass != nullptr)
	{
		GenerateField();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("SquareClass non trovata in ChessBoard"));
	}
}

void AChessboard::ReplayMove(int32 moveNumber)
{
	PushAndPopUntilMove(moveNumber);//replay back
	RestoreBoardColors();//restore board colors to default white-black pattern

	if (!StackMoves.IsEmpty())
	{
		//highlight squares of the move in replay (and the king in check if there is)
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
	//Destroy all pieces and squares
	for (AActor* Actor : ChessPieces)
	{
		Actor->Destroy();
	}
	for (AActor* Actor : Squares)
	{
		Actor->Destroy();
	}

	//reset variables:
	//	PositionValue = 0;
	WhiteMaterial = 0;
	BlackMaterial = 0;
	bCastleBlackLong = true;
	bCastleBlackShort = true;
	bCastleWhiteLong = true;
	bCastleWhiteShort = true;
	EnPassantPossibleCapture = FVector2D(-10, -10);
	WhiteKing = BlackKing = nullptr;
	WhitePieces.Empty();
	BlackPieces.Empty();
	StackUndoMoves.Empty();
	StackMoves.Empty();
	XY_Square.Empty();

	//recreate the board:
	GenerateField();
}

FVector2D* AChessboard::GetXYFromPiece(AChessPiece* p)
{
	if (p)
	{
		return &p->PiecePosition;
	}
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

TSharedPtr<Chess_Move> AChessboard::GetMoveShowedOnBoard()
{
	if (!StackMoves.IsEmpty())
	{
		return StackMoves.Top();//return the last move
	}
	return nullptr;
}

TArray<TSharedPtr<Chess_Move>> AChessboard::GetMovesAndShowHints(AChessPiece* Piece, bool showMoves)//TODO: showMoves is not used
{
	TArray<FVector2D> xys = TArray<FVector2D>();
	TArray<TSharedPtr<Chess_Move>> moves = Piece->GetPieceLegalMoves();
	for (TSharedPtr<Chess_Move> move : moves)//show the possible moves on the board setting the squares as selected
	{
		FVector2D xy = move->To;
		ASquare* square = GetSquareFromXY(xy);
		if (square && !square->IsDanger()) {
			square->SetAsSelected();
		}
	}
	return moves;
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

//STANDARD CHESSBOARD:
AChessPiece* AChessboard::SpawnStarterPieceByXYPosition(const int32 InX, const int32 InY)
{
	FVector Location = AChessboard::GetRelativeLocationByXYPosition(InX, InY);
	FRotator Rotation = FRotator(0, 0, 0);
	AChessPiece* Piece = nullptr;

	//spawn pieces:

	if (InY == 1 || InY == 6)
	{
		Piece = GetWorld()->SpawnActor<AChessPiece>(Pawn, Location, Rotation);
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
//TEST CHESSBOARD:
AChessPiece* AChessboard::SpawnStarterPieceByXYPosition(const int32 InX, const int32 InY)
{
	//TODO: TESTA L'ARROCCO ARTIFICIALMENTE
	FVector Location = AChessboard::GetRelativeLocationByXYPosition(InX, InY);
	FRotator Rotation = FRotator(0, 0, 0);
	AChessPiece* Piece = nullptr;

	if (InX == 5 && InY == 1) {
		Piece = GetWorld()->SpawnActor<AChessPiece>(Queen, Location, Rotation);
		Piece->SetColorAndMaterial(WHITE);
	}
	if (InX == 5 && InY == 6) {
		Piece = GetWorld()->SpawnActor<AChessPiece>(Queen, Location, Rotation);
		Piece->SetColorAndMaterial(WHITE);
	}

	if (InX == 6 && InY == 2) {
		Piece = GetWorld()->SpawnActor<AChessPiece>(King, Location, Rotation);
		Piece->SetColorAndMaterial(BLACK);
		BlackKing = Piece;
	}
	if (InX ==7 && InY == 3) {
		Piece = GetWorld()->SpawnActor<AChessPiece>(Bishop, Location, Rotation);
		Piece->SetColorAndMaterial(WHITE);
	}

	if (InX == 1 && InY == 7) {
		Piece = GetWorld()->SpawnActor<AChessPiece>(King, Location, Rotation);
		Piece->SetColorAndMaterial(WHITE);
		WhiteKing = Piece;
	}

	//if (InX == 7 && InY == 6) {
	//	Piece = GetWorld()->SpawnActor<AChessPiece>(Pawn, Location, Rotation);
	//	Piece->SetColorAndMaterial(BLACK);
	//}

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
	if (p)
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
	return SquareSize * FVector(InX, InY, 0) + FVector(0, 0, 10);
}

FVector2D AChessboard::GetXYPositionByRelativeLocation(const FVector& Location) const
{
	const double x = Location[0] / SquareSize;
	const double y = Location[1] / SquareSize;
	return FVector2D(x, y);
}

TSharedPtr<Chess_Move> AChessboard::PromoteLastMove(CP ChessPieceEnum)
{
	if (StackMoves.Top() && StackMoves.Top()->MoveClass == MoveType::PAWN_PROMOTION) {//if the last move is a pawn promotion
		if (Chess_MovePawnPromotion* pawnPromotionMove = static_cast<Chess_MovePawnPromotion*>(StackMoves.Top().Get())) {//get it as a pawn promotion move
			pawnPromotionMove->PromotePawn(false, ChessPieceEnum);//promote the pawn

			RestoreBoardColors();

			pawnPromotionMove->RollbackMove(false);//force the rollback of the move to avoid fake check
			pawnPromotionMove->MakeMove(false);//force the move to avoid fake check

			if (AChess_GameMode* GameModeRef = Cast<AChess_GameMode>(UGameplayStatics::GetGameMode(GetWorld())))
			{
				GameModeRef->UpdateLastMove(StackMoves.Top());//notify the HUD of the move change
			}
		}
	}
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
			if (pieceMove->To == *KingPosition)
			{
				return true;
			}
		}
	}
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
		for (TSharedPtr<Chess_Move> tmpMove : Piece->GetPieceMoves()) {
			if (tmpMove->IsLegal())
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

	if (!StackMoves.IsEmpty() && StackMoves.Top()->PlayerOnCheck != ChessColor::NAC)//TODO:to this in chess_move
	{
		return false;//not a stall but a check/Checkmate
	}

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
	UE_LOG(LogTemp, Error, TEXT("GetKingPosition: Unexpected ChessColor NAC as input parameter"));
	return nullptr;
}

void AChessboard::HandledMakeAMove(TSharedPtr<Chess_Move> move, bool simulate)
{
	bool wl=bCastleWhiteLong, ws=bCastleWhiteShort, bl=bCastleBlackLong, bs=bCastleBlackShort;
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
		float pv = WhiteMaterial - BlackMaterial;//white material - black material
		//int32 tmp1 = move->From.X, tmp2 = move->From.Y, tmp3 = move->To.X, tmp4 = move->To.Y;
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Il valore della mossa da %d, %d a %d, %d è %f:->%f"), tmp1, tmp2, tmp3, tmp4, value, pv));

		StackMoves.Push(move);
		//OnMove.Broadcast(move->ToString());

		if (move->MoveClass == MoveType::PAWN_PROMOTION && StackUndoMoves.IsEmpty()) {//if is a promotion and is not a replay -> spawn widget 
			UE_LOG(LogTemp, Display, TEXT("Spawn promotion HUD"));
			if (AChess_GameMode* GameMode = Cast<AChess_GameMode>(GetWorld()->GetAuthGameMode())) {
				GameMode->ShowPromotionWidget(move->GetMoveColor());
			}
		}
	}
	if (bl != bCastleBlackLong || bs != bCastleBlackShort || ws != bCastleWhiteShort || wl != bCastleWhiteLong)//debug porpouse
	{
		UE_LOG(LogTemp, Display, TEXT("HandledMakeAMove: Castle variables changed"));
	}

	//Enpassant handling:
	EnPassantPossibleCapture = FVector2D(-10, -10);
	if (FMath::Abs(move->To.Y - move->From.Y) == 2 && FMath::Abs(move->To.X - move->From.X) == 0)//when a piece move by two in y and 0 in x
	{
		if (ACP_Pawn* PawnTriggeringEnPassant = Cast<ACP_Pawn>(move->CapturingPiece))//and is a pawn
		{
			EnPassantPossibleCapture = move->To;//can be captured enpassant
		}
	}
}

void AChessboard::HandledRollbackAMove(TSharedPtr<Chess_Move> move, bool simulate)
{
	if (move == nullptr)
	{
		return;
	}
	move->RollbackMove(simulate);
	if (!simulate)
	{
		StackUndoMoves.Push(move);
	}

	//Enpassant handling
	EnPassantPossibleCapture = FVector2D(-10, -10);
	if (!StackMoves.IsEmpty())
	{
		TSharedPtr<Chess_Move> passedMove = StackMoves.Top();//look at last move for enpassant capture
		if (FMath::Abs(passedMove->To.Y - passedMove->From.Y) == 2 && FMath::Abs(passedMove->To.X - passedMove->From.X) == 0)
		{
			if (ACP_Pawn* PawnTriggeringEnPassant = Cast<ACP_Pawn>(passedMove->CapturingPiece))
			{
				EnPassantPossibleCapture = passedMove->To;
			}
		}
	}
}

void AChessboard::PushAndPopUntilMove(int32 moveNumber)
{
	int32 actualMoveNumber = StackMoves.Num();
	TSharedPtr<Chess_Move> tmpMove;
	if (moveNumber <= actualMoveNumber)//replay back
	{
		for (int32 i = 0; i < actualMoveNumber - moveNumber; i++)
		{
			if (!StackMoves.IsEmpty())
			{
				tmpMove = StackMoves.Pop();
				HandledRollbackAMove(tmpMove, false);
				//StackUndoMoves.Push(tmpMove);
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
				tmpMove = StackUndoMoves.Pop();
				HandledMakeAMove(tmpMove, false);
				//StackMoves.Push(tmpMove);
			}
			else
			{
				break;
			}
		}
	}
}
