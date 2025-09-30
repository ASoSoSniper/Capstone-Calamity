// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleObject.h"
#include "Troop.h"
#include "MergedArmy.h"
#include "CapstoneProjectGameModeBase.h"
#include "GameFramework/GameModeBase.h"

#pragma region General Logic
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

void ABattleObject::SetSelected(bool selected)
{
	selectedByPlayer = selected;
}
ABaseHex* ABattleObject::GetHex()
{
	return hex;
}
#pragma endregion

#pragma region Setup and Teardown
void ABattleObject::Start()
{
	//Find hex the object is placed on
	hex = hexNav->GetCurrentHex();
	hex->battle = this;
	AGlobalSpawner::spawnerObject->controller->PlayUISound(AGlobalSpawner::spawnerObject->controller->battleStartSound);

	//Once created, begin organizing armies into factions
	CreateFactions();
}
//Collect all units in the hex and sort them into factions
void ABattleObject::CreateFactions()
{
	//Get the number of troops in the hex (since we'll be removing elements mid-loop)
	int hexPop = hex->troopsInHex.Num();

	//The attacking faction is very likely to own the last troop to enter the hex, so the troop at the end of the array is set as the attacker
	attackingFaction = hex->GetAttackerFaction();

	//Set attacking faction to group 1 for consistency
	currentBattle.Group1.Add(attackingFaction, new FUnitData(attackingFaction, UnitTypes::Army));

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

bool ABattleObject::IsEnding()
{
	return ending;
}
bool ABattleObject::EndCondition() const
{
	return currentBattle.Group1.IsEmpty() || currentBattle.Group2.IsEmpty();
}
void ABattleObject::EndBattle()
{
	if (ending) return;

	bool group1ContainsHuman = false;
	bool group2ContainsHuman = false;

	//Check which groups are alive, if any. A group will be dead if it is empty.
	if (Group1IsAlive(group1ContainsHuman))
	{
		if (group1Anims) group1Anims->isFightWon = true;
	}
	else
	{
		if (group1Anims) group1Anims->isFightLost = true;
	}

	if (Group2IsAlive(group2ContainsHuman))
	{
		if (group2Anims) group2Anims->isFightWon = true;
	}
	else
	{
		if (group2Anims) group2Anims->isFightLost = true;
	}

	//If humans exist in any capacity (since they would be removed otherwise),
	//then humans are on the winning side and the victory sound plays.
	if (group1ContainsHuman || group2ContainsHuman)
	{
		AGlobalSpawner::spawnerObject->controller->PlayUISound(AGlobalSpawner::spawnerObject->controller->battleVictorySound);
	}
	//Otherwise, visibility being set to Human indicates a prior human presence
	//before being slain, thus a defeat sound is played.
	else if (visibility->faction == Factions::Human)
	{
		AGlobalSpawner::spawnerObject->controller->PlayUISound(AGlobalSpawner::spawnerObject->controller->battleDefeatSound);
	}

	attacking = false;
	ending = true;
}
void ABattleObject::DestroyBattle()
{
	currentBattle.ExtractAllFactions(hex);

	hex->battle = nullptr;
	Destroy();
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
#pragma endregion

#pragma region Army Management
Factions ABattleObject::FleeFromBattle(Factions faction)
{
	if (!currentBattle.ContainsFaction(faction)) return Factions::None;

	TSet<ABaseHex*> usedHexes;
	ATroop* troop = currentBattle.ExtractFaction(faction, hex, usedHexes, true);

	troop->GetUnitData()->SetHPByAlpha(0.5f, false);

	//Hack to instantly destroy the battle if everyone has fled
	if (currentBattle.Group1.IsEmpty() && currentBattle.Group2.IsEmpty())
		timeTillEnd = 0;

	return faction;
}

void ABattleObject::GetGroupHealthAndMorale(int groupIndex, int& HP, int& maxHP, int& morale, int& maxMorale) const
{
	const TMap<Factions, FUnitData*>& group = (groupIndex == 0) ? currentBattle.Group1 : currentBattle.Group2;

	for (const TPair<Factions, FUnitData*>& army : group)
	{
		HP += army.Value->GetCurrentHP();
		maxHP += army.Value->GetMaxHP();
		morale += army.Value->GetCurrentMorale();
		maxMorale += army.Value->GetMaxMorale();
	}
}

void ABattleObject::AddUnitToFaction(AMovementAI* troop)
{
	//Get data from the troop
	FUnitData* unitData = troop->GetUnitData();
	Factions unitFaction = unitData->GetFaction();

	//Add the troop to an army, and if successful, destroy its presence in world-space
	if (currentBattle.AddUnit(unitData)) 
		troop->Destroy();

	//If the newly joined unit is (an ally to) the player, set the battle to be visible to the player
	if (UnitActions::GetFaction(Factions::Human)->GetFactionRelationship(unitFaction) == FactionRelationship::Ally)
	{
		if (visibility->faction != Factions::Human)
			visibility->faction = unitFaction;
	}
}
void ABattleObject::DestroyArmy(Factions faction)
{
	if (currentBattle.Group1.Contains(faction))
	{
		currentBattle.Group1[faction]->DestroyWorldData();
		currentBattle.Group1.Remove(faction);
	}
	else if (currentBattle.Group2.Contains(faction))
	{
		currentBattle.Group2[faction]->DestroyWorldData();
		currentBattle.Group2.Remove(faction);
	}
}

TMap<UnitTypes, FUnitComposition> ABattleObject::GetArmyComposition(TMap<Factions, FUnitData*>& group)
{
	TMap<UnitTypes, FUnitComposition> unitsInArmy;
	unitsInArmy.Add(UnitTypes::Infantry, FUnitComposition{});
	unitsInArmy.Add(UnitTypes::Cavalry, FUnitComposition{});
	unitsInArmy.Add(UnitTypes::Scout, FUnitComposition{});
	unitsInArmy.Add(UnitTypes::Ranged, FUnitComposition{});
	unitsInArmy.Add(UnitTypes::Shielder, FUnitComposition{});
	unitsInArmy.Add(UnitTypes::Settler, FUnitComposition{});

	int totalUnits = 0;

	//Add troop type quantities for each faction in the group
	for (TPair<Factions, FUnitData*>& army : group)
	{
		FUnitData* data = army.Value;

		TMap<UnitTypes, FUnitComposition> unitComp = data->GetUnitComposition();
		for (TPair<UnitTypes, FUnitComposition>& unit : unitComp)
		{
			unitsInArmy[unit.Key].quantity += unit.Value.quantity;
			totalUnits += unit.Value.quantity;
		}
	}

	//Use the total number to calculate what percent of the army each unit type occupies
	for (auto& unit : unitsInArmy)
	{
		unit.Value.compPercent = (float)unit.Value.quantity / (float)totalUnits;
	}

	return unitsInArmy;
}

#pragma endregion

#pragma region Combat
float ABattleObject::DisplayBattleProgress()
{
	int group1MaxHP = 0;
	int group2MaxHP = 0;
	int group1HP = 0;
	int group2HP = 0;

	int group1MaxMorale = 0;
	int group2MaxMorale = 0;
	int group1Morale = 0;
	int group2Morale = 0;

	GetGroupHealthAndMorale(0, group1HP, group1MaxHP, group1Morale, group1MaxMorale);
	GetGroupHealthAndMorale(1, group2HP, group2MaxHP, group2Morale, group2MaxMorale);

	float group1Percent = (float)group1HP / (float)group1MaxHP;
	float group2Percent = (float)group2HP / (float)group2MaxHP;

	float totalPercent = (group1Percent + 1 - group2Percent) / 2;

	return totalPercent;
}

bool ABattleObject::IsAttacking()
{
	return attacking;
}
int ABattleObject::GetGroup1Die()
{
	return group1Die;
}
int ABattleObject::GetGroup2Die()
{
	return group2Die;
}

void ABattleObject::Attack()
{
	if (!attacking) return;

	//End the battle if one group is completely wiped out
	if (EndCondition())
	{
		EndBattle();
		return;
	}

	int group1Damage = 0;
	int group2Damage = 0;

	CalculateGroupDamage(group1Damage, group2Damage);
	ApplyGroupDamage(group1Damage, group2Damage);

	//End the battle if one group is completely wiped out
	if (EndCondition())
	{
		EndBattle();
	}
}
void ABattleObject::CalculateGroupDamage(int& group1DamageTotal, int& group2DamageTotal)
{
	if (!currentBattle.Group1.IsEmpty())
		groupCompositions[0] = GetArmyComposition(currentBattle.Group1);
	if (!currentBattle.Group2.IsEmpty())
		groupCompositions[1] = GetArmyComposition(currentBattle.Group2);

	auto GroupDamage = [&](TMap<UnitTypes, FUnitComposition>& attacker, TMap<UnitTypes, FUnitComposition>& reciever) -> int
		{
			int totalDamage = 0;

			//For each unit type
			for (auto& unit : attacker)
			{
				float damage = 0;

				//For each enemy type in the enemy army
				for (auto& enemyUnit : reciever)
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
				totalDamage += FMath::RoundToInt(damage) * unit.Value.quantity;
			}

			return totalDamage;
		};

	int group1Damage = GroupDamage(groupCompositions[0], groupCompositions[1]);
	int group2Damage = GroupDamage(groupCompositions[1], groupCompositions[0]);

	--currentTickTillRoll;
	if (currentTickTillRoll <= 0)
	{
		currentTickTillRoll = ticksTillRoll;
		RollDie(group1Die);
		RollDie(group2Die);
	}

	int group1DieModded = FMath::Max(group1Die - hex->GetDefenderBonus(), 1);

	group1DamageTotal = FMath::RoundToInt((float)group1Damage * GetRollModifier(group1DieModded));
	group2DamageTotal = FMath::RoundToInt((float)group2Damage * GetRollModifier(group2Die));
}
void ABattleObject::ApplyGroupDamage(const int& group1Damage, const int& group2Damage)
{
	//Created array of armies to remove this combat tick
	TArray<Factions> armiesToRemove;

	auto DamageGroup = [&](TMap<Factions, FUnitData*>& group, int damage)
		{
			for (TPair<Factions, FUnitData*>& army : group)
			{
				army.Value->DamageHP(damage);

				if (!army.Value->IsAlive() || DecayMorale(army.Value, moraleDecayRate) <= 0.f)
				{
					armiesToRemove.Add(army.Key);
				}
			}
		};

	DamageGroup(currentBattle.Group1, group2Damage);
	DamageGroup(currentBattle.Group2, group1Damage);

	//Remove armies that satisfy removal requirements
	if (!armiesToRemove.IsEmpty())
	{
		for (int i = 0; i < armiesToRemove.Num(); i++)
		{
			Factions faction = armiesToRemove[i];
			FUnitData* army = currentBattle.Group1.Contains(faction)
				? currentBattle.Group1[faction] : currentBattle.Group2[faction];

			if (!army->IsAlive())
				DestroyArmy(armiesToRemove[i]);
			else
				FleeFromBattle(armiesToRemove[i]);
		}
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

float ABattleObject::DecayMorale(FUnitData* faction, float percentReduction)
{
	if (!faction) return 0.f;

	float alpha = faction->GetMoraleAlpha() - percentReduction;

	faction->SetMoraleByAlpha(alpha);

	return faction->GetCurrentMorale();
}

bool ABattleObject::Group1IsAlive(bool& containsHuman) const
{
	containsHuman = currentBattle.Group1.Contains(Factions::Human);

	return !currentBattle.Group1.IsEmpty();
}
bool ABattleObject::Group2IsAlive(bool& containsHuman) const
{
	containsHuman = currentBattle.Group2.Contains(Factions::Human);

	return !currentBattle.Group2.IsEmpty();
}
#pragma endregion

#pragma region Battle Struct
bool ABattleObject::Battle::AddUnit(FUnitData* data)
{
	Factions unitFaction = data->GetFaction();
	EngagementSelect assignment = DetermineConflictAlignment(unitFaction);

	auto AddToFactionArmy = [&](TMap<Factions, FUnitData*>& armies)
		{
			//If army of faction does not already exist, initialize the army:
			if (!armies.Contains(unitFaction))
			{
				FUnitData* army = nullptr;

				//If unit to add is an army, set faction army to that unit
				if (data->GetUnitType() == UnitTypes::Army)
					army = data;
				//Otherwise, create a new army and add the unit to that
				else
				{
					army = new FUnitData(unitFaction, UnitTypes::Army);
					army->AddUnitData(data);
				}

				//Add the army to the list of faction armies in the group
				armies.Add(unitFaction, army);
			}
			//If already initialized, simply add the unit to the existing army
			else
				armies[unitFaction]->AddUnitData(data);
		};

	switch (assignment)
	{
	case EngagementSelect::JoinGroup1:
		AddToFactionArmy(Group1);
		return true;
	case EngagementSelect::JoinGroup2:
		AddToFactionArmy(Group2);
		return true;
	default:
		return false;
	}
}
EngagementSelect ABattleObject::Battle::DetermineConflictAlignment(Factions& unitFaction)
{
	//
	//  ALREADY PRESENT
	//  If the faction is already present, no checks are needed:
	//

	if (Group1.Contains(unitFaction))
		return EngagementSelect::JoinGroup1;
	else if (Group2.Contains(unitFaction))
		return EngagementSelect::JoinGroup2;

	//
	//  EMPTY GROUPS:
	//  If one or both groups are empty, perform a more complex relationship check:
	// 

	//If Group 1 is empty, Group 2 is also empty, so this unit can join Group 1 to start things off
	if (Group1.IsEmpty())
	{
		return EngagementSelect::JoinGroup1;
	}

	Faction* factionObject = UnitActions::GetFaction(unitFaction);

	//If Group 2 is empty, check whether Group 1 is composed entirely of enemies or allies, and assign accordingly
	if (Group2.IsEmpty())
	{
		//Declare variables to record each possible relationship in Group 1
		int alliesInGroup1 = 0;
		int neutralsInGroup1 = 0;
		int enemiesInGroup1 = 0;

		//Check Group 1 relationships to this unit's faction
		for (TPair<Factions, FUnitData*>& army : Group1)
		{
			if (army.Key != unitFaction)
			{
				FactionRelationship factionAlignment = factionObject->GetFactionRelationship(army.Key);

				//Record the faction's relationship
				switch (factionAlignment)
				{
				case FactionRelationship::Ally:
					alliesInGroup1++;
					break;
				case FactionRelationship::Neutral:
					neutralsInGroup1++;
					break;
				case FactionRelationship::Enemy:
					enemiesInGroup1++;
					break;
				}
			}
		}
		//If Group 1 is composed entirely of enemies and Group 2 is empty, join Group 2
		if (alliesInGroup1 == 0 && neutralsInGroup1 == 0 && enemiesInGroup1 > 0)
		{
			return EngagementSelect::JoinGroup2;
		}
		//If Group 1 is composed entirely of allies, join Group 1
		else if (alliesInGroup1 > 0 && neutralsInGroup1 == 0 && enemiesInGroup1 == 0)
		{
			return EngagementSelect::JoinGroup1;
		}

		//Otherwise, do not join the conflict
		return EngagementSelect::DoNotJoin;
	}

	// 
	//  NON-EMPTY GROUPS:
	//  If both groups possess factions, perform a more streamlined check:
	//

	auto ContainsEnemy = [&factionObject, &unitFaction](TMap<Factions, FUnitData*>& group) -> bool
		{
			for (TPair<Factions, FUnitData*>& army : group)
			{
				if (army.Key != unitFaction)
				{
					FactionRelationship factionAlignment = factionObject->GetFactionRelationship(army.Key);

					//If this faction not a direct ally, deny Group as a possible selection
					if (factionAlignment != FactionRelationship::Ally)
					{
						return true;
					}
				}
			}

			return false;
		};

	bool cannotJoinGroup1 = ContainsEnemy(Group1);
	bool cannotJoinGroup2 = ContainsEnemy(Group2);

	if (!cannotJoinGroup1 && cannotJoinGroup2)
	{
		return EngagementSelect::JoinGroup1;
	}
	else if (cannotJoinGroup1 && !cannotJoinGroup2)
	{
		return EngagementSelect::JoinGroup2;
	}

	return EngagementSelect::DoNotJoin;
}
bool ABattleObject::Battle::ContainsFaction(Factions faction) const
{
	return Group1.Contains(faction) || Group2.Contains(faction);
}
ATroop* ABattleObject::Battle::ExtractFaction(Factions& faction, ABaseHex* hex, TSet<ABaseHex*> usedHexes, bool spawnAtOutpost)
{
	if (!ContainsFaction(faction)) return nullptr;
	if (!hex) return nullptr;

	ATroop* troop = nullptr;

	auto SpawnUnit = [&](TMap<Factions, FUnitData*> group) -> ATroop*
		{
			float hpPercent = group[faction]->GetHPAlpha();

			ABaseHex* spawnPoint = spawnAtOutpost ? 
				UnitActions::GetClosestOutpostHex(faction, hex)->FindFreeAdjacentHex(faction, usedHexes, true, EHexSearchRules::ContainsAny, false) 
				: hex->FindFreeAdjacentHex(faction, usedHexes, false, EHexSearchRules::ContainsAny, false);

			FUnitData* unitToSpawn = nullptr;

			switch (group[faction]->GetSavedUnitCount())
			{
			case 1:
				group[faction]->DestroyWorldData();

				unitToSpawn = group[faction]->GetSavedUnits()[0];
				if (group[faction]->GetCurrentMorale() <= 0.f)
					unitToSpawn->SetMoraleByValue(1);

				return AGlobalSpawner::spawnerObject->SpawnTroop(spawnPoint, unitToSpawn, hpPercent);

			default:
				if (group[faction]->GetCurrentMorale() <= 0.f)
					group[faction]->SetMoraleByValue(1);
				return AGlobalSpawner::spawnerObject->SpawnArmy(spawnPoint, group[faction]->GetSavedUnits(), hpPercent);
			}
		};

	if (Group1.Contains(faction))
	{
		troop = SpawnUnit(Group1);
		Group1.Remove(faction);
	}
	else
	{
		troop = SpawnUnit(Group2);
		Group2.Remove(faction);
	}

	return troop;
}
TArray<ATroop*> ABattleObject::Battle::ExtractAllFactions(ABaseHex* hex)
{
	TArray<ATroop*> extracted;
	TSet<ABaseHex*> usedHexes;

	auto ExtractGroup = [&](TMap<Factions, FUnitData*>& group)
		{
			for (TPair<Factions, FUnitData*>& army : group)
			{
				ATroop* troop = ExtractFaction(army.Key, hex, usedHexes);
				if (troop)
					extracted.Add(troop);
			}
		};

	if (!Group1.IsEmpty())
		ExtractGroup(Group1);
	if (!Group2.IsEmpty())
		ExtractGroup(Group2);

	return extracted;
}
#pragma endregion