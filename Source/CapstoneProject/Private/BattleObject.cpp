// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleObject.h"
#include "Troop.h"

// Sets default values
ABattleObject::ABattleObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	interact = CreateDefaultSubobject<UInteractable>(TEXT("Interaction Component"));
	hexNav = CreateDefaultSubobject<UHexNav>(TEXT("Hex Nav"));
	sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Body"));
	sphere->SetupAttachment(RootComponent);
	sphere->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	sphere->InitSphereRadius(20.f);
	sphere->bHiddenInGame = false;
}

// Called when the game starts or when spawned
void ABattleObject::BeginPlay()
{
	Super::BeginPlay();
	
	
}

void ABattleObject::Start()
{
	//Find hex the object is placed on
	hex = Cast<ABaseHex>(hexNav->currentHex);

	//Once created, begin organizing armies into factions
	CreateFactions();
}

// Called every frame
void ABattleObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!hex->troopsInHex.IsEmpty())
	{
		//AddUnitToFaction(hex->troopsInHex[0]);
	}

	if (attacking)
	{
		currentAttackTime -= DeltaTime;
		if (currentAttackTime <= 0.f)
		{
			Attack();
			currentAttackTime = attackRate;
		}
	}
}

//Collect all units in the hex and sort them into factions
void ABattleObject::CreateFactions()
{
	//For each troop in the hex:
	for (int i = 0; i < hex->troopsInHex.Num(); ++i)
	{
		//Extract data from the troop
		UnitActions::UnitData unitData = UnitActions::CollectUnitData(hex->troopsInHex[i]->unitStats);

		//If the troop's faction does not yet exist in the battle, create it and add it to the list
		if (!factionsInBattle.Contains(unitData.faction))
		{
			TArray<UnitActions::UnitData> newArmy;
			factionsInBattle.Add(unitData.faction, newArmy);
		}

		//Add the troop data to its respective faction
		factionsInBattle[unitData.faction].Add(unitData);
	}

	//After all troop data is collected, remove the troops from the hex and destroy them in the world
	int hexPop = hex->troopsInHex.Num();
	for (int i = 0; i < hexPop; ++i)
	{
		AMovementAI* currentTroop = hex->troopsInHex[0];
		hex->troopsInHex.Remove(currentTroop);
		currentTroop->Destroy();
	}
	//Use the collected troops and factions to generate singular faction units
	GenerateArmies();
}

//Add a unit to a faction following the creation of this battle
void ABattleObject::AddUnitToFaction(AMovementAI* troop)
{
	UnitActions::UnitData unitData = UnitActions::CollectUnitData(troop->unitStats);

	if (!factionsInBattle.Contains(unitData.faction))
	{
		TArray<UnitActions::UnitData> newArmy;
		factionsInBattle.Add(unitData.faction, newArmy);
	}

	factionsInBattle[unitData.faction].Add(unitData);
	AddUnitToArmy(unitData);
	hex->troopsInHex.Remove(troop);
	troop->Destroy();
}

//Use the created factions and the unit data each possess to generate singular units for each faction
void ABattleObject::GenerateArmies()
{
	//For each faction in the battle:
	for (auto faction : factionsInBattle)
	{
		//If this faction unit does not already exist, create one and add all stats from each unit in the faction to it
		if (!armies.Contains(faction.Key))
		{
			UnitActions::UnitData army{faction.Key, UnitTypes::Army, false, 0, 0, 0, 0, 0, 0};
			TArray<UnitActions::UnitData> factionData = faction.Value;
			for (int i = 0; i < faction.Value.Num(); ++i)
			{
				UnitActions::UnitData unit = factionData[i];
				army.currentHP += unit.currentHP;
				army.maxHP += unit.maxHP;
				army.defense += unit.defense;
				army.currentMorale += unit.currentMorale;
				army.maxMorale += unit.maxMorale;
				army.minDamage += unit.minDamage;
				army.maxDamage += unit.maxDamage;
				army.reinforceRate += unit.reinforceRate;
			}
			armies.Add(faction.Key, army);
		}
	}

	AssignGroups();
}

void ABattleObject::AddUnitToArmy(UnitActions::UnitData data)
{
	if (armies.Contains(data.faction))
	{
		armies[data.faction].currentHP += data.currentHP;
		armies[data.faction].maxHP += data.maxHP;
		armies[data.faction].defense += data.defense;
		armies[data.faction].currentMorale += data.currentMorale;
		armies[data.faction].maxMorale += data.maxMorale;
		armies[data.faction].minDamage += data.minDamage;
		armies[data.faction].maxDamage += data.maxDamage;
		armies[data.faction].reinforceRate += data.reinforceRate;
	}
}

void ABattleObject::AssignGroups()
{
	//For each faction in the battle:
	for (auto army : armies)
	{		
		//Determine this faction's alignment to other factions in the conflict, using that to select which group to join, if any
		EngagementSelect assignment = UnitActions::DetermineConflictAlignment(army.Key, currentBattle.Group1, currentBattle.Group2);

		switch (assignment)
		{
		case EngagementSelect::JoinGroup1:
			if (!currentBattle.Group1.Contains(army.Key))
			{
				currentBattle.Group1.Add(army.Key);
			}
			break;
		case EngagementSelect::JoinGroup2:
			if (!currentBattle.Group2.Contains(army.Key))
			{
				currentBattle.Group2.Add(army.Key);
			}
			break;
		}
	}

	attacking = true;
}

void ABattleObject::Attack()
{
	int group1Damage = 0;
	int group2Damage = 0;

	//Create random damage value from each group
	for (int i = 0; i < currentBattle.Group1.Num(); ++i)
	{
		group1Damage += FMath::RandRange(armies[currentBattle.Group1[i]].minDamage, armies[currentBattle.Group1[i]].maxDamage);
	}
	for (int i = 0; i < currentBattle.Group2.Num(); ++i)
	{
		group2Damage += FMath::RandRange(armies[currentBattle.Group2[i]].minDamage, armies[currentBattle.Group2[i]].maxDamage);
	}

	int group1Size = currentBattle.Group1.Num();
	int group2Size = currentBattle.Group2.Num();
	//Apply damage to each group
	for (int i = 0; i < group1Size; ++i)
	{
		armies[currentBattle.Group1[i]].currentHP -= group2Damage;
		armies[currentBattle.Group1[i]].currentHP = FMath::Clamp(armies[currentBattle.Group1[i]].currentHP, 0, armies[currentBattle.Group1[i]].maxHP);

		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Purple, FString::Printf(TEXT("Group 1 took %d damage! %d health remaining!"), group2Damage, armies[currentBattle.Group1[i]].currentHP));

		if (!IsAlive(armies[currentBattle.Group1[i]]))
		{
			Factions deadFaction = currentBattle.Group1[i];

			currentBattle.Group1.Remove(deadFaction);
			armies.Remove(deadFaction);
			factionsInBattle.Remove(deadFaction);
		}
	}
	for (int i = 0; i < group2Size; ++i)
	{
		armies[currentBattle.Group2[i]].currentHP -= group1Damage;
		armies[currentBattle.Group2[i]].currentHP = FMath::Clamp(armies[currentBattle.Group2[i]].currentHP, 0, armies[currentBattle.Group2[i]].maxHP);

		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Purple, FString::Printf(TEXT("Group 2 took %d damage! %d health remaining!"), group1Damage, armies[currentBattle.Group2[i]].currentHP));

		if (!IsAlive(armies[currentBattle.Group2[i]]))
		{
			Factions deadFaction = currentBattle.Group2[i];

			currentBattle.Group2.Remove(deadFaction);
			armies.Remove(deadFaction);
			factionsInBattle.Remove(deadFaction);
		}
	}

	if (currentBattle.Group1.IsEmpty() || currentBattle.Group2.IsEmpty())
	{
		attacking = false;
		EndBattle();
	}
}

void ABattleObject::EndBattle()
{
	for (auto faction : factionsInBattle)
	{
		if (faction.Value.IsEmpty()) continue;

		for (int i = 0; i < faction.Value.Num(); ++i)
		{
			switch (faction.Value[i].unitType)
			{
			case UnitTypes::Army:
				spawner->SpawnArmy(hex, faction.Value[i].savedUnits);
				break;
			default:
				spawner->SpawnTroop(hex, faction.Value[i]);
				break;
			}
		}
	}

	hex->battleInProgress = false;
	Destroy();
}

bool ABattleObject::IsAlive(UnitActions::UnitData& group)
{
	if (group.currentHP <= 0)
	{
		return false;
	}

	return true;
}

