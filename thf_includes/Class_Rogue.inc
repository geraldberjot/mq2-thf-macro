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
		/declare abilityName string local
		/declare minEnd int local
		/declare i int local
	
		/if (outerDamageLevel > 0) {
			/for i 1 to ${combatItemsBoost.Size}
				/varset combatItem ${FindItem[=${combatItemsBoost[i]}]}
				/if (${combatItemsBoost.ID}) {
					|- Timers were already created in the sub routine "createCombatItemsArray"
					/if (${timer-${combatItemsBoost.ID}.Value} <= 0) {
						/casting ${combatItemsBoost.ID}|item
						/varset ${timer-${combatItemsBoost.ID}} ${timer-${combatItemsBoost.ID}.OriginalValue}
						/delay 1
					}
				}
			/next i
		}
		
		/if (outerDamageLevel > 1) {
			/for i 1 to ${combatItemsBurn.Size}
				/varset combatItem ${FindItem[=${combatItemsBurn[i]}]}
				/if (${combatItemsBurn.ID}) {
					|- Timers were already created in the sub routine "createCombatItemsArray"
					/if (${timer-${combatItemsBurn.ID}.Value} <= 0) {
						/casting ${combatItemsBurn.ID}|item
						/varset ${timer-${combatItemsBurn.ID}} ${timer-${combatItemsBurn.ID}.OriginalValue}
						/delay 1
					}
				}
			/next i
		}
		
		|- Use Backstab.
		/call Attempt_Backstab

		|- For 1 to the size of MyAbilities array, use each ability.
		/for i 1 to ${MyAbilities.Size}
			/varset abilityName ${MyAbilities[${i}].Arg[1,/]}
			|- Check for MinEnd
			/if (${MyAbilities[${i}].Find[/MinEnd|]}) {
				/call argueString MinEnd "${MyAbilities[${i}]}"
				/varset minEnd ${Macro.Return}
			} else {
				/varset minEnd 0
			}
	
			/if (${Me.PctEndurance} < ${minEnd}) {
				/if (${outerDebug}) {
					/echo I don't have enough endurance to use [${abilityName}].
					/chat ${channelNumber} I don't have enough endurance to use [${abilityName}].
				}
			} else {
				|- Use alternate ability
				/if (${Me.AltAbility[${abilityName}]}) {
					/if (!${Me.AltAbilityReady[${abilityName}]} || ${altTimer_${Me.AltAbility[${abilityName}].ID}}) {
						/if (${outerDebug}) {
							/echo Alternate Ability [${abilityName}] is not ready.
							/chat ${channelNumber} Alternate Ability [${abilityName}] is not ready.
						}
					} else {
						/call thf_Casting ${outerAssistTarget} "${abilityName}"
					}
				|- Use combat ability (discipline)
				} else /if (${Me.CombatAbility[${abilityName}]}) {
					/if (!${Me.CombatAbilityReady[${abilityName}]}) {
						/if (${outerDebug}) {
							/echo Discipline [${abilityName}] is not ready.
							/chat ${channelNumber} Discipline [${abilityName}] is not ready.
						}
					} else {
						/call thf_Casting ${outerAssistTarget} "${abilityName}/Instant"
					}
				|- Use rogue evade as a combat ability 
				} else /if (${abilityName.Equal[Evade]}) {
					/call AutoEvade
				}
			}
		/next i
	}

	/if (${outerDebug}) /echo <== CombatAbilities -|
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
		/call check_rogStrike
		|- If backstab is ready, use it.
		/if (${Me.AbilityReady[Backstab]}) /doability Backstab
	}

	/if (${outerDebug}) /echo <== Attempt_Backstab -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- rogueStrike
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Attempts to arm 'Assassin's Strike' type disciplines before using backstab.
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB check_rogStrike()
	/if (${outerDebug}) /echo |- rogueStrike ==>
	
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
			/if (${Me.CombatAbilityReady[${outerStrikeDiscipline}]) {
				/disc ${outerStrikeDiscipline}
			} else {
				/declare i int local
				/for i 1 to ${strikeItems.size}
					/varset strikeItem ${FindItem[=${strikeItems[i]}]}
					/if (${strikeItem.ID}) {
						|- Timers were already created in the sub routine "createItemsArray"
						/if (${timer-${strikeItem.ID}.Value} <= 0) {
							/casting ${strikeItem.ID}|item
							/varset ${timer-${strikeItem.ID}} ${timer-${strikeItem.ID}.OriginalValue}
							goto :strike
						}
					}
				/next i
			}

			:strike
			/attack on
			/squelch /stick unpause
			/delay 10
		}
	}

	/if (${outerDebug}) /echo <== rogueStrike -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- AutoHide	 	 	 	 	 	 	 	 	 	 	 	 	 -|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Automatically engages sneak and hide when not in combat.	 -|
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
|- AutoEvade	 	 	 	 	 	 	 	 	 -|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Automatically engages hide while in combat.	 -|
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
	
	/varset itemID ${FindItem[=Frozen Whispering Tunic].ID}
	/if (${itemID}) {
		/echo "Frozen Whispering Tunic"
		/declare timer-${itemID} timer outer 5m
		/call BuildArray strikeItems ${itemID} outer
	}

	/varset itemID ${FindItem[=Cloak of the Deceiver].ID}
	/if (${itemID}) {
		/echo "Cloak of the Deceiver"
		/declare timer-${itemID} timer outer 40s
		/call BuildArray strikeItems ${itemID} outer
	}
	
	/varset itemID ${FindItem[=Shroud of the Deceiver].ID}
	/if (${itemID}) {
		/echo "Shroud of the Deceiver"
		/declare timer-${itemID} timer outer 40s
		/call BuildArray strikeItems ${itemID} outer
	}

	/varset itemID ${FindItem[=Fizzlethorp's Cloak of Shadows].ID}
	/if (${itemID}) {
		/echo "Fizzlethorp's Cloak of Shadows"
		/declare timer-${itemID} timer outer 40s
		/call BuildArray strikeItems ${itemID} outer
	}

	/varset itemID ${FindItem[=Ancient Frozen Razor of Entropy].ID}
	/if (${itemID}) {
		/echo "Ancient Frozen Razor of Entropy"
		/declare timer-${itemID} timer outer 3m
		/call BuildArray combatItemsBoost ${itemID} outer
	}
	
	/varset itemID ${FindItem[=Razor of the Rogue].ID}
	/if (${itemID}) {
		/echo "Razor of the Rogue"
		/declare timer-${itemID} timer outer 2m
		/call BuildArray combatItemsBoost ${itemID} outer
	}
	
	/varset itemID ${FindItem[=Flahran, The Dragon's Bane].ID}
	/if (${itemID}) {
		/echo "Flahran, The Dragon's Bane"
		/declare timer-${itemID} timer outer 6m
		/call BuildArray combatItemsBoost ${itemID} outer
	}
	
	/varset itemID ${FindItem[=TriXzin, Guard of the King].ID}
	/if (${itemID}) {
		/echo "TriXzin, Guard of the King"
		/declare timer-${itemID} timer outer 3,5m
		/call BuildArray combatItemsBurn ${itemID} outer
	}

	/if (${outerDebug}) /echo <== createItemsArray -|
/RETURN

SUB ROG_BackgroundEvents

	 /if (${AutoHide} && !${Me.Invis} && !${Me.Moving} && !${Me.Combat}) /call AutoHide

/RETURN



SUB ROG_MacroSettings
/if (${outerDebug}) /echo |- ROG_MacroSettings ==>

	 |- iniTHF settings
	 |/call writeToIni "${iniTHF},Class Specific Info,Rogue Sneak Attack Disciplines" "Sneak Attack,Thief's Vengeance,Assassin's Strike,Kyv Strike,Ancient: Chaos Strike,Daggerfall,Razorarc,Swiftblade,Swiftblade Rk. II,Swiftblade Rk. III,Daggerlunge,Daggerlunge Rk. II,Daggerlunge Rk. III,Daggerswipe,Daggerswipe Rk. II,Daggerswipe Rk. III,Daggerstrike,Daggerstrike Rk. II,Daggerstrike Rk. III,Daggerthrust,Daggerthrust Rk. II,Daggerthrust Rk. III" 0
   /call writeToIni "${iniTHF},Class Specific Info,Rogue Sneak Attack Disciplines" "Assassin's Chaos Rk. I,Assassin's Chaos Rk. II,Assassin's Chaos Rk. III,Deep Shadows Rk. I,Deep Shadows Rk. II,Deep Shadows Rk. III,Feint of Shadow Rk. I,Feint of Shadow Rk. II,Feint of Shadow Rk. III,Death Bringer Rk. I,Death Bringer Rk. II,Death Bringer Rk. III" 0

	 |- Adv Settings
	 /call writeToIni "${iniTHF},Rogue,Functions" "check_Burns/check_CombatBuffs/check_Buffs" 0

	/if (${outerDebug}) /echo <== ROG_MacroSettings -|
/RETURN



SUB ROG_CharacterSettings
/if (${outerDebug}) /echo |- ROG_CharacterSettings ==>

	 /call writeToIni "${iniToon},${Me.Class},Auto-Hide (On/Off)" "On" false
	 /call writeToIni "${iniToon},${Me.Class},Auto-Evade (On/Off)" "On" false

/if (${outerDebug}) /echo <== ROG_CharacterSettings -|
/RETURN



SUB ROG_Aliases
/if (${outerDebug}) /echo |- ROG_Aliases ==>

/if (${outerDebug}) /echo <== ROG_Aliases -|
/RETURN