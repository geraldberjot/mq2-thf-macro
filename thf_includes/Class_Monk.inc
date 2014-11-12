|------------------------------------------------------------------------|
|- e3_Classes_Monk.inc v3.0 ( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ											-|
|------------------------------------------------------------------------|
|- Contains Monk specific functions related to,							-|
|-		Setup and Ini entry information									-|
|-		MainLoop, and AssistLoop hooks									-|
|-		Over-all functionality / flow									-|
|------------------------------------------------------------------------|



SUB MNK_Setup
/if (${outerDebug}) /echo |- MNK_Setup ==>
	
/if (${outerDebug}) /echo <== MNK_Setup -|
/RETURN



SUB MNK_checkBackgroundEvents

/RETURN



SUB MNK_MacroSettings
/if (${outerDebug}) /echo |- MNK_MacroSettings ==>

	| Adv Settings
	/call writeToIni "${iniAdvanced},MNK Functions,MNK Function#1" "check_Burns" 1
	/call writeToIni "${iniAdvanced},MNK Functions,MNK Function#2" "check_CombatBuffs" 1
	/call writeToIni "${iniAdvanced},MNK Functions,MNK Function#3" "checkBuffs" 1

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