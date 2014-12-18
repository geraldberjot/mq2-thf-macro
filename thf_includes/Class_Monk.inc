|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Class_Monk.inc v1.0
|- Adapted for THF by Conradd (macro from Killians of PEQ)
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Contains Monk specific functions related to,
|-		Setup and Ini entry information
|-		MainLoop, and AssistLoop hooks
|-		Over-all functionality / flow
|---------------------------------------------------------------------------------------------------------------------------------------------------|

SUB MNK_Setup
	/if (${outerDebug}) /echo |- MNK_Setup ==>
	
	/if (${outerDebug}) /echo <== MNK_Setup -|
/RETURN

SUB MNK_CheckBackgroundEvents

/RETURN

SUB MNK_MacroSettings
	/if (${outerDebug}) /echo |- MNK_MacroSettings ==>

	/call writeToIni "${iniTHF},Monk,Functions" "check_Burns/check_CombatBuffs/check_Buffs" FALSE

	/if (${outerDebug}) /echo <== MNK_MacroSettings -|
/RETURN

SUB MNK_CharacterSettings
	/if (${outerDebug}) /echo |- MNK_CharacterSettings ==>

	/if (${outerDebug}) /echo <== MNK_CharacterSettings -|
/RETURN

SUB MNK_Aliases
	/if (${outerDebug}) /echo |- MNK_Aliases ==>

	/if (${outerDebug}) /echo <== MNK_Aliases -|
/RETURN