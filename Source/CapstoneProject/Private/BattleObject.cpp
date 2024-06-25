// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleObject.h"
#include "Troop.h"
#include "MergedArmy.h"
#include "CapstoneProjectGameModeBase.h"
#include "GameFramework/GameModeBase.h"

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

	interact->CreateCollision(group1Mesh);
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

	group1Anims = Cast<UCombatAnims>(group1Mesh->GetAnimInstance());
	group2Anims = Cast<UCombatAnims>(group2Mesh->GetAnimInstance());
}

void ABattleObject::Destroyed()
{
	if (selectedByPlayer)
	{
		AActor* controllerTemp = UGameplayStatics::GetActorOfClass(GetWorld(), ABasePlayerController::StaticClass());
		ABasePlayerController* controller = Cast<ABasePlayerController>(controllerTemp);

		if (controller) controller->Deselect();
	}

	Super::Destroyed();
}

void ABattleObject::Start()
{
	//Find hex the object is placed on
	hex = hexNav->GetCurrentHex();
	hex->battle = this;
	AGlobalSpawner::spawnerObject->controller->PlayUISound(AGlobalSpawner::spawnerObject->controller->battleStartSound);

	//Once created, begin organizing armies into factions
	CreateFactions();
}

// Called every frame
void ABattleObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!group1Anims || !group2Anims)
	{
		group1Anims = Cast<UCombatAnims>(group1Mesh->GetAnimInstance());
		group2Anims = Cast<UCombatAnims>(group2Mesh->GetAnimInstance());
	}

	if (group1Anims) group1Anims->timeScale = ACapstoneProjectGameModeBase::timeScale;
	if (group2Anims) group2Anims->timeScale = ACapstoneProjectGameModeBase::timeScale;

	if (!hex->troopsInHex.IsEmpty() && !ending)
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
		timeTillEnd -= DeltaTime * ACapstoneProjectGameModeBase::timeScale;
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
	//Get the number of troops in the hex (since we'll be removing elements mid-loop)
	int hexPop = hex->troopsInHex.Num();

	//The attacking faction is very likely to own the last troop to enter the hex, so the troop at the end of the array is set as the attacker
	attackingFaction = hex->GetAttackerFaction();

	//Set attacking faction to group 1 for consistency
	currentBattle.Group1.Add(attackingFaction);

	//For each troop in the hex, add it to its respective faction
	for (int i = 0; i < hexPop; ++i)
	{
		if (hex->troopsInHex.IsEmpty()) break;

		AddUnitToFaction(hex->troopsInHex[0]);
	}

	//Once sides are determined, visualize each group based on their faction composition
	GenerateModels();

	//Begin the combat loop
	attacking = true;
}

//Add a unit to a faction following the creation of this battle
void ABattleObject::AddUnitToFaction(AMovementAI* troop)
{
	//Extract data from the troop
	UnitActions::UnitData unitData = UnitActions::CollectUnitData(troop->unitStats);

	//If the troop's faction does not yet exist in the battle, create it and add it to the list
	if (!factionsInBattle.Contains(unitData.faction))
	{
		TArray<UnitActions::UnitData> newArmy;
		factionsInBattle.Add(unitData.faction, newArmy);

		//If the newly joined unit is (an ally to) the player, set the battle to be visible to the player
		if (UnitActions::GetFaction(Factions::Human)->GetFactionRelationship(unitData.faction) == FactionRelationship::Ally)
		{
			visibility->faction = Factions::Human;
		}
	}

	//Add this unit to the faction unit list and its data to the faction army unit
	AddUnitToArmy(unitData);

	//Remove the unit from the hex and destroy it in the game world
	hex->troopsInHex.Remove(troop);
	troop->Destroy();
}

void ABattleObject::GenerateModels()
{
	USkeletalMesh* robotMesh = LoadObject<USkeletalMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Animations/robot_all_beta.robot_all_beta'"));
	USkeletalMesh* alienMesh = LoadObject<USkeletalMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Animations/robot_all_beta.robot_all_beta'"));

	if (currentBattle.Group1.Contains(Factions::Human))
	{
		group1Mesh->SetSkeletalMesh(robotMesh);
		group1Mesh->SetMaterial(0, AGlobalSpawner::spawnerObject->troopFactionMaterials[Factions::Human].visibleTexture);
	}
	else
	{
		group1Mesh->SetSkeletalMesh(alienMesh);
		group1Mesh->SetMaterial(0, AGlobalSpawner::spawnerObject->troopFactionMaterials[Factions::Alien1].visibleTexture);
	}

	if (currentBattle.Group2.Contains(Factions::Human))
	{
		group2Mesh->SetSkeletalMesh(robotMesh);
		group2Mesh->SetMaterial(0, AGlobalSpawner::spawnerObject->troopFactionMaterials[Factions::Human].visibleTexture);
	}
	else
	{
		group2Mesh->SetSkeletalMesh(alienMesh);
		group2Mesh->SetMaterial(0, AGlobalSpawner::spawnerObject->troopFactionMaterials[Factions::Alien1].visibleTexture);
	}
}

//Adds the input unit to the faction unit list and its data to the faction army unit
void ABattleObject::AddUnitToArmy(UnitActions::UnitData data)
{
	factionsInBattle[data.faction].Add(data);

	if (!armies.Contains(data.faction))
	{
		UnitActions::UnitData army{ data.faction, UnitTypes::Army, false, 0, 0, 0, 0, 0, 0 };

		armies.Add(data.faction, army);
	}

	armies[data.faction] = UnitActions::AddUnitData(armies[data.faction], data);

	AssignFactionToGroup(data.faction);
}

void ABattleObject::AssignFactionToGroup(Factions army)
{
	//Skip if already in one of the two groups
	if (currentBattle.Group1.Contains(army) || currentBattle.Group2.Contains(army)) return;

	//Determine this faction's alignment to other factions in the conflict, using that to select which group to join, if any
	EngagementSelect assignment = UnitActions::DetermineConflictAlignment(army, currentBattle.Group1, currentBattle.Group2);

	//Assign to the resulting group
	switch (assignment)
	{
	case EngagementSelect::JoinGroup1:
		currentBattle.Group1.Add(army);
		break;
	case EngagementSelect::JoinGroup2:
		currentBattle.Group2.Add(army);
		break;
	}
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
	
	if (currentBattle.Group1.IsEmpty())
	{
		if (group1Anims) group1Anims->isFightLost = true;
	}
	else
	{
		if (group1Anims) group1Anims->isFightWon = true;
	}

	if (currentBattle.Group2.IsEmpty())
	{
		if (group2Anims) group2Anims->isFightLost = true;
	}
	else
	{
		if (group2Anims) group2Anims->isFightWon = true;
	}

	if (currentBattle.Group1.Contains(Factions::Human) || currentBattle.Group2.Contains(Factions::Human))
	{
		AGlobalSpawner::spawnerObject->controller->PlayUISound(AGlobalSpawner::spawnerObject->controller->battleVictorySound);
	}
	else
	{
		AGlobalSpawner::spawnerObject->controller->PlayUISound(AGlobalSpawner::spawnerObject->controller->battleDefeatSound);
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

	hex->battle = nullptr;
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

	RemoveArmy(faction);
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
			spawn = AGlobalSpawner::spawnerObject->SpawnArmy(spawnPoint, factionsInBattle[faction][i].savedUnits, hpPercent);
			break;

		default:
			spawn = AGlobalSpawner::spawnerObject->SpawnTroop(spawnPoint, factionsInBattle[faction][i], hpPercent);
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
	if (!currentBattle.Group1.IsEmpty())
		groupCompositions[0] = GetArmyComposition(currentBattle.Group1);
	if (!currentBattle.Group2.IsEmpty())
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
				percent = AGlobalSpawner::spawnerObject->troopStats[unit.Key].attackvsInfantry;
				break;
			case UnitTypes::Cavalry:
				percent = AGlobalSpawner::spawnerObject->troopStats[unit.Key].attackvsCavalry;
				break;
			case UnitTypes::Ranged:
				percent = AGlobalSpawner::spawnerObject->troopStats[unit.Key].attackvsRanged;
				break;
			case UnitTypes::Shielder:
				percent = AGlobalSpawner::spawnerObject->troopStats[unit.Key].attackvsShielder;
				break;
			case UnitTypes::Scout:
				percent = AGlobalSpawner::spawnerObject->troopStats[unit.Key].attackvsScout;
				break;
			case UnitTypes::Settler:
				percent = AGlobalSpawner::spawnerObject->troopStats[unit.Key].attackvsSettler;
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
		damage *= AGlobalSpawner::spawnerObject->troopStats[unit.Key].damage;

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
				percent = AGlobalSpawner::spawnerObject->troopStats[unit.Key].defendvsInfantry;
				break;
			case UnitTypes::Cavalry:
				percent = AGlobalSpawner::spawnerObject->troopStats[unit.Key].defendvsCavalry;
				break;
			case UnitTypes::Ranged:
				percent = AGlobalSpawner::spawnerObject->troopStats[unit.Key].defendvsRanged;
				break;
			case UnitTypes::Shielder:
				percent = AGlobalSpawner::spawnerObject->troopStats[unit.Key].defendvsShielder;
				break;
			case UnitTypes::Scout:
				percent = AGlobalSpawner::spawnerObject->troopStats[unit.Key].defendvsScout;
				break;
			case UnitTypes::Settler:
				percent = AGlobalSpawner::spawnerObject->troopStats[unit.Key].defendvsSettler;
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
		damage *= AGlobalSpawner::spawnerObject->troopStats[unit.Key].damage;

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
		if (!factionsInBattle.Contains(faction)) continue;

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

float ABattleObject::DisplayBattleProgress()
{
	int group1TotalHealth = 0;
	int group2TotalHealth = 0;
	int group1CurrentHealth = 0;
	int group2CurrentHealth = 0;

	for (int i = 0; i < currentBattle.Group1.Num(); i++)
	{
		group1CurrentHealth += armies[currentBattle.Group1[i]].currentHP;
		group1TotalHealth += armies[currentBattle.Group1[i]].maxHP;
	}

	for (int i = 0; i < currentBattle.Group2.Num(); i++)
	{
		group2CurrentHealth += armies[currentBattle.Group2[i]].currentHP;
		group2TotalHealth += armies[currentBattle.Group2[i]].maxHP;
	}

	float group1Percent = (float)group1CurrentHealth / (float)group1TotalHealth;
	float group2Percent = (float)group2CurrentHealth / (float)group2TotalHealth;

	float totalPercent = (group1Percent + 1 - group2Percent) / 2;

	return totalPercent;
}
