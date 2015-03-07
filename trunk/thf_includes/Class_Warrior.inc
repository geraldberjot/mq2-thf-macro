|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Class_Warrior.inc v1.0
|- Adapted for THF by Conradd (macro from Killians of PEQ)
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Contains Warrior specific functions related to,
|-		Setup and Ini entry information
|-		MainLoop, and AssistLoop hooks
|-		Over-all functionality / flow
|---------------------------------------------------------------------------------------------------------------------------------------------------|

SUB WAR_CombatAbilities
	/if (${outerDebug}) /echo |- WAR_CombatAbilities ==>

	|- Check Target distance.
	/if (${Target.Distance} > ${Spawn[${Target.ID}].MaxRangeTo}) {
		/if (${outerDebug}) {
			/echo I am too far away from [${Spawn[id ${outerAssistTarget}].CleanName}] to use abilities.
			/chat ${channelNumber} I am too far away from [${Spawn[id ${outerAssistTarget}].CleanName}] to use abilities.
		}
	} else {
		/if (${Select[${Me.TargetOfTarget},${Me.CleanName}]}) {
			/if (${outerDebug} || ${outerDebug_Assists}) /echo I currently have aggro on [${Target.CleanName}], skipping taunt.
		} else {
			/declare i int local

			|- Simple Taunt
			/if (${Me.AbilityReady[Taunt]}) {
				/doability Taunt
			}
			|- Provoke
			/if (${Me.CombatAbilityReady[${outerProvokeDiscipline}]}) {
				/call thf_Casting ${outerProvokeDiscipline}
			}
			
			/for i 1 to ${aggroItems.Size}
				/call thf_Casting ${aggroItems[${i}]}
			/next i

			|- Area Effect Aggro
			/if (${outerAoeAggro}) {
				/if (${Me.CombatAbilityReady[${outerProvokeDiscipline}]}) {
					/disc ${outerProvokeDiscipline}
				}
				
				/for i 1 to ${aoeAggroItems.Size}
					/call thf_Casting ${aoeAggroItems[${i}]}
				/next i
			}
		}
	
	}

	/if (${outerDebug}) /echo <== WAR_CombatAbilities -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- attemptProvoke
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB check_warProvoke(provokeDisc)
	/if (${outerDebug}) /echo |- check_warProvoke ==>

	/if (!${Defined[MinBellowEndurance]}) {
		/if (${provokeDisc.Equal[Ancient: Chaos Cry]}) {
			/declare MinBellowEndurance int outer 300
		} else /if (${provokeDisc.Equal[Bellow of the Mastruq]}) {
			/declare MinBellowEndurance int outer 275
		} else /if (${provokeDisc.Equal[Incite]}) {
			/declare MinBellowEndurance int outer 250
		} else /if (${provokeDisc.Equal[Berate]}) {
			/declare MinBellowEndurance int outer 200
		} else /if (${provokeDisc.Equal[Bellow]}) {
			/declare MinBellowEndurance int outer 175
		} else /if (${provokeDisc.Equal[Provoke]}) {
			/declare MinBellowEndurance int outer 85
		}
	}
	
	/if (${Me.CombatAbilityReady[${provokeDisc}]}) {
	
		|- check endurance
		/if (${Me.Endurance} < ${MinBellowEndurance}) {
			/if (${outerDebug}) /echo |- attemptProvoke -|- I do not have enough endurance to use [${provokeDisc}].
		} else {
	
			/if (${provokeTimer_${Target.ID}} && ${Spawn[${Me.TargetOfTarget}].CleanName.Equal[${Me}]}) {
				/if (${outerDebug}) /echo |- attemptProvoke -|- I still have aggro, and have not met my recast timer for [${provokeDisc}] [${provokeTimer_${Target.ID}}].
			} else {
	
				/call useDisc 0 "${provokeDisc}"
				/call createTimer provokeTimer_${Target.ID} 600
			}
		}
	}

	/if (${outerDebug}) /echo <== check_warProvoke -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB WAR_Setup
	/if (${outerDebug}) /echo |- WAR_Setup ==>

	/call iniToVar "${iniTHF},Class Specific Info,Warrior Provoke Disciplines" warriorBellows string outer

	|- Pick up the lastest discipline
	/call getLatestProvokeDiscipline
	
	/call createItemsArray

	/if (${outerDebug}) /echo <== WAR_Setup -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB WAR_BackgroundEvents

/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB WAR_MacroSettings
	/if (${outerDebug}) /echo |- WAR_MacroSettings ==>

	|- MacroData settings
	/call writeToIni "${iniTHF},Class Specific Info,Warrior Provoke Disciplines" "Provoke,Bellow,Berate,Incite,Bellow of the Mastruq,Ancient: Chaos Cry" 0
	
	/call writeToIni "${iniTHF},Warrior,Functions" "check_Burns/check_CombatBuffs/check_Buffs" FALSE
	
	/if (${outerDebug}) /echo <== WAR_MacroSettings -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB WAR_CharacterSettings
	/if (${outerDebug}) /echo |- WAR_CharacterSettings ==>

	/if (${outerDebug}) /echo <== WAR_CharacterSettings -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB WAR_Aliases
	/if (${outerDebug}) /echo |- WAR_Aliases ==>

	/if (${outerDebug}) /echo <== WAR_Aliases -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Check every strike discipline to get the highest one
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB getLatestProvokeDiscipline
	/if (${outerDebug}) /echo |- getLatestProvokeDiscipline ==>
	
	/if (!${Defined[outerProvokeDiscipline]}) /declare outerProvokeDiscipline string outer

	/if (${Me.CombatAbility[Enraged Scowl Rk. III]}) {
		/varset outerProvokeDiscipline "Enraged Scowl Rk. III"
	} else /if (${Me.CombatAbility[Enraged Scowl Rk. II]}) {
		/varset outerProvokeDiscipline "Enraged Scowl Rk. II"
	} else /if (${Me.CombatAbility[Enraged Scowl Rk. I]}) {
		/varset outerProvokeDiscipline "Enraged Scowl Rk. I"
	} else /if (${Me.CombatAbility[Bazu Bellow]}) {
		/varset outerProvokeDiscipline "Bazu Bellow"
	} else /if (${Me.CombatAbility[Ancient: Chaos Cry]}) {
		/varset outerProvokeDiscipline "Ancient: Chaos Cry"
	} else /if (${Me.CombatAbility[Bellow of the Mastruq]}) {
		/varset outerProvokeDiscipline "Bellow of the Mastruq"
	} else /if (${Me.CombatAbility[Incite]}) {
		/varset outerProvokeDiscipline "Incite"
	} else /if (${Me.CombatAbility[Berate]}) {
		/varset outerProvokeDiscipline "Berate"
	} else /if (${Me.CombatAbility[Bellow]}) {
		/varset outerProvokeDiscipline "Bellow"
	} else /if (${Me.CombatAbility[Provoke]}) {
		/varset outerProvokeDiscipline "Provoke"
	}
	/if (${outerDebug}) {
		/echo ${outerProvokeDiscipline}
		/echo <== getLatestProvokeDiscipline -|
	}
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Create the timers of the strike items available
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB createItemsArray
	/if (${outerDebug}) /echo |- createItemsArray ==>

	/if (!${Defined[itemID]}) /declare itemID int local
	
	/call createItemTimerAndAddToArray "Shield of The Omniscient" "aggroItems"
	/call createItemTimerAndAddToArray "Lucid Gauntlets of View" "aggroItems"
	/call createItemTimerAndAddToArray "Frozen Gladiator's Legguards" "aggroItems"
	/call createItemTimerAndAddToArray "Ebony Shield of Mane" "aggroItems"

	/call createItemTimerAndAddToArray "Artorias, the Abysswalker" "aoeAggroItems"
	/call createItemTimerAndAddToArray "Belt of Eternal Chaos" "aoeAggroItems"

	/if (${outerDebug}) /echo <== createItemsArray -|
/RETURN