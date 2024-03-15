// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Chess_GameInstance.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/ScrollBox.h"
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

	UFUNCTION()
	void ScoreUpdateHandler();

	UFUNCTION()
	void ResetHandler();

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* ResetButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* AIScoreText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* HumanScoreText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UScrollBox* StoryBoardScrollBox;
	//virtual void BeginPlay() override;
private:
	UChess_GameInstance* GameInstanceRef;
};
