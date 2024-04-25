// Fill out your copyright notice in the Description page of Project Settings.


#include "Chess_PawnPromotionWidget.h"
#include "Chess_GameMode.h"
#include "Chessboard.h"

void UChess_PawnPromotionWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // Bind the buttons to the functions
    BishopButton->OnClicked.AddDynamic(this, &UChess_PawnPromotionWidget::clickOnBishop);
    RookButton->OnClicked.AddDynamic(this, &UChess_PawnPromotionWidget::clickOnRook);
    QueenButton->OnClicked.AddDynamic(this, &UChess_PawnPromotionWidget::clickOnQueen);
    KnightButton->OnClicked.AddDynamic(this, &UChess_PawnPromotionWidget::clickOnKnight);
}

void UChess_PawnPromotionWidget::clickOnBishop()
{
    if (AChess_GameMode* GameMode = Cast<AChess_GameMode>(GetWorld()->GetAuthGameMode())) {
        GameMode->SelectedPawnPromotionHandler(CP::BISHOP);
    }
    this->SetVisibility(ESlateVisibility::Hidden);
}

void UChess_PawnPromotionWidget::clickOnRook()
{
    if (AChess_GameMode* GameMode = Cast<AChess_GameMode>(GetWorld()->GetAuthGameMode())) {
        GameMode->SelectedPawnPromotionHandler(CP::ROOK);
    }
    this->SetVisibility(ESlateVisibility::Hidden);
}

void UChess_PawnPromotionWidget::clickOnQueen()
{
    if (AChess_GameMode* GameMode = Cast<AChess_GameMode>(GetWorld()->GetAuthGameMode())) {
        GameMode->SelectedPawnPromotionHandler(CP::QUEEN);
    }
    this->SetVisibility(ESlateVisibility::Hidden);
}

void UChess_PawnPromotionWidget::clickOnKnight()
{
    if (AChess_GameMode* GameMode = Cast<AChess_GameMode>(GetWorld()->GetAuthGameMode())) {
        GameMode->SelectedPawnPromotionHandler(CP::KNIGHT);
    }
    this->SetVisibility(ESlateVisibility::Hidden);
}
