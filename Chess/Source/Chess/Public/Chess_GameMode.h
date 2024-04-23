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
	//Used to select the white player and start the game, used on beginplay and on reset events
	void ChoosePlayerAndStartGame();

public:


	UPROPERTY(BlueprintAssignable)
	//Delegate used to broadcast to all the listeners (chessboard and hudwidget) the move number that has to be replayed
	FOnReplayMove OnReplayMove;

	//Delegate used to notify the HUD widget to show pawn promotion widget
	FOnShowPromotionWidget OnShowPromotionWidget;

	//Delegate used to notify the HUD widget to make clickable or not the changeOpponentButton and the replayButtons (true -> clickable, false -> not clickable)
	FOnTurnSwap OnTurnSwap;

	//Delegate used to notify the HUD widget to update the storyboard with the new move or update the already written move
	FOnMoveUpdate OnMoveUpdate;

	//Delegate used to notify the HUD widget to update the storyboard removing moves until the specified number and the chessboard to rollback to the specified move
	FOnTurnGoBack OnTurnGoBack;

	//Method that notify the HUD to update storyboard with the new move or update the already written move
	void UpdateLastMove(TSharedPtr<Chess_Move> move);

	//Method that notify the HUD to show pawn promotion widget
	void ShowPromotionWidget(ChessColor playerColor);

	//Broadcast HUD and chessboard to rollback chessboard state and HUD to the specified turn
	void GoBackToActualMove();

	//Broadcast HUD and chessboard that turn is passed to the specified player (if true is passed the player is the human player)
	void PlayerSwapNotify(bool IsHumanPlayer);
	
	//Method used to handle the pawn promotion selection
	void SelectedPawnPromotionHandler(CP ChessPieceEnum);

	//Method used to replay the specified move broadcasting the move number to all the listeners
	void ReplayMove(int32 moveNumber);

	//Method used to pass the turn to the next player
	void TurnNextPlayer();

	//Used to know if the game is on replay mode
	bool bIsOnReplay = false;
	//Used to pass the turn to the next player or to notify the game is over
	bool bIsGameOver = false;

	int32 TurnNumber = 1;//Next Turn number
	int32 NextMoveNumber = 1;//Next Move number
	int32 NextActualMoveNumber = 1;//Move displayed on the chessboard

	UPROPERTY(VisibleAnywhere)
	//Chessboard reference to track the game state
	AChessboard* Board;

	UPROPERTY(EditDefaultsOnly)
	//Class of the chessboard to spawn
	TSubclassOf<AChessboard> BoardClass;
private:
	//Auiliar method used to pass the turn to the next player
	void ToggleCurrentPlayer();

	//Method used to control if the game is in checkmate or check, return true if the game is in checkmate, false otherwise
	bool ControlChecks();

	//Method used to control if the game is in stall, return true if the game is in stall, false otherwise
	bool ControlStall();//TODO: Non funziona se ad andare in stallo è il player umano

	UFUNCTION()
	//Method used to reset the game, used on reset event provided by GameInstance
	void ResetHandler();

	//Current player, 0 -> HumanPlayer, 1 -> EnemyPlayer, default is 0 (HumanPlayer is White)
	int32 CurrentPlayer = 0;

	//ausiliar variable used to know if the player has to select the piece promotion, if true the player has to select the piece promotion before to continue
	bool bMustSelectPiecePromotion = false;

	//Difficulty of the enemy player, default is 0 (RandomPlayer), setted at the start of the game by gaminstance
	int32 EnemyDifficulty = 0;//0 -> RandomPlayer, 1 -> Minimax ...

	//Array of players, the first player is the human player, the second player is the enemy player
	TArray<IChess_PlayerInterface*> Players;
};
