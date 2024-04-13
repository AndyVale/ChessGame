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


UCLASS()
class CHESS_API UChess_GameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 GameDifficulty = -1;
	
	UPROPERTY(BlueprintAssignable)
	FOnReset OnResetEvent;

	UPROPERTY(BlueprintAssignable)
	FOnScoreUpdate OnScoreUpdate;

	UPROPERTY(BlueprintAssignable)
	FOnRematch OnRematch;

	UPROPERTY(EditAnywhere)
	int32 ScoreHumanPlayer = 0;

	// score value for AI player
	UPROPERTY(EditAnywhere)
	int32 ScoreAIPlayer = 0;

	// message to show every turn
	UPROPERTY(EditAnywhere)
	FString CurrentTurnMessage = "Current Player";

	// increment the score for human player and broadcast to update points
	void IncrementScoreHumanPlayer();

	// increment the score for AI player and broadcast to update points
	void IncrementScoreAIPlayer();

	// get the score for human player
	int32 GetScoreHumanPlayer();

	// get the score for AI player
	int32 GetScoreAIPlayer();

	// get the current turn message
	UFUNCTION(BlueprintCallable)
	FString GetTurnMessage();

	// set the turn message
	UFUNCTION(BlueprintCallable)
	void SetTurnMessage(FString Message);

	UFUNCTION(BlueprintCallable)//start a new game resetting points
		void ResetPointsAndGame();

	UFUNCTION(BlueprintCallable)//start a new game without resetting points
		void ResetGame();
private:
	// score value for human players
	//UFUNCTION(BlueprintCallable)
	void ResetSignal();

	UFUNCTION(BlueprintCallable)
	void UpdateScoreSignal();
};
