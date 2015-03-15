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
				/call thf_Casting ${outerAssistTarget} ${outerProvokeDiscipline}
			}
			
			/for i 1 to ${aggroItems.Size}
				/call thf_Casting ${outerAssistTarget} ${aggroItems[${i}]}
			/next i

			|- Area Effect Aggro
			/if (${outerAoeAggro}) {
				/if (${Me.CombatAbilityReady[${outerProvokeDiscipline}]}) {
					/call thf_Casting ${outerAssistTarget} ${outerProvokeDiscipline}
				}
				
				/for i 1 to ${aoeAggroItems.Size}
					/call thf_Casting ${outerAssistTarget} ${aoeAggroItems[${i}]}
				/next i
			}
		}
	}

	/if (${outerDebug}) /echo <== WAR_CombatAbilities -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB WAR_Setup
	/if (${outerDebug}) /echo |- WAR_Setup ==>

	|- Pick up the lastest discipline
	/call getLatestWarriorDiscipline
	
	/call createWarriorItemsArray

	/if (${outerDebug}) /echo <== WAR_Setup -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB WAR_BackgroundEvents

/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB WAR_MacroSettings
	/if (${outerDebug}) /echo |- WAR_MacroSettings ==>

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
SUB getLatestWarriorDiscipline
	/if (${outerDebug}) /echo |- getLatestWarriorDiscipline ==>
	
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
		/echo <== getLatestWarriorDiscipline -|
	}
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Create the timers of the strike items available
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB createWarriorItemsArray
	/if (${outerDebug}) /echo |- createWarriorItemsArray ==>

	/call createItemTimerAndAddToArray "Shield of The Omniscient" "aggroItems"
	/call createItemTimerAndAddToArray "Lucid Gauntlets of View" "aggroItems"
	/call createItemTimerAndAddToArray "Frozen Gladiator's Legguards" "aggroItems"
	/call createItemTimerAndAddToArray "Ebony Shield of Mane" "aggroItems"

	/call createItemTimerAndAddToArray "Artorias, the Abysswalker" "aoeAggroItems"
	/call createItemTimerAndAddToArray "Belt of Eternal Chaos" "aoeAggroItems"

	/if (${outerDebug}) /echo <== createWarriorItemsArray -|
/RETURN