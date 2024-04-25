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
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//Overrides from IChess_PlayerInterface
	virtual void OnTurn() override;
	virtual void OnWin() override;

	//Wrapper function to call the minimax algorithm
	void MakeMinimaxMove();
private:
	//Minimax algorithm functions:
	TSharedPtr<Chess_Move> FindBestMove(AChessboard* board);
	float EvaluatePieces(AChessboard* board, bool isMax);
	//alphabeta minimax algorithm, must be initialized with -MAX_FLOAT and +MAX_FLOAT in alfa and beta respectively, depth is the depth of the tree to be explored, isMax = True if the player is WHITE, False if the player is the BLACK
	float AlfaBetaMinimax(float alfa, float beta, AChessboard* board, int32 depth, bool isMax);

	UFUNCTION()
	//Reset handler binded to the reset event
	void ResetHandler();

	//reference to the game instance to avoid casting every time (used to bind events and select the depth of minimax (work in progress))
	UChess_GameInstance* GameInstanceRef;
};