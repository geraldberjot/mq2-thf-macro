|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Class_Berserker.inc v1.0
|- Adapted for THF by Conradd (macro from Killians of PEQ)
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Contains BER specific functions related to,
|-		Setup and Ini entry information
|-		MainLoop, and AssistLoop hooks
|-		Over-all functionality / flow
|---------------------------------------------------------------------------------------------------------------------------------------------------|

SUB BER_CombatAbilities
	/if (${outerDebug}) /echo |- BER_CombatAbilities ==>
	
	|- Check Target distance.
	/if (${Target.Distance} > ${Spawn[${Target.ID}].MaxRangeTo}) {
		/if (${outerDebug}) {
			/echo I am too far away from [${Spawn[id ${outerAssistTarget}].CleanName}] to use abilities.
			/channel I am too far away from [${Spawn[id ${outerAssistTarget}].CleanName}] to use abilities.
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
					/channel I don't have enough endurance to use [${abilityName}].
				}
			} else {
				|- Use alternate ability
				/if (${Me.AltAbility[${abilityName}]}) {
					/if (!${Me.AltAbilityReady[${abilityName}]} || ${altTimer_${Me.AltAbility[${abilityName}].ID}}) {
						/if (${outerDebug}) {
							/echo Alternate Ability [${abilityName}] is not ready.
							/channel Alternate Ability [${abilityName}] is not ready.
						}
					} else {
						/call thf_Casting ${outerAssistTarget} "${abilityName}"
					}
				|- Use combat ability (discipline)
				} else /if (${Me.CombatAbility[${abilityName}]}) {
					/if (!${Me.CombatAbilityReady[${abilityName}]}) {
						/if (${outerDebug}) {
							/echo Discipline [${abilityName}] is not ready.
							/channel Discipline [${abilityName}] is not ready.
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
	
	/if (${outerDebug}) /echo <== BER_CombatAbilities -|
/RETURN

SUB BER_Setup
	/if (${outerDebug}) /echo |- BER_Setup ==>
	
	/call getLatestVolleyDiscipline
	
	/if (${outerDebug}) /echo <== BER_Setup -|
/RETURN

SUB BER_CheckBackgroundEvents

/RETURN

SUB BER_MacroSettings
	/if (${outerDebug}) /echo |- BER_MacroSettings ==>

	/call writeToIni "${iniTHF},Berserker,Functions" "check_Burns/check_CombatBuffs/check_Buffs" FALSE

	/if (${outerDebug}) /echo <== BER_MacroSettings -|
/RETURN

SUB BER_CharacterSettings
	/if (${outerDebug}) /echo |- BER_CharacterSettings ==>

	/if (${outerDebug}) /echo <== BER_CharacterSettings -|
/RETURN

SUB BER_Aliases
	/if (${outerDebug}) /echo |- BER_Aliases ==>

	/if (${outerDebug}) /echo <== BER_Aliases -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Check every strike discipline to get the highest one
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB getLatestVolleyDiscipline()
	/if (${outerDebug}) /echo |- getLatestVolleyDiscipline ==>
	
	/if (!${Defined[outerVolleyDiscipline]}) /declare outerVolleyDiscipline string outer

	/if (${Me.CombatAbility[Enraged Volley Rk. III]}) {
		/varset outerVolleyDiscipline "Enraged Volley Rk. III"
	} else /if (${Me.CombatAbility[Enraged Volley Rk. II]}) {
		/varset outerVolleyDiscipline "Enraged Volley Rk. II"
	} else /if (${Me.CombatAbility[Enraged Volley Rk. I]}) {
		/varset outerVolleyDiscipline "Enraged Volley Rk. I"
	} else /if (${Me.CombatAbility[Destroyer's Volley]}) {
		/varset outerVolleyDiscipline "Destroyer's Volley"
	} else /if (${Me.CombatAbility[Rage Volley]}) {
		/varset outerVolleyDiscipline "Rage Volley"
	}
	
	/if (${outerDebug}) /echo <== getLatestVolleyDiscipline -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Create the timers of the items used in combat
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB createCombatItemsArray()
	/if (${outerDebug}) /echo |- createcombatItemsBoostArray ==>
	
	/if (!${Defined[combatItem]}) /declare combatItem item local
	
	/varset combatItem ${FindItem[=Ancient Frozen Taelosian Blood Axe]}
	/if (${combatItem.ID}) {
		/declare timer-${combatItem.ID} timer outer 3m
		/call BuildArray combatItemsBoost ${combatItem.Name} outer
	}
	
	/varset combatItem ${FindItem[=Axe of Frenzy]}
	/if (${combatItem.ID}) {
		/declare timer-${combatItem.ID} timer outer 0
		/call BuildArray combatItemsBoost ${combatItem.Name} outer
	}
	
	/varset combatItem ${FindItem[=Kygern, The Trapped Destroyer]}
	/if (${combatItem.ID}) {
		/declare timer-${combatItem.ID} timer outer 6m
		/call BuildArray combatItemsBoost ${combatItem.Name} outer
	}
	
	/varset combatItem ${FindItem[=Vaelthic, The Rage Bringer]}
	/if (${combatItem.ID}) {
		/declare timer-${combatItem.ID} timer outer 3,5m
		/call BuildArray combatItemsBurn ${combatItem.Name} outer
	}
	
	/if (${outerDebug}) /echo <== createcombatItemsBoostArray -|
/RETURN
