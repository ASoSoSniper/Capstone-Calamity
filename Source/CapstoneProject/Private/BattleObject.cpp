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

	interact->CreateExtraCollision(group2Mesh);
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

	//The attacking faction is very likely to own the last troop to enter the hex, so the troop at the end of the array is set as the attacker
	attackingFaction = hex->troopsInHex[hex->troopsInHex.Num() - 1]->unitStats->faction;

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
	for (auto& faction : factionsInBattle)
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
	//Set attacking faction to group 1 for consistency
	currentBattle.Group1.Add(attackingFaction);

	//For each faction in the battle:
	for (auto& army : armies)
	{	
		//Skip if attacking faction, since we already added it to group 1
		if (army.Key == attackingFaction) continue;

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

	CalculateGroupDamage();

	int group1DamageTotal = group1Damage * GetRollModifier(group1Die);
	int group2DamageTotal = group2Damage * GetRollModifier(group2Die);

	//Apply damage to each group
	for (int i = 0; i < currentBattle.Group1.Num(); ++i)
	{
		armies[currentBattle.Group1[i]].currentHP -= group2DamageTotal;
		armies[currentBattle.Group1[i]].currentHP = FMath::Clamp(armies[currentBattle.Group1[i]].currentHP, 0, armies[currentBattle.Group1[i]].maxHP);

		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Purple, FString::Printf(TEXT("Group 1 took %d damage! %d health remaining! %d morale remaining!"), group2Damage, armies[currentBattle.Group1[i]].currentHP, armies[currentBattle.Group1[i]].currentMorale));
	}
	for (int i = 0; i < currentBattle.Group2.Num(); ++i)
	{
		armies[currentBattle.Group2[i]].currentHP -= group1DamageTotal;
		armies[currentBattle.Group2[i]].currentHP = FMath::Clamp(armies[currentBattle.Group2[i]].currentHP, 0, armies[currentBattle.Group2[i]].maxHP);

		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Purple, FString::Printf(TEXT("Group 2 took %d damage! %d health remaining! %d morale remaining!"), group1Damage, armies[currentBattle.Group2[i]].currentHP, armies[currentBattle.Group1[i]].currentMorale));
	}

	//Remove armies as they are killed
	for (auto army : armies)
	{
		if (!IsAlive(army.Value))
		{
			RemoveArmy(army.Key);
		}
	}
	for (auto army : armies)
	{
		if (DecayMorale(army.Key, moraleDecayRate) <= 0.f)
		{
			FleeFromBattle(army.Key);
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
	if (!hex) return spawnedTroops;
	for (int i = 0; i < factionsInBattle[faction].Num(); ++i)
	{
		float currHP = armies[faction].currentHP;
		float maxHP = armies[faction].maxHP;
		float hpPercent = (currHP / maxHP);
		
		ATroop* spawn = nullptr;
		ABaseHex* spawnPoint = spawnAtOutpost ? UnitActions::GetClosestOutpostHex(faction, hex)->FindFreeAdjacentHex(faction, usedHexes) : hex->FindFreeAdjacentHex(faction, usedHexes);
		if (!spawnPoint) usedHexes.Add(spawnPoint);

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

int ABattleObject::RollDie(int& groupDie)
{
	groupDie = FMath::RandRange(1, 6);
	return group1Die;
}

float ABattleObject::GetRollModifier(int& groupDie)
{
	switch (group1Die)
	{
	case 1:
		return 0.85f;
	case 2:
		return 0.85f;
	case 3:
		return 1.f;
	case 4:
		return 1.f;
	case 5: 
		return 1.15f;
	case 6:
		return 1.15f;
	}

	return 1.f;
}

float ABattleObject::GetMoralePercent(Factions faction)
{
	if (!armies.Contains(faction)) return 0.f;

	return (float)armies[faction].currentMorale / (float)armies[faction].maxMorale;
}

float ABattleObject::DecayMorale(Factions faction, float percentReduction)
{
	if (!armies.Contains(faction)) return 0.f;

	float newPercent = GetMoralePercent(faction) - percentReduction;

	armies[faction].currentMorale = FMath::RoundToInt(FMath::Clamp((float)armies[faction].maxMorale * newPercent, 0.f, (float)armies[faction].maxMorale));

	return armies[faction].currentMorale;
}

void ABattleObject::CalculateGroupDamage()
{
	TMap<UnitTypes, UnitComposition> group1Units = GetArmyComposition(currentBattle.Group1);
	TMap<UnitTypes, UnitComposition> group2Units = GetArmyComposition(currentBattle.Group2);

	group1Damage = 0;
	group2Damage = 0;

	//For each unit type
	for (auto& unit : group1Units)
	{
		float damage = 0;

		//For each enemy type in the enemy army
		for (auto& enemyUnit : group2Units)
		{
			float percent = 0;

			//Select the damage modifier based on the unit type
			switch (enemyUnit.Key)
			{
			case UnitTypes::Infantry:
				percent = spawner->troopStats[unit.Key].attackvsInfantry;
				break;
			case UnitTypes::Cavalry:
				percent = spawner->troopStats[unit.Key].attackvsCavalry;
				break;
			case UnitTypes::Ranged:
				percent = spawner->troopStats[unit.Key].attackvsRanged;
				break;
			case UnitTypes::Shielder:
				percent = spawner->troopStats[unit.Key].attackvsShielder;
				break;
			case UnitTypes::Scout:
				percent = spawner->troopStats[unit.Key].attackvsScout;
				break;
			case UnitTypes::Settler:
				percent = spawner->troopStats[unit.Key].attackvsSettler;
				break;
			}

			//Convert to decimal
			percent *= 0.01f;
			//Multiply by the percent of this unit type in the enemy army
			percent *= enemyUnit.Value.compPercent;

			//Add this percent to the total damage
			damage += percent;
		}

		//When all percentages have been added up, multiply the total by the unit type's base damage
		damage *= spawner->troopStats[unit.Key].damage;

		//Add to the total group damage the calculated damage times the number of this unit type in the army
		group1Damage += FMath::RoundToInt(damage) * unit.Value.quantity;
	}

	//For each unit type
	for (auto& unit : group2Units)
	{
		float damage = 0;

		//For each enemy type in the enemy army
		for (auto& enemyUnit : group1Units)
		{
			float percent = 0;

			//Select the damage modifier based on the unit type
			switch (enemyUnit.Key)
			{
			case UnitTypes::Infantry:
				percent = spawner->troopStats[unit.Key].defendvsInfantry;
				break;
			case UnitTypes::Cavalry:
				percent = spawner->troopStats[unit.Key].defendvsCavalry;
				break;
			case UnitTypes::Ranged:
				percent = spawner->troopStats[unit.Key].defendvsRanged;
				break;
			case UnitTypes::Shielder:
				percent = spawner->troopStats[unit.Key].defendvsShielder;
				break;
			case UnitTypes::Scout:
				percent = spawner->troopStats[unit.Key].defendvsScout;
				break;
			case UnitTypes::Settler:
				percent = spawner->troopStats[unit.Key].defendvsSettler;
				break;
			}

			//Convert to decimal
			percent *= 0.01f;
			//Multiply by the percent of this unit type in the enemy army
			percent *= enemyUnit.Value.compPercent;

			//Add this percent to the total damage
			damage += percent;
		}

		//When all percentages have been added up, multiply the total by the unit type's base damage
		damage *= spawner->troopStats[unit.Key].damage;

		//Add to the total group damage the calculated damage times the number of this unit type in the army
		group2Damage += FMath::RoundToInt(damage) * unit.Value.quantity;
	}
}

TMap<UnitTypes, ABattleObject::UnitComposition> ABattleObject::GetArmyComposition(TArray<Factions>& group)
{
	TMap<UnitTypes, UnitComposition> unitsInArmy;

	//For each faction in the group:
	for (Factions& faction : group)
	{
		//For each unit in the faction:
		for (int i = 0; i < factionsInBattle[faction].Num(); i++)
		{
			//Special case for armies, don't include them in the TMap, rather get the units they're composed of
			if (factionsInBattle[faction][i].unitType == UnitTypes::Army)
			{
				//For each unit in the army's saved units pool:
				for (int j = 0; j < factionsInBattle[faction][i].savedUnits.Num(); j++)
				{
					//If the TMap doesn't already have this unit type, add it with an initial value of zero
					if (!unitsInArmy.Contains(factionsInBattle[faction][i].savedUnits[j].unitType))
					{
						unitsInArmy.Add(factionsInBattle[faction][i].savedUnits[j].unitType, UnitComposition{0, 0.f});
					}

					//Tally up this unit type in the total army composition
					++unitsInArmy[factionsInBattle[faction][i].savedUnits[j].unitType].quantity;
				}

				//If an army, skip the steps applicable only to individual units
				continue;
			}

			//For non-armies:

			//If the TMap doesn't already have this unit type, add it with an initial value of zero
			if (!unitsInArmy.Contains(factionsInBattle[faction][i].unitType))
			{
				unitsInArmy.Add(factionsInBattle[faction][i].unitType, UnitComposition{ 0, 0.f });
			}

			//Tally up this unit type in the total army composition
			++unitsInArmy[factionsInBattle[faction][i].unitType].quantity;
		}
	}

	//After all units are collected, find the total number in the group
	int totalUnits = 0;
	for (auto& unit : unitsInArmy)
	{
		totalUnits += unit.Value.quantity;
	}

	//Use the total number to calculate what percent of the army each unit type occupies
	for (auto& unit : unitsInArmy)
	{
		unit.Value.compPercent = (float)unit.Value.quantity / (float)totalUnits;
	}

	return unitsInArmy;
}