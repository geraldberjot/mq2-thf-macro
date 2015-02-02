|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Class_Ranger.inc v1.0
|- Adapted for THF by Conradd (macro from Killians of PEQ)
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Contains Ranger specific functions related to,
|-		Setup and Ini entry information
|-		MainLoop, and AssistLoop hooks
|-		Over-all functionality / flow
|---------------------------------------------------------------------------------------------------------------------------------------------------|

SUB RNG_Setup
	/if (${outerDebug}) /echo |- RNG_Setup ==>
	
	/if (${outerDebug}) /echo <== RNG_Setup -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB RNG_BackgroundEvents

/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB RNG_MacroSettings
	/if (${outerDebug}) /echo |- RNG_MacroSettings ==>

	/call writeToIni "${iniTHF},Ranger,Functions" "check_Burns/check_CombatBuffs/check_Heals/check_DoTs/check_Nukes/check_Buffs" FALSE

	/if (${outerDebug}) /echo <== RNG_MacroSettings -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB RNG_CharacterSettings
	/if (${outerDebug}) /echo |- RNG_CharacterSettings ==>

	/if (${outerDebug}) /echo <== RNG_CharacterSettings -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB RNG_Aliases
	/if (${outerDebug}) /echo |- RNG_Aliases ==>

	/if (${outerDebug}) /echo <== RNG_Aliases -|
/RETURN