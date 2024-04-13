// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleObject.h"
#include "Troop.h"
#include "MergedArmy.h"
#include "CapstoneProjectGameModeBase.h"
#include "GameFramework/GameModeBase.h"
#include "CombatAnims.h"

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

	group1Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Group 1 Mesh"));
	group1Mesh->SetupAttachment(RootComponent);
	group1Mesh->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
	group1Mesh->SetAnimationMode(EAnimationMode::AnimationBlueprint);

	group2Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Group 2 Mesh"));
	group2Mesh->SetupAttachment(RootComponent);
	group2Mesh->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
	group2Mesh->SetAnimationMode(EAnimationMode::AnimationBlueprint);

	group1Mesh->SetCollisionProfileName("BlockAllDynamic");
	group2Mesh->SetCollisionProfileName("BlockAllDynamic");

	interact->CreateExtraCollision(group2Mesh);
	visibility->otherSkeletalMesh = group2Mesh;
}

// Called when the game starts or when spawned
void ABattleObject::BeginPlay()
{
	Super::BeginPlay();
	
	group1Mesh->SetRelativeLocation(GetActorRightVector() * 30.f);
	group2Mesh->SetRelativeLocation(GetActorRightVector() * -30.f);

	group1Mesh->SetRelativeRotation(FRotator(0, -90.f, 0));
	group2Mesh->SetRelativeRotation(FRotator(0, 90.f, 0));

	groupCompositions.Add(TMap<UnitTypes, FUnitComposition>());
	groupCompositions.Add(TMap<UnitTypes, FUnitComposition>());
}

void ABattleObject::Start()
{
	//Find hex the object is placed on
	hex = Cast<ABaseHex>(hexNav->currentHex);
	spawner->controller->PlayUISound(spawner->controller->battleStartSound);

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

	if (ending)
	{
		timeTillEnd -= DeltaTime;
		if (timeTillEnd <= 0.f)
		{
			DestroyBattle();
			ending = false;
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

void ABattleObject::GenerateModels()
{
	USkeletalMesh* robotMesh = LoadObject<USkeletalMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Animations/robot_all_beta.robot_all_beta'"));
	USkeletalMesh* alienMesh = LoadObject<USkeletalMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Animations/robot_all_beta.robot_all_beta'"));

	if (currentBattle.Group1.Contains(Factions::Human))
	{
		group1Mesh->SetSkeletalMesh(robotMesh);
		group1Mesh->SetMaterial(0, spawner->troopFactionMaterials[Factions::Human].visibleTexture);
	}
	else
	{
		group1Mesh->SetSkeletalMesh(alienMesh);
		group1Mesh->SetMaterial(0, spawner->troopFactionMaterials[Factions::Alien1].visibleTexture);
	}

	if (currentBattle.Group2.Contains(Factions::Human))
	{
		group2Mesh->SetSkeletalMesh(robotMesh);
		group2Mesh->SetMaterial(0, spawner->troopFactionMaterials[Factions::Human].visibleTexture);
	}
	else
	{
		group2Mesh->SetSkeletalMesh(alienMesh);
		group2Mesh->SetMaterial(0, spawner->troopFactionMaterials[Factions::Alien1].visibleTexture);
	}

	//UAnimBlueprint* animBP = LoadObject<UAnimBlueprint>(nullptr, TEXT("/Game/3DModels/Animations/Robot_Beta_Combat_BP.Robot_Beta_Combat_BP"));
	if (animBP)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Blue, TEXT("Combat BP Found!"));

		group1Mesh->SetAnimInstanceClass(animBP->GeneratedClass);
		group2Mesh->SetAnimInstanceClass(animBP->GeneratedClass);
	}
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

	GenerateModels();
	
	attacking = true;
}

void ABattleObject::Attack()
{
	if (!attacking) return;

	//End the battle if one group is completely wiped out
	if (currentBattle.Group1.IsEmpty() || currentBattle.Group2.IsEmpty())
	{
		EndBattle();
		return;
	}

	CalculateGroupDamage();

	--currentTickTillRoll;
	if (currentTickTillRoll <= 0)
	{
		currentTickTillRoll = ticksTillRoll;
		RollDie(group1Die);
		RollDie(group2Die); 

		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Orange, FString::Printf(TEXT("Group 1 rolled %d!\nGroup 2 rolled %d!"), group1Die, group2Die));
	}

	int group1DieModded = FMath::Clamp(group1Die - hex->defenderBonus, 1, group1Die - hex->defenderBonus);

	int group1DamageTotal = FMath::RoundToInt((float)group1Damage * GetRollModifier(group1DieModded));
	int group2DamageTotal = FMath::RoundToInt((float)group2Damage * GetRollModifier(group2Die));

	//Apply damage to each group
	for (int i = 0; i < currentBattle.Group1.Num(); ++i)
	{
		armies[currentBattle.Group1[i]].currentHP -= group2DamageTotal;
		armies[currentBattle.Group1[i]].currentHP = FMath::Clamp(armies[currentBattle.Group1[i]].currentHP, 0, armies[currentBattle.Group1[i]].maxHP);

		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Purple, FString::Printf(TEXT("Group 1 took %d damage! %d health remaining! %d morale remaining!"), group2DamageTotal, armies[currentBattle.Group1[i]].currentHP, armies[currentBattle.Group1[i]].currentMorale));
	}
	for (int i = 0; i < currentBattle.Group2.Num(); ++i)
	{
		armies[currentBattle.Group2[i]].currentHP -= group1DamageTotal;
		armies[currentBattle.Group2[i]].currentHP = FMath::Clamp(armies[currentBattle.Group2[i]].currentHP, 0, armies[currentBattle.Group2[i]].maxHP);

		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Purple, FString::Printf(TEXT("Group 2 took %d damage! %d health remaining! %d morale remaining!"), group1DamageTotal, armies[currentBattle.Group2[i]].currentHP, armies[currentBattle.Group1[i]].currentMorale));
	}

	//Created array of armies to remove this combat tick
	TArray<Factions> armiesToRemove;

	//Add armies to the remove list as they are killed or their morale is depleted
	for (auto& army : armies)
	{
		if (!IsAlive(army.Value))
		{
			armiesToRemove.Add(army.Key);
			continue;
		}

		if (DecayMorale(army.Key, moraleDecayRate) <= 0.f)
		{
			armiesToRemove.Add(FleeFromBattle(army.Key));
		}
	}
	
	//Remove armies that satisfy removal requirements
	if (!armiesToRemove.IsEmpty())
	{
		for (int i = 0; i < armiesToRemove.Num(); i++)
		{
			RemoveArmy(armiesToRemove[i]);
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
	if (ending) return;

	UCombatAnims* group1Anims = Cast<UCombatAnims>(group1Mesh->GetAnimInstance());
	UCombatAnims* group2Anims = Cast<UCombatAnims>(group2Mesh->GetAnimInstance());
	
	if (currentBattle.Group1.IsEmpty())
	{
		group1Anims->isFightLost = true;
	}
	else
	{
		group1Anims->isFightWon = true;
	}

	if (currentBattle.Group2.IsEmpty())
	{
		group2Anims->isFightLost = true;
	}
	else
	{
		group2Anims->isFightWon = true;
	}

	if (currentBattle.Group1.Contains(Factions::Human) || currentBattle.Group2.Contains(Factions::Human))
	{
		spawner->controller->PlayUISound(spawner->controller->battleVictorySound);
	}
	else
	{
		spawner->controller->PlayUISound(spawner->controller->battleDefeatSound);
	}

	attacking = false;
	ending = true;
}

void ABattleObject::DestroyBattle()
{
	for (auto faction : factionsInBattle)
	{
		if (faction.Value.IsEmpty()) continue;
		ExtractFactionUnits(faction.Key);
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

Factions ABattleObject::FleeFromBattle(Factions faction)
{
	if (!factionsInBattle.Contains(faction)) return Factions::None;

	TArray<ATroop*> fleeingTroops = ExtractFactionUnits(faction, true);

	for (int i = 0; i < fleeingTroops.Num(); i++)
	{
		//Troop health reduced by 50%
		float troopHP = fleeingTroops[i]->unitStats->currentHP;
		troopHP *= 0.5f;
		if (troopHP < 1.f) troopHP = 1.f;
		fleeingTroops[i]->unitStats->currentHP = troopHP;
	}

	return faction;
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
	return groupDie;
}

float ABattleObject::GetRollModifier(int& groupDie)
{
	switch (groupDie)
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
	groupCompositions[0] = GetArmyComposition(currentBattle.Group1);
	groupCompositions[1] = GetArmyComposition(currentBattle.Group2);

	group1Damage = 0;
	group2Damage = 0;

	//For each unit type
	for (auto& unit : groupCompositions[0])
	{
		float damage = 0;

		//For each enemy type in the enemy army
		for (auto& enemyUnit : groupCompositions[1])
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
	for (auto& unit : groupCompositions[1])
	{
		float damage = 0;

		//For each enemy type in the enemy army
		for (auto& enemyUnit : groupCompositions[0])
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

TMap<UnitTypes, FUnitComposition> ABattleObject::GetArmyComposition(TArray<Factions>& group)
{
	TMap<UnitTypes, FUnitComposition> unitsInArmy;

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
						unitsInArmy.Add(factionsInBattle[faction][i].savedUnits[j].unitType, FUnitComposition{0, 0.f});
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
				unitsInArmy.Add(factionsInBattle[faction][i].unitType, FUnitComposition{ 0, 0.f });
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