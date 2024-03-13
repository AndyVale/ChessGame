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
    FHitResult Hit = FHitResult(ForceInit);
    GetWorld()->GetFirstPlayerController()->GetHitResultUnderCursor(ECollisionChannel::ECC_Pawn, true, Hit);

    if (!bIsMyTurn)
        return;

    AChess_GameMode* GameMode = Cast<AChess_GameMode>(GetWorld()->GetAuthGameMode());
    if (GameMode == nullptr) {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("GameMode is null in OnClick function"));
        return;
    }

    AChessboard* Board = GameMode->Board;
    if (Board == nullptr) {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Board is null in OnClick function"));
        return;
    }

    if (Hit.bBlockingHit)
    {
        if(SelectedPiece == nullptr)
        {
            if (AChessPiece* CurrPiece = Cast<AChessPiece>(Hit.GetActor()))
            {
                //select piece
                if (CurrPiece->PieceColor == MyColor) {
                    SelectedPiece = CurrPiece;
                    Board->GetFeasibleMoves(SelectedPiece, true);
                }
                else {
                    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("You have to select a piece of your color!"));
                }
            }
           /* else if(ASquare* CurrSquare = Cast<ASquare>(Hit.GetActor()))
            {
                //illegal
                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("You have to select a piece"));
            }*/
        }
        else
        {
            AActor* CurrClicked = nullptr;//using an AActor beacuse no special methods are used here
            Cast<AChessPiece>(Hit.GetActor()) ? CurrClicked = Cast<AChessPiece>(Hit.GetActor()) : CurrClicked = Cast<ASquare>(Hit.GetActor());

            if (CurrClicked && Board->MakeASafeMove(Board->GetXYPositionByRelativeLocation(SelectedPiece->GetActorLocation()), Board->GetXYPositionByRelativeLocation(CurrClicked->GetActorLocation())))
            {
                bIsMyTurn = false;
            }
            Board->CancelFeasibleMoves();
            SelectedPiece = nullptr;
            if (!bIsMyTurn)
                GameMode->TurnNextPlayer();
        }
    }
}
