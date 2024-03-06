// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Chessboard.h"
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Chess_GameMode.generated.h"

/**
 * 
 */
UCLASS()
class CHESS_API AChess_GameMode : public AGameModeBase
{
	GENERATED_BODY()

	AChess_GameMode();
	~AChess_GameMode();

	virtual void BeginPlay() override;
	void ChoosePlayerAndStartGame();

	bool IsGameOver;

	//IChess_PieceInterface* selectedPiece;

	// reference to a GameField object
	UPROPERTY(VisibleAnywhere)
	AChessboard* Board;

	// TSubclassOf template class that provides UClass type safety
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AChessboard> BoardClass;
};
