|----------------------------------------------------------------------------|
|- e3_Classes_Wizard.inc v2.2 ( originally designed to work with e3.mac )	-|
|- Written by Conradd of THF from Killians of PEQ												-|
|----------------------------------------------------------------------------|
|- Contains Wizard specific functions related to,							-|
|-		Setup and Ini entry information										-|
|-		MainLoop, and AssistLoop hooks										-|
|-		Over-all functionality / flow										-|
|----------------------------------------------------------------------------|



|--------------------------------------------------------------------|
|- Wizard's Class specific mainLoop hook.							-|
|--------------------------------------------------------------------|
|- Prioritizes macro functions based on the Wizard class dynamic.	-|
|--------------------------------------------------------------------|
SUB WIZ_Main
/if (${Debug}) /echo |- WIZ_Main ==>

	| checkCombatBuffs.
	/if (!${medBreak} && !${idle} && ${CombatBuffs.Size}) /call CombatBuffs
	/if (!${actionTaken}) {
	
		| Assist Functions.
		/if (${assisting}) /call ${Me.Class.ShortName}_Assist
	}

/if (${Debug}) /echo <== WIZ_Main -|
/RETURN



|--------------------------------------------------------------------|
|- Wizard's Class specific Assist Loop.								-|
|--------------------------------------------------------------------|
|- Prioritizes Assist functions based on the Wizard class dynamic.	-|
|--------------------------------------------------------------------|
SUB WIZ_Assist
/if (${Debug}) /echo |- WIZ_Assist ==>

	/call e3_AssistFunctions

/if (${Debug}) /echo <== WIZ_Assist -|
/RETURN



SUB WIZ_Setup
/if (${Debug}) /echo |- WIZ_Setup ==>
	
/if (${Debug}) /echo <== WIZ_Setup -|
/RETURN



SUB WIZ_backgroundEvents
/if (${Debug}) /echo |- WIZ_backgroundEvents ==>

/if (${Debug}) /echo <== WIZ_backgroundEvents -|
/RETURN



SUB WIZ_MacroSettings
/if (${Debug}) /echo |- WIZ_MacroSettings ==>

/if (${Debug}) /echo <== WIZ_MacroSettings -|
/RETURN



SUB WIZ_CharacterSettings
/if (${Debug}) /echo |- WIZ_CharacterSettings ==>

/if (${Debug}) /echo <== WIZ_CharacterSettings -|
/RETURN



SUB WIZ_Aliases
/if (${Debug}) /echo |- WIZ_Aliases ==>

/if (${Debug}) /echo <== WIZ_Aliases -|
/RETURN