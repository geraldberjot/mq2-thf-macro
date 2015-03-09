|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Class_Rogue.inc v1.0
|- Adapted for THF by Conradd (macro from Killians of PEQ)
|---------------------------------------------------------------------------------------------------------------------------------------------------|

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Uses ROGUE combat abilities, AAs, and disciplines.
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB ROG_CombatAbilities
	/if (${outerDebug}) /echo |- ROG_CombatAbilities ==>

	|- Check Target distance.
	/if (${Target.Distance} > ${Spawn[${Target.ID}].MaxRangeTo}) {
		/if (${outerDebug}) {
			/echo I am too far away from [${Spawn[id ${outerAssistTarget}].CleanName}] to use abilities.
			/chat ${channelNumber} I am too far away from [${Spawn[id ${outerAssistTarget}].CleanName}] to use abilities.
		}
	} else {
		/declare combatItem item local
		/declare i int local
	
		/if (outerDamageLevel > 0) {
			/for i 1 to ${combatItemsBoost.Size}
				/varset combatItem ${FindItem[=${combatItemsBoost[${i}]}]}
				/if (${combatItemsBoost.ID}) {
					/call thf_Casting ${Me.ID} ${combatItemsBoost.ID}
					/delay 1
				}
			/next i
		}
		
		/if (outerDamageLevel > 1) {
			/for i 1 to ${combatItemsBurn.Size}
				/varset combatItem ${FindItem[=${combatItemsBurn[${i}]}]}
				/if (${combatItemsBurn.ID}) {
					/call thf_Casting ${Me.ID} ${combatItemsBurn.ID}
					/delay 1
				}
			/next i
		}
		
		|- Use Backstab.
		/call Attempt_Backstab

		|- Use evade.
		/call AutoEvade
	}

	/if (${outerDebug}) /echo <== ROG_CombatAbilities -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Attempt_Backstab
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Attempts to arm 'Assassin's Strike' type disciplines before using backstab.
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB Attempt_Backstab
	/if (${outerDebug}) /echo |- Attempt_Backstab ==>

	|- Check that I have a piercer in my main hand
	/if (${Me.Inventory[Mainhand].Type.NotEqual[Piercing]}) {
		/if (${outerDebug}) {
			/echo I need a piercing weapon in my main hand to backstab...
			/chat ${channelNumber} I need a piercing weapon in my main hand to backstab...
		}
	} else {
		/call check_Strike
		|- If backstab is ready, use it.
		/if (${Me.AbilityReady[Backstab]}) /doability Backstab
	}

	/if (${outerDebug}) /echo <== Attempt_Backstab -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- check_Strike
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Attempts to arm 'Assassin's Strike' type disciplines before using backstab.
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB check_Strike()
	/if (${outerDebug}) /echo |- check_Strike ==>
	
	/if (${Me.AbilityReady[Hide]}) {
		|- Stop moving.
		/if (${Stick.Active}) {
			/squelch /stick pause
			/delay 2s !${Me.Moving}
		}
	 	 
		|- Turn off combat.
		/if (${Me.Combat}) {
			/attack off
			/delay 2s !${Me.Combat}
		}
	 	 
		|- Sneak
		/doability Sneak
		/delay 20 ${Me.Sneaking}
	 	 
		|- Hide.
		/doability Hide
		/delay 20 ${Me.Invis}
	 	 
		|- Use 'Assassin's Strike' type disc.
		/if (${Me.Invis}) {
			/call thf_Casting ${Me.ID} ${outerStrikeDiscipline}
			/if (!${castReturn.Equal[SUCCESS]}) {
				/declare i int local
				/for i 1 to ${strikeItems.size}
					/varset strikeItem ${FindItem[=${strikeItems[${i}]}]}
					/if (${strikeItem.ID}) {
						/call thf_Casting ${Me.ID} ${strikeItem.ID}
						goto :strike
					}
				/next i
			}

			:strike
			/attack on
			/squelch /stick unpause
			/delay 10
		}
	}

	/if (${outerDebug}) /echo <== check_Strike -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- AutoHide
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Automatically engages sneak and hide when not in combat.
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB AutoHide
	 /if (${outerDebug}) /echo |- AutoHide ==>

	 |- Engage sneak.
	 /if (!${Me.Sneaking} && ${Me.AbilityReady[Sneak]}) {
	 	 /doability Sneak
	 	 /delay 2s ${Me.Sneaking}
	 }
	 
	 |- Engage hide.
	 /if (${Me.Sneaking} && ${Me.AbilityReady[Hide]}) {
	 	 /doability Hide
	 	 /delay 2s ${Me.Invis}
	 }

	 /if (${outerDebug}) /echo <== AutoHide -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- AutoEvade
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Automatically engages hide while in combat.
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB AutoEvade
	 /if (${outerDebug}) /echo |- AutoEvade ==>

	 /if (${Me.AbilityReady[Hide]}) {
	 	 
	 	 |- Turn off combat.
	 	 /if (${Me.Combat}) {
	 	 	 /attack off
	 	 	 /delay 2s !${Me.Combat}
	 	 }
	 	 
	 	 |- Hide.
	 	 /doability Hide
	 	 /delay 20 ${Me.Invis}
	 	 
	 	 |- Turn combat back on.
	 	 /if (!${Me.Combat}) {
	 	 	 /attack on
	 	 	 /delay 2s ${Me.Combat}
	 	 }
	 }

	 /if (${outerDebug}) /echo <== AutoEvade -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|-
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB ROG_Setup
	/if (${outerDebug}) /echo |- ROG_Setup ==>

	|- Auto-Hide
	/call iniToVar "${iniToon},Rogue,Auto-Hide (On/Off)" AutoHide bool outer

	|- Auto-Evade
	/call iniToVar "${iniToon},Rogue,Auto-Evade (On/Off)" AutoEvade bool outer
	/if (${AutoEvade}) /call BuildArray MyAbilities Evade

	|- Pick up the lastest discipline
	/call getLatestouterStrikeDiscipline
	
	/call createItemsArray
	
	/if (${outerDebug}) /echo <== ROG_Setup -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|-
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB ROG_BackgroundEvents

	 /if (${AutoHide} && !${Me.Invis} && !${Me.Moving} && !${Me.Combat}) /call AutoHide

/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|-
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB ROG_MacroSettings
/if (${outerDebug}) /echo |- ROG_MacroSettings ==>

	 |- Adv Settings
	 /call writeToIni "${iniTHF},Rogue,Functions" "check_Burns/check_CombatBuffs/check_Buffs" 0

	/if (${outerDebug}) /echo <== ROG_MacroSettings -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|-
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB ROG_CharacterSettings
/if (${outerDebug}) /echo |- ROG_CharacterSettings ==>

	 /call writeToIni "${iniToon},${Me.Class},Auto-Hide (On/Off)" "On" false
	 /call writeToIni "${iniToon},${Me.Class},Auto-Evade (On/Off)" "On" false

/if (${outerDebug}) /echo <== ROG_CharacterSettings -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|-
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB ROG_Aliases
/if (${outerDebug}) /echo |- ROG_Aliases ==>

/if (${outerDebug}) /echo <== ROG_Aliases -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Check every strike discipline to get the highest one
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB getLatestouterStrikeDiscipline
	/if (${outerDebug}) /echo |- getLatestouterStrikeDiscipline ==>
	
	/if (!${Defined[outerStrikeDiscipline]}) /declare outerStrikeDiscipline string outer

	/if (${Me.CombatAbility[Death Bringer Rk. III]}) {
		/varset outerStrikeDiscipline "Death Bringer Rk. III"
	} else /if (${Me.CombatAbility[Death Bringer Rk. II]}) {
		/varset outerStrikeDiscipline "Death Bringer Rk. II"
	} else /if (${Me.CombatAbility[Death Bringer Rk. I]}) {
		/varset outerStrikeDiscipline "Death Bringer Rk. I"
	} else /if (${Me.CombatAbility[Daggerfall]}) {
		/varset outerStrikeDiscipline "Daggerfall"
	} else /if (${Me.CombatAbility[Ancient: Chaos Strike]}) {
		/varset outerStrikeDiscipline "Ancient: Chaos Strike"
	} else /if (${Me.CombatAbility[Kyv Strike]}) {
		/varset outerStrikeDiscipline "Kyv Strike"
	} else /if (${Me.CombatAbility[Assassin's Strike]}) {
		/varset outerStrikeDiscipline "Assassin's Strike"
	} else /if (${Me.CombatAbility[Thief's Vengeance]}) {
		/varset outerStrikeDiscipline "Thief's Vengeance"
	} else /if (${Me.CombatAbility[Sneak Attack]}) {
		/varset outerStrikeDiscipline "Sneak Attack"
	}
	/if (${outerDebug}) {
		/echo ${outerStrikeDiscipline}
		/echo <== getLatestouterStrikeDiscipline -|
	}
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Create the timers of the strike items available
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB createItemsArray
	/if (${outerDebug}) /echo |- createItemsArray ==>

	/if (!${Defined[itemID]}) /declare itemID int local
	
	/call createItemTimerAndAddToArray "Frozen Whispering Tunic" "strikeItems"
	/call createItemTimerAndAddToArray "Cloak of the Deceiver" "strikeItems"
	/call createItemTimerAndAddToArray "Shroud of the Deceiver" "strikeItems"
	/call createItemTimerAndAddToArray "Fizzlethorp's Cloak of Shadows" "strikeItems"

	/call createItemTimerAndAddToArray "Ancient Frozen Razor of Entropy" "combatItemsBoost"
	/call createItemTimerAndAddToArray "Razor of the Rogue" "combatItemsBoost"
	/call createItemTimerAndAddToArray "Flahran, The Dragon's Bane" "combatItemsBoost"
	/call createItemTimerAndAddToArray "TriXzin, Guard of the King" "combatItemsBurn"
	
	/if (${outerDebug}) /echo <== createItemsArray -|
/RETURN
