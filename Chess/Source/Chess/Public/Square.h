//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Square.generated.h"

class AChessPiece;

UCLASS()
class CHESS_API ASquare : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASquare();
	// Sets the color (black or white) of the square
	void setPiece(AChessPiece* p);
	bool bIsSelected = false;

	AChessPiece* getPiece();

	void SetSquareColor(bool c);

	void SetAsSelected();

	bool IsSelected();

	//Sets square material to check material
	void SetDangerColor();

	bool IsDanger();

	//Sets square material to replay material
	void InReplay();
	bool bIsInReplay = false;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	AChessPiece* PieceOn;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	USceneComponent* Scene;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* SquareMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UMaterialInstance* BlackMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UMaterialInstance* WhiteMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UMaterialInstance* SelectedMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UMaterialInstance* DangerMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UMaterialInstance* ReplayMaterial;

public:
	// Called every frame
	//virtual void Tick(float DeltaTime) override;

};
