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
    PrimaryActorTick.bCanEverTick = false;
    AutoPossessPlayer = EAutoReceiveInput::Player0;
    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    SetRootComponent(Camera);
    GameInstanceRef = Cast<UChess_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
    if (!GameInstanceRef)
    {
		UE_LOG(LogTemp, Error, TEXT("GameInstanceRef is null in AChess_HumanPlayer"));
	}
    PlayerColor = WHITE;
    bIsMyTurn = false;
}

// Called when the game starts or when spawned
void AChess_HumanPlayer::BeginPlay()
{
    Super::BeginPlay();
    if (AChess_GameMode* gm = Cast<AChess_GameMode>(GetWorld()->GetAuthGameMode()))
    {
        gm->OnReplayMove.AddDynamic(this, &AChess_HumanPlayer::ReplayHandler);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("GameMode is null in AChess_HumanPlayer"));
    }
}

void AChess_HumanPlayer::ReplayHandler(int32 moveNumber) {
    bIsMyTurn = PlayerColor == ChessColor::WHITE ? moveNumber % 2 == 0 : moveNumber % 2 == 1;//if moveNumber is even and I'm white or moveNumber is odd and I'm black is my turn
    SelectedPiece = nullptr;//remove piece selected
    ActiveMoves.Empty();//remove active moves
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
    if (AChess_GameMode* GameMode = Cast<AChess_GameMode>(GetWorld()->GetAuthGameMode()))
    {
        GameMode->PlayerSwapNotify(true);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("GameMode is null in AChess_HumanPlayer OnTurn"));
    }
    GameInstanceRef->SetTurnMessage(PlayerColor == ChessColor::WHITE ? TEXT("White Turn") : TEXT("Black Turn"));
}

void AChess_HumanPlayer::OnWin()
{
    GameInstanceRef->IncrementScoreHumanPlayer();
}

void AChess_HumanPlayer::OnClick()
{
    FHitResult Hit = FHitResult(ForceInit);
    GetWorld()->GetFirstPlayerController()->GetHitResultUnderCursor(ECollisionChannel::ECC_Pawn, true, Hit);

    if (!bIsMyTurn)//if it's not my turn I can't move
    {
        return;
    }

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

    if (Hit.bBlockingHit)//if I click on something and the board is not showing a replay TODO: trova un'altra soluzione
    {
        if (SelectedPiece == nullptr)//Piece is not selected yet
        {
            //Select piece
            if (AChessPiece* CurrPiece = Cast<AChessPiece>(Hit.GetActor()))
            {
                if (CurrPiece->PieceColor == PlayerColor) {
                    SelectedPiece = CurrPiece;
                    ActiveMoves = Board->GetMovesAndShowHints(CurrPiece, true);
                }
                else {
                    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("You have to select a piece of your color!"));
                }
            }
        }
        else
        {
            //Move piece (or eat)
            AActor* CurrClicked = nullptr;//using an AActor beacuse no 'special' methods are used here

            //try to cast to a chess piece, if it fails cast to a square, if it fails return
            Cast<AChessPiece>(Hit.GetActor()) ? CurrClicked = Cast<AChessPiece>(Hit.GetActor()) : CurrClicked = Cast<ASquare>(Hit.GetActor());
            if (!CurrClicked)
            {
                return;//clicked on background or something else
            }

            //get the old location of the piece and the location of the clicked piece/square
            FVector2D oldLoc = Board->GetXYPositionByRelativeLocation(SelectedPiece->GetActorLocation());
            FVector2D newLoc = Board->GetXYPositionByRelativeLocation(CurrClicked->GetActorLocation());


            //Move handling:
            TSharedPtr<Chess_Move> move = nullptr;

            if (oldLoc == newLoc)//Undo move
            {
                GameMode->Board->CancelFeasibleSquares();
                ActiveMoves.Empty();
                SelectedPiece = nullptr;
                return;
            }

            //search for the move in the active moves, if it's not found the move is not valid and the move still nullptr
            for (TSharedPtr<Chess_Move> tmpMove : ActiveMoves)
            {
                if (tmpMove->From == oldLoc && tmpMove->To == newLoc)
                {
                    move = tmpMove;
                    break;
                }
            }


            if (GameMode->bIsOnReplay && move)
            {
                GameMode->GoBackToActualMove();//Remove history board after the performed move
            }

            if (CurrClicked && move)//if the move is valid
            {
                Board->HandledMakeAMove(move, false);
                GameMode->UpdateLastMove(move);
                bIsMyTurn = false;
            }
            //else undo the move

            //in anycase remove the active moves and the selected piece
            GameMode->Board->CancelFeasibleSquares();
            ActiveMoves.Empty();
            SelectedPiece = nullptr;

            if (!bIsMyTurn)//if the move was valid bIsMyTurn is false
            {
                GameMode->TurnNextPlayer();
            }
        }
    }
}
