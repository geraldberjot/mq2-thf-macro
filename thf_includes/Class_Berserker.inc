|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Class_Berserker.inc v1.0
|- Adapted for THF by Conradd (macro from Killians of PEQ)
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Contains CLR specific functions related to,
|-		Setup and Ini entry information
|-		MainLoop, and AssistLoop hooks
|-		Over-all functionality / flow
|---------------------------------------------------------------------------------------------------------------------------------------------------|

SUB BER_Setup
	/if (${outerDebug}) /echo |- BER_Setup ==>
	
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