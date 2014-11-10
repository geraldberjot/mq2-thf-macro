|----------------------------------------------------------------|
|- thf_Alerts.inc												-|
|- Written by Conradd of THF									-|
|----------------------------------------------------------------|
|- Contains various alerts.										-|
|----------------------------------------------------------------|

#EVENT tactics_stampede "#*#You hear the pounding of hooves#*#"
SUB EVENT_tactics_stampede
	/if (${Zone.ShortName.Equal[potactics]}) {
		/gsay STAMPEDE!
		/beep
	}
/RETURN

#EVENT charFlag "You receive a character flag#*#"
SUB EVENT_charFlag
	/bc I have received a character flag!
/RETURN

#EVENT Define_UquaKey "#*#The #1# must unlock the door to the next room.#*#"
SUB EVENT_Define_UquaKey(string line, string KeyToUse)
	/rs >>^<< The ${KeyToUse} unlocks the door >>^<<
	/popup >>^<< The ${KeyToUse} unlocks the door >>^<<
/RETURN

#EVENT setClass_Warrior "#*#Brute force and brawn#*#"
SUB EVENT_setClass_Warrior
	/popup Spawn must be killed by a >>^<< WARRIOR >>^<<
	/rs Spawn must be killed by a >>^<< WARRIOR >>^<<
/RETURN

#EVENT setClass_Shaman "#*#Cringes at the appearance of talismans#*#"
SUB EVENT_setClass_Shaman
	/popup Spawn must be killed by a >>^<< SHAMAN >>^<< 
	/rs Spawn must be killed by a >>^<< SHAMAN >>^<< 
/RETURN

#EVENT setClass_Beastlord "#*#Deep gashes of feral savagery#*#"
SUB EVENT_setClass_Beastlord
	/popup Spawn must be killed by a >>^<< BEASTLORD >>^<< 
	/rs Spawn must be killed by a >>^<< BEASTLORD >>^<< 
/RETURN

#EVENT setClass_Necromancer "#*#Doom of death#*#"
SUB EVENT_setClass_Necromancer
	/popup Spawn must be killed by a >>^<< NECROMANCER >>^<< 
	/rs Spawn must be killed by a >>^<< NECROMANCER >>^<< 
/RETURN

#EVENT setClass_Cleric "#*#Dread of celestial spirit#*#"
SUB EVENT_setClass_Cleric
	/popup Spawn must be killed by a >>^<< CLERIC >>^<< 
	/rs Spawn must be killed by a >>^<< CLERIC >>^<< 
/RETURN

#EVENT setClass_Shadowknight "#*#Dreads the strike of dead#*#"
SUB EVENT_setClass_Shadowknight
	/popup Spawn must be killed by a >>^<< SHADOWKNIGHT >>^<< 
	/rs Spawn must be killed by a >>^<< SHADOWKNIGHT >>^<< 
/RETURN

#EVENT setClass_Monk "#*#Focused tranquility#*#"
SUB EVENT_setClass_Monk
	/popup Spawn must be killed by a >>^<< MONK >>^<< 
	/rs Spawn must be killed by a >>^<< MONK >>^<< 
/RETURN

#EVENT setClass_Bard "#*#Foreboding melody#*#"
SUB EVENT_setClass_Bard
	/popup Spawn must be killed by a >>^<< BARD >>^<< 
	/rs Spawn must be killed by a >>^<< BARD >>^<< 
/RETURN

#EVENT setClass_Paladin "#*#Holy blade#*#"
SUB EVENT_setClass_Paladin
	/popup Spawn must be killed by a >>^<< PALADIN >>^<< 
	/rs Spawn must be killed by a >>^<< PALADIN >>^<< 
/RETURN

#EVENT setClass_Rogue "#*#Ignores anything behind it#*#"
SUB EVENT_setClass_Rogue
	/popup Spawn must be killed by a >>^<< ROGUE >>^<< 
	/rs Spawn must be killed by a >>^<< ROGUE >>^<< 
/RETURN

#EVENT setClass_Enchanter "#*#Mind and body vulnerable#*#"
SUB EVENT_setClass_Enchanter
	/popup Spawn must be killed by a >>^<< ENCHANTER >>^<< 
	/rs Spawn must be killed by a >>^<< ENCHANTER >>^<< 
/RETURN

#EVENT setClass_Wizard "#*#Falters when struck with the power of the elements#*#"
SUB EVENT_setClass_Wizard
	/popup Spawn must be killed by a >>^<< WIZARD >>^<< 
	/rs Spawn must be killed by a >>^<< WIZARD >>^<< 
/RETURN

#EVENT setClass_Berserker "#*#Shies from heavy blades#*#"
SUB EVENT_setClass_Berserker
	/popup Spawn must be killed by a >>^<< BERSERKER >>^<< 
	/rs Spawn must be killed by a >>^<< BERSERKER >>^<< 
/RETURN

#EVENT setClass_Magician "#*#Summoned elements#*#"
SUB EVENT_setClass_Magician
	/popup Spawn must be killed by a >>^<< MAGICIAN >>^<< 
	/rs Spawn must be killed by a >>^<< MAGICIAN >>^<< 
/RETURN

#EVENT setClass_Druid "#*#The creature seems weak in the face of the power of nature#*#"
SUB EVENT_setClass_Druid
	/popup Spawn must be killed by a >>^<< DRUID >>^<< 
	/rs Spawn must be killed by a >>^<< DRUID >>^<< 
/RETURN

#EVENT setClass_Ranger "#*#True shots and fast blades#*#"
SUB EVENT_setClass_Ranger
	/popup Spawn must be killed by a >>^<< RANGER >>^<< 
	/rs Spawn must be killed by a >>^<< RANGER >>^<< 
/RETURN

#EVENT startClass_Priests "#*#The creature cannot stand up to the power of healers#*#"
SUB EVENT_startClass_Priests
	/if (!${Defined[murauderKill]}) {
		/declare murauderKill int outer 4
	} else /if (${Defined[murauderKill]}) {
		/varset murauderKill 4
	}
	/rs Spawn must be killed by a >>^<< PRIEST >>^<< 
/RETURN

#EVENT startClass_Hybrids "#*#The creature appears weak to the combined effort of might and magic#*#"
SUB EVENT_startClass_Hybrids
	/if (!${Defined[murauderKill]}) {
		/declare murauderKill int outer 2
	} else /if (${Defined[murauderKill]}) {
		/varset murauderKill 2
	}
	/rs Spawn must be killed by a >>^<< HYBRID >>^<< 
/RETURN

#EVENT startClass_Casters "#*#The creature will perish under the strength of intelligent magic#*#"
SUB EVENT_startClass_Casters
	/if (!${Defined[murauderKill]}) {
		/declare murauderKill int outer 1
	} else /if (${Defined[murauderKill]}) {
		/varset murauderKill 1
	}
	/rs Spawn must be killed by a >>^<< CASTER >>^<< 
/RETURN

#EVENT startClass_Melees "#*#The creature appears weak to the combined effort of strength and cunning#*#"
SUB EVENT_startClass_Melees
	/if (!${Defined[murauderKill]}) {
		/declare murauderKill int outer 3
	} else /if (${Defined[murauderKill]}) {
		/varset murauderKill 3
	}
	/rs Spawn must be killed by a >>^<< MELEE >>^<< 
/RETURN

#EVENT MobSet "#*#shudders as if somehow changing.#*#"
SUB EVENT_MobSet
	/if (${murauderKill} < 4) {
		/if (${Defined[murauderKill]}) /varcalc murauderKill ${murauderKill} + 1
	} else {
		/if (${Defined[murauderKill]}) /varset murauderKill 1
	}
	/if (${murauderKill} == 1) {
		/rs Vrex Xalkaks Marauder must be killed by a  >>^<< CASTER >>^<< 
	} else /if (${murauderKill} == 2) {
		/rs Vrex Xalkaks Marauder must be killed by a >>^<< HYBRID >>^<< 
	} else /if (${murauderKill} == 3) {
		/rs Vrex Xalkaks Marauder must be killed by a >>^<< PRIEST >>^<< 
	} else /if (${murauderKill} == 4) {
		/rs Vrex Xalkaks Marauder must be killed by a >>^<< MELEE >>^<< 
	}
/RETURN