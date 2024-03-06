// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Square.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Chessboard.generated.h"

UCLASS()
class CHESS_API AChessboard : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AChessboard();

	UPROPERTY(Transient)
	TMap<FVector2D, ASquare*> SquareMap;
	//UPROPERTY(Transient)
	//TMap<FVector2D, AChessPiece> PieceMap;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ASquare> SquareClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 BoardSize;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SquareSize;

	//UFUNCTION(BluePrintCallable)
	//void ResetField();

	virtual void OnConstruction(const FTransform& Transform) override;
	void GenerateField();

	FVector GetRelativeLocationByXYPosition(const int32 InX, const int32 InY) const;

	FVector2D GetXYPositionByRelativeLocation(const FVector& Location) const;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	//virtual void Tick(float DeltaTime) override;

};
