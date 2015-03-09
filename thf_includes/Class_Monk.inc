|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Class_Monk.inc v1.0
|- Adapted for THF by Conradd (macro from Killians of PEQ)
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Contains Monk specific functions related to,
|-		Setup and Ini entry information
|-		MainLoop, and AssistLoop hooks
|-		Over-all functionality / flow
|---------------------------------------------------------------------------------------------------------------------------------------------------|

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Uses MONK combat abilities, AAs, and disciplines.
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB MNK_CombatAbilities
	/if (${outerDebug}) /echo |- MNK_CombatAbilities ==>

	|- Check for mending
	/if (${Me.PctHPs} <= ${outerMendPct}) {
		/doability mend
	}
	
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
		
		|- Hundred hand haste
		/call thf_Casting ${Me.ID} ${outerHundredDiscipline}
		
		|- Decrease target HP
		/call thf_Casting ${outerAssistTarget} ${Spell[Hermit's Insight].RankName}
		
		
		
	}

	/if (${outerDebug}) /echo <== MNK_CombatAbilities -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- 
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB MNK_Setup
	/if (${outerDebug}) /echo |- MNK_Setup ==>
	
	|- Mend
	/call iniToVar "${iniToon},Melee,Mend" outerMendPct int outer
	/if (!${Int[outerMendPct]}) {
		/varset outerMendPct 30
	}

	|- Pick up the lastest discipline
	/call getLatestDiscipline
	
	/call createItemsArray
	
	/if (${outerDebug}) /echo <== MNK_Setup -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- 
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB MNK_BackgroundEvents

/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- 
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB MNK_MacroSettings
	/if (${outerDebug}) /echo |- MNK_MacroSettings ==>

	/call writeToIni "${iniTHF},Monk,Functions" "check_Burns/check_CombatBuffs/check_Buffs" FALSE
	
	|- Entry for the health pct to use mend
	/call writeToIni "${iniToon},Melee,Mend" "50" FALSE

	/if (${outerDebug}) /echo <== MNK_MacroSettings -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- 
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB MNK_CharacterSettings
	/if (${outerDebug}) /echo |- MNK_CharacterSettings ==>

	/if (${outerDebug}) /echo <== MNK_CharacterSettings -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- 
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB MNK_Aliases
	/if (${outerDebug}) /echo |- MNK_Aliases ==>

	/if (${outerDebug}) /echo <== MNK_Aliases -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Check every strike discipline to get the highest one
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB getLatestDiscipline
	/if (${outerDebug}) /echo |- getLatestDiscipline ==>
	
	
	
	Phantom Shadows Rk. I
	Swift Strike Rk. I
	
	/if (!${Defined[outerHundredDiscipline]}) /declare outerHundredDiscipline string outer

	/if (${Me.CombatAbility[Hands of the Master Rk. III]}) {
		/varset outerHundredDiscipline "Hands of the Master Rk. III"
	} else /if (${Me.CombatAbility[Hands of the Master Rk. II]}) {
		/varset outerHundredDiscipline "Hands of the Master Rk. II"
	} else /if (${Me.CombatAbility[Hands of the Master Rk. I]}) {
		/varset outerHundredDiscipline "Hands of the Master Rk. I"
	} else /if (${Me.CombatAbility[Fists of Wu]}) {
		/varset outerHundredDiscipline "Fists of Wu"
	} else /if (${Me.CombatAbility[Aura of Speed]}) {
		/varset outerHundredDiscipline "Aura of Speed"
	} else /if (${Me.CombatAbility[Nerve Spasm]}) {
		/varset outerHundredDiscipline "Nerve Spasm"
	}
	
	/if (!${Defined[outerInfusedChaosDiscipline]}) /declare outerInfusedChaosDiscipline string outer

	/if (${Me.CombatAbility[Infused Chaos Rk. III]}) {
		/varset outerInfusedChaosDiscipline "Infused Chaos Rk. III"
	} else /if (${Me.CombatAbility[Infused Chaos Rk. II]}) {
		/varset outerInfusedChaosDiscipline "Infused Chaos Rk. II"
	} else /if (${Me.CombatAbility[Infused Chaos Rk. I]}) {
		/varset outerInfusedChaosDiscipline "Infused Chaos Rk. I"
	} else /if (${Me.CombatAbility[Tranquil Focus]}) {
		/varset outerInfusedChaosDiscipline "Tranquil Focus"
	} else /if (${Me.CombatAbility[Innerflame Discipline]}) {
		/varset outerInfusedChaosDiscipline "Innerflame Discipline"
	}
	
	/if (${outerDebug}) {
		/echo ${outerHundredDiscipline}
		/echo ${outerInfusedChaosDiscipline}
		/echo <== getLatestDiscipline -|
	}
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Create the timers of the strike items available
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB createItemsArray
	/if (${outerDebug}) /echo |- createItemsArray ==>

	/call createItemTimerAndAddToArray "Epaullettes of Tranquility" "infusedChaosItems"
	/call createItemTimerAndAddToArray "Epaullettes of Peace" "infusedChaosItems"
	/call createItemTimerAndAddToArray "Tranquility Epaullettes" "infusedChaosItems"

	/call createItemTimerAndAddToArray "Ancient Frozen Fistwraps" "combatItemsBoost"
	/call createItemTimerAndAddToArray "Numinad, The Right Hand of Marr" "combatItemsBoost"
	/call createItemTimerAndAddToArray "Fist of the Tranquil" "combatItemsBoost"
	/call createItemTimerAndAddToArray "Asheint, Cloak of Tranquility" "combatItemsBurn"
	
	/if (${outerDebug}) /echo <== createItemsArray -|
/RETURN
