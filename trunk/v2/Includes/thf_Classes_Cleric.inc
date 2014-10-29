|----------------------------------------------------------------------------|
|- e3_Classes_Cleric.inc v2.2 ( originally designed to work with e3.mac )	-|
|- Written by Conradd of THF from Killians of PEQ												-|
|----------------------------------------------------------------------------|
|- Contains Cleric specific functions related to,							-|
|-		Setup and Ini entry information										-|
|-		MainLoop, and AssistLoop hooks										-|
|-		Over-all functionality / flow										-|
|----------------------------------------------------------------------------|



|--------------------------------------------------------------------|
|- Cleric's Class specific mainLoop hook.							-|
|--------------------------------------------------------------------|
|- Prioritizes macro functions based on the Cleric class dynamic.	-|
|--------------------------------------------------------------------|
SUB CLR_Main
/if (${Debug}) /echo |- CLR_Main ==>

	| checkCombatBuffs.
	/if (!${medBreak} && !${idle} && ${CombatBuffs.Size}) /call CombatBuffs
	/if (!${actionTaken}) {
	
		| Assist Functions.
		/if (${assisting}) /call ${Me.Class.ShortName}_Assist
	}

/if (${Debug}) /echo <== CLR_Main -|
/RETURN



|--------------------------------------------------------------------|
|- Cleric's Class specific Assist Loop.								-|
|--------------------------------------------------------------------|
|- Prioritizes Assist functions based on the Cleric class dynamic.	-|
|--------------------------------------------------------------------|
SUB CLR_Assist
/if (${Debug}) /echo |- CLR_Assist ==>

	/call e3_AssistFunctions

/if (${Debug}) /echo <== CLR_Assist -|
/RETURN



SUB CLR_Setup
/if (${Debug}) /echo |- CLR_Setup ==>
	
/if (${Debug}) /echo <== CLR_Setup -|
/RETURN



SUB CLR_backgroundEvents
/RETURN



SUB CLR_MacroSettings
/if (${Debug}) /echo |- CLR_MacroSettings ==>

/if (${Debug}) /echo <== CLR_MacroSettings -|
/RETURN



SUB CLR_CharacterSettings
/if (${Debug}) /echo |- CLR_CharacterSettings ==>

/if (${Debug}) /echo <== CLR_CharacterSettings -|
/RETURN



SUB CLR_Aliases
/if (${Debug}) /echo |- CLR_Aliases ==>

/if (${Debug}) /echo <== CLR_Aliases -|
/RETURN