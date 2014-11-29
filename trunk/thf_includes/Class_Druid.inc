|----------------------------------------------------------------------------|
|- e3_Classes_Druid.inc v3.0 ( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ												-|
|----------------------------------------------------------------------------|
|- Contains Druid specific functions related to,							-|
|-		Setup and Ini entry information										-|
|-		MainLoop, and AssistLoop hooks										-|
|-		Over-all functionality / flow										-|
|----------------------------------------------------------------------------|



SUB DRU_Setup
/if (${outerDebug}) /echo |- DRU_Setup ==>
	
	/call iniToVar "${iniToon},Druid,Evac Spell" Evac_Spell string outer
	
/if (${outerDebug}) /echo <== DRU_Setup -|
/RETURN



SUB DRU_checkBackgroundEvents

/RETURN



SUB DRU_MacroSettings
/if (${outerDebug}) /echo |- DRU_MacroSettings ==>

	| Adv Settings
	/call writeToIni "${iniTHF},DRU Functions,DRU Function#1" "check_healChain" 1
	/call writeToIni "${iniTHF},DRU Functions,DRU Function#2" "check_Heals" 1
	/call writeToIni "${iniTHF},DRU Functions,DRU Function#3" "check_CombatBuffs" 1
	/call writeToIni "${iniTHF},DRU Functions,DRU Function#4" "check_Burns" 1
	/call writeToIni "${iniTHF},DRU Functions,DRU Function#5" "check_Debuffs" 1
	/call writeToIni "${iniTHF},DRU Functions,DRU Function#6" "check_DoTs" 1
	/call writeToIni "${iniTHF},DRU Functions,DRU Function#7" "check_Rain" 1
	/call writeToIni "${iniTHF},DRU Functions,DRU Function#8" "check_Nukes" 1
	/call writeToIni "${iniTHF},DRU Functions,DRU Function#9" "checkBuffs" 1

/if (${outerDebug}) /echo <== DRU_MacroSettings -|
/RETURN



SUB DRU_CharacterSettings
/if (${outerDebug}) /echo |- DRU_CharacterSettings ==>

	| Bot Ini
	/call writeToIni "${iniToon},Druid,Evac Spell"

/if (${outerDebug}) /echo <== DRU_CharacterSettings -|
/RETURN



SUB DRU_Aliases
/if (${outerDebug}) /echo |- DRU_Aliases ==>

/if (${outerDebug}) /echo <== DRU_Aliases -|
/RETURN