// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Chess_PlayerInterface.h"
#include "Chessboard.h"
#include "ChessPiece.h"
#include "Chess_GameMode.h"
#include "Chess_MinimaxPlayer.generated.h"
class Chess_Move;
UCLASS()
class CHESS_API AChess_MinimaxPlayer : public APawn, public IChess_PlayerInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AChess_MinimaxPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	ChessColor Color = ChessColor::BLACK;
	UChess_GameInstance* GameInstanceRef;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void OnTurn() override;
	virtual void OnWin() override;
	virtual void OnLose() override;
	void MakeMinimaxMove();
private:
	TSharedPtr<Chess_Move> FindBestMove(AChessboard* board);
	float EvaluatePieces(AChessboard* board, bool isMax);
	//int32 Minimax(AChessboard* board, int32 depth, bool isMax);
	float AlfaBetaMinimax(float alfa, float beta, AChessboard* board, int32 depth, bool isMax);
};