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

	//FOnClick OnCLick;
	//UChess_UserWidget* HudRef;

	UFUNCTION()
	void WhiteButtonClickHandler();

	UFUNCTION()
	void BlackButtonClickHandler();

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

	void SetTurnNumberText(FString t);

	void SetWhiteMoveText(FString t);

	void SetBlackMoveText(FString t);
private:
	void ButtonClickHandler(bool isWhiteButton);
	//void SetGoBackButton();

};
