/*-------------------------------------------------------------------------------

BARONY
File: monster_vampire.cpp
Desc: implements all of the vampire monster's code

Copyright 2013-2016 (c) Turning Wheel LLC, all rights reserved.
See LICENSE for details.

-------------------------------------------------------------------------------*/

#include "main.hpp"
#include "game.hpp"
#include "stat.hpp"
#include "entity.hpp"
#include "items.hpp"
#include "monster.hpp"
#include "sound.hpp"
#include "net.hpp"
#include "collision.hpp"
#include "classdescriptions.hpp"
#include "player.hpp"

void initVampire(Entity* my, Stat* myStats)
{
	int c;
	node_t* node;

	my->sprite = 437; //Vampire head model

	//my->flags[GENIUS] = true;
	my->flags[UPDATENEEDED] = true;
	my->flags[BLOCKSIGHT] = true;
	my->flags[INVISIBLE] = false;

	if ( multiplayer != CLIENT )
	{
		MONSTER_SPOTSND = -1;
		MONSTER_SPOTVAR = 1;
		MONSTER_IDLESND = -1;
		MONSTER_IDLEVAR = 1;
	}
	if ( multiplayer != CLIENT && !MONSTER_INIT )
	{
		if ( myStats != NULL )
		{
			if ( !myStats->leader_uid )
			{
				myStats->leader_uid = 0;
			}

			// apply random stat increases if set in stat_shared.cpp or editor
			setRandomMonsterStats(myStats);

			// generate 6 items max, less if there are any forced items from boss variants
			int customItemsToGenerate = ITEM_CUSTOM_SLOT_LIMIT;

			// boss variants	

			// random effects
			if ( rand() % 10 == 0 )
			{
				myStats->EFFECTS[EFF_ASLEEP] = true;
				myStats->EFFECTS_TIMERS[EFF_ASLEEP] = 1800 + rand() % 1800;
			}

			// generates equipment and weapons if available from editor
			createMonsterEquipment(myStats);

			// create any custom inventory items from editor if available
			createCustomInventory(myStats, customItemsToGenerate);

			// count if any custom inventory items from editor
			int customItems = countCustomItems(myStats);
			//max limit of 6 custom items per entity.

			// count any inventory items set to default in edtior
			int defaultItems = countDefaultItems(myStats);

			// generate the default inventory items for the monster, provided the editor sprite allowed enough default slots
			switch ( defaultItems )
			{
				case 6:
				case 5:
				case 4:
				case 3:
				case 2:
				case 1:
					break;
				default:
					break;
			}

			//give shield
			if ( myStats->shield == NULL && myStats->EDITOR_ITEMS[ITEM_SLOT_SHIELD] == 1 )
			{
				switch ( rand() % 10 )
				{
					case 0:
					case 1:
					case 2:
						myStats->shield = newItem(TOOL_TORCH, SERVICABLE, 0, 1, rand(), false, NULL);
						break;
					case 3:
					case 4:
						break;
					case 5:
					case 6:
						myStats->shield = newItem(WOODEN_SHIELD, WORN, 0, 1, rand(), false, NULL);
						break;
					case 7:
					case 8:
						myStats->shield = newItem(BRONZE_SHIELD, WORN, 0, 1, rand(), false, NULL);
						break;
					case 9:
						myStats->shield = newItem(IRON_SHIELD, WORN, 0, 1, rand(), false, NULL);
						break;
				}
			}

			//give weapon
			if ( myStats->weapon == NULL && myStats->EDITOR_ITEMS[ITEM_SLOT_WEAPON] == 1 )
			{
				switch ( rand() % 10 )
				{
					case 0:
					case 1:
						myStats->weapon = newItem(SHORTBOW, WORN, 0, 1, rand(), false, NULL);
						break;
					case 2:
					case 3:
						myStats->weapon = newItem(BRONZE_AXE, WORN, 0, 1, rand(), false, NULL);
						break;
					case 4:
					case 5:
						myStats->weapon = newItem(BRONZE_SWORD, WORN, 0, 1, rand(), false, NULL);
						break;
					case 6:
						myStats->weapon = newItem(IRON_SPEAR, WORN, 0, 1, rand(), false, NULL);
						break;
					case 7:
						myStats->weapon = newItem(IRON_AXE, WORN, 0, 1, rand(), false, NULL);
						break;
					case 8:
						myStats->weapon = newItem(IRON_SWORD, WORN, 0, 1, rand(), false, NULL);
						break;
					case 9:
						myStats->weapon = newItem(CROSSBOW, WORN, 0, 1, rand(), false, NULL);
						break;
				}
			}

			// give helmet
			if ( myStats->helmet == NULL && myStats->EDITOR_ITEMS[ITEM_SLOT_HELM] == 1 )
			{
				switch ( rand() % 10 )
				{
					case 0:
					case 1:
					case 2:
						break;
					case 3:
						myStats->helmet = newItem(HAT_HOOD, WORN, 0, 1, rand() % 4, false, NULL);
						break;
					case 4:
						myStats->helmet = newItem(HAT_PHRYGIAN, WORN, 0, 1, 0, false, NULL);
						break;
					case 5:
						myStats->helmet = newItem(HAT_WIZARD, WORN, 0, 1, 0, false, NULL);
						break;
					case 6:
					case 7:
						myStats->helmet = newItem(LEATHER_HELM, WORN, 0, 1, 0, false, NULL);
						break;
					case 8:
					case 9:
						myStats->helmet = newItem(IRON_HELM, WORN, 0, 1, 0, false, NULL);
						break;
				}
			}

			// give cloak
			if ( myStats->cloak == NULL && myStats->EDITOR_ITEMS[ITEM_SLOT_CLOAK] == 1 )
			{
				switch ( rand() % 10 )
				{
					case 0:
					case 1:
					case 2:
					case 3:
					case 4:
					case 5:
						break;
					case 6:
					case 7:
					case 8:
						myStats->cloak = newItem(CLOAK, WORN, 0, 1, rand(), false, NULL);
						break;
					case 9:
						myStats->cloak = newItem(CLOAK_MAGICREFLECTION, WORN, 0, 1, rand(), false, NULL);
						break;
				}
			}

			// give armor
			if ( myStats->breastplate == NULL && myStats->EDITOR_ITEMS[ITEM_SLOT_ARMOR] == 1 )
			{
				switch ( rand() % 10 )
				{
					case 0:
					case 1:
					case 2:
					case 3:
					case 4:
						break;
					case 5:
					case 6:
					case 7:
						myStats->breastplate = newItem(LEATHER_BREASTPIECE, WORN, 0, 1, rand(), false, NULL);
						break;
					case 8:
					case 9:
						myStats->breastplate = newItem(IRON_BREASTPIECE, WORN, 0, 1, rand(), false, NULL);
						break;
				}
			}

			// give gloves
			if ( myStats->gloves == NULL && myStats->EDITOR_ITEMS[ITEM_SLOT_GLOVES] == 1 )
			{
				switch ( rand() % 10 )
				{
					case 0:
					case 1:
					case 2:
					case 3:
					case 4:
						break;
					case 5:
					case 6:
					case 7:
						myStats->gloves = newItem(GLOVES, WORN, 0, 1, rand(), false, NULL);
						break;
					case 8:
					case 9:
						myStats->gloves = newItem(GAUNTLETS, WORN, 0, 1, rand(), false, NULL);
						break;
				}
			}

			// give boots
			if ( myStats->shoes == NULL && myStats->EDITOR_ITEMS[ITEM_SLOT_BOOTS] == 1 )
			{
				switch ( rand() % 10 )
				{
					case 0:
					case 1:
					case 2:
					case 3:
					case 4:
						break;
					case 5:
					case 6:
					case 7:
						myStats->shoes = newItem(LEATHER_BOOTS, WORN, 0, 1, rand(), false, NULL);
						break;
					case 8:
					case 9:
						myStats->shoes = newItem(IRON_BOOTS, WORN, 0, 1, rand(), false, NULL);
						break;
				}
			}
		}
	}

	// torso
	Entity* entity = newEntity(438, 0, map.entities);
	entity->sizex = 4;
	entity->sizey = 4;
	entity->skill[2] = my->getUID();
	entity->flags[PASSABLE] = true;
	entity->flags[NOUPDATE] = true;
	entity->flags[USERFLAG2] = my->flags[USERFLAG2];
	entity->focalx = limbs[VAMPIRE][1][0]; // 0
	entity->focaly = limbs[VAMPIRE][1][1]; // 0
	entity->focalz = limbs[VAMPIRE][1][2]; // 0
	entity->behavior = &actVampireLimb;
	entity->parent = my->getUID();
	node = list_AddNodeLast(&my->children);
	node->element = entity;
	node->deconstructor = &emptyDeconstructor;
	node->size = sizeof(Entity*);

	// right leg
	entity = newEntity(444, 0, map.entities);
	entity->sizex = 4;
	entity->sizey = 4;
	entity->skill[2] = my->getUID();
	entity->flags[PASSABLE] = true;
	entity->flags[NOUPDATE] = true;
	entity->flags[USERFLAG2] = my->flags[USERFLAG2];
	entity->focalx = limbs[VAMPIRE][2][0]; // 0
	entity->focaly = limbs[VAMPIRE][2][1]; // 0
	entity->focalz = limbs[VAMPIRE][2][2]; // 2
	entity->behavior = &actVampireLimb;
	entity->parent = my->getUID();
	node = list_AddNodeLast(&my->children);
	node->element = entity;
	node->deconstructor = &emptyDeconstructor;
	node->size = sizeof(Entity*);

	// left leg
	entity = newEntity(443, 0, map.entities);
	entity->sizex = 4;
	entity->sizey = 4;
	entity->skill[2] = my->getUID();
	entity->flags[PASSABLE] = true;
	entity->flags[NOUPDATE] = true;
	entity->flags[USERFLAG2] = my->flags[USERFLAG2];
	entity->focalx = limbs[VAMPIRE][3][0]; // 0
	entity->focaly = limbs[VAMPIRE][3][1]; // 0
	entity->focalz = limbs[VAMPIRE][3][2]; // 2
	entity->behavior = &actVampireLimb;
	entity->parent = my->getUID();
	node = list_AddNodeLast(&my->children);
	node->element = entity;
	node->deconstructor = &emptyDeconstructor;
	node->size = sizeof(Entity*);

	// right arm
	entity = newEntity(439, 0, map.entities);
	entity->sizex = 4;
	entity->sizey = 4;
	entity->skill[2] = my->getUID();
	entity->flags[PASSABLE] = true;
	entity->flags[NOUPDATE] = true;
	entity->flags[USERFLAG2] = my->flags[USERFLAG2];
	entity->focalx = limbs[VAMPIRE][4][0]; // 0
	entity->focaly = limbs[VAMPIRE][4][1]; // 0
	entity->focalz = limbs[VAMPIRE][4][2]; // 1.5
	entity->behavior = &actVampireLimb;
	entity->parent = my->getUID();
	node = list_AddNodeLast(&my->children);
	node->element = entity;
	node->deconstructor = &emptyDeconstructor;
	node->size = sizeof(Entity*);

	// left arm
	entity = newEntity(441, 0, map.entities);
	entity->sizex = 4;
	entity->sizey = 4;
	entity->skill[2] = my->getUID();
	entity->flags[PASSABLE] = true;
	entity->flags[NOUPDATE] = true;
	entity->flags[USERFLAG2] = my->flags[USERFLAG2];
	entity->focalx = limbs[VAMPIRE][5][0]; // 0
	entity->focaly = limbs[VAMPIRE][5][1]; // 0
	entity->focalz = limbs[VAMPIRE][5][2]; // 1.5
	entity->behavior = &actVampireLimb;
	entity->parent = my->getUID();
	node = list_AddNodeLast(&my->children);
	node->element = entity;
	node->deconstructor = &emptyDeconstructor;
	node->size = sizeof(Entity*);

	// world weapon
	entity = newEntity(-1, 0, map.entities);
	entity->sizex = 4;
	entity->sizey = 4;
	entity->skill[2] = my->getUID();
	entity->flags[PASSABLE] = true;
	entity->flags[NOUPDATE] = true;
	entity->flags[USERFLAG2] = my->flags[USERFLAG2];
	entity->focalx = limbs[VAMPIRE][6][0]; // 1.5
	entity->focaly = limbs[VAMPIRE][6][1]; // 0
	entity->focalz = limbs[VAMPIRE][6][2]; // -.5
	entity->behavior = &actVampireLimb;
	entity->parent = my->getUID();
	entity->pitch = .25;
	node = list_AddNodeLast(&my->children);
	node->element = entity;
	node->deconstructor = &emptyDeconstructor;
	node->size = sizeof(Entity*);

	// shield
	entity = newEntity(-1, 0, map.entities);
	entity->sizex = 4;
	entity->sizey = 4;
	entity->skill[2] = my->getUID();
	entity->flags[PASSABLE] = true;
	entity->flags[NOUPDATE] = true;
	entity->flags[USERFLAG2] = my->flags[USERFLAG2];
	entity->focalx = limbs[VAMPIRE][7][0]; // 2
	entity->focaly = limbs[VAMPIRE][7][1]; // 0
	entity->focalz = limbs[VAMPIRE][7][2]; // 0
	entity->behavior = &actVampireLimb;
	entity->parent = my->getUID();
	node = list_AddNodeLast(&my->children);
	node->element = entity;
	node->deconstructor = &emptyDeconstructor;
	node->size = sizeof(Entity*);

	// cloak
	entity = newEntity(-1, 0, map.entities);
	entity->sizex = 4;
	entity->sizey = 4;
	entity->skill[2] = my->getUID();
	entity->scalex = 1.01;
	entity->scaley = 1.01;
	entity->scalez = 1.01;
	entity->flags[PASSABLE] = true;
	entity->flags[NOUPDATE] = true;
	entity->flags[USERFLAG2] = my->flags[USERFLAG2];
	entity->focalx = limbs[VAMPIRE][8][0]; // 0
	entity->focaly = limbs[VAMPIRE][8][1]; // 0
	entity->focalz = limbs[VAMPIRE][8][2]; // 4
	entity->behavior = &actVampireLimb;
	entity->parent = my->getUID();
	node = list_AddNodeLast(&my->children);
	node->element = entity;
	node->deconstructor = &emptyDeconstructor;
	node->size = sizeof(Entity*);

	// helmet
	entity = newEntity(-1, 0, map.entities);
	entity->sizex = 4;
	entity->sizey = 4;
	entity->skill[2] = my->getUID();
	entity->scalex = 1.01;
	entity->scaley = 1.01;
	entity->scalez = 1.01;
	entity->flags[PASSABLE] = true;
	entity->flags[NOUPDATE] = true;
	entity->flags[USERFLAG2] = my->flags[USERFLAG2];
	entity->focalx = limbs[VAMPIRE][9][0]; // 0
	entity->focaly = limbs[VAMPIRE][9][1]; // 0
	entity->focalz = limbs[VAMPIRE][9][2]; // -1.75
	entity->behavior = &actVampireLimb;
	entity->parent = my->getUID();
	node = list_AddNodeLast(&my->children);
	node->element = entity;
	node->deconstructor = &emptyDeconstructor;
	node->size = sizeof(Entity*);

	// mask
	entity = newEntity(-1, 0, map.entities);
	entity->sizex = 4;
	entity->sizey = 4;
	entity->skill[2] = my->getUID();
	entity->scalex = .99;
	entity->scaley = .99;
	entity->scalez = .99;
	entity->flags[PASSABLE] = true;
	entity->flags[NOUPDATE] = true;
	entity->flags[USERFLAG2] = my->flags[USERFLAG2];
	entity->focalx = limbs[VAMPIRE][10][0]; // 0
	entity->focaly = limbs[VAMPIRE][10][1]; // 0
	entity->focalz = limbs[VAMPIRE][10][2]; // .5
	entity->behavior = &actVampireLimb;
	entity->parent = my->getUID();
	node = list_AddNodeLast(&my->children);
	node->element = entity;
	node->deconstructor = &emptyDeconstructor;
	node->size = sizeof(Entity*);

	if ( multiplayer == CLIENT )
	{
		my->sprite = 437; // vampire head model
		return;
	}

	// set head model
	//if ( myStats->appearance < 5 )
	//{
	//	my->sprite = 113 + 12 * myStats->sex + myStats->appearance;
	//}
	//else if ( myStats->appearance == 5 )
	//{
	//	my->sprite = 332 + myStats->sex;
	//}
	//else if ( myStats->appearance >= 6 && myStats->appearance < 12 )
	//{
	//	my->sprite = 341 + myStats->sex * 13 + myStats->appearance - 6;
	//}
	//else if ( myStats->appearance >= 12 )
	//{
	//	my->sprite = 367 + myStats->sex * 13 + myStats->appearance - 12;
	//}
	//else
	//{
	//	my->sprite = 113; // default
	//}
}

void actVampireLimb(Entity* my)
{
	int i;

	Entity* parent = NULL;
	if ( (parent = uidToEntity(my->skill[2])) == NULL )
	{
		list_RemoveNode(my->mynode);
		return;
	}

	if ( my->light != NULL )
	{
		list_RemoveNode(my->light->node);
		my->light = NULL;
	}

	if ( multiplayer != CLIENT )
	{
		for ( i = 0; i < MAXPLAYERS; i++ )
		{
			if ( inrange[i] )
			{
				if ( i == 0 && selectedEntity == my )
				{
					parent->skill[13] = i + 1;
				}
				else if ( client_selected[i] == my )
				{
					parent->skill[13] = i + 1;
				}
			}
		}
	}

	int torch = 0;
	if ( my->flags[INVISIBLE] == false )
	{
		if ( my->sprite == 93 )   // torch
		{
			torch = 6;
		}
		else if ( my->sprite == 94 )     // lantern
		{
			torch = 9;
		}
		else if ( my->sprite == 529 )	// crystal shard
		{
			torch = 4;
		}
	}
	if ( torch != 0 )
	{
		my->light = lightSphereShadow(my->x / 16, my->y / 16, torch, 50 + 15 * torch);
	}
}

void vampireDie(Entity* my)
{
	node_t* node, *nextnode;

	int c;
	for ( c = 0; c < 5; c++ )
	{
		Entity* gib = spawnGib(my);
		serverSpawnGibForClient(gib);
	}
	if ( spawn_blood )
	{
		int x, y;
		x = std::min<unsigned int>(std::max<int>(0, my->x / 16), map.width - 1);
		y = std::min<unsigned int>(std::max<int>(0, my->y / 16), map.height - 1);
		if ( map.tiles[y * MAPLAYERS + x * MAPLAYERS * map.height] )
		{
			if ( !checkObstacle(my->x, my->y, my, NULL) )
			{
				Entity* entity = newEntity(160, 1, map.entities);
				entity->x = my->x;
				entity->y = my->y;
				entity->z = 7.4 + (rand() % 20) / 100.f;
				entity->parent = my->getUID();
				entity->sizex = 2;
				entity->sizey = 2;
				entity->yaw = (rand() % 360) * PI / 180.0;
				entity->flags[UPDATENEEDED] = true;
				entity->flags[PASSABLE] = true;
			}
		}
	}
	playSoundEntity(my, 28, 128);
	int i = 0;
	for ( node = my->children.first; node != NULL; node = nextnode )
	{
		nextnode = node->next;
		if ( node->element != NULL && i >= 2 )
		{
			Entity* entity = (Entity*)node->element;
			if ( entity->light != NULL )
			{
				list_RemoveNode(entity->light->node);
			}
			entity->light = NULL;
			list_RemoveNode(entity->mynode);
		}
		list_RemoveNode(node);
		i++;
	}
	list_RemoveNode(my->mynode);
	return;
}

#define VAMPIREWALKSPEED .12

void vampireMoveBodyparts(Entity* my, Stat* myStats, double dist)
{
	node_t* node;
	Entity* entity = NULL, *entity2 = NULL;
	Entity* rightbody = NULL;
	Entity* weaponarm = NULL;
	int bodypart;
	bool wearingring = false;

	// set invisibility
	if ( multiplayer != CLIENT )
	{
		if ( myStats->ring != NULL )
			if ( myStats->ring->type == RING_INVISIBILITY )
			{
				wearingring = true;
			}
		if ( myStats->cloak != NULL )
			if ( myStats->cloak->type == CLOAK_INVISIBILITY )
			{
				wearingring = true;
			}
		if ( myStats->EFFECTS[EFF_INVISIBLE] == true || wearingring == true )
		{
			my->flags[INVISIBLE] = true;
			my->flags[BLOCKSIGHT] = false;
			bodypart = 0;
			for ( node = my->children.first; node != NULL; node = node->next )
			{
				if ( bodypart < 2 )
				{
					bodypart++;
					continue;
				}
				if ( bodypart >= 7 )
				{
					break;
				}
				entity = (Entity*)node->element;
				if ( !entity->flags[INVISIBLE] )
				{
					entity->flags[INVISIBLE] = true;
					serverUpdateEntityBodypart(my, bodypart);
				}
				bodypart++;
			}
		}
		else
		{
			my->flags[INVISIBLE] = false;
			my->flags[BLOCKSIGHT] = true;
			bodypart = 0;
			for ( node = my->children.first; node != NULL; node = node->next )
			{
				if ( bodypart < 2 )
				{
					bodypart++;
					continue;
				}
				if ( bodypart >= 7 )
				{
					break;
				}
				entity = (Entity*)node->element;
				if ( entity->flags[INVISIBLE] )
				{
					entity->flags[INVISIBLE] = false;
					serverUpdateEntityBodypart(my, bodypart);
				}
				bodypart++;
			}
		}

		// sleeping
		if ( myStats->EFFECTS[EFF_ASLEEP] )
		{
			my->z = 1.5;
			my->pitch = PI / 4;
		}
		else
		{
			my->z = -1;
			my->pitch = 0;
		}

		// levitation
		bool levitating = isLevitating(myStats);
		if ( levitating )
		{
			my->z -= 1; // floating
		}
	}

	// move bodyparts
	for ( bodypart = 0, node = my->children.first; node != NULL; node = node->next, bodypart++ )
	{
		if ( bodypart < 2 )
		{
			continue;
		}
		entity = (Entity*)node->element;
		entity->x = my->x;
		entity->y = my->y;
		entity->z = my->z;
		entity->yaw = my->yaw;
		if ( bodypart == 3 || bodypart == 6 )
		{
			if ( bodypart == 3 )
			{
				rightbody = (Entity*)node->next->element;
			}
			node_t* shieldNode = list_Node(&my->children, 8);
			if ( shieldNode )
			{
				Entity* shield = (Entity*)shieldNode->element;
				if ( dist > 0.1 && (bodypart != 6 || shield->flags[INVISIBLE]) )
				{
					if ( !rightbody->skill[0] )
					{
						entity->pitch -= dist * VAMPIREWALKSPEED;
						if ( entity->pitch < -PI / 4.0 )
						{
							entity->pitch = -PI / 4.0;
							if ( bodypart == 3 )
							{
								entity->skill[0] = 1;
								if ( dist > .4 )
								{
									node_t* tempNode = list_Node(&my->children, 3);
									if ( tempNode )
									{
										Entity* foot = (Entity*)tempNode->element;
										if ( foot->sprite == 443 )
										{
											playSoundEntityLocal(my, 7 + rand() % 7, 32);
										}
										else if ( foot->sprite == 444 )
										{
											playSoundEntityLocal(my, 14 + rand() % 7, 32);
										}
										else
										{
											playSoundEntityLocal(my, rand() % 7, 32);
										}
									}
								}
							}
						}
					}
					else
					{
						entity->pitch += dist * VAMPIREWALKSPEED;
						if ( entity->pitch > PI / 4.0 )
						{
							entity->pitch = PI / 4.0;
							if ( bodypart == 3 )
							{
								entity->skill[0] = 0;
								if ( dist > .4 )
								{
									node_t* tempNode = list_Node(&my->children, 3);
									if ( tempNode )
									{
										Entity* foot = (Entity*)tempNode->element;
										if ( foot->sprite == 443 )
										{
											playSoundEntityLocal(my, 7 + rand() % 7, 32);
										}
										else if ( foot->sprite == 444 )
										{
											playSoundEntityLocal(my, 14 + rand() % 7, 32);
										}
										else
										{
											playSoundEntityLocal(my, rand() % 7, 32);
										}
									}
								}
							}
						}
					}
				}
				else
				{
					if ( entity->pitch < 0 )
					{
						entity->pitch += 1 / fmax(dist * .1, 10.0);
						if ( entity->pitch > 0 )
						{
							entity->pitch = 0;
						}
					}
					else if ( entity->pitch > 0 )
					{
						entity->pitch -= 1 / fmax(dist * .1, 10.0);
						if ( entity->pitch < 0 )
						{
							entity->pitch = 0;
						}
					}
				}
			}
		}
		else if ( bodypart == 4 || bodypart == 5 || bodypart == 9 )
		{
			if ( bodypart == 5 )
			{
				weaponarm = entity;
				if ( MONSTER_ATTACK == 1 )
				{
					// vertical chop
					if ( MONSTER_ATTACKTIME == 0 )
					{
						MONSTER_ARMBENDED = 0;
						MONSTER_WEAPONYAW = 0;
						entity->pitch = -3 * PI / 4;
						entity->roll = 0;
					}
					else
					{
						if ( entity->pitch >= -PI / 2 )
						{
							MONSTER_ARMBENDED = 1;
						}
						if ( entity->pitch >= PI / 4 )
						{
							entity->skill[0] = rightbody->skill[0];
							MONSTER_WEAPONYAW = 0;
							entity->pitch = rightbody->pitch;
							entity->roll = 0;
							MONSTER_ARMBENDED = 0;
							MONSTER_ATTACK = 0;
						}
						else
						{
							entity->pitch += .25;
						}
					}
				}
				else if ( MONSTER_ATTACK == 2 )
				{
					// horizontal chop
					if ( MONSTER_ATTACKTIME == 0 )
					{
						MONSTER_ARMBENDED = 1;
						MONSTER_WEAPONYAW = -3 * PI / 4;
						entity->pitch = 0;
						entity->roll = -PI / 2;
					}
					else
					{
						if ( MONSTER_WEAPONYAW >= PI / 8 )
						{
							entity->skill[0] = rightbody->skill[0];
							MONSTER_WEAPONYAW = 0;
							entity->pitch = rightbody->pitch;
							entity->roll = 0;
							MONSTER_ARMBENDED = 0;
							MONSTER_ATTACK = 0;
						}
						else
						{
							MONSTER_WEAPONYAW += .25;
						}
					}
				}
				else if ( MONSTER_ATTACK == 3 )
				{
					// stab
					if ( MONSTER_ATTACKTIME == 0 )
					{
						MONSTER_ARMBENDED = 0;
						MONSTER_WEAPONYAW = 0;
						entity->pitch = 2 * PI / 3;
						entity->roll = 0;
					}
					else
					{
						if ( MONSTER_ATTACKTIME >= 5 )
						{
							MONSTER_ARMBENDED = 1;
							entity->pitch = -PI / 6;
						}
						if ( MONSTER_ATTACKTIME >= 10 )
						{
							entity->skill[0] = rightbody->skill[0];
							MONSTER_WEAPONYAW = 0;
							entity->pitch = rightbody->pitch;
							entity->roll = 0;
							MONSTER_ARMBENDED = 0;
							MONSTER_ATTACK = 0;
						}
					}
				}
			}
			else if ( bodypart == 9 )
			{
				entity->pitch = entity->fskill[0];
			}

			if ( bodypart != 5 || (MONSTER_ATTACK == 0 && MONSTER_ATTACKTIME == 0) )
			{
				if ( dist > 0.1 )
				{
					if ( entity->skill[0] )
					{
						entity->pitch -= dist * VAMPIREWALKSPEED;
						if ( entity->pitch < -PI / 4.0 )
						{
							entity->skill[0] = 0;
							entity->pitch = -PI / 4.0;
						}
					}
					else
					{
						entity->pitch += dist * VAMPIREWALKSPEED;
						if ( entity->pitch > PI / 4.0 )
						{
							entity->skill[0] = 1;
							entity->pitch = PI / 4.0;
						}
					}
				}
				else
				{
					if ( entity->pitch < 0 )
					{
						entity->pitch += 1 / fmax(dist * .1, 10.0);
						if ( entity->pitch > 0 )
						{
							entity->pitch = 0;
						}
					}
					else if ( entity->pitch > 0 )
					{
						entity->pitch -= 1 / fmax(dist * .1, 10.0);
						if ( entity->pitch < 0 )
						{
							entity->pitch = 0;
						}
					}
				}
			}
			if ( bodypart == 9 )
			{
				entity->fskill[0] = entity->pitch;
				entity->roll = my->roll - fabs(entity->pitch) / 2;
				entity->pitch = 0;
			}
		}
		switch ( bodypart )
		{
			// torso
			case 2:
				if ( multiplayer != CLIENT )
				{
					if ( myStats->breastplate == NULL )
					{
						entity->sprite = 438;
					}
					else
					{
						entity->sprite = itemModel(myStats->breastplate);
					}
					if ( multiplayer == SERVER )
					{
						// update sprites for clients
						if ( entity->skill[10] != entity->sprite )
						{
							entity->skill[10] = entity->sprite;
							serverUpdateEntityBodypart(my, bodypart);
						}
						if ( entity->getUID() % (TICKS_PER_SECOND * 10) == ticks % (TICKS_PER_SECOND * 10) )
						{
							serverUpdateEntityBodypart(my, bodypart);
						}
					}
				}
				entity->x -= .25 * cos(my->yaw);
				entity->y -= .25 * sin(my->yaw);
				entity->z += 2.5;
				break;
				// right leg
			case 3:
				if ( multiplayer != CLIENT )
				{
					if ( myStats->shoes == NULL )
					{
						entity->sprite = 444;
					}
					else
					{
						if ( setBootSprite(myStats, entity, SPRITE_BOOT_RIGHT_OFFSET) != 0 )
						{
							// successfully set sprite for the human model
						}
					}
					if ( multiplayer == SERVER )
					{
						// update sprites for clients
						if ( entity->skill[10] != entity->sprite )
						{
							entity->skill[10] = entity->sprite;
							serverUpdateEntityBodypart(my, bodypart);
						}
						if ( entity->getUID() % (TICKS_PER_SECOND * 10) == ticks % (TICKS_PER_SECOND * 10) )
						{
							serverUpdateEntityBodypart(my, bodypart);
						}
					}
				}
				entity->x += 1 * cos(my->yaw + PI / 2) + .25 * cos(my->yaw);
				entity->y += 1 * sin(my->yaw + PI / 2) + .25 * sin(my->yaw);
				entity->z += 5;
				if ( my->z >= 1.4 && my->z <= 1.6 )
				{
					entity->yaw += PI / 8;
					entity->pitch = -PI / 2;
				}
				break;
				// left leg
			case 4:
				if ( multiplayer != CLIENT )
				{
					if ( myStats->shoes == NULL )
					{
						entity->sprite = 443;
					}
					else
					{
						if ( setBootSprite(myStats, entity, SPRITE_BOOT_LEFT_OFFSET) != 0 )
						{
							// successfully set sprite for the human model
						}
					}
					if ( multiplayer == SERVER )
					{
						// update sprites for clients
						if ( entity->skill[10] != entity->sprite )
						{
							entity->skill[10] = entity->sprite;
							serverUpdateEntityBodypart(my, bodypart);
						}
						if ( entity->getUID() % (TICKS_PER_SECOND * 10) == ticks % (TICKS_PER_SECOND * 10) )
						{
							serverUpdateEntityBodypart(my, bodypart);
						}
					}
				}
				entity->x -= 1 * cos(my->yaw + PI / 2) - .25 * cos(my->yaw);
				entity->y -= 1 * sin(my->yaw + PI / 2) - .25 * sin(my->yaw);
				entity->z += 5;
				if ( my->z >= 1.4 && my->z <= 1.6 )
				{
					entity->yaw -= PI / 8;
					entity->pitch = -PI / 2;
				}
				break;
				// right arm
			case 5:
			{
				if ( multiplayer != CLIENT )
				{
					if ( myStats->gloves == NULL )
					{
						entity->sprite = 439;
					}
					else
					{
						if ( setBootSprite(myStats, entity, SPRITE_GLOVE_RIGHT_OFFSET) != 0 )
						{
							// successfully set sprite for the human model
						}
					}
					if ( multiplayer == SERVER )
					{
						// update sprites for clients
						if ( entity->skill[10] != entity->sprite )
						{
							entity->skill[10] = entity->sprite;
							serverUpdateEntityBodypart(my, bodypart);
						}
						if ( entity->getUID() % (TICKS_PER_SECOND * 10) == ticks % (TICKS_PER_SECOND * 10) )
						{
							serverUpdateEntityBodypart(my, bodypart);
						}
					}
					if ( !MONSTER_ARMBENDED )
					{
						entity->sprite += 2 * (myStats->weapon != NULL);
					}
				}
				entity->x += 2.5 * cos(my->yaw + PI / 2) - .20 * cos(my->yaw);
				entity->y += 2.5 * sin(my->yaw + PI / 2) - .20 * sin(my->yaw);
				entity->z += 1.5;
				node_t* tempNode = list_Node(&my->children, 7);
				if ( tempNode )
				{
					Entity* weapon = (Entity*)tempNode->element;
					if ( multiplayer == CLIENT )
					{
						if ( !MONSTER_ARMBENDED )
						{
							if ( entity->skill[7] == 0 )
							{
								entity->skill[7] = entity->sprite;
							}
							entity->sprite = entity->skill[7];
							entity->sprite += 2 * (weapon->flags[INVISIBLE] != true);
						}
					}
					if ( weapon->flags[INVISIBLE] || MONSTER_ARMBENDED )
					{
						entity->focalx = limbs[VAMPIRE][4][0]; // 0
						entity->focaly = limbs[VAMPIRE][4][1]; // 0
						entity->focalz = limbs[VAMPIRE][4][2]; // 1.5
					}
					else
					{
						entity->focalx = limbs[VAMPIRE][4][0] + 0.75;
						entity->focaly = limbs[VAMPIRE][4][1];
						entity->focalz = limbs[VAMPIRE][4][2] - 0.75;
					}
				}
				entity->yaw += MONSTER_WEAPONYAW;
				if ( my->z >= 1.4 && my->z <= 1.6 )
				{
					entity->pitch = 0;
				}
				break;
			}
			// left arm
			case 6:
			{
				if ( multiplayer != CLIENT )
				{
					if ( myStats->gloves == NULL )
					{
						entity->sprite = 441;
					}
					else
					{
						if ( setBootSprite(myStats, entity, SPRITE_GLOVE_LEFT_OFFSET) != 0 )
						{
							// successfully set sprite for the human model
						}
					}
					entity->sprite += 2 * (myStats->shield != NULL);
					if ( multiplayer == SERVER )
					{
						// update sprites for clients
						if ( entity->skill[10] != entity->sprite )
						{
							entity->skill[10] = entity->sprite;
							serverUpdateEntityBodypart(my, bodypart);
						}
						if ( entity->getUID() % (TICKS_PER_SECOND * 10) == ticks % (TICKS_PER_SECOND * 10) )
						{
							serverUpdateEntityBodypart(my, bodypart);
						}
					}
				}
				entity->x -= 2.5 * cos(my->yaw + PI / 2) + .20 * cos(my->yaw);
				entity->y -= 2.5 * sin(my->yaw + PI / 2) + .20 * sin(my->yaw);
				entity->z += 1.5;
				node_t* tempNode = list_Node(&my->children, 8);
				if ( tempNode )
				{
					Entity* shield = (Entity*)tempNode->element;
					if ( shield->flags[INVISIBLE] )
					{
						entity->focalx = limbs[VAMPIRE][5][0]; // 0
						entity->focaly = limbs[VAMPIRE][5][1]; // 0
						entity->focalz = limbs[VAMPIRE][5][2]; // 1.5
					}
					else
					{
						entity->focalx = limbs[VAMPIRE][5][0] + 0.75;
						entity->focaly = limbs[VAMPIRE][5][1];
						entity->focalz = limbs[VAMPIRE][5][2] - 0.75;
					}
				}
				if ( my->z >= 1.4 && my->z <= 1.6 )
				{
					entity->pitch = 0;
				}
				break;
			}
			// weapon
			case 7:
				if ( multiplayer != CLIENT )
				{
					if ( myStats->weapon == NULL || myStats->EFFECTS[EFF_INVISIBLE] || wearingring )
					{
						entity->flags[INVISIBLE] = true;
					}
					else
					{
						entity->sprite = itemModel(myStats->weapon);
						if ( itemCategory(myStats->weapon) == SPELLBOOK )
						{
							entity->flags[INVISIBLE] = true;
						}
						else
						{
							entity->flags[INVISIBLE] = false;
						}
					}
					if ( multiplayer == SERVER )
					{
						// update sprites for clients
						if ( entity->skill[10] != entity->sprite )
						{
							entity->skill[10] = entity->sprite;
							serverUpdateEntityBodypart(my, bodypart);
						}
						if ( entity->skill[11] != entity->flags[INVISIBLE] )
						{
							entity->skill[11] = entity->flags[INVISIBLE];
							serverUpdateEntityBodypart(my, bodypart);
						}
						if ( entity->getUID() % (TICKS_PER_SECOND * 10) == ticks % (TICKS_PER_SECOND * 10) )
						{
							serverUpdateEntityBodypart(my, bodypart);
						}
					}
				}
				if ( weaponarm != NULL )
				{
					if ( entity->sprite == items[SHORTBOW].index )
					{
						entity->x = weaponarm->x - .5 * cos(weaponarm->yaw);
						entity->y = weaponarm->y - .5 * sin(weaponarm->yaw);
						entity->z = weaponarm->z + 1;
						entity->pitch = weaponarm->pitch + .25;
					}
					else if ( entity->sprite == items[ARTIFACT_BOW].index )
					{
						entity->x = weaponarm->x - 1.5 * cos(weaponarm->yaw);
						entity->y = weaponarm->y - 1.5 * sin(weaponarm->yaw);
						entity->z = weaponarm->z + 2;
						entity->pitch = weaponarm->pitch + .25;
					}
					else if ( entity->sprite == items[CROSSBOW].index )
					{
						entity->x = weaponarm->x;
						entity->y = weaponarm->y;
						entity->z = weaponarm->z + 1;
						entity->pitch = weaponarm->pitch;
					}
					else if ( entity->sprite == items[TOOL_LOCKPICK].index )
					{
						entity->x = weaponarm->x + 1.5 * cos(weaponarm->yaw);
						entity->y = weaponarm->y + 1.5 * sin(weaponarm->yaw);
						entity->z = weaponarm->z + 1.5;
						entity->pitch = weaponarm->pitch + .25;
					}
					else
					{
						entity->x = weaponarm->x + .5 * cos(weaponarm->yaw) * (MONSTER_ATTACK == 0);
						entity->y = weaponarm->y + .5 * sin(weaponarm->yaw) * (MONSTER_ATTACK == 0);
						entity->z = weaponarm->z - .5 * (MONSTER_ATTACK == 0);
						entity->pitch = weaponarm->pitch + .25 * (MONSTER_ATTACK == 0);
					}
					entity->yaw = weaponarm->yaw;
					entity->roll = weaponarm->roll;
					if ( !MONSTER_ARMBENDED )
					{
						entity->focalx = limbs[VAMPIRE][6][0]; // 1.5
						if ( entity->sprite == items[CROSSBOW].index )
						{
							entity->focalx += 2;
						}
						entity->focaly = limbs[VAMPIRE][6][1]; // 0
						entity->focalz = limbs[VAMPIRE][6][2]; // -.5
					}
					else
					{
						entity->focalx = limbs[VAMPIRE][6][0] + 1.5; // 3
						entity->focaly = limbs[VAMPIRE][6][1]; // 0
						entity->focalz = limbs[VAMPIRE][6][2] - 2; // -2.5
						entity->yaw -= sin(weaponarm->roll) * PI / 2;
						entity->pitch += cos(weaponarm->roll) * PI / 2;
					}
				}
				break;
				// shield
			case 8:
				if ( multiplayer != CLIENT )
				{
					if ( myStats->shield == NULL )
					{
						entity->flags[INVISIBLE] = true;
						entity->sprite = 0;
					}
					else
					{
						entity->flags[INVISIBLE] = false;
						entity->sprite = itemModel(myStats->shield);
					}
					if ( myStats->EFFECTS[EFF_INVISIBLE] || wearingring )
					{
						entity->flags[INVISIBLE] = true;
					}
					if ( multiplayer == SERVER )
					{
						// update sprites for clients
						if ( entity->skill[10] != entity->sprite )
						{
							entity->skill[10] = entity->sprite;
							serverUpdateEntityBodypart(my, bodypart);
						}
						if ( entity->skill[11] != entity->flags[INVISIBLE] )
						{
							entity->skill[11] = entity->flags[INVISIBLE];
							serverUpdateEntityBodypart(my, bodypart);
						}
						if ( entity->getUID() % (TICKS_PER_SECOND * 10) == ticks % (TICKS_PER_SECOND * 10) )
						{
							serverUpdateEntityBodypart(my, bodypart);
						}
					}
				}
				entity->x -= 2.5 * cos(my->yaw + PI / 2) + .20 * cos(my->yaw);
				entity->y -= 2.5 * sin(my->yaw + PI / 2) + .20 * sin(my->yaw);
				entity->z += 2.5;
				if ( entity->sprite == items[TOOL_TORCH].index )
				{
					entity2 = spawnFlame(entity, SPRITE_FLAME);
					entity2->x += 2 * cos(my->yaw);
					entity2->y += 2 * sin(my->yaw);
					entity2->z -= 2;
				}
				else if ( entity->sprite == items[TOOL_CRYSTALSHARD].index )
				{
					entity2 = spawnFlame(entity, SPRITE_CRYSTALFLAME);
					entity2->x += 2 * cos(my->yaw);
					entity2->y += 2 * sin(my->yaw);
					entity2->z -= 2;
				}
				else if ( entity->sprite == items[TOOL_LANTERN].index )
				{
					entity->z += 2;
					entity2 = spawnFlame(entity, SPRITE_FLAME);
					entity2->x += 2 * cos(my->yaw);
					entity2->y += 2 * sin(my->yaw);
					entity2->z += 1;
				}
				break;
				// cloak
			case 9:
				if ( multiplayer != CLIENT )
				{
					if ( myStats->cloak == NULL || myStats->EFFECTS[EFF_INVISIBLE] || wearingring )
					{
						entity->flags[INVISIBLE] = true;
					}
					else
					{
						entity->flags[INVISIBLE] = false;
						entity->sprite = itemModel(myStats->cloak);
					}
					if ( multiplayer == SERVER )
					{
						// update sprites for clients
						if ( entity->skill[10] != entity->sprite )
						{
							entity->skill[10] = entity->sprite;
							serverUpdateEntityBodypart(my, bodypart);
						}
						if ( entity->skill[11] != entity->flags[INVISIBLE] )
						{
							entity->skill[11] = entity->flags[INVISIBLE];
							serverUpdateEntityBodypart(my, bodypart);
						}
						if ( entity->getUID() % (TICKS_PER_SECOND * 10) == ticks % (TICKS_PER_SECOND * 10) )
						{
							serverUpdateEntityBodypart(my, bodypart);
						}
					}
				}
				entity->x -= cos(my->yaw);
				entity->y -= sin(my->yaw);
				entity->yaw += PI / 2;
				break;
				// helm
			case 10:
				entity->focalx = limbs[VAMPIRE][9][0]; // 0
				entity->focaly = limbs[VAMPIRE][9][1]; // 0
				entity->focalz = limbs[VAMPIRE][9][2]; // -1.75
				entity->pitch = my->pitch;
				entity->roll = 0;
				if ( multiplayer != CLIENT )
				{
					entity->sprite = itemModel(myStats->helmet);
					if ( myStats->helmet == NULL || myStats->EFFECTS[EFF_INVISIBLE] || wearingring )
					{
						entity->flags[INVISIBLE] = true;
					}
					else
					{
						entity->flags[INVISIBLE] = false;
					}
					if ( multiplayer == SERVER )
					{
						// update sprites for clients
						if ( entity->skill[10] != entity->sprite )
						{
							entity->skill[10] = entity->sprite;
							serverUpdateEntityBodypart(my, bodypart);
						}
						if ( entity->skill[11] != entity->flags[INVISIBLE] )
						{
							entity->skill[11] = entity->flags[INVISIBLE];
							serverUpdateEntityBodypart(my, bodypart);
						}
						if ( entity->getUID() % (TICKS_PER_SECOND * 10) == ticks % (TICKS_PER_SECOND * 10) )
						{
							serverUpdateEntityBodypart(my, bodypart);
						}
					}
				}
				if ( entity->sprite != items[STEEL_HELM].index )
				{
					if ( entity->sprite == items[HAT_PHRYGIAN].index )
					{
						entity->focalx = limbs[VAMPIRE][9][0] - .5;
						entity->focaly = limbs[VAMPIRE][9][1] - 3.25;
						entity->focalz = limbs[VAMPIRE][9][2] + 2.25;
						entity->roll = PI / 2;
					}
					else if ( entity->sprite >= items[HAT_HOOD].index && entity->sprite < items[HAT_HOOD].index + items[HAT_HOOD].variations )
					{
						entity->focalx = limbs[VAMPIRE][9][0] - .5;
						entity->focaly = limbs[VAMPIRE][9][1] - 2.5;
						entity->focalz = limbs[VAMPIRE][9][2] + 2.25;
						entity->roll = PI / 2;
					}
					else if ( entity->sprite == items[HAT_WIZARD].index )
					{
						entity->focalx = limbs[VAMPIRE][9][0];
						entity->focaly = limbs[VAMPIRE][9][1] - 4.75;
						entity->focalz = limbs[VAMPIRE][9][2] + 2.25;
						entity->roll = PI / 2;
					}
					else if ( entity->sprite == items[HAT_JESTER].index )
					{
						entity->focalx = limbs[VAMPIRE][9][0];
						entity->focaly = limbs[VAMPIRE][9][1] - 4.75;
						entity->focalz = limbs[VAMPIRE][9][2] + 2.25;
						entity->roll = PI / 2;
					}
				}
				break;
				// mask
			case 11:
				entity->focalx = limbs[VAMPIRE][10][0]; // 0
				entity->focaly = limbs[VAMPIRE][10][1]; // 0
				entity->focalz = limbs[VAMPIRE][10][2]; // .5
				entity->pitch = my->pitch;
				entity->roll = PI / 2;
				if ( multiplayer != CLIENT )
				{
					if ( myStats->mask == NULL || myStats->EFFECTS[EFF_INVISIBLE] || wearingring )
					{
						entity->flags[INVISIBLE] = true;
					}
					else
					{
						entity->flags[INVISIBLE] = false;
					}
					if ( myStats->mask != NULL )
					{
						if ( myStats->mask->type == TOOL_GLASSES )
						{
							entity->sprite = 165; // GlassesWorn.vox
						}
						else
						{
							entity->sprite = itemModel(myStats->mask);
						}
					}
					if ( multiplayer == SERVER )
					{
						// update sprites for clients
						if ( entity->skill[10] != entity->sprite )
						{
							entity->skill[10] = entity->sprite;
							serverUpdateEntityBodypart(my, bodypart);
						}
						if ( entity->skill[11] != entity->flags[INVISIBLE] )
						{
							entity->skill[11] = entity->flags[INVISIBLE];
							serverUpdateEntityBodypart(my, bodypart);
						}
						if ( entity->getUID() % (TICKS_PER_SECOND * 10) == ticks % (TICKS_PER_SECOND * 10) )
						{
							serverUpdateEntityBodypart(my, bodypart);
						}
					}
				}
				if ( entity->sprite != 165 )
				{
					entity->focalx = limbs[VAMPIRE][10][0] + .35; // .35
					entity->focaly = limbs[VAMPIRE][10][1] - 2; // -2
					entity->focalz = limbs[VAMPIRE][10][2]; // .5
				}
				else
				{
					entity->focalx = limbs[VAMPIRE][10][0] + .25; // .25
					entity->focaly = limbs[VAMPIRE][10][1] - 2.25; // -2.25
					entity->focalz = limbs[VAMPIRE][10][2]; // .5
				}
				break;
		}
	}
	// rotate shield a bit
	node_t* shieldNode = list_Node(&my->children, 8);
	if ( shieldNode )
	{
		Entity* shieldEntity = (Entity*)shieldNode->element;
		if ( shieldEntity->sprite != items[TOOL_TORCH].index && shieldEntity->sprite != items[TOOL_LANTERN].index && shieldEntity->sprite != items[TOOL_CRYSTALSHARD].index )
		{
			shieldEntity->yaw -= PI / 6;
		}
	}
	if ( MONSTER_ATTACK != 0 )
	{
		MONSTER_ATTACKTIME++;
	}
	else
	{
		MONSTER_ATTACKTIME = 0;
	}
}
