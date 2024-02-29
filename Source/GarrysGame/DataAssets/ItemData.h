#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ItemData.generated.h"

UCLASS()
class GARRYSGAME_API UItemData : public UDataAsset
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditDefaultsOnly)
	FString ItemName;
	UPROPERTY(EditDefaultsOnly)
	float ItemValue;
	UPROPERTY(EditDefaultsOnly)
	UStaticMesh* ItemMesh;

public:

	UFUNCTION(BlueprintCallable)
	FString GetItemName() const { return ItemName; }

	UFUNCTION(BlueprintCallable)
	float GetItemValue() const { return ItemValue; }

	UFUNCTION(BlueprintCallable)
	UStaticMesh* GetItemMesh() const { return ItemMesh; }
};
