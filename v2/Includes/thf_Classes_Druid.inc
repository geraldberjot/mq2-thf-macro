|----------------------------------------------------------------------------|
|- e3_Classes_Druid.inc v2.2 ( originally designed to work with e3.mac )	-|
|- Written by Conradd of THF from Killians of PEQ												-|
|----------------------------------------------------------------------------|
|- Contains Druid specific functions related to,							-|
|-		Setup and Ini entry information										-|
|-		MainLoop, and AssistLoop hooks										-|
|-		Over-all functionality / flow										-|
|----------------------------------------------------------------------------|



|--------------------------------------------------------------------|
|- Druid's Class specific mainLoop hook.							-|
|--------------------------------------------------------------------|
|- Prioritizes macro functions based on the Druid class dynamic.	-|
|--------------------------------------------------------------------|
SUB DRU_Main
/if (${Debug}) /echo |- DRU_Main ==>

	| checkCombatBuffs.
	/if (!${medBreak} && !${idle} && ${CombatBuffs.Size}) /call CombatBuffs
	/if (!${actionTaken}) {
	
		| Assist Functions.
		/if (${assisting}) /call ${Me.Class.ShortName}_Assist
	}

/if (${Debug}) /echo <== DRU_Main -|
/RETURN



|--------------------------------------------------------------------|
|- Druid's Class specific Assist Loop.								-|
|--------------------------------------------------------------------|
|- Prioritizes Assist functions based on the Druid class dynamic.	-|
|--------------------------------------------------------------------|
SUB DRU_Assist
/if (${Debug}) /echo |- DRU_Assist ==>

	/call e3_AssistFunctions

/if (${Debug}) /echo <== DRU_Assist -|
/RETURN



SUB DRU_Setup
/if (${Debug}) /echo |- DRU_Setup ==>
	
/if (${Debug}) /echo <== DRU_Setup -|
/RETURN



SUB DRU_backgroundEvents
/if (${Debug}) /echo |- DRU_backgroundEvents ==>

/if (${Debug}) /echo <== DRU_backgroundEvents -|
/RETURN



SUB DRU_MacroSettings
/if (${Debug}) /echo |- DRU_MacroSettings ==>

/if (${Debug}) /echo <== DRU_MacroSettings -|
/RETURN



SUB DRU_CharacterSettings
/if (${Debug}) /echo |- DRU_CharacterSettings ==>

/if (${Debug}) /echo <== DRU_CharacterSettings -|
/RETURN



SUB DRU_Aliases
/if (${Debug}) /echo |- DRU_Aliases ==>

/if (${Debug}) /echo <== DRU_Aliases -|
/RETURN