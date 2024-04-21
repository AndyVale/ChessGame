// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "GameFramework/Actor.h"
#include "GameFramework/GameModeBase.h"
#include "Chessboard.h"
#include "Chess_PlayerInterface.h"
#include "Chess_GameMode.generated.h"

DECLARE_DELEGATE_OneParam(FOnShowPromotionWidget, ChessColor);//delegate used to notify Chess_UserWidget to show pawnpromotion HUD
DECLARE_DELEGATE_OneParam(FOnTurnSwap, bool);//Used to notify Chess_UserWidget to make it clickable (true) or not (false) the changeOpponentButton
DECLARE_DELEGATE_TwoParams(FOnMoveUpdate, const FString, int32);//notify Chess_UserWidget to update storyboard at the specified move number with the specified string
DECLARE_MULTICAST_DELEGATE_OneParam(FOnTurnGoBack, int32);//notify Chess_UserWidget to update storyboard removign moves until number specified


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
	FOnReplayMove OnReplayMove;

	FOnShowPromotionWidget OnShowPromotionWidget;
	FOnTurnSwap OnTurnSwap;
	FOnMoveUpdate OnMoveUpdate;//TODO: Add binding and put in gamemode(?)
	FOnTurnGoBack OnTurnGoBack;

	//method that notify the HUD to update storyboard with the new move or update the already written move
	void UpdateLastMove(TSharedPtr<Chess_Move> move);

	//Aggiungere delegato per la mossa in stringa. Il delgato deve avere 2 parametri (stringa e intero nMossa). Viene richiamato attraverso una funzione che ha come parametro la mossa (verrà invocata dai player)
	void ShowPromotionWidget(ChessColor playerColor);

	//Broadcast HUD and chessboard to rollback chessboard state and HUD to the specified turn
	void GoBackToActualMove();

	void PlayerSwapNotify(bool IsHumanPlayer);

	void SelectedPawnPromotionHandler(CP ChessPieceEnum);

	void ReplayMove(int32 moveNumber);

	bool bIsOnReplay = false;
	bool bIsGameOver = false;

	int32 EnemyDifficulty = 0;//0 -> RandomPlayer, 1 -> Minimax ...

	void TurnNextPlayer();
	TArray<IChess_PlayerInterface*> Players;

	int32 TurnNumber = 1;//Next Turn number
	int32 NextMoveNumber = 1;//Next Move number
	int32 NextActualMoveNumber = 1;//Move displayed on the chessboard

	// reference to a GameField object
	UPROPERTY(VisibleAnywhere)
	AChessboard* Board;

	// TSubclassOf template class that provides UClass type safety
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AChessboard> BoardClass;
private:
	void ToggleCurrentPlayer();

	bool ControlChecks();

	bool ControlStall();//TODO: Non funziona se ad andare in stallo è il player umano

	UFUNCTION()
	void ResetHandler();

	int32 CurrentPlayer = 0;
	bool bMustSelectPiecePromotion = false;

};
