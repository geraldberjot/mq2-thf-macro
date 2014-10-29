|----------------------------------------------------------------------------|
|- e3_Classes_Rogue.inc v2.2 ( originally designed to work with e3.mac )	-|
|- Written by Conradd of THF from Killians of PEQ												-|
|----------------------------------------------------------------------------|
|- Contains Rogue specific functions related to,							-|
|-		Setup and Ini entry information										-|
|-		MainLoop, and AssistLoop hooks										-|
|-		Over-all functionality / flow										-|
|----------------------------------------------------------------------------|



|--------------------------------------------------------------------|
|- Rogue's Class specific mainLoop hook.							-|
|--------------------------------------------------------------------|
|- Prioritizes macro functions based on the Rogue class dynamic.	-|
|--------------------------------------------------------------------|
SUB ROG_Main
/if (${Debug}) /echo |- ROG_Main ==>

	| checkCombatBuffs.
	/if (!${medBreak} && !${idle} && ${CombatBuffs.Size}) /call CombatBuffs
	/if (!${actionTaken}) {
	
		| Assist Functions.
		/if (${assisting}) /call ${Me.Class.ShortName}_Assist
	}

/if (${Debug}) /echo <== ROG_Main -|
/RETURN



|--------------------------------------------------------------------|
|- Rogue's Class specific Assist Loop.								-|
|--------------------------------------------------------------------|
|- Prioritizes Assist functions based on the Rogue class dynamic.	-|
|--------------------------------------------------------------------|
SUB ROG_Assist
/if (${Debug}) /echo |- ROG_Assist ==>

	/call e3_AssistFunctions

/if (${Debug}) /echo <== ROG_Assist -|
/RETURN



SUB ROG_Setup
/if (${Debug}) /echo |- ROG_Setup ==>
	
/if (${Debug}) /echo <== ROG_Setup -|
/RETURN



SUB ROG_backgroundEvents
/if (${Debug}) /echo |- ROG_backgroundEvents ==>

/if (${Debug}) /echo <== ROG_backgroundEvents -|
/RETURN



SUB ROG_MacroSettings
/if (${Debug}) /echo |- ROG_MacroSettings ==>

/if (${Debug}) /echo <== ROG_MacroSettings -|
/RETURN



SUB ROG_CharacterSettings
/if (${Debug}) /echo |- ROG_CharacterSettings ==>

/if (${Debug}) /echo <== ROG_CharacterSettings -|
/RETURN



SUB ROG_Aliases
/if (${Debug}) /echo |- ROG_Aliases ==>

/if (${Debug}) /echo <== ROG_Aliases -|
/RETURN