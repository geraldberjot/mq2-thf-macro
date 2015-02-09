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
			/chat ${channelNumber} I am too far away from [${Spawn[id ${outerAssistTarget}].CleanName}] to use abilities.
		}
	} else {
		/declare combatItem item local
		/declare abilityName string local
		/declare minEnd int local
		/declare i int local
	
		/if (outerDamageLevel > 0) {
			/for i 1 to ${combatItemsBoost.Size}
				/varset combatItem ${FindItem[=${combatItemsBoost[${i}]}]}
				/if (${combatItem.ID}) {
					|- Timers were already created in the sub routine "createCombatItemsArray"
					/if (${timer-${combatItem.ID}.Value} <= 0) {
						/casting ${combatItem.ID}|item
						/varset ${timer-${combatItem.ID}} ${timer-${combatItem.ID}.OriginalValue}
						/delay 1
					}
				}
			/next i
		}
		
		/if (outerDamageLevel > 1) {
			/for i 1 to ${combatItemsBurn.Size}
				/varset combatItem ${FindItem[=${combatItemsBurn[${i}]}]}
				/if (${combatItem.ID}) {
					|- Timers were already created in the sub routine "createCombatItemsArray"
					/if (${timer-${combatItem.ID}.Value} <= 0) {
						/casting ${combatItem.ID}|item
						/varset ${timer-${combatItem.ID}} ${timer-${combatItem.ID}.OriginalValue}
						/delay 1
					}
				}
			/next i
		}
		
		|- Use combat ability (discipline)
		/if (!${Me.CombatAbilityReady[${outerVolleyDiscipline}]}) {
			/if (${outerDebug}) /echo Discipline [${outerVolleyDiscipline}] is not ready.
		} else {
			/call thf_Casting ${outerAssistTarget} "${outerVolleyDiscipline}/Instant"
		}
		/if (!${Me.CombatAbilityReady[${outerEchoingWhispersDiscipline}]}) {
			/if (${outerDebug}) /echo Discipline [${outerEchoingWhispersDiscipline}] is not ready.
		} else {
			/call thf_Casting ${outerAssistTarget} "${outerEchoingWhispersDiscipline}/Instant"
		}
		
	}
	
	/if (${outerDebug}) /echo <== BER_CombatAbilities -|
/RETURN

SUB BER_Setup
	/if (${outerDebug}) /echo |- BER_Setup ==>
	
	/call getLatestDisciplines
	
	/if (${outerDebug}) /echo <== BER_Setup -|
/RETURN

SUB BER_BackgroundEvents

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
SUB getLatestDisciplines()
	/if (${outerDebug}) /echo |- getLatestDisciplines ==>
	
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
	
	/if (!${Defined[outerEchoingWhispersDiscipline]}) /declare outerEchoingWhispersDiscipline string outer

	/if (${Me.CombatAbility[Echoing Whispers Rk. III]}) {
		/varset outerEchoingWhispersDiscipline "Echoing Whispers Rk. III"
	} else /if (${Me.CombatAbility[Echoing Whispers Rk. II]}) {
		/varset outerEchoingWhispersDiscipline "Echoing Whispers Rk. II"
	} else /if (${Me.CombatAbility[Echoing Whispers Rk. I]}) {
		/varset outerEchoingWhispersDiscipline "Echoing Whispers Rk. I"
	}
	
	/if (${outerDebug}) /echo <== getLatestDisciplines -|
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
