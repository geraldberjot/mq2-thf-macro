|------------------------------------------------------------------------------------|
|- e3_Classes_Shadow Knight.inc v3.0 ( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ														-|
|------------------------------------------------------------------------------------|
|- Contains Shadow Knight specific functions related to,							-|
|-		Setup and Ini entry information												-|
|-		MainLoop, and AssistLoop hooks												-|
|-		Over-all functionality / flow												-|
|------------------------------------------------------------------------------------|



SUB SHD_Setup
/if (${outerDebug}) /echo |- SHD_Setup ==>

	/if (${Ini[${iniCharacter},${Me.Class},LifeTap#1].Length}) /call IniToArray "${iniCharacter},${Me.Class},LifeTap#" lifeTaps

/if (${outerDebug}) /echo <== SHD_Setup -|
/RETURN



SUB SHD_checkBackgroundEvents

/RETURN



SUB SHD_MacroSettings
/if (${outerDebug}) /echo |- SHD_MacroSettings ==>

/if (${outerDebug}) /echo <== SHD_MacroSettings -|
/RETURN



SUB SHD_CharacterSettings
/if (${outerDebug}) /echo |- SHD_CharacterSettings ==>

	/call writeToIni "${iniCharacter},${Me.Class},LifeTap#1"

/if (${outerDebug}) /echo <== SHD_CharacterSettings -|
/RETURN



SUB SHD_Aliases
/if (${outerDebug}) /echo |- SHD_Aliases ==>

	| Adv Settings
	/call writeToIni "${iniAdvanced},SHD Functions,SHD Function#1" "check_Burns" 1
	/call writeToIni "${iniAdvanced},SHD Functions,SHD Function#2" "check_lifeTap" 1
	/call writeToIni "${iniAdvanced},SHD Functions,SHD Function#3" "check_Debuffs" 1
	/call writeToIni "${iniAdvanced},SHD Functions,SHD Function#4" "check_CombatBuffs" 1
	/call writeToIni "${iniAdvanced},SHD Functions,SHD Function#5" "check_DoTs" 1
	/call writeToIni "${iniAdvanced},SHD Functions,SHD Function#6" "check_Nukes" 1
	/call writeToIni "${iniAdvanced},SHD Functions,SHD Function#7" "checkBuffs" 1
	/call writeToIni "${iniAdvanced},SHD Functions,SHD Function#8" "check_Pets" 1

/if (${outerDebug}) /echo <== SHD_Aliases -|
/RETURN