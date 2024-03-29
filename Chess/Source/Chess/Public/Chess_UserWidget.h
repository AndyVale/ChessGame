// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Chess_StoryBoardEntry.h"
#include "Chess_GameInstance.h"
#include "Chess_GameMode.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "Components/TextBlock.h"
#include "Components/ScrollBox.h"
#include "Chess_PawnPromotionWidget.h"
//#include "Components/ListView.h"
#include "Chess_UserWidget.generated.h"

/**
 * 
 */


UCLASS()
class CHESS_API UChess_UserWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TSubclassOf<UChess_StoryBoardEntry> StoryBoardEntry;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* ResetButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* RematchButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* AIScoreText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* HumanScoreText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* CurrentPlayerText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UScrollBox* StoryBoardScrollBox;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UChess_PawnPromotionWidget* PawnPromotionWidget;

	UFUNCTION()
	void ReplayMove(int32 moveNumber);

	UFUNCTION()
	void SetPromotionWidgetVisible(ChessColor playerColor);

	void RemoveLastMove();
private:
	UChess_GameInstance* GameInstanceRef;

	AChess_GameMode* GameMode;

	TArray<UChess_StoryBoardEntry*> TurnsHistory;

	UFUNCTION()
	void ResetOnClick();

	UFUNCTION()
	void RematchOnClick();

	UFUNCTION()
	void ScoreUpdateHandler();

	UFUNCTION()
	void ResetHandler();

	UFUNCTION()
	void MoveHandler(FString stringMove, int32 moveNumber);

	//Note: Button click is handled in Chess_StoryBoardEntry
	
	UFUNCTION()
	void ShowPlayerHandler();
};
