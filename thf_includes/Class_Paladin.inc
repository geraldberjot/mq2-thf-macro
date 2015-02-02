|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Class_Paladin.inc v1.0
|- Adapted for THF by Conradd (macro from Killians of PEQ)
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Contains Paladin specific functions related to,
|-		Setup and Ini entry information
|-		MainLoop, and AssistLoop hooks
|-		Over-all functionality / flow
|---------------------------------------------------------------------------------------------------------------------------------------------------|

SUB PAL_Setup
	/if (${outerDebug}) /echo |- PAL_Setup ==>
	
	/call iniToVar "${iniToon},Paladin,Auto-Yaulp (On/Off)" AutoYaulp bool outer
	/call iniToVar "${iniToon},Paladin,Yaulp Spell" yaulpSpell string outer
	
	/if (${outerDebug}) /echo <== PAL_Setup -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB PAL_BackgroundEvents

/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB PAL_MacroSettings
	/if (${outerDebug}) /echo |- PAL_MacroSettings ==>

	/call writeToIni "${iniTHF},Paladin,Functions" "check_Yaulp/check_Heals/check_Burns/check_CombatBuffs/check_Nukes/check_Buffs" FALSE

	/if (${outerDebug}) /echo <== PAL_MacroSettings -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB PAL_CharacterSettings
	/if (${outerDebug}) /echo |- PAL_CharacterSettings ==>

	/call writeToIni "${iniToon},Paladin,Auto-Yaulp (On/Off)" On 0
	/call writeToIni "${iniToon},Paladin,Yaulp Spell"
	
	/if (${outerDebug}) /echo <== PAL_CharacterSettings -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB PAL_Aliases
	/if (${outerDebug}) /echo |- PAL_Aliases ==>

	/if (${outerDebug}) /echo <== PAL_Aliases -|
/RETURN