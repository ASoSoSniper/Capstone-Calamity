// Fill out your copyright notice in the Description page of Project Settings.


#include "UAIA_Troop_Investigate.h"
#include "BaseHex.h"
#include "Troop.h"

void UUAIA_Troop_Investigate::ExecuteAction(IUAI_Controller* controller)
{
	ATroop* troop = Cast<ATroop>(controller);
	if (!troop) return;

	ABaseHex* hex = troop->hexNav->GetCurrentHex();
	if (hex && hex->HasUnsearchedPOI())
		hex->BeginInvestigation(troop);
}