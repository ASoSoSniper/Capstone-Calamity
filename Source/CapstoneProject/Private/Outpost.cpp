// Fill out your copyright notice in the Description page of Project Settings.


#include "Outpost.h"
#include "OutpostStorage.h"
#include "BuildingAttachment.h"

AOutpost::AOutpost()
{
	storageBuilding = CreateDefaultSubobject<UOutpostStorage>(TEXT("Storage"));
}

TArray<ABaseHex*> AOutpost::ClaimLand()
{
	TArray<ABaseHex*> hexesToClaim;

	if (unitStats->faction == Factions::None)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("No faction assigned, cannot claim land"));
		return hexesToClaim;
	}

	ABaseHex* startHex = Cast<ABaseHex>(hexNav->currentHex);

	startHex->hexOwner = unitStats->faction;
	hexesToClaim.Add(startHex);

	for (int i = 0; i < range; ++i)
	{
		TArray<ABaseHex*> currentLayer;

		for (int j = 0; j < hexesToClaim.Num(); ++j)
		{
			TArray<ABaseHex*> tempList;
			tempList = ScanHex(hexesToClaim[j]);
			for (int k = 0; k < tempList.Num(); ++k)
			{
				currentLayer.Add(tempList[k]);
			}
		}

		for (int j = 0; j < currentLayer.Num(); ++j)
		{
			hexesToClaim.Add(currentLayer[j]);
		}
	}

	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Blue, FString::Printf(TEXT("%d hexes claimed"), hexesToClaim.Num()));

	return hexesToClaim;
}

TArray<ABaseHex*> AOutpost::ScanHex(ABaseHex* hex)
{
	FCollisionQueryParams queryParams;
	queryParams.AddIgnoredActor(hex);
	TArray<AActor*> objectsInHex = Cast<ABaseHex>(hex)->GetObjectsInHex();
	if (objectsInHex.Num() > 0) queryParams.AddIgnoredActors(objectsInHex);

	TArray<ABaseHex*> foundHexes;

	for (int i = 0; i < 6; ++i)
	{
		//Establish direction
		FVector traceStart = hex->GetActorLocation() + FRotationMatrix(FRotator(0, 30 + (i * 60), 0)).GetUnitAxis(EAxis::X) * traceStartOffset;
		FVector traceEnd = traceStart + FRotationMatrix(FRotator(0, 30 + (i * 60), 0)).GetUnitAxis(EAxis::X) * traceLength;
		FHitResult hit;

		//Trace
		bool found = GetWorld()->LineTraceSingleByChannel(hit, traceStart, traceEnd, ECC_Visibility, queryParams, FCollisionResponseParams::DefaultResponseParam);

		//Check for hex actor class
		ABaseHex* foundHex = found ? Cast<ABaseHex>(hit.GetActor()) : nullptr;

		//Check found hex for neutrality
		if (foundHex)
		{
			if (foundHex->hexOwner == Factions::None)
			{
				//Add hex to list of found hexes
				foundHex->hexOwner = unitStats->faction;
				foundHexes.Add(foundHex);
			}
		}
	}

	return foundHexes;
}

void AOutpost::BuildingAction()
{
	claimedHexes = ClaimLand();
	BuildAttachment(BuildingAttachments::Storage);
}

void AOutpost::BuildAttachment(BuildingAttachments attachment)
{	
	switch (attachment)
	{
	case BuildingAttachments::Storage:
		storageBuilding->Activate();
		break;
	}

	
}
