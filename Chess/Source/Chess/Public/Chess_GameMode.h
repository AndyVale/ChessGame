// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "GameFramework/Actor.h"
#include "GameFramework/GameModeBase.h"
#include "Chessboard.h"
#include "Chess_PlayerInterface.h"
#include "Chess_GameMode.generated.h"

DECLARE_DELEGATE_OneParam(FOnShowPromotionWidget, ChessColor);
DECLARE_DELEGATE_TwoParams(FOnMoveUpdate, const FString, int32);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerSwap);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReplayMove, int32, MoveNumber);

UCLASS()
class CHESS_API AChess_GameMode : public AGameModeBase
{
	GENERATED_BODY()

	AChess_GameMode();
	~AChess_GameMode();

	virtual void BeginPlay() override;

	UFUNCTION()
	void ChoosePlayerAndStartGame();

public:
	UPROPERTY(BlueprintAssignable)
	FOnPlayerSwap OnPlayerSwap;

	UPROPERTY(BlueprintAssignable)
	FOnReplayMove OnReplayMove;

	FOnShowPromotionWidget OnShowPromotionWidget;
	FOnMoveUpdate OnMoveUpdate;//TODO: Add binding and put in gamemode(?)
	
	//method that notify the HUD to update storyboard with the new move or update the already written move
	void UpdateLastMove(TSharedPtr<Chess_Move> move);

	//Aggiungere delegato per la mossa in stringa. Il delgato deve avere 2 parametri (stringa e intero nMossa). Viene richiamato attraverso una funzione che ha come parametro la mossa (verrà invocata dai player)
	void ShowPromotionWidget(ChessColor playerColor);

	void SelectedPawnPromotionHandler(CP ChessPieceEnum);

	void ReplayMove(int32 moveNumber);
	
	bool IsGameOver=false;

	void TurnNextPlayer();
	TArray<IChess_PlayerInterface*> Players;

	int32 TurnNumber = 1;
	int32 MoveNumber = 1;

	// reference to a GameField object
	UPROPERTY(VisibleAnywhere)
	AChessboard* Board;

	// TSubclassOf template class that provides UClass type safety
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AChessboard> BoardClass;
private:
	void ToggleCurrentPlayer();

	bool ControlChecks();

	bool ControlStall();

	UFUNCTION()
	void ResetHandler();

	int32 CurrentPlayer = 0;
	bool bMustSelectPiecePromotion = false;

};
