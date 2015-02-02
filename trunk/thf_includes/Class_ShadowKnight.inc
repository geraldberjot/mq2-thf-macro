|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Class_ShadowKnight.inc v1.0
|- Adapted for THF by Conradd (macro from Killians of PEQ)
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Contains Shadow Knight specific functions related to,
|-		Setup and Ini entry information
|-		MainLoop, and AssistLoop hooks
|-		Over-all functionality / flow
|---------------------------------------------------------------------------------------------------------------------------------------------------|

SUB SHD_Setup
	/if (${outerDebug}) /echo |- SHD_Setup ==>

	/if (${Ini[${iniToon},${Me.Class},LifeTap#1].Length}) /call iniToArray "${iniToon},${Me.Class},LifeTap#" lifeTaps

	/if (${outerDebug}) /echo <== SHD_Setup -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB SHD_BackgroundEvents

/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB SHD_MacroSettings
	/if (${outerDebug}) /echo |- SHD_MacroSettings ==>

	/if (${outerDebug}) /echo <== SHD_MacroSettings -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB SHD_CharacterSettings
	/if (${outerDebug}) /echo |- SHD_CharacterSettings ==>

	/call writeToIni "${iniToon},${Me.Class},LifeTaps"

	/if (${outerDebug}) /echo <== SHD_CharacterSettings -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB SHD_Aliases
	/if (${outerDebug}) /echo |- SHD_Aliases ==>

	/call writeToIni "${iniTHF},ShadowKnight,Functions" "check_Burns/check_lifeTap/check_Debuffs/check_CombatBuffs/check_DoTs/check_Nukes/check_Buffs/check_Pets" FALSE

	/if (${outerDebug}) /echo <== SHD_Aliases -|
/RETURN