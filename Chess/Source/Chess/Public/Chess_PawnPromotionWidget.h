// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Chess_PawnPromotionWidget.generated.h"

/**
 *
 */
UCLASS()
class CHESS_API UChess_PawnPromotionWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* BishopButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* RookButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* QueenButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* KnightButton;
private:
	UFUNCTION()
	void clickOnBishop();

	UFUNCTION()
	void clickOnRook();

	UFUNCTION()
	void clickOnQueen();

	UFUNCTION()
	void clickOnKnight();

	/*
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTexture* WhiteBishopTexture;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTexture* BlackBishopTexture;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTexture* WhiteRookTexture;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTexture* BlackRookTexture;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTexture* WhiteQueenTexture;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTexture* BlackQueenTexture;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTexture* WhiteKnightTexture;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTexture* BlackKnightTexture;*/
};
