// Fill out your copyright notice in the Description page of Project Settings.


#include "Chess_HumanPlayer.h"
#include "Square.h"
#include "Chess_GameMode.h"
#include "ChessPiece.h"
#include <EnhancedInputComponent.h>

// Sets default values
AChess_HumanPlayer::AChess_HumanPlayer()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessPlayer = EAutoReceiveInput::Player0;
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("DAJE"));

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	SetRootComponent(Camera);
	//GameInstance = Cast<UChessGameInstance>(UGameplayStatics::GetGameInstance(G
	Color = PlayerColor::WHITE;
}

// Called when the game starts or when spawned
void AChess_HumanPlayer::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AChess_HumanPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AChess_HumanPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AChess_HumanPlayer::OnTurn()
{
	bIsMyTurn = true;
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("UUAAANM"));
	//GameInstance->SetTurnMessage(TEXT("Human Turn"));
}

void AChess_HumanPlayer::OnWin()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("You Win!"));
	//GameInstance->SetTurnMessage(TEXT("Human Wins!"));
	//GameInstance->IncrementScoreHumanPlayer();
}

void AChess_HumanPlayer::OnLose()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("You Lose!"));
	//GameInstance->SetTurnMessage(TEXT("Human Loses!"));
}

void AChess_HumanPlayer::OnClick()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Onclick"));
    FHitResult Hit = FHitResult(ForceInit);
    GetWorld()->GetFirstPlayerController()->GetHitResultUnderCursor(ECollisionChannel::ECC_Pawn, true, Hit);

    if (!bIsMyTurn)
        return;

    //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("TUO TURNOOOOOOOOOOO"));

    AChess_GameMode* GameMode = Cast<AChess_GameMode>(GetWorld()->GetAuthGameMode());
    if (GameMode == nullptr) {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("GameMode is null in OnClick function"));
        return;
    }

    AChessboard* Board = GameMode->Board;
    if (Board == nullptr) {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Board is null in OnClick function"));
        return;
    }

    if (Hit.bBlockingHit)
    {
        FVector tempLoc;
        if (AChessPiece* CurrPiece = Cast<AChessPiece>(Hit.GetActor()))
        {
            HandlePieceClick(GameMode, Board, CurrPiece);
        }
        else if (ASquare* CurrSquare = Cast<ASquare>(Hit.GetActor()))
        {
            HandleSquareClick(GameMode, Board, CurrSquare);
        }
    }
}

void AChess_HumanPlayer::HandlePieceClick(AChess_GameMode* GameMode, AChessboard* Board, AChessPiece* CurrPiece)
{
    if (GameMode->SelectedPiece == nullptr) { // select
        if (CurrPiece->PieceColor == MyColor) {
            GameMode->SelectedPiece = CurrPiece;
            Board->ShowFeasibleMoves(GameMode->SelectedPiece);
        }
        else {
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Your color is white"));
        }
    }
    else { // eat or delete
        if (Board->MakeASafeMove(Board->GetXYPositionByRelativeLocation(GameMode->SelectedPiece->GetActorLocation()), Board->GetXYPositionByRelativeLocation(CurrPiece->GetActorLocation())))
        {
            bIsMyTurn = false;
        }
        Board->CancelFeasibleMoves();
        GameMode->SelectedPiece = nullptr;
        if (!bIsMyTurn)
            GameMode->TurnNextPlayer();
    }
}

void AChess_HumanPlayer::HandleSquareClick(AChess_GameMode* GameMode, AChessboard* Board, ASquare* CurrSquare)
{
    if (GameMode->SelectedPiece == nullptr) { // illegal move
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("You have to select a piece of your color"));
    }
    else { // move
        if (Board->MakeASafeMove(Board->GetXYPositionByRelativeLocation(GameMode->SelectedPiece->GetActorLocation()), Board->GetXYPositionByRelativeLocation(CurrSquare->GetActorLocation())))
        {
            bIsMyTurn = false;
        }
        Board->CancelFeasibleMoves();
        GameMode->SelectedPiece = nullptr;
        if (!bIsMyTurn)
            GameMode->TurnNextPlayer();
    }
}

