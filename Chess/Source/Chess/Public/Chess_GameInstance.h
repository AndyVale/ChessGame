// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Chessboard.h"
#include "Engine/GameInstance.h"
#include "Chess_GameInstance.generated.h"

//Dal momento che i punteggi sono sempre memorizzati nella gameinstance ho deciso di mettere qui il delagato per l'invio dell'informazione riguardante il loro aggiornamento (incremento e reset).
//questo nell'ottica di poter aggiungere in un secondo momento diversi widget e diverse gamemode che quindi perderebbero efficacia nell'invio di messaggi nei confronti della gameinstance che invece
//rimane sempre costante.

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnScoreUpdate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReset);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRematch);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMessageChange);


UCLASS()
class CHESS_API UChess_GameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//Game difficulty, -1 for no difficulty selected, 0 for random, 1 for minimax. Setted in the main menu
	int32 GameDifficulty = -1;
	
	UPROPERTY(BlueprintAssignable)
	//delegate for reset event used by ResetPointsAndGame, it's used to reset the points and the game without changing the difficulty
	FOnReset OnResetEvent;

	UPROPERTY(BlueprintAssignable)
	//delegate for score update event used by IncrementScoreHumanPlayer and IncrementScoreAIPlayer, it's used to update the score
	FOnScoreUpdate OnScoreUpdate;

	UPROPERTY(BlueprintAssignable)
	//delegate for rematch event used by ResetGame, it's used to reset the game without changing the difficulty
	FOnRematch OnRematch;

	UPROPERTY(BlueprintAssignable)
	//delegate for message change event used by SetTurnMessage, it's used to update the message in the HUD
	FOnMessageChange OnMessageChange;

	UFUNCTION(BlueprintCallable)
	//Set the turn message and broadcast to update the HUD
	void SetTurnMessage(FString Message);

	UFUNCTION(BlueprintCallable)
	//Start a new game resetting points
	void ResetPointsAndGame();

	UFUNCTION(BlueprintCallable)
	//Start a new game without resetting points
	void ResetGame();

	//Increment the score for human player and broadcast to update points
	void IncrementScoreHumanPlayer();

	//Increment the score for AI player and broadcast to update points
	void IncrementScoreAIPlayer();

	//Get the score of human player
	int32 GetScoreHumanPlayer();

	//Get the score of AI player
	int32 GetScoreAIPlayer();

	//Get the current turn message
	FString GetTurnMessage();
	
private:
	//Broadcast the reset event
	//UFUNCTION(BlueprintCallable)
	void ResetSignal();

	UFUNCTION(BlueprintCallable)
	void UpdateScoreSignal();

	//Message for the HUD: it's updated by the function SetTurnMessage
	FString CurrentTurnMessage;

	//Points for the human player and the AI player: they are updated by the functions IncrementScoreHumanPlayer and IncrementScoreAIPlayer and ResetPointsAndGame
	int32 ScoreHumanPlayer = 0;
	int32 ScoreAIPlayer = 0;
};
