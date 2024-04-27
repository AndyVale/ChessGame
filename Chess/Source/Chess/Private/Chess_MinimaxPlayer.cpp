// Fill out your copyright notice in the Description page of Project Settings.


#include "Chess_MinimaxPlayer.h"
#include "chrono"
int32 nodeVisited = 0;

// Sets default values
AChess_MinimaxPlayer::AChess_MinimaxPlayer()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	GameInstanceRef = Cast<UChess_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (!GameInstanceRef)
	{
		UE_LOG(LogTemp, Error, TEXT("GameInstanceRef is null in AChess_MinimaxPlayer"));
	}
	bIsMyTurn = false;
	PlayerColor = ChessColor::BLACK;
}

// Called when the game starts or when spawned
void AChess_MinimaxPlayer::BeginPlay()
{
	Super::BeginPlay();
	if (GameInstanceRef)
	{
		GameInstanceRef->OnResetEvent.AddDynamic(this, &AChess_MinimaxPlayer::ResetHandler);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("GameInstanceRef is null in AChess_MinimaxPlayer at beginplay"));
	}
}

// Called every frame
void AChess_MinimaxPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AChess_MinimaxPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AChess_MinimaxPlayer::OnTurn()
{
	FTimerHandle TimerHandle;
	if (GameInstanceRef)
	{
		GameInstanceRef->SetTurnMessage(PlayerColor == ChessColor::WHITE ? TEXT("White Turn") : TEXT("Black Turn"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("GameInstanceRef is null in AChess_MinimaxPlayer onTurn"));
	}

	if (AChess_GameMode* GameMode = Cast<AChess_GameMode>(GetWorld()->GetAuthGameMode()))
	{
		GameMode->PlayerSwapNotify(false);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("GameMode is null in AChess_MinimaxPlayer onTurn"));
	}

	bIsMyTurn = true;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [&] { AChess_MinimaxPlayer::MakeMinimaxMove(); }, 1, false);
}

void AChess_MinimaxPlayer::OnWin()
{
	GameInstanceRef->IncrementScoreAIPlayer();
}

int32 visitedNode;//debug purposes

void AChess_MinimaxPlayer::MakeMinimaxMove()
{
	if (AChess_GameMode* GM = Cast<AChess_GameMode>(GetWorld()->GetAuthGameMode())) {

		auto start = std::chrono::high_resolution_clock::now();//debug purposes

		TSharedPtr<Chess_Move> miniMaxMove = FindBestMove(GM->Board);//Find the best move

		//debug purposes
		auto end = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
		int32 a = visitedNode;
		double m = (a * 1.0) / duration;
		UE_LOG(LogTemp, Error, TEXT("%d,       %lld,"), a, duration, m);
		//end debug purposes

		if (miniMaxMove && bIsMyTurn)//if the move is valid and is still my turn (no reset occurred)
		{
			GM->Board->HandledMakeAMove(miniMaxMove, false);
			GM->UpdateLastMove(miniMaxMove);//notify the HUD of the move

			bIsMyTurn = false;
			GM->TurnNextPlayer();
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Minimax player has no moves to make"));
			//reset occurred or move is not valid
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("GameMode is null in AChess_MinimaxPlayer MakeMinimaxMove"));
	}
}
TSharedPtr<Chess_Move> AChess_MinimaxPlayer::FindBestMove(AChessboard* board)
{
	float bestScore;
	float actualScore;
	TArray<TSharedPtr<Chess_Move>> bestMoves = TArray< TSharedPtr<Chess_Move>>();
	TSharedPtr<Chess_Move> lastBestMove = nullptr;
	TArray<AChessPiece*>  piecesMap;
	TArray<TSharedPtr<Chess_Move>> moves = TArray<TSharedPtr<Chess_Move>>();

	visitedNode = 0;//debug purposes

	if (PlayerColor == ChessColor::BLACK)//play as Minimizer
	{
		bestScore = +20000.0f;
		piecesMap = board->GetPieces(BLACK);

		//get all legal moves
		for (auto& piece : piecesMap)
		{
			moves.Append(piece->GetPieceLegalMoves());
		}

		moves.Sort([](const TSharedPtr<Chess_Move>& a, const  TSharedPtr<Chess_Move>& b) {//order in descending order -> best move (at depth 0) for black first
			return a->MoveValue < b->MoveValue;
			});

		for (TSharedPtr<Chess_Move> move : moves)
		{
			visitedNode++;
			board->HandledMakeAMove(move, true);
			actualScore = AlfaBetaMinimax(-20000.0f, 20000.0f, board, 3, true);
			board->HandledRollbackAMove(move, true);

			if (actualScore < bestScore)
			{
				bestMoves.Empty();
				bestScore = actualScore;
			}

			if (actualScore == bestScore)
			{
				lastBestMove = move;
				bestMoves.Add(lastBestMove);
			}
		}
		/*if (bestMoves.Num() > 0)
		{
			int32 indx = FMath::Rand() % bestMoves.Num();
			return bestMoves[indx];
		}*/
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Minimax expected best score: %f"), bestScore));
		return lastBestMove;//nullptr;//
	}
	else//play as Maximizer
	{
		bestScore = -20000.0f;
		piecesMap = board->GetPieces(WHITE);

		for (auto& piece : piecesMap)
		{
			moves.Append(piece->GetPieceLegalMoves());
		}

		moves.Sort([](const TSharedPtr<Chess_Move>& a, const  TSharedPtr<Chess_Move>& b) {//order in ascending order -> best move (at depth 0) for white first
			return a->MoveValue > b->MoveValue;
			});

		for (TSharedPtr<Chess_Move> move : moves)
		{
			board->HandledMakeAMove(move, true);
			actualScore = AlfaBetaMinimax(-20000.0f, 20000.0f, board, 2, false);
			board->HandledRollbackAMove(move, true);

			if (actualScore > bestScore)
			{
				bestMoves.Empty();
				bestScore = actualScore;
			}

			if (actualScore == bestScore)
			{
				lastBestMove = move;
				bestMoves.Add(lastBestMove);
			}
		}

		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("%f"), bestScore));
		return lastBestMove;
	}
}

float AChess_MinimaxPlayer::EvaluatePieces(AChessboard* board, bool isMax)
{
	if (PlayerColor == ChessColor::NAC) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("EvaluatePieces::Color is not a color"));
	}
	//if (isMax)
	{
		if (board->CheckControl(ChessColor::WHITE))//Minimizer Wins
		{
			if (board->MateControl(ChessColor::WHITE))
			{
				return -10000.0f;
			}
		}
	}
	//else
	{
		if (board->CheckControl(ChessColor::BLACK))//Maximizer Wins
		{
			if (board->MateControl(ChessColor::BLACK))
			{
				return 10000.0f;
			}
		}
	}
	
	float pv = board->WhiteMaterial - board->BlackMaterial;//Material Balance
	//Center Control 
	//Development -> Are Knights and Bishops in the spawn squares?
	//Pawn structure
	//float pv = board->PositionValue;//white material - black material

	return pv;//Black is minimixer, White is maximixer
}

//first call with alfa = -MAX_flt, beta = MAX_flt; WHITE is always Maximizer, BLACK is always Minimizer
float AChess_MinimaxPlayer::AlfaBetaMinimax(float alfa, float beta, AChessboard* board, int32 depth, bool isMax)
{
	float value = EvaluatePieces(board, isMax);
	TArray<AChessPiece*> pieces;
	TArray<TSharedPtr<Chess_Move>> moves = TArray<TSharedPtr<Chess_Move>>();
	bool stall = true;//no legal moves -> stall

	//is terminal:
	if (value == -10000.0f)//MAX under checkmate
	{
		return value;
	}

	if (value == 10000.0f)//MIN under checkmate
	{
		return value;
	}

	if (depth == 0)//approx value:
	{
		return value;
	}


	if (isMax)//Maximizer - White
	{
		value = -20000.0f;//v <- -inf
		pieces = board->GetPieces(WHITE);

		//get all moves (legal and not legal for performance reasons, the legality test is make before do the move)
		for (auto& piece : pieces)
		{
			moves.Append(piece->GetPieceMoves());
		}

		if (moves.IsEmpty())//stall
		{
			return 0;//TODO:change this to a stall value (high value if loosing, low value if winning)
		}

		moves.Sort([](const TSharedPtr<Chess_Move>& a, const  TSharedPtr<Chess_Move>& b) {//order in descending order -> best move for white first to have a better pruning
			return a->MoveValue > b->MoveValue;
			});

		for (TSharedPtr<Chess_Move> move : moves)
		{
			visitedNode++;
			if (move->IsLegal())
			{
				stall = false;
				board->HandledMakeAMove(move, true);
				value = FMath::Max(value, AlfaBetaMinimax(alfa, beta, board, depth - 1, false));
				board->HandledRollbackAMove(move, true);

				if (value >= beta)//Minimizer wont select this path, so prune
				{
					return value;
				}
				alfa = FMath::Max(alfa, value);//Update upperbound
			}
			else {
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Non legal"));
			}
		}

		if (stall)
		{
			return 0;//TODO:change this to a stall value (high value if loosing, low value if winning)
		}

		return value;
	}
	else //Minimizer - Black
	{
		value = 20000.0f;//v <- +inf
		pieces = board->GetPieces(BLACK);

		for (auto& piece : pieces)
		{
			moves.Append(piece->GetPieceMoves());
		}
		
		if (moves.IsEmpty())//stall
		{
			return 0;//TODO:change this to a stall value (low value if loosing, high value if winning)
		}

		moves.Sort([](const TSharedPtr<Chess_Move>& a, const  TSharedPtr<Chess_Move>& b) {//order in ascending order -> best move for black first
			return a->MoveValue < b->MoveValue;
			});
		for (TSharedPtr<Chess_Move> move : moves)
		{
			visitedNode++;
			if (move->IsLegal())
			{
				stall = false;
				board->HandledMakeAMove(move, true);
				value = FMath::Min(value, AlfaBetaMinimax(alfa, beta, board, depth - 1, true));
				board->HandledRollbackAMove(move, true);
				//move->RollbackMove(true);
				if (value <= alfa)//Maximizer wont select this path, so prune
				{
					return value;
				}
				beta = FMath::Min(beta, value);//Update lowerbound
			}
			else {
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Non legal"));
			}
		}

		if (stall)
		{
			return 0;//TODO:change this to a stall value (low value if loosing, high value if winning)
		}

		return value;
	}
}

void AChess_MinimaxPlayer::ResetHandler()
{
	bIsMyTurn = PlayerColor == ChessColor::WHITE;
	bIsMyTurn = bIsMyTurn;
}


