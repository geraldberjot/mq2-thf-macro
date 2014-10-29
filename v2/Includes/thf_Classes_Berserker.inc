|----------------------------------------------------------------------------|
|- e3_Classes_CLR.inc v2.2 ( originally designed to work with e3.mac )	-|
|- Written by Conradd of THF from Killians of PEQ												-|
|----------------------------------------------------------------------------|
|- Contains CLR specific functions related to,							-|
|-		Setup and Ini entry information										-|
|-		MainLoop, and AssistLoop hooks										-|
|-		Over-all functionality / flow										-|
|----------------------------------------------------------------------------|



|--------------------------------------------------------------------|
|- CLR's Class specific mainLoop hook.							-|
|--------------------------------------------------------------------|
|- Prioritizes macro functions based on the CLR class dynamic.	-|
|--------------------------------------------------------------------|
SUB BER_Main
/if (${Debug}) /echo |- BER_Main ==>

	| checkCombatBuffs.
	/if (!${medBreak} && !${idle} && ${CombatBuffs.Size}) /call CombatBuffs
	/if (!${actionTaken}) {
	
		| Assist Functions.
		/if (${assisting}) /call ${Me.Class.ShortName}_Assist
	}

/if (${Debug}) /echo <== BER_Main -|
/RETURN



|--------------------------------------------------------------------|
|- CLR's Class specific Assist Loop.								-|
|--------------------------------------------------------------------|
|- Prioritizes Assist functions based on the CLR class dynamic.	-|
|--------------------------------------------------------------------|
SUB BER_Assist
/if (${Debug}) /echo |- BER_Assist ==>

	/call e3_AssistFunctions

/if (${Debug}) /echo <== BER_Assist -|
/RETURN



SUB BER_Setup
/if (${Debug}) /echo |- BER_Setup ==>
	
/if (${Debug}) /echo <== BER_Setup -|
/RETURN



SUB BER_backgroundEvents
/if (${Debug}) /echo |- BER_backgroundEvents ==>

/if (${Debug}) /echo <== BER_backgroundEvents -|
/RETURN



SUB BER_MacroSettings
/if (${Debug}) /echo |- BER_MacroSettings ==>

/if (${Debug}) /echo <== BER_MacroSettings -|
/RETURN



SUB BER_CharacterSettings
/if (${Debug}) /echo |- BER_CharacterSettings ==>

/if (${Debug}) /echo <== BER_CharacterSettings -|
/RETURN



SUB BER_Aliases
/if (${Debug}) /echo |- BER_Aliases ==>

/if (${Debug}) /echo <== BER_Aliases -|
/RETURN