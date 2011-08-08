/*
 * Copyright 2010 OpenXcom Developers.
 *
 * This file is part of OpenXcom.
 *
 * OpenXcom is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenXcom is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenXcom.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef OPENXCOM_BATTLEITEM_H
#define OPENXCOM_BATTLEITEM_H

#include "../Battlescape/Position.h"
#include "yaml.h"
#include <string>

namespace OpenXcom
{

class Item;
class RuleItem;
class BattleUnit;

/**
 * Represents a single item in the battlescape.
 * Contains battle-related info about an item like the position, ammo quantity, ...
 * @sa RuleItem
 * @sa Item
 */
class BattleItem
{
private:
	RuleItem *_rules;
	Position _position;
	BattleUnit *_owner, *_previousOwner;
	std::string _inventorySlot;
	int _inventoryX, _inventoryY;
	BattleItem *_ammoItem;
	int _itemProperty[3];
public:
	/// Creates a item of the specified type.
	BattleItem(RuleItem *rules);
	/// Cleans up the item.
	~BattleItem();
	/// Loads the item from YAML.
	void load(const YAML::Node& node);
	/// Saves the item to YAML.
	void save(YAML::Emitter& out) const;
	/// Gets the item's ruleset.
	RuleItem *const getRules() const;
	/// Gets the item's ammo quantity
	int getAmmoQuantity() const;
	/// Sets the item's ammo quantity.
	void setAmmoQuantity(int qty);
	/// Gets the turn to explode on
	int getExplodeTurn() const;
	/// Sets the turn to explode on.
	void setExplodeTurn(int turn);
	/// Spend one bullet.
	bool spendBullet();
	/// Gets the item's owner.
	BattleUnit *getOwner() const;
	/// Gets the item's previous owner.
	BattleUnit *getPreviousOwner() const;
	/// Sets the item's owner.
	void setOwner(BattleUnit *owner);
	/// Gets the item's inventory slot.
	std::string getSlot() const;
	/// Sets the item's inventory slot.
	void setSlot(std::string slot);
	/// Gets the item's inventory X position.
	int getSlotX() const;
	/// Sets the item's inventory X position.
	void setSlotX(int x);
	/// Gets the item's inventory Y position.
	int getSlotY() const;
	/// Sets the item's inventory Y position.
	void setSlotY(int y);
	/// Checks if the item is occupying a slot.
	bool occupiesSlot(int x, int y) const;
	/// Gets the item's ammo item.
	BattleItem *getAmmoItem();
	/// Sets the item's ammo item.
	int setAmmoItem(BattleItem *item);
};

}

#endif
