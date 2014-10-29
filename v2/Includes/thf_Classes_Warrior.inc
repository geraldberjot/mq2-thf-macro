|----------------------------------------------------------------------------|
|- e3_Classes_Warrior.inc v2.2 ( originally designed to work with e3.mac )	-|
|- Written by Conradd of THF from Killians of PEQ												-|
|----------------------------------------------------------------------------|
|- Contains Warrior specific functions related to,							-|
|-		Setup and Ini entry information										-|
|-		MainLoop, and AssistLoop hooks										-|
|-		Over-all functionality / flow										-|
|----------------------------------------------------------------------------|



|--------------------------------------------------------------------|
|- Warrior's Class specific mainLoop hook.							-|
|--------------------------------------------------------------------|
|- Prioritizes macro functions based on the Warrior class dynamic.	-|
|--------------------------------------------------------------------|
SUB WAR_Main
/if (${Debug}) /echo |- WAR_Main ==>

	| checkCombatBuffs.
	/if (!${medBreak} && !${idle} && ${CombatBuffs.Size}) /call CombatBuffs
	/if (!${actionTaken}) {
	
		| Assist Functions.
		/if (${assisting}) /call ${Me.Class.ShortName}_Assist
	}

/if (${Debug}) /echo <== WAR_Main -|
/RETURN



|--------------------------------------------------------------------|
|- Warrior's Class specific Assist Loop.								-|
|--------------------------------------------------------------------|
|- Prioritizes Assist functions based on the Warrior class dynamic.	-|
|--------------------------------------------------------------------|
SUB WAR_Assist
/if (${Debug}) /echo |- WAR_Assist ==>

	/call e3_AssistFunctions

/if (${Debug}) /echo <== WAR_Assist -|
/RETURN



SUB WAR_Setup
/if (${Debug}) /echo |- WAR_Setup ==>
	
/if (${Debug}) /echo <== WAR_Setup -|
/RETURN



SUB WAR_backgroundEvents
/if (${Debug}) /echo |- WAR_backgroundEvents ==>

/if (${Debug}) /echo <== WAR_backgroundEvents -|
/RETURN



SUB WAR_MacroSettings
/if (${Debug}) /echo |- WAR_MacroSettings ==>

/if (${Debug}) /echo <== WAR_MacroSettings -|
/RETURN



SUB WAR_CharacterSettings
/if (${Debug}) /echo |- WAR_CharacterSettings ==>

/if (${Debug}) /echo <== WAR_CharacterSettings -|
/RETURN



SUB WAR_Aliases
/if (${Debug}) /echo |- WAR_Aliases ==>

/if (${Debug}) /echo <== WAR_Aliases -|
/RETURN