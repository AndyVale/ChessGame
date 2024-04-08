// Fill out your copyright notice in the Description page of Project Settings.


#include "Chess_MinimaxPlayer.h"
int32 nodeVisited = 0;

// Sets default values
AChess_MinimaxPlayer::AChess_MinimaxPlayer()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	GameInstanceRef = Cast<UChess_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
}

// Called when the game starts or when spawned
void AChess_MinimaxPlayer::BeginPlay()
{
	Super::BeginPlay();

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
	//bMyTurn = true;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [&] { AChess_MinimaxPlayer::MakeMinimaxMove(); }, 0.001, false);
}

void AChess_MinimaxPlayer::OnWin()
{
	GameInstanceRef->SetTurnMessage(TEXT("Minimax player Wins!"));
	GameInstanceRef->IncrementScoreAIPlayer();
}

void AChess_MinimaxPlayer::OnLose()
{
}

void AChess_MinimaxPlayer::MakeMinimaxMove()
{
	AChess_GameMode* GM = Cast<AChess_GameMode>(GetWorld()->GetAuthGameMode());

	TSharedPtr<Chess_Move> miniMaxMove = FindBestMove(GM->Board);
	int32 a = nodeVisited;
	if (miniMaxMove)
	{
		GM->Board->MakeAMove(miniMaxMove, false);
		if (miniMaxMove->bPromotionAfterMove) {
			TArray<CP> randomPromotedPiece = { CP::BISHOP, CP::KNIGHT, CP::ROOK, CP::QUEEN };
			int32 randomPromotionIndx = 3;//FMath::Rand() % randomPromotedPiece.Num();
			GM->Board->PromoteLastMove(randomPromotedPiece[randomPromotionIndx]);
		}
	}
	else
	{
		//stallo
	}


	if (miniMaxMove) {
		GM->UpdateLastMove(miniMaxMove);//notify the HUD of the move
	}
	GM->TurnNextPlayer();
}

TSharedPtr<Chess_Move> AChess_MinimaxPlayer::FindBestMove(AChessboard* board)
{
	float bestScore;
	float actualScore;
	TArray< TSharedPtr<Chess_Move>> bestMoves = TArray< TSharedPtr<Chess_Move>>();
	TSharedPtr<Chess_Move> bestMove = nullptr;
	TMap<AChessPiece*, FVector2D> piecesMap;
	TArray<Chess_Move> moves = TArray<Chess_Move>();

	if (Color == ChessColor::BLACK)//play as Minimizer
	{
		bestScore = +20000.0f;
		piecesMap = board->GetPieces(BLACK);
		for (auto& piece_xy : piecesMap)
		{
			for (FVector2D& xy : board->GetFeasibleSquares(piece_xy.Key, false))
			{
				moves.Add(Chess_Move(piece_xy.Key->PiecePosition, xy, board));
			}
		}

		moves.Sort([](const Chess_Move& a, const  Chess_Move& b) {//order in descending order -> best move for black first
			return a.MoveValue < b.MoveValue;
			});

		for (Chess_Move& move : moves)
		{
			move.MakeMove(true);
			if (move.bPromotionAfterMove)
			{
				move.PromotePawn(true, board->Queen);
			}
			actualScore = AlfaBetaMinimax(-20000.0f, 20000.0f, board, 3, true);
			move.RollbackMove(true);
			if (actualScore < bestScore)
			{
				bestMoves.Empty();
				bestMove = MakeShareable<Chess_Move>(new Chess_Move(move));//new Chess_Move(move);
				bestScore = actualScore;
			}
			if (actualScore == bestScore)
			{
				bestMoves.Add(bestMove);
			}
		}
		/*if (bestMoves.Num() > 0)
		{
			int32 indx = FMath::Rand() % bestMoves.Num();
			return bestMoves[indx];
		}*/
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("%f"), bestScore));
		return bestMove;//nullptr;//
	}
	else//play as Maximizer
	{
		bestScore = -20000.0f;
		piecesMap = board->GetPieces(WHITE);
		for (auto& piece_xy : piecesMap)
		{
			for (FVector2D& xy : board->GetFeasibleSquares(piece_xy.Key, false))
			{
				Chess_Move move = Chess_Move(piece_xy.Key->PiecePosition, xy, board);
				move.MakeMove(true);
				if (move.bPromotionAfterMove)
				{
					move.PromotePawn(true, board->Queen);
				}
				actualScore = AlfaBetaMinimax(-20000.0f, 20000.0f, board, 2, false);
				move.RollbackMove(true);
				if (actualScore >= bestScore)
				{
					bestMove = MakeShareable<Chess_Move>(new Chess_Move(move));//new Chess_Move(move);
					bestScore = actualScore;
				}
			}
		}
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("%f"), bestScore));
		return bestMove;
	}
}

float AChess_MinimaxPlayer::EvaluatePieces(AChessboard* board)
{
	if (Color == ChessColor::NAC) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("EvaluatePieces::Color is not a color"));
	}

	if (board->CheckControl(ChessColor::BLACK))//Maximizer Wins
	{
		if (board->MateControl(ChessColor::BLACK))
		{
			return 10000.0f;
		}
	}

	if (board->CheckControl(ChessColor::WHITE))//Minimizer Wins
	{
		if (board->MateControl(ChessColor::WHITE))
		{
			return -10000.0f;
		}
	}

	float pv = board->WhiteMaterial - board->BlackMaterial;//Material Balance
	//Center Control 
	//Development -> Are Knights and Bishops in the spawn squares?
	//Pawn structure
	//float pv = board->PositionValue;//white material - black material
	//if (pv1 != pv)
	//{
	//	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Ops"));
	//}
	return pv;//Black is minimixer, White is maximixer
}

//first call with alfa = -MAX_flt, beta = MAX_flt; WHITE is always Maximizer, BLACK is always Minimizer
float AChess_MinimaxPlayer::AlfaBetaMinimax(float alfa, float beta, AChessboard* board, int32 depth, bool isMax)
{
	
	float value = EvaluatePieces(board);
	TMap<AChessPiece*, FVector2D> piecesMap;
	TArray<Chess_Move> moves = TArray<Chess_Move>();

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
		piecesMap = board->GetPieces(WHITE);
		for (auto& piece_xy : piecesMap)
		{
			for (FVector2D& xy : board->GetFeasibleSquares(piece_xy.Key, false))
			{
				moves.Add(Chess_Move(piece_xy.Key->PiecePosition, xy, board));
			}
		}

		if (moves.IsEmpty())//stall
		{
			return 0;
		}

		moves.Sort([](const Chess_Move& a, const  Chess_Move& b) {//order in descending order -> best move for white first
			return a.MoveValue > b.MoveValue;
			});

		for (Chess_Move& move : moves)
		{
			move.MakeMove(true);
			if (move.bPromotionAfterMove)
			{
				move.PromotePawn(true, board->Queen);
			}
			value = FMath::Max(value, AlfaBetaMinimax(alfa, beta, board, depth - 1, false));
			move.RollbackMove(true);
			if (value >= beta)//Minimizer wont select this path, so prune
			{
				return value;
			}
			alfa = FMath::Max(alfa, value);//Update upperbound
		}
		return value;
	}
	else //Minimizer - Black
	{
		value = 20000.0f;//v <- +inf
		piecesMap = board->GetPieces(BLACK);

		for (auto& piece_xy : piecesMap)
		{
			for (FVector2D& xy : board->GetFeasibleSquares(piece_xy.Key, false))
			{
				moves.Add(Chess_Move(piece_xy.Key->PiecePosition, xy, board));
			}
		}

		if (moves.IsEmpty())//stall
		{
			return 0;
		}

		moves.Sort([](const Chess_Move& a, const  Chess_Move& b) {//order in ascending order -> best move for black first
			return a.MoveValue < b.MoveValue;
			});

		for (Chess_Move& move : moves)
		{
			move.MakeMove(true);
			if (move.bPromotionAfterMove)
			{
				move.PromotePawn(true, board->Queen);
			}
			value = FMath::Min(value, AlfaBetaMinimax(alfa, beta, board, depth - 1, true));
			move.RollbackMove(true);
			if (value <= alfa)//Maximizer wont select this path, so prune
			{
				return value;
			}
			beta = FMath::Min(beta, value);//Update lowerbound
		}
		return value;
	}
}