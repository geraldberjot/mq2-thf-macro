|----------------------------------------------------------------------------|
|- e3_Classes_Ranger.inc v3.0 ( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ												-|
|----------------------------------------------------------------------------|
|- Contains Ranger specific functions related to,							-|
|-		Setup and Ini entry information										-|
|-		MainLoop, and AssistLoop hooks										-|
|-		Over-all functionality / flow										-|
|----------------------------------------------------------------------------|



SUB RNG_Setup
/if (${outerDebug}) /echo |- RNG_Setup ==>
	
/if (${outerDebug}) /echo <== RNG_Setup -|
/RETURN



SUB RNG_checkBackgroundEvents

/RETURN



SUB RNG_MacroSettings
/if (${outerDebug}) /echo |- RNG_MacroSettings ==>

	| Adv Settings
	/call writeToIni "${iniAdvanced},RNG Functions,RNG Function#1" "check_Burns" 1
	/call writeToIni "${iniAdvanced},RNG Functions,RNG Function#2" "check_CombatBuffs" 1
	/call writeToIni "${iniAdvanced},RNG Functions,RNG Function#3" "check_Heals"	 1
	/call writeToIni "${iniAdvanced},RNG Functions,RNG Function#4" "check_DoTs" 1
	/call writeToIni "${iniAdvanced},RNG Functions,RNG Function#5" "check_Nukes" 1
	/call writeToIni "${iniAdvanced},RNG Functions,RNG Function#6" "check_Buffs" 1

/if (${outerDebug}) /echo <== RNG_MacroSettings -|
/RETURN



SUB RNG_CharacterSettings
/if (${outerDebug}) /echo |- RNG_CharacterSettings ==>

/if (${outerDebug}) /echo <== RNG_CharacterSettings -|
/RETURN



SUB RNG_Aliases
/if (${outerDebug}) /echo |- RNG_Aliases ==>

/if (${outerDebug}) /echo <== RNG_Aliases -|
/RETURN