|----------------------------------------------------------------------------|
|- e3_Classes_Magician.inc v2.2 ( originally designed to work with e3.mac )	-|
|- Written by Conradd of THF from Killians of PEQ												-|
|----------------------------------------------------------------------------|
|- Contains Magician specific functions related to,							-|
|-		Setup and Ini entry information										-|
|-		MainLoop, and AssistLoop hooks										-|
|-		Over-all functionality / flow										-|
|----------------------------------------------------------------------------|



|--------------------------------------------------------------------|
|- Magician's Class specific mainLoop hook.							-|
|--------------------------------------------------------------------|
|- Prioritizes macro functions based on the Magician class dynamic.	-|
|--------------------------------------------------------------------|
SUB MAG_Main
/if (${Debug}) /echo |- MAG_Main ==>

	| checkCombatBuffs.
	/if (!${medBreak} && !${idle} && ${CombatBuffs.Size}) /call CombatBuffs
	/if (!${actionTaken}) {
	
		| Assist Functions.
		/if (${assisting}) /call ${Me.Class.ShortName}_Assist
	}

/if (${Debug}) /echo <== MAG_Main -|
/RETURN



|--------------------------------------------------------------------|
|- Magician's Class specific Assist Loop.								-|
|--------------------------------------------------------------------|
|- Prioritizes Assist functions based on the Magician class dynamic.	-|
|--------------------------------------------------------------------|
SUB MAG_Assist
/if (${Debug}) /echo |- MAG_Assist ==>

	/call e3_AssistFunctions

/if (${Debug}) /echo <== MAG_Assist -|
/RETURN



SUB MAG_Setup
/if (${Debug}) /echo |- MAG_Setup ==>
	
/if (${Debug}) /echo <== MAG_Setup -|
/RETURN



SUB MAG_backgroundEvents
/if (${Debug}) /echo |- MAG_backgroundEvents ==>

/if (${Debug}) /echo <== MAG_backgroundEvents -|
/RETURN



SUB MAG_MacroSettings
/if (${Debug}) /echo |- MAG_MacroSettings ==>

/if (${Debug}) /echo <== MAG_MacroSettings -|
/RETURN



SUB MAG_CharacterSettings
/if (${Debug}) /echo |- MAG_CharacterSettings ==>

/if (${Debug}) /echo <== MAG_CharacterSettings -|
/RETURN



SUB MAG_Aliases
/if (${Debug}) /echo |- MAG_Aliases ==>

/if (${Debug}) /echo <== MAG_Aliases -|
/RETURN