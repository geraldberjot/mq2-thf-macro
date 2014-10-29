|------------------------------------------------------------------------|
|- e3_Classes_Monk.inc v2.2 ( originally designed to work with e3.mac )	-|
|- Written by Conradd of THF from Killians of PEQ											-|
|------------------------------------------------------------------------|
|- Contains Monk specific functions related to,							-|
|-		Setup and Ini entry information									-|
|-		MainLoop, and AssistLoop hooks									-|
|-		Over-all functionality / flow									-|
|------------------------------------------------------------------------|



|----------------------------------------------------------------|
|- Monk's Class specific mainLoop hook.							-|
|----------------------------------------------------------------|
|- Prioritizes macro functions based on the Monk class dynamic.	-|
|----------------------------------------------------------------|
SUB MNK_Main
/if (${Debug}) /echo |- MNK_Main ==>

	| checkCombatBuffs.
	/if (!${medBreak} && !${idle} && ${CombatBuffs.Size}) /call CombatBuffs
	/if (!${actionTaken}) {
	
		| Assist Functions.
		/if (${assisting}) /call ${Me.Class.ShortName}_Assist
	}

/if (${Debug}) /echo <== MNK_Main -|
/RETURN



|--------------------------------------------------------------------|
|- Monk's Class specific Assist Loop.								-|
|--------------------------------------------------------------------|
|- Prioritizes Assist functions based on the Monk class dynamic.	-|
|--------------------------------------------------------------------|
SUB MNK_Assist
/if (${Debug}) /echo |- MNK_Assist ==>

	/call e3_AssistFunctions

/if (${Debug}) /echo <== MNK_Assist -|
/RETURN



SUB MNK_Setup
/if (${Debug}) /echo |- MNK_Setup ==>
	
/if (${Debug}) /echo <== MNK_Setup -|
/RETURN



SUB MNK_backgroundEvents
/if (${Debug}) /echo |- MNK_backgroundEvents ==>

/if (${Debug}) /echo <== MNK_backgroundEvents -|
/RETURN



SUB MNK_MacroSettings
/if (${Debug}) /echo |- MNK_MacroSettings ==>

/if (${Debug}) /echo <== MNK_MacroSettings -|
/RETURN



SUB MNK_CharacterSettings
/if (${Debug}) /echo |- MNK_CharacterSettings ==>

/if (${Debug}) /echo <== MNK_CharacterSettings -|
/RETURN



SUB MNK_Aliases
/if (${Debug}) /echo |- MNK_Aliases ==>

/if (${Debug}) /echo <== MNK_Aliases -|
/RETURN