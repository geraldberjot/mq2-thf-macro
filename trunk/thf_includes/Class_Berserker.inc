|------------------------------------------------------------------------|
|- e3_Classes_CLR.inc v3.0 ( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ											-|
|------------------------------------------------------------------------|
|- Contains CLR specific functions related to,							-|
|-		Setup and Ini entry information									-|
|-		MainLoop, and AssistLoop hooks									-|
|-		Over-all functionality / flow									-|
|------------------------------------------------------------------------|



SUB BER_Setup
/if (${outerDebug}) /echo |- BER_Setup ==>
	
/if (${outerDebug}) /echo <== BER_Setup -|
/RETURN



SUB BER_checkBackgroundEvents

/RETURN



SUB BER_MacroSettings
/if (${outerDebug}) /echo |- BER_MacroSettings ==>

	| Adv Settings
	/call writeToIni "${iniTHF},BER Functions,BER Function#1" "check_Burns" 1
	/call writeToIni "${iniTHF},BER Functions,BER Function#2" "check_CombatBuffs" 1
	/call writeToIni "${iniTHF},BER Functions,BER Function#3" "check_Buffs" 1

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