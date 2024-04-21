// Fill out your copyright notice in the Description page of Project Settings.

#include "Chess_Move.h"
#include "ChessPiece.h"
#include "Square.h"
#include "Chessboard.h"
#include "CP_Bishop.h"
#include "CP_King.h"
#include "CP_Knight.h"
#include "CP_Pawn.h"
#include "CP_Queen.h"
#include "CP_Rook.h"

Chess_Move::Chess_Move(FVector2D f, FVector2D t, AChessboard* board) : 
    PlayerOnCheck(ChessColor::NAC), PlayerOnCheckMate(ChessColor::NAC), PlayerOnStall(ChessColor::NAC), From(f), To(t), ReferredBoard(board)
{
    CapturingPiece = board->GetPieceFromXY(f);
    if (CapturingPiece == nullptr)
    {
        int a = 5;
    }
    CapturedPiece = board->GetPieceFromXY(t);
    MoveClass = STANDARD;
    //PieceLetter = GetPieceLetter(PieceFrom);
    //if the piece get the opposite end of the board
    if (GetMoveColor() == ChessColor::BLACK && To.Y == ReferredBoard->BoardSize - 1 || GetMoveColor() == ChessColor::WHITE && To.Y == 0)
    {
        if (ACP_Pawn* pawn = Cast<ACP_Pawn>(CapturingPiece))//and is a pawn->needs a promotion
        {
            MoveClass = MoveType::PAWN_PROMOTION;
        }
    }
    MoveValue = MoveValueCalculation();
}

Chess_Move::Chess_Move(FVector2D f, FVector2D t, AChessboard* board, FVector2D enPassantCapturePosition) : 
    PlayerOnCheck(ChessColor::NAC), PlayerOnCheckMate(ChessColor::NAC), PlayerOnStall(ChessColor::NAC), From(f), To(t), ReferredBoard(board)
{
    MoveClass = EN_PASSANT;
    CapturingPiece = board->GetPieceFromXY(f);
    if (CapturingPiece == nullptr)
    {
        int a = 5;
    }
    CapturedPiece = board->GetPieceFromXY(t);
    EnPassantCapturedPiece = board->GetPieceFromXY(enPassantCapturePosition);
    MoveValue = MoveValueCalculation();
}


Chess_Move::Chess_Move(FVector2D f, FVector2D t, AChessboard* board, bool isQueenSide) :
    PlayerOnCheck(ChessColor::NAC), PlayerOnCheckMate(ChessColor::NAC), PlayerOnStall(ChessColor::NAC), From(f), To(t), ReferredBoard(board)
{
    MoveClass = CASTLE;
    CapturingPiece = board->GetPieceFromXY(f);
    if (CapturingPiece == nullptr)
    {
        int a = 5;
    }
    CapturedPiece = board->GetPieceFromXY(t);
    MoveValue = MoveValueCalculation();
    bIsQueenSide = isQueenSide;
}



void Chess_Move::CalculateResult()
{
    ChessColor OpponentColor = CapturingPiece->PieceColor == WHITE ? BLACK : WHITE;

    if (ReferredBoard->CheckControl(OpponentColor))
    {
        PlayerOnCheck = OpponentColor;
        if (ReferredBoard->MateControl(OpponentColor))
        {
            PlayerOnCheckMate = OpponentColor;
        }
    }
    else
    {
        if (ReferredBoard->StallControl(OpponentColor))
        {
            PlayerOnStall = OpponentColor;
        }
    }
}

void Chess_Move::MakeMove(bool simulate)
{
    //TODO CONTROLLA SIA TUTTO GIUSTO
    if (!CapturingPiece)
    {
        UE_LOG(LogTemp, Error, TEXT("MakeAMove:Impossible move, no pieces in old position"));
        return;
    }

    //Making the move:
    ReferredBoard->RemovePiece(CapturingPiece);
    if (CapturedPiece)
    {
        ReferredBoard->RemovePiece(CapturedPiece);//remove NewPiece
    }
    ReferredBoard->SetPieceFromXY(To, CapturingPiece);

    //Moving the piece
    if (!simulate)//not a simulation, move the piece
    {
        if (CapturedPiece)
        {
            if (CapturedPiece->PieceColor == BLACK)//TODO:RIARRANGIA
                CapturedPiece->SetActorLocation(ReferredBoard->GetRelativeLocationByXYPosition(-1, -1));
            else
                CapturedPiece->SetActorLocation(ReferredBoard->GetRelativeLocationByXYPosition(8, 8));
            CapturedPiece->SetActorHiddenInGame(true);
        }
        (CapturingPiece)->SetActorLocation(ReferredBoard->GetRelativeLocationByXYPosition(To[0], To[1]));
    }

    //CalculateResult test for check, checkmate and stall. Is importat to avoid this procedure in case it's a simulative move (for performance reasons)
    if (MoveClass == MoveType::PAWN_PROMOTION)
    {
        PromotePawn(simulate, nullptr);
    }

    if (MoveClass == MoveType::EN_PASSANT)
    {
        EnPassantCapture(simulate);
    }

    if (MoveClass == MoveType::CASTLE)
    {
        CastleMove(simulate); //TODO //IL MINIMAX NON FUNZIONA CON L'ARROCO, alcune CastledRook risultano null (inconsistenza tra arrocchi possibili ed effettivamente fattibili)
    }

    if (!simulate)
    {
        CalculateResult();
    }

    UpdateCastleVariables();
}


void Chess_Move::RollbackMove(bool simulate)
{
    if (MoveClass == MoveType::CASTLE)
    {
        CastleMoveRollback(simulate);
    }

    if (MoveClass == MoveType::EN_PASSANT)
    {
        EnPassantCaptureRollback(simulate);
    }

    if (MoveClass == MoveType::PAWN_PROMOTION && PawnPromotionAusRef)//if move was a promotion and was already done
    {
        PromotePawnRollback(simulate);
    }

    //ReferredBoard->RemovePiece(CapturedPiece);
    ReferredBoard->RemovePiece(CapturingPiece);

    ReferredBoard->SetPieceFromXY(To, CapturedPiece);
    ReferredBoard->SetPieceFromXY(From, CapturingPiece);


    if (CapturedPiece)
    {
        if (!simulate)
        {
            CapturedPiece->SetActorHiddenInGame(false);
            CapturedPiece->SetActorLocation(ReferredBoard->GetRelativeLocationByXYPosition(To[0], To[1]));
        }
    }

    if (CapturingPiece) {

        if (!simulate)
        {
            CapturingPiece->SetActorHiddenInGame(false);
            CapturingPiece->SetActorLocation(ReferredBoard->GetRelativeLocationByXYPosition(From[0], From[1]));
        }
    }

    RollbackCastleVariables();
}

bool Chess_Move::IsLegal()
{
    MakeMove(true);
    bool isLegal = !ReferredBoard->CheckControl(GetMoveColor());
    RollbackMove(true);
    return isLegal;
}

float Chess_Move::MoveValueCalculation()
{
    float value = 0;
    if (CapturingPiece)
    {
        if (GetMoveColor() == ChessColor::WHITE)
        {//positive counting for white
            //value -= PieceFrom->GetPositionValue(From);//move value = newPiecePositionValue - oldPiecePositionValue
            //value += PieceFrom->GetPositionValue(To);
            if (CapturedPiece)
            {
                value += CapturedPiece->GetPieceValue();
            }
        }
        else
        {//negative counting for black
            //value += PieceFrom->GetPositionValue(From);//move value = newPiecePositionValue - oldPiecePositionValue
            //value -= PieceFrom->GetPositionValue(To);
            if (CapturedPiece)
            {
                value -= CapturedPiece->GetPieceValue();
            }
        }
    }
    return value;
}


void Chess_Move::UpdateCastleVariables()
{
    bool isWhite = GetMoveColor() == ChessColor::WHITE;

    if ((isWhite && ReferredBoard->bCastleWhiteLong && From == FVector2D(0, 7)) || (!isWhite && ReferredBoard->bCastleWhiteLong && To == FVector2D(0, 7)))//long castle was feasible and moved the rook/captured the rook
    {
        ReferredBoard->bCastleWhiteLong = false;
        bSetWhiteCastleLongOnUndo = true;
    }
    else if ((isWhite && ReferredBoard->bCastleWhiteShort && From == FVector2D(7, 7)) || (!isWhite && ReferredBoard->bCastleWhiteShort && To == FVector2D(7, 7)))//same for king side rook
    {
        ReferredBoard->bCastleWhiteShort = false;
        bSetWhiteCastleShortOnUndo = true;
    }
    else
    {
        if (isWhite && ReferredBoard->bCastleWhiteLong && From == FVector2D(4, 7))
        {
            ReferredBoard->bCastleWhiteLong = false;
            bSetWhiteCastleLongOnUndo = true;
        }
        if (isWhite && ReferredBoard->bCastleWhiteShort && From == FVector2D(4, 7))
        {
            ReferredBoard->bCastleWhiteShort = false;
            bSetWhiteCastleShortOnUndo = true;
        }
    }
    
    if ((!isWhite && ReferredBoard->bCastleBlackLong && From == FVector2D(0, 0)) || (isWhite && ReferredBoard->bCastleBlackLong && To == FVector2D(0, 0)))//long castle was feasible and moved the rook/captured the rook
    {
        ReferredBoard->bCastleBlackLong = false;
        bSetBlackCastleLongOnUndo = true;
    }
    else if ((!isWhite && ReferredBoard->bCastleBlackShort && From == FVector2D(7, 0)) || (isWhite && ReferredBoard->bCastleBlackShort && To == FVector2D(7, 0)))//same for king side rook
    {
        ReferredBoard->bCastleBlackShort = false;
        bSetBlackCastleShortOnUndo = true;
    }
    else
    {
        if (!isWhite && ReferredBoard->bCastleBlackLong && From == FVector2D(4, 0))
        {
            ReferredBoard->bCastleBlackLong = false;
            bSetBlackCastleLongOnUndo = true;
        }
        if (!isWhite && ReferredBoard->bCastleBlackShort && From == FVector2D(4, 0))
        {
            ReferredBoard->bCastleBlackShort = false;
            bSetBlackCastleShortOnUndo = true;
        }
    }
}

void Chess_Move::RollbackCastleVariables()
{
    if (bSetWhiteCastleLongOnUndo)
    {
        ReferredBoard->bCastleWhiteLong = true;
    }
    if (bSetBlackCastleLongOnUndo)
    {
        ReferredBoard->bCastleBlackLong = true;
    }
    if (bSetWhiteCastleShortOnUndo)
    {
        ReferredBoard->bCastleWhiteShort = true;
    }
    if (bSetBlackCastleShortOnUndo)
    {
        ReferredBoard->bCastleBlackShort = true;
    }
}

FString Chess_Move::ToString() const
{
    FString AlgebraicNotation;
    FString Columns = "abcdefgh";
    FString Rows = "87654321";
    // Converti le coordinate 'From' e 'To' in notazione algebrica
    AlgebraicNotation += PawnPromotionAusRef == nullptr ? GetPieceLetter(CapturingPiece) : GetPieceLetter(PawnPromotionAusRef);
    AlgebraicNotation += Columns[From.X];
    AlgebraicNotation += Rows[From.Y];
    if (CapturedPiece != nullptr)//eat
    {
        AlgebraicNotation += 'x';
    }
    else {
        AlgebraicNotation += '-';
    }
    AlgebraicNotation += Columns[To.X];
    AlgebraicNotation += Rows[To.Y];

    AlgebraicNotation += PawnPromotionAusRef != nullptr ? "=" + GetPieceLetter(CapturingPiece) : "";//for pawn promotion TODO: controlla se è giusta la notazione

    if (PlayerOnCheck != NAC)
    {
        if (PlayerOnCheckMate == NAC)//it's only a check
        {
            AlgebraicNotation += '+';
        }
        else//it's checkmate
        {
            AlgebraicNotation += '#';
        }
    }

    if (MoveClass == MoveType::CASTLE)
    {
        AlgebraicNotation = bIsQueenSide ? "0-0-0" : "0-0";
    }

    return AlgebraicNotation;
}

ChessColor Chess_Move::GetMoveColor() const
{
    if (CapturingPiece)
    {
        return CapturingPiece->PieceColor;
    }
    return ChessColor::NAC;
}

Chess_Move::~Chess_Move()
{
}

FString Chess_Move::GetPieceLetter(AChessPiece* piece)
{
    if (const ACP_Bishop* tmp = Cast<ACP_Bishop>(piece)) {
        return FString("B");
    }
    if (const ACP_King* tmp = Cast<ACP_King>(piece)) {
        return FString("K");
    }
    if (const ACP_Knight* tmp = Cast<ACP_Knight>(piece)) {
        return FString("N");
    }
    if (const ACP_Pawn* tmp = Cast<ACP_Pawn>(piece)) {
        return FString("");
    }
    if (const ACP_Queen* tmp = Cast<ACP_Queen>(piece)) {
        return FString("Q");
    }
    if (const ACP_Rook* tmp = Cast<ACP_Rook>(piece)) {
        return FString("R");
    }
    return FString("");
}

//--------------------- PawnPromotion methods

void Chess_Move::PromotePawnRollback(bool simulate)
{
    //if (simulate) return;

    if (PawnPromotionAusRef == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("PromoteRollbackPawn:Impossible rollback, no promoted piece found"));
        return;
    }
    if (!ReferredBoard->GetXYFromPiece(CapturingPiece))
    {
        return;
    }
    //swap pieces:
    AChessPiece* swapVar = CapturingPiece;//TODO: (Label=L"robacinese",Name="None) problem
    ReferredBoard->RemovePiece(CapturingPiece);
    CapturingPiece = PawnPromotionAusRef;
    PawnPromotionAusRef = swapVar;
    ReferredBoard->SetPieceFromXY(To, CapturingPiece);

    if (!simulate)
    {
        CapturingPiece->SetActorHiddenInGame(false);
        CapturingPiece->SetActorLocation(ReferredBoard->GetRelativeLocationByXYPosition(To[0], To[1]));//put the piece in the position
        PawnPromotionAusRef->SetActorHiddenInGame(true);
    }
    else {//if was a simulate move destory the promoted piece to avoid memory leak (TODO: controlla se unreal lo fa già da solo)
        PawnPromotionAusRef->Destroy();
        PawnPromotionAusRef = nullptr;//to make it possible tu reassign a new piece as promoted piece
    }
}

void Chess_Move::PromotePawn(bool simulate, TSubclassOf<AChessPiece> selectedPiece)
{
    if (selectedPiece != nullptr)//first promotion (not executed if is a replay)
    {
        FVector position = ReferredBoard->GetRelativeLocationByXYPosition(To.X, To.Y);
        FRotator rotation = FRotator(0, 0, 0);
        ChessColor pColor = GetMoveColor();
        if (PawnPromotionAusRef != nullptr) {//never reached (in theory)
            PawnPromotionAusRef->Destroy();
        }
        PawnPromotionAusRef = ReferredBoard->GetWorld()->SpawnActor<AChessPiece>(selectedPiece, position, rotation);
        PawnPromotionAusRef->SetColorAndMaterial(pColor);
        PawnPromotionAusRef->SetActorHiddenInGame(true);
    }

    if (PawnPromotionAusRef) {//swap pieces
        AChessPiece* swapVar = CapturingPiece;
        if (!simulate) {//show the effective piece
            CapturingPiece->SetActorHiddenInGame(true);
            PawnPromotionAusRef->SetActorHiddenInGame(false);
            CapturingPiece->SetActorLocation(ReferredBoard->GetRelativeLocationByXYPosition(-1, -1));//Move chesspiece away from chessboard to avoid fake click
        }

        ReferredBoard->RemovePiece(CapturingPiece);
        CapturingPiece = PawnPromotionAusRef;
        PawnPromotionAusRef = swapVar;
        ReferredBoard->SetPieceFromXY(To, CapturingPiece);
    }
    CalculateResult();//update attributes for "toString" method
}

//--------------------- EnPassant methods

void Chess_Move::EnPassantCapture(bool simulate)
{
    //capture piece:
    if (EnPassantCapturedPiece)
    {
        ReferredBoard->RemovePiece(EnPassantCapturedPiece);
        if (!simulate)
        {
            if (EnPassantCapturedPiece->PieceColor == BLACK)//TODO:RIARRANGIA
                EnPassantCapturedPiece->SetActorLocation(ReferredBoard->GetRelativeLocationByXYPosition(-1, -1));
            else
                EnPassantCapturedPiece->SetActorLocation(ReferredBoard->GetRelativeLocationByXYPosition(8, 8));
            EnPassantCapturedPiece->SetActorHiddenInGame(true);
        }
    }
}

void Chess_Move::EnPassantCaptureRollback(bool simulate)
{
    //restore piece:
    if (EnPassantCapturedPiece)
    {
        FVector2D EnPassantCapturedPiecePosition = FVector2D(To.X, From.Y);
        ReferredBoard->SetPieceFromXY(EnPassantCapturedPiecePosition, EnPassantCapturedPiece);
        if (!simulate)
        {
            EnPassantCapturedPiece->SetActorHiddenInGame(false);
            EnPassantCapturedPiece->SetActorLocation(ReferredBoard->GetRelativeLocationByXYPosition(EnPassantCapturedPiecePosition.X, EnPassantCapturedPiecePosition.Y));
        }
    }
}

//--------------------- Castle methods


void Chess_Move::CastleMove(bool simulate)
{
    FVector2D newRookPosition;
    if (bIsQueenSide)
    {
        FVector2D   BlackLeftRookPosition = FVector2D(0, 0), WhiteLeftRookPosition = FVector2D(0, 7), rookBlackQueenSidePos = FVector2D(3, 0), rookWhiteQueenSidePos = FVector2D(3, 7);
        CastledRook = GetMoveColor() == ChessColor::WHITE ? ReferredBoard->GetPieceFromXY(WhiteLeftRookPosition) : ReferredBoard->GetPieceFromXY(BlackLeftRookPosition);
        newRookPosition = GetMoveColor() == ChessColor::WHITE ? rookWhiteQueenSidePos : rookBlackQueenSidePos;
    }
    else
    {
        FVector2D BlackRightRookPosition = FVector2D(7, 0), WhiteRightRookPosition = FVector2D(7, 7),rookBlackKingSidePos = FVector2D(5, 0), rookWhiteKingSidePos = FVector2D(5, 7);
        CastledRook = GetMoveColor() == ChessColor::WHITE ? ReferredBoard->GetPieceFromXY(WhiteRightRookPosition) : ReferredBoard->GetPieceFromXY(BlackRightRookPosition);
        newRookPosition = GetMoveColor() == ChessColor::WHITE ? rookWhiteKingSidePos : rookBlackKingSidePos;
    }
    ReferredBoard->RemovePiece(CastledRook);
    ReferredBoard->SetPieceFromXY(newRookPosition, CastledRook);
    
    if (!simulate)
    {
        CastledRook->SetActorLocation(ReferredBoard->GetRelativeLocationByXYPosition(newRookPosition.X, newRookPosition.Y));
    }
}

void Chess_Move::CastleMoveRollback(bool simulate)
{
    FVector2D oldRookPosition;
    if (bIsQueenSide)
    {
        FVector2D BlackLeftRookPosition = FVector2D(0, 0), WhiteLeftRookPosition = FVector2D(0, 7), rookBlackQueenSidePos = FVector2D(3, 0), rookWhiteQueenSidePos = FVector2D(3, 7);
        oldRookPosition = GetMoveColor() == ChessColor::WHITE ? WhiteLeftRookPosition : BlackLeftRookPosition;
    }
    else
    {
        FVector2D BlackRightRookPosition = FVector2D(7, 0), WhiteRightRookPosition = FVector2D(7, 7), rookBlackKingSidePos = FVector2D(5, 0), rookWhiteKingSidePos = FVector2D(5, 7);
        oldRookPosition = GetMoveColor() == ChessColor::WHITE ? WhiteRightRookPosition : BlackRightRookPosition;
    }
    ReferredBoard->RemovePiece(CastledRook);
    ReferredBoard->SetPieceFromXY(oldRookPosition, CastledRook);

    if (!simulate)
    {
        CastledRook->SetActorLocation(ReferredBoard->GetRelativeLocationByXYPosition(oldRookPosition.X, oldRookPosition.Y));
    }
}
