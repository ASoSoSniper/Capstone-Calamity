// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleObject.h"
#include "Troop.h"
#include "MergedArmy.h"
#include "CapstoneProjectGameModeBase.h"

// Sets default values
ABattleObject::ABattleObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	baseRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Base Root"));
	
	SetRootComponent(baseRoot);

	interact = CreateDefaultSubobject<UInteractable>(TEXT("Interaction Component"));
	hexNav = CreateDefaultSubobject<UHexNav>(TEXT("Hex Nav"));
	visibility = CreateDefaultSubobject<UMeshVisibility>(TEXT("Mesh Visibility"));

	group1Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Group 1 Mesh"));
	group1Mesh->SetupAttachment(RootComponent);
	group1Mesh->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));

	group2Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Group 2 Mesh"));
	group2Mesh->SetupAttachment(RootComponent);
	group2Mesh->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));

	group1Mesh->SetCollisionProfileName("BlockAllDynamic");
	group2Mesh->SetCollisionProfileName("BlockAllDynamic");
}

// Called when the game starts or when spawned
void ABattleObject::BeginPlay()
{
	Super::BeginPlay();
	
	group1Mesh->SetRelativeLocation(GetActorRightVector() * 30.f);
	group2Mesh->SetRelativeLocation(GetActorRightVector() * -30.f);

	group1Mesh->SetRelativeRotation(FRotator(0, -90.f, 0));
	group2Mesh->SetRelativeRotation(FRotator(0, 90.f, 0));
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
		AddUnitToFaction(hex->troopsInHex[0]);
	}

	if (attacking)
	{
		currentAttackTime -= DeltaTime * ACapstoneProjectGameModeBase::timeScale;
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
			if (UnitActions::GetFactionRelationship(Factions::Human, unitData.faction) == FactionRelationship::Ally)
			{
				visibility->faction = Factions::Human;
			}
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
				army = UnitActions::AddUnitData(army, factionData[i]);
			}
			armies.Add(faction.Key, army);
		}
	}
	AssignGroups();
}

//After adding the unit to the faction, add its stats to the existing army unit
void ABattleObject::AddUnitToArmy(UnitActions::UnitData data)
{
	if (armies.Contains(data.faction))
	{
		armies[data.faction] = UnitActions::AddUnitData(armies[data.faction], data);
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
	UStaticMesh* robotMesh = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Robot_Token_02.Robot_Token_02'"));
	UStaticMesh* alienMesh = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Robot_Token_02.Robot_Token_02'"));

	if (currentBattle.Group1.Contains(Factions::Human))
	{
		group1Mesh->SetStaticMesh(robotMesh);
	}
	else
	{
		group1Mesh->SetStaticMesh(alienMesh);
	}

	if (currentBattle.Group2.Contains(Factions::Human))
	{
		group2Mesh->SetStaticMesh(robotMesh);
	}
	else
	{
		group2Mesh->SetStaticMesh(alienMesh);
	}
	
	attacking = true;
}

void ABattleObject::Attack()
{
	//End the battle if one group is completely wiped out
	if (currentBattle.Group1.IsEmpty() || currentBattle.Group2.IsEmpty())
	{
		EndBattle();
		return;
	}

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

	//Apply damage to each group
	for (int i = 0; i < currentBattle.Group1.Num(); ++i)
	{
		armies[currentBattle.Group1[i]].currentHP -= group2Damage;
		armies[currentBattle.Group1[i]].currentHP = FMath::Clamp(armies[currentBattle.Group1[i]].currentHP, 0, armies[currentBattle.Group1[i]].maxHP);

		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Purple, FString::Printf(TEXT("Group 1 took %d damage! %d health remaining!"), group2Damage, armies[currentBattle.Group1[i]].currentHP));
	}
	for (int i = 0; i < currentBattle.Group2.Num(); ++i)
	{
		armies[currentBattle.Group2[i]].currentHP -= group1Damage;
		armies[currentBattle.Group2[i]].currentHP = FMath::Clamp(armies[currentBattle.Group2[i]].currentHP, 0, armies[currentBattle.Group2[i]].maxHP);

		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Purple, FString::Printf(TEXT("Group 2 took %d damage! %d health remaining!"), group1Damage, armies[currentBattle.Group2[i]].currentHP));
	}

	//Remove armies as they are killed
	for (auto army : armies)
	{
		if (!IsAlive(army.Value))
		{
			RemoveArmy(army.Key);
		}
	}
	
	//End the battle if one group is completely wiped out
	if (currentBattle.Group1.IsEmpty() || currentBattle.Group2.IsEmpty())
	{
		EndBattle();
	}
}

void ABattleObject::EndBattle()
{
	for (auto faction : factionsInBattle)
	{
		if (faction.Value.IsEmpty()) continue;
		ExtractFactionUnits(faction.Key);
	}

	attacking = false;
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

void ABattleObject::FleeFromBattle(Factions faction)
{
	if (!factionsInBattle.Contains(faction)) return;

	TArray<ATroop*> fleeingTroops = ExtractFactionUnits(faction, true);

	for (int i = 0; i < fleeingTroops.Num(); i++)
	{
		//Troop health reduced by 50%
		float troopHP = fleeingTroops[i]->unitStats->currentHP;
		troopHP *= 0.5f;
		if (troopHP < 1.f) troopHP = 1.f;
		fleeingTroops[i]->unitStats->currentHP = troopHP;
	}

	RemoveArmy(faction);
	if (currentBattle.Group1.IsEmpty() || currentBattle.Group2.IsEmpty())
	{
		EndBattle();
	}
}

TArray<ATroop*> ABattleObject::ExtractFactionUnits(Factions faction, bool spawnAtOutpost)
{
	TArray<ATroop*> spawnedTroops;
	TArray<ABaseHex*> usedHexes;

	if (factionsInBattle[faction].IsEmpty() || !armies.Contains(faction)) return spawnedTroops;

	for (int i = 0; i < factionsInBattle[faction].Num(); ++i)
	{
		float currHP = armies[faction].currentHP;
		float maxHP = armies[faction].maxHP;
		float hpPercent = (currHP / maxHP);

		ATroop* spawn = nullptr;
		ABaseHex* spawnPoint = spawnAtOutpost ? UnitActions::GetClosestOutpostHex(faction, hex)->FindFreeAdjacentHex(faction, usedHexes) : hex->FindFreeAdjacentHex(faction, usedHexes);
		usedHexes.Add(spawnPoint);

		switch (factionsInBattle[faction][i].unitType)
		{
		case UnitTypes::Army:
			spawn = spawner->SpawnArmy(spawnPoint, factionsInBattle[faction][i].savedUnits, hpPercent);
			break;

		default:
			spawn = spawner->SpawnTroop(spawnPoint, factionsInBattle[faction][i], hpPercent);
			break;
		}

		spawnedTroops.Add(spawn);
	}
	
	return spawnedTroops;
}

void ABattleObject::RemoveArmy(Factions faction)
{
	armies.Remove(faction);
	factionsInBattle.Remove(faction);

	if (currentBattle.Group1.Contains(faction))
	{
		currentBattle.Group1.Remove(faction);
	}
	else if (currentBattle.Group2.Contains(faction))
	{
		currentBattle.Group2.Remove(faction);
	}
}

