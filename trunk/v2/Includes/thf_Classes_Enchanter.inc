|----------------------------------------------------------------------------|
|- e3_Classes_Enchanter.inc v2.2 ( originally designed to work with e3.mac )	-|
|- Written by Conradd of THF from Killians of PEQ												-|
|----------------------------------------------------------------------------|
|- Contains Enchanter specific functions related to,							-|
|-		Setup and Ini entry information										-|
|-		MainLoop, and AssistLoop hooks										-|
|-		Over-all functionality / flow										-|
|----------------------------------------------------------------------------|



|--------------------------------------------------------------------|
|- Enchanter's Class specific mainLoop hook.							-|
|--------------------------------------------------------------------|
|- Prioritizes macro functions based on the Enchanter class dynamic.	-|
|--------------------------------------------------------------------|
SUB ENC_Main
/if (${Debug}) /echo |- ENC_Main ==>

	| checkCombatBuffs.
	/if (!${medBreak} && !${idle} && ${CombatBuffs.Size}) /call CombatBuffs
	/if (!${actionTaken}) {
	
		| Assist Functions.
		/if (${assisting}) /call ${Me.Class.ShortName}_Assist
	}

/if (${Debug}) /echo <== ENC_Main -|
/RETURN



|--------------------------------------------------------------------|
|- Enchanter's Class specific Assist Loop.								-|
|--------------------------------------------------------------------|
|- Prioritizes Assist functions based on the Enchanter class dynamic.	-|
|--------------------------------------------------------------------|
SUB ENC_Assist
/if (${Debug}) /echo |- ENC_Assist ==>

	/call e3_AssistFunctions

/if (${Debug}) /echo <== ENC_Assist -|
/RETURN



SUB ENC_Setup
/if (${Debug}) /echo |- ENC_Setup ==>
	
/if (${Debug}) /echo <== ENC_Setup -|
/RETURN



SUB ENC_backgroundEvents
/if (${Debug}) /echo |- ENC_backgroundEvents ==>

/if (${Debug}) /echo <== ENC_backgroundEvents -|
/RETURN



SUB ENC_MacroSettings
/if (${Debug}) /echo |- ENC_MacroSettings ==>

/if (${Debug}) /echo <== ENC_MacroSettings -|
/RETURN



SUB ENC_CharacterSettings
/if (${Debug}) /echo |- ENC_CharacterSettings ==>

/if (${Debug}) /echo <== ENC_CharacterSettings -|
/RETURN



SUB ENC_Aliases
/if (${Debug}) /echo |- ENC_Aliases ==>

/if (${Debug}) /echo <== ENC_Aliases -|
/RETURN