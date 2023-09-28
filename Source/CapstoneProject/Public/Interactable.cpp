// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactable.h"
#include "PlayerMovement.h"
#include "BasePlayerController.h"

#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UInteractable::UInteractable()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	
}


// Called when the game starts
void UInteractable::BeginPlay()
{
	Super::BeginPlay();

	APlayerMovement* player = Cast<APlayerMovement>(UGameplayStatics::GetActorOfClass(GetWorld(), APlayerMovement::StaticClass()));
	if (player)
	{
		AController* tempController = player->GetController();
		controller = Cast<ABasePlayerController>(tempController);
	}

	collider = GetOwner()->FindComponentByClass<UShapeComponent>();
	if (collider)
	{
		collider->OnBeginCursorOver.AddDynamic(this, &UInteractable::MouseHover);
		collider->OnClicked.AddDynamic(this, &UInteractable::Selected);
	}
}


// Called every frame
void UInteractable::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!controller)
	{
		APlayerMovement* player = Cast<APlayerMovement>(UGameplayStatics::GetActorOfClass(GetWorld(), APlayerMovement::StaticClass()));
		if (player)
		{
			AController* tempController = player->GetController();
			controller = Cast<ABasePlayerController>(tempController);
		}
	}
}

void UInteractable::MouseHover(UPrimitiveComponent* item)
{
	if (controller) controller->SetHoveredWorldObject(GetOwner());

	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Blue, FString::Printf(TEXT("Hovering over Hex")));
}

void UInteractable::Selected(UPrimitiveComponent* item, FKey ButtonPressed)
{
	if (controller) controller->SetSelectedWorldObject(GetOwner());

	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, FString::Printf(TEXT("Selected Hex!")));
}

