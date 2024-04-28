// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/HorizontalBox.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Blueprint/UserWidget.h"
#include "Chess_StoryBoardEntry.generated.h"

/**
 *
 */
class UChess_UserWidget;

UCLASS()
class CHESS_API UChess_StoryBoardEntry : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	UFUNCTION()
	//method to handle white button click
	void WhiteButtonClickHandler();

	UFUNCTION()
	//method to handle black button click
	void BlackButtonClickHandler();

	//variable to store if a white (black) move is already done to bind events to buttons accordingly and avoid multiple moves in a single turn
	bool bWhiteMoveDone = false;
	bool bBlackMoveDone = false;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* TurnNumberText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* WhiteBackButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* BlackBackButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* WhiteMoveText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* BlackMoveText;

	//method to set the text of the turn number (is the number of the turn shown at left of each row)
	void SetTurnNumberText(FString t);
	//method to set the text of the white move (in the storyboard, the move of the white player is shown at the right of the turn number)
	void SetWhiteMoveText(FString t);
	//method to set the text of the black move (in the storyboard, the move of the black player is shown at the right of the white move)
	void SetBlackMoveText(FString t);
private:
	//Aux method to handle button click events
	void ButtonClickHandler(bool isWhiteButton);
};
