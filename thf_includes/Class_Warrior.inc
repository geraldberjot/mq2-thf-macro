|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Class_Warrior.inc v1.0
|- Adapted for THF by Conradd (macro from Killians of PEQ)
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Contains Warrior specific functions related to,
|-		Setup and Ini entry information
|-		MainLoop, and AssistLoop hooks
|-		Over-all functionality / flow
|---------------------------------------------------------------------------------------------------------------------------------------------------|

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

	/if (${outerDebug}) /echo <== WAR_Setup -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB WAR_CheckBackgroundEvents

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


|**

Additional warrior taunt discs.  I'm not sure hwo thses work, so I haven't added them.
	bazu bellow
	scowl
	scowl rk. ii
	scowl rk. iii
	sneer
	sneer
	sneer
	bazu bluster
	bazu
	bazu
	jeer
	jeer
	jeer
	bazu roar
	bazu
	bazu
	scoff
	grendlaen roar
	scorn
	krondal's roar
	ridicule
