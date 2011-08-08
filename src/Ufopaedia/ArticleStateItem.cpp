/*
 * Copyright 2011 OpenXcom Developers.
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

#include <sstream>

#include "Ufopaedia.h"
#include "ArticleStateItem.h"
#include "../Ruleset/Ruleset.h"
#include "../Ruleset/ArticleDefinition.h"
#include "../Ruleset/RuleItem.h"
#include "../Ruleset/RuleInventory.h"
#include "../Engine/Game.h"
#include "../Engine/Palette.h"
#include "../Engine/Surface.h"
#include "../Engine/SurfaceSet.h"
#include "../Engine/Language.h"
#include "../Resource/ResourcePack.h"
#include "../Interface/Text.h"
#include "../Interface/TextButton.h"
#include "../Interface/TextList.h"

namespace OpenXcom
{
	
	ArticleStateItem::ArticleStateItem(Game *game, ArticleDefinitionItem *defs) : ArticleState(game, defs->id)
	{
		// add screen elements
		_txtTitle = new Text(140, 32, 5, 24);
		
		// Set palette
		_game->setPalette(_game->getResourcePack()->getPalette("PALETTES.DAT_4")->getColors());
		
		ArticleState::initLayout();
		
		// add other elements
		add(_txtTitle);
		
		// Set up objects
		_game->getResourcePack()->getSurface("BACK08.SCR")->blit(_bg);
		_btnOk->setColor(Palette::blockOffset(9)+2);
		_btnPrev->setColor(Palette::blockOffset(9)+2);
		_btnNext->setColor(Palette::blockOffset(9)+2);
		
		_txtTitle->setColor(Palette::blockOffset(14)+15);
		_txtTitle->setBig();
		_txtTitle->setAlign(ALIGN_LEFT);
		_txtTitle->setWordWrap(true);
		_txtTitle->setText(Ufopaedia::buildText(_game, defs->title));
		
		// IMAGE
		_image = new Surface(32, 48, 157, 5);
		add(_image);
		
		SurfaceSet *texture = _game->getResourcePack()->getSurfaceSet("BIGOBS.PCK");
		Surface *graphic = texture->getFrame(defs->item->getBigSprite());
		graphic->setX((RuleInventory::HAND_W - defs->item->getInventoryWidth()) * RuleInventory::SLOT_W/2);
		graphic->setY((RuleInventory::HAND_H - defs->item->getInventoryHeight()) * RuleInventory::SLOT_H/2);
		graphic->blit(_image);
		
		std::vector<std::string> *ammo_data = defs->item->getCompatibleAmmo();

		// SHOT STATS TABLE (for firearms only)
		if (defs->item->getBattleType() == BT_FIREARM)
		{
			_txtShotType = new Text(80, 10, 8, 67);
			add(_txtShotType);
			_txtShotType->setColor(Palette::blockOffset(14)+15);
			_txtShotType->setAlign(ALIGN_LEFT);
			_txtShotType->setText(_game->getLanguage()->getString("STR_SHOT_TYPE").c_str());
			
			_txtAccuracy = new Text(80, 10, 88, 67);
			add(_txtAccuracy);
			_txtAccuracy->setColor(Palette::blockOffset(14)+15);
			_txtAccuracy->setAlign(ALIGN_LEFT);
			_txtAccuracy->setText(_game->getLanguage()->getString("STR_ACCURACY_UC").c_str());
			
			_txtTuCost = new Text(80, 10, 140, 67);
			add(_txtTuCost);
			_txtTuCost->setColor(Palette::blockOffset(14)+15);
			_txtTuCost->setAlign(ALIGN_LEFT);
			_txtTuCost->setText(_game->getLanguage()->getString("STR_TU_COST").c_str());
			
			_lstInfo = new TextList(185, 55, 8, 82);
			add(_lstInfo);
			
			_lstInfo->setColor(Palette::blockOffset(15)+4); // color for %-data!
			_lstInfo->setColumns(3, 80, 52, 52);
			_lstInfo->setBig();
			
			int current_row = 0;
			if (defs->item->getAccuracyAuto()>0)
			{
				_lstInfo->addRow(3, 
								 _game->getLanguage()->getString("STR_AUTO").c_str(), 
								 Text::formatPercentage(defs->item->getAccuracyAuto()).c_str(),
								 Text::formatPercentage(defs->item->getTUAuto()).c_str());
				_lstInfo->getCell(current_row, 0)->setColor(Palette::blockOffset(14)+15);
				current_row += 1;
			}
								 
			if (defs->item->getAccuracySnap()>0)
			{
				_lstInfo->addRow(3, 
								 _game->getLanguage()->getString("STR_SNAP").c_str(), 
								 Text::formatPercentage(defs->item->getAccuracySnap()).c_str(),
								 Text::formatPercentage(defs->item->getTUSnap()).c_str());
				_lstInfo->getCell(current_row, 0)->setColor(Palette::blockOffset(14)+15);
				current_row += 1;
			}
			
			if (defs->item->getAccuracyAimed()>0)
			{
				_lstInfo->addRow(3, 
								 _game->getLanguage()->getString("STR_AIMED").c_str(), 
								 Text::formatPercentage(defs->item->getAccuracyAimed()).c_str(),
								 Text::formatPercentage(defs->item->getTUAimed()).c_str());
				_lstInfo->getCell(current_row, 0)->setColor(Palette::blockOffset(14)+15);
				current_row += 1;
			}
			
			_lstInfo->draw();

			// text_info is BELOW the info table
			_txtInfo = new Text((ammo_data->size()<3 ? 300 : 180), 60, 8, 138);
		}
		else
		{
			// text_info is larger and starts on top
			_txtInfo = new Text(300, 125, 8, 67);
		}

		add(_txtInfo);
		
		_txtInfo->setColor(Palette::blockOffset(14)+15);
		_txtInfo->setAlign(ALIGN_LEFT);
		_txtInfo->setWordWrap(true);
		_txtInfo->setText(Ufopaedia::buildText(_game, defs->text));
		
		
		// AMMO column
		std::wstringstream ss;
		
		for (int i = 0; i<3; ++i)
		{
			_txtAmmoType[i] = new Text(80, 10, 200, 24 + i*49);
			add(_txtAmmoType[i]);
			_txtAmmoType[i]->setColor(Palette::blockOffset(14)+15);
			_txtAmmoType[i]->setAlign(ALIGN_CENTER);
			
			_txtAmmoDamage[i] = new Text(80, 14, 200, 40 + i*49);
			add(_txtAmmoDamage[i]);
			_txtAmmoDamage[i]->setColor(Palette::blockOffset(2));
			_txtAmmoDamage[i]->setAlign(ALIGN_CENTER);
			_txtAmmoDamage[i]->setBig();
			
			_imageAmmo[i] = new Surface(32, 48, 280, 24 + i*49);
			add(_imageAmmo[i]);
		}

		switch (defs->item->getBattleType()) 
		{
			case BT_FIREARM:
				_txtDamage = new Text(80, 10, 200, 7);
				add(_txtDamage);
				_txtDamage->setColor(Palette::blockOffset(14)+15);
				_txtDamage->setAlign(ALIGN_CENTER);
				_txtDamage->setText(_game->getLanguage()->getString("STR_DAMAGE_UC").c_str());
				
				_txtAmmo = new Text(35, 10, 280, 7);
				add(_txtAmmo);
				_txtAmmo->setColor(Palette::blockOffset(14)+15);
				_txtAmmo->setAlign(ALIGN_CENTER);
				_txtAmmo->setText(_game->getLanguage()->getString("STR_AMMO").c_str());
				
				for (unsigned int i = 0; i < ammo_data->size(); ++i)
				{
					if (Ufopaedia::isArticleAvailable(_game, (*ammo_data)[i]))
					{
						RuleItem *ammo_rule = _game->getRuleset()->getItem((*ammo_data)[i]);
						setDamageTypeText(_txtAmmoType[i], ammo_rule);
						
						ss.str(L"");ss.clear();
						ss << ammo_rule->getPower();
						_txtAmmoDamage[i]->setText(ss.str().c_str());

						graphic = texture->getFrame(ammo_rule->getBigSprite());
						graphic->setX((RuleInventory::HAND_W - ammo_rule->getInventoryWidth()) * RuleInventory::SLOT_W/2);
						graphic->setY((RuleInventory::HAND_H - ammo_rule->getInventoryHeight()) * RuleInventory::SLOT_H/2);
						graphic->blit(_imageAmmo[i]);
					}
				}
				break;
			case BT_AMMO:
			case BT_GRENADE:
				setDamageTypeText(_txtAmmoType[0], defs->item);
				
				ss.str(L"");ss.clear();
				ss << defs->item->getPower();
				_txtAmmoDamage[0]->setText(ss.str().c_str());
				break;
		}
	}
	
	void ArticleStateItem::setDamageTypeText(Text *text_field, RuleItem *ammo_rule)
	{
		switch(ammo_rule->getDamageType())
		{
			case DT_AP:
				text_field->setText(_game->getLanguage()->getString("STR_ARMOR_PIERCING").c_str());
				break;
			case DT_HE:
				text_field->setText(_game->getLanguage()->getString("STR_HIGH_EXPLOSIVE_UC").c_str());
				break;
			case DT_IN:
				text_field->setText(_game->getLanguage()->getString("STR_INCENDIARY").c_str());
				break;
			case DT_LASER:
				text_field->setText(_game->getLanguage()->getString("STR_LASER_BEAM").c_str());
				break;
			case DT_PLASMA:
				text_field->setText(_game->getLanguage()->getString("STR_PLASMA_BEAM_UC").c_str());
				break;
			case DT_STUN:
				text_field->setText(_game->getLanguage()->getString("STR_STUN_UC").c_str());
				break;
			default:
				text_field->setText(_game->getLanguage()->getString("STR_UNKNOWN").c_str());
				break;
		}
	}
	
	ArticleStateItem::~ArticleStateItem()
	{}
	
}
