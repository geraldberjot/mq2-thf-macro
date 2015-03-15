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
					/call thf_Casting ${Me.ID} ${combatItem.ID}
					/delay 1
				}
			/next i
		}
		
		/if (outerDamageLevel > 1) {
			/for i 1 to ${combatItemsBurn.Size}
				/varset combatItem ${FindItem[=${combatItemsBurn[${i}]}]}
				/if (${combatItem.ID}) {
					/call thf_Casting ${Me.ID} ${combatItem.ID}
					/delay 1
				}
			/next i
		}
		
		|- Use combat ability (discipline)
		/call thf_Casting ${outerAssistTarget} "${outerVolleyDiscipline}"
		/call thf_Casting ${outerAssistTarget} "${outerEchoingWhispersDiscipline}"
	}
	
	/if (${outerDebug}) /echo <== BER_CombatAbilities -|
/RETURN

SUB BER_Setup
	/if (${outerDebug}) /echo |- BER_Setup ==>
	
	/call getLatestBerserkerDisciplines
	
	/call createBerserkerItemsArray
	
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
SUB getLatestBerserkerDisciplines()
	/if (${outerDebug}) /echo |- getLatestBerserkerDisciplines ==>
	
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
	
	/if (${outerDebug}) /echo <== getLatestBerserkerDisciplines -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Create the timers of the items used in combat
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB createBerserkerItemsArray()
	/if (${outerDebug}) /echo |- createBerserkerItemsArray ==>
	
	/call createItemTimerAndAddToArray "Ancient Frozen Taelosian Blood Axe" "combatItemsBoost"
	/call createItemTimerAndAddToArray "Axe of Frenzy" "combatItemsBoost"
	/call createItemTimerAndAddToArray "Kygern, The Trapped Destroyer" "combatItemsBoost"

	/call createItemTimerAndAddToArray "Vaelthic, The Rage Bringer" "combatItemsBurn"
	
	/if (${outerDebug}) /echo <== createBerserkerItemsArray -|
/RETURN
