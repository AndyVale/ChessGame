// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "GameFramework/Actor.h"
#include "GameFramework/GameModeBase.h"
#include "Chessboard.h"
#include "Chess_PlayerInterface.h"
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
public:
	bool IsGameOver;

	TArray<IChess_PlayerInterface*> Players;

	// reference to a GameField object
	UPROPERTY(VisibleAnywhere)
	AChessboard* Board;

	//UPROPERTY(VisibleAnywhere)
	//  AChessPiece* SelectedPiece;

	// TSubclassOf template class that provides UClass type safety
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AChessboard> BoardClass;
};
