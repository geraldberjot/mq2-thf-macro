|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Class_Druid.inc v1.0
|- Adapted for THF by Conradd (macro from Killians of PEQ)
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Contains Druid specific functions related to,
|-		Setup and Ini entry information
|-		MainLoop, and AssistLoop hooks
|-		Over-all functionality / flow
|---------------------------------------------------------------------------------------------------------------------------------------------------|

SUB DRU_Setup
	/if (${outerDebug}) /echo |- DRU_Setup ==>
	
	/if (${outerDebug}) /echo <== DRU_Setup -|
/RETURN

SUB DRU_CheckBackgroundEvents

/RETURN

SUB DRU_MacroSettings
	/if (${outerDebug}) /echo |- DRU_MacroSettings ==>

	/call writeToIni "${iniTHF},Druid,Functions" "check_healChain/check_Heals/check_CombatBuffs/check_Burns/check_Debuffs/check_DoTs/check_Rain/check_Nukes/check_Buffs" FALSE

	/if (${outerDebug}) /echo <== DRU_MacroSettings -|
/RETURN

SUB DRU_CharacterSettings
	/if (${outerDebug}) /echo |- DRU_CharacterSettings ==>

	/if (${outerDebug}) /echo <== DRU_CharacterSettings -|
/RETURN

SUB DRU_Aliases
	/if (${outerDebug}) /echo |- DRU_Aliases ==>

	/if (${outerDebug}) /echo <== DRU_Aliases -|
/RETURN