|----------------------------------------------------------------------------|
|- e3_Classes_Paladin.inc v3.0 ( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ												-|
|----------------------------------------------------------------------------|
|- Contains Paladin specific functions related to,							-|
|-		Setup and Ini entry information										-|
|-		MainLoop, and AssistLoop hooks										-|
|-		Over-all functionality / flow										-|
|----------------------------------------------------------------------------|



SUB PAL_Setup
/if (${outerDebug}) /echo |- PAL_Setup ==>
	
	/call iniToVar "${iniCharacter},Paladin,Auto-Yaulp (On/Off)" AutoYaulp bool outer
	/call iniToVar "${iniCharacter},Paladin,Yaulp Spell" yaulpSpell string outer
	
/if (${outerDebug}) /echo <== PAL_Setup -|
/RETURN



SUB PAL_checkBackgroundEvents

/RETURN



SUB PAL_MacroSettings
/if (${outerDebug}) /echo |- PAL_MacroSettings ==>

	| Adv Settings
	/call writeToIni "${iniAdvanced},PAL Functions,PAL Function#1" "check_Yaulp" 1
	/call writeToIni "${iniAdvanced},PAL Functions,PAL Function#2" "check_Heals" 1
	/call writeToIni "${iniAdvanced},PAL Functions,PAL Function#3" "check_Burns" 1
	/call writeToIni "${iniAdvanced},PAL Functions,PAL Function#4" "check_CombatBuffs" 1
	/call writeToIni "${iniAdvanced},PAL Functions,PAL Function#5" "check_Nukes" 1
	/call writeToIni "${iniAdvanced},PAL Functions,PAL Function#6" "check_Buffs" 1

/if (${outerDebug}) /echo <== PAL_MacroSettings -|
/RETURN



SUB PAL_CharacterSettings
/if (${outerDebug}) /echo |- PAL_CharacterSettings ==>

	/call writeToIni "${iniCharacter},Paladin,Auto-Yaulp (On/Off)" On 0
	/call writeToIni "${iniCharacter},Paladin,Yaulp Spell"
	
/if (${outerDebug}) /echo <== PAL_CharacterSettings -|
/RETURN



SUB PAL_Aliases
/if (${outerDebug}) /echo |- PAL_Aliases ==>

/if (${outerDebug}) /echo <== PAL_Aliases -|
/RETURN