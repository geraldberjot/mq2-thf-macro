|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- thf_Setup.inc
|- Written by Conradd of THF
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Contains setup functions for thf.mac
|---------------------------------------------------------------------------------------------------------------------------------------------------|

#include thf_includes\ninjadvloot.inc
#include thf_includes\wait4Rez.inc
#include thf_includes\sub.inc
#include thf_includes\events.inc

#include thf_includes\thf_Alerts.inc
#include thf_includes\thf_Assists.inc
#include thf_includes\thf_Background.inc

#include thf_includes\thf_Bard.inc
#include thf_includes\thf_Beastlord.inc
#include thf_includes\thf_Berserker.inc
#include thf_includes\thf_Cleric.inc
#include thf_includes\thf_Druid.inc
#include thf_includes\thf_Enchanter.inc
#include thf_includes\thf_Magician.inc
#include thf_includes\thf_Monk.inc
#include thf_includes\thf_Necromancer.inc
#include thf_includes\thf_Paladin.inc
#include thf_includes\thf_Ranger.inc
#include thf_includes\thf_Rogue.inc
#include thf_includes\thf_ShadowKnight.inc
#include thf_includes\thf_Shaman.inc
#include thf_includes\thf_Warrior.inc
#include thf_includes\thf_Wizard.inc

SUB thfSetup(bool debug)
	/if (${outerDebug}) /echo |- thfSetup ==>

	/declare outerDebug bool outer FALSE
	/if (${Bool[${debug}]}) /varset outerDebug TRUE

	|- The file path for thf.ini will still need to be updated in corresponding includes because you must use /noparse to write variables to inis.
	/declare iniTHF string outer thf_inis\thf.ini
	/declare i int local

	/call checkPlugins
	
	/echo Loading THF macro...

	|- create a macro data
	/if (!${Ini[${iniTHF}].Length}) {
		/echo Welcome to THF ! performing first time setup...
		/call makeThfIni
	}

	|- Import Paths
	|-- General Settings
	/if (!${Ini[${iniTHF},Paths,General].Length}) {
		/echo ERROR: Could not find designated file path for [General].  Please review review settings in [${iniTHF} > Paths].
		/endmacro
	} else {
		/declare iniGeneral string outer ${Ini[${iniTHF},Paths,General]}
	}

	|-- Advanced Settings
	/if (!${Ini[${iniTHF},Paths,Advanced].Length}) {
		/echo ERROR: Could not find designated file path for [Advanced].  Please review review settings in [${iniTHF} > Paths].
		/endmacro
	} else {
		/declare iniAdvanced string outer ${Ini[${iniTHF},Paths,Advanced]}
	}
	
	|-- Toon Settings
	/if (!${Ini[${iniTHF},Paths,Toon].Length}) {
		/echo ERROR: Could not find designated file path for [Toon].  Please review review settings in [${iniTHF} > Paths].
		/endmacro
	} else {
		/declare iniToon string outer ${Ini[${iniTHF},Paths,Toon]}
	}
	
	|- Create Ini files
	|-- Macro Setting Inis
	/if (!${Bool[${Ini[${iniGeneral}]}]}) /call EVENT_MakeMacroSettingsIni
	
	|-- Generate iniToon.
	/if (!${Bool[${Ini[${iniToon}]}]}) /call EVENT_MakeCharacterSettingsIni
	
	|-- Macro Aliases
	/call EVENT_UpdateAliases

	|- Import ini settings
	/echo Importing user settings...
	
	|-- Debug
	/if (!${modeSelect.Equal[Debug]}) /call iniToVar "${iniAdvanced},Debug,Debug Mode (On/Off)" Debug bool outer
	
	|-- Chat Channel
	/declare ChatToggle string outer /bc
	/if (${Ini[${Macro_Ini},General,Macro Output Channel].Length}) /call iniToVar "${Macro_Ini},General,Macro Output Channel" ChatToggle string outer

	/call IniToArray "${iniAdvanced},Includes,Include#" macroSetups
	/call IniToArray "${iniAdvanced},${Me.Class.ShortName} Functions,${Me.Class.ShortName} Function#" mainLoop_Array
	
	|- Setup Includes
	/for i 1 to ${macroSetups.Size}
		/if (${macroSetups[${i}].Equal[class]}) /varset macroSetups[${i}] ${Me.Class.ShortName}
		/if (${Bool[${macroSetups[${i}]}]}) /call ${macroSetups[${i}]}_Setup
	/next i

	|- Complete a pending exchange
	/if (${Ini[${iniTHF},${MacroQuest.Server}-${Me.CleanName},Pending Exchange].Length} && ${Ini[${iniTHF},${MacroQuest.Server}-${Me.CleanName},Pending Exchange].NotEqual[NONE]} && !${itemExchange_Timer}) /call completePendingExchange
	
	/echo THF loaded !
	/echo
	
	/if (${outerDebug}) {
		/echo
		/echo
		/echo *** Debug_Mode ON ***
		/echo The macro has paused so you can review setup information, type /mqpause to continue.
		/echo
		/echo
		/mqpause
	}

	/if (${outerDebug}) /echo <== thfSetup -|	
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Creates a file settings file to change Paths, and store various pieces of data
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB makeThfIni()
	/if (${Debug}) /echo |- makeThfIni ==>

	/noparse /ini "thf_inis\thf.ini" "Paths" "Toon" "thf_inis\${MacroQuest.Server}-${Me.CleanName}.ini"
	/noparse /ini "thf_inis\thf.ini" "Paths" "General" "thf_inis\General.ini"
	/noparse /ini "thf_inis\thf.ini" "Paths" "Advanced" "thf_inis\Advanced.ini"

	/if (${Debug}) /echo <== makeThfIni -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Generates a general Macro Settings file, to be used by multiple includes.
|---------------------------------------------------------------------------------------------------------------------------------------------------|
#EVENT MakeMacroSettingsIni "[MQ2] Make Macro ini"
SUB EVENT_MakeMacroSettingsIni()
	/if (${Debug}) /echo |- MakeMacroSettingsIni ==>

	/echo Creating macro settings files...

	/call writeToIni "${iniGeneral},General,Macro Output Channel" /bc
	
	/call writeToIni "${iniAdvanced},Debug,Debug Mode (On/Off)" Off
	/call writeToIni "${iniAdvanced},Includes,Include#1" background 1
	/call writeToIni "${iniAdvanced},Includes,Include#2" basics 1
	/call writeToIni "${iniAdvanced},Includes,Include#3" casting 1
	/call writeToIni "${iniAdvanced},Includes,Include#4" begForBuffs 1
	/call writeToIni "${iniAdvanced},Includes,Include#5" buffCheck 1
	/call writeToIni "${iniAdvanced},Includes,Include#6" buy 1
	/call writeToIni "${iniAdvanced},Includes,Include#7" pet 1
	/call writeToIni "${iniAdvanced},Includes,Include#8" heal 1
	/call writeToIni "${iniAdvanced},Includes,Include#9" healChain 1
	/call writeToIni "${iniAdvanced},Includes,Include#10" assist 1
	/call writeToIni "${iniAdvanced},Includes,Include#11" class 1
	/call writeToIni "${iniAdvanced},Includes,Include#12" corpseSummon 1
	/call writeToIni "${iniAdvanced},Includes,Include#13" loot 1
	/call writeToIni "${iniAdvanced},Includes,Include#14" sell 1
	/call writeToIni "${iniAdvanced},Includes,Include#15" smoothMoves 1
	/call writeToIni "${iniAdvanced},Includes,Include#16" Wait4Rez 1
	
	/delay 15
	
	| Import the array of Includes
	/call IniToArray "${iniAdvanced},Includes,Include#" macroSetups
	
	| Call each setup
	/declare i int local
	
	/for i 1 to ${macroSetups.Size}
		/if (${macroSetups[${i}].Equal[class]}) /varset macroSetups[${i}] ${Me.Class.ShortName}
		/if (${Bool[${macroSetups[${i}]}]}) /call ${macroSetups[${i}]}_MacroSettings
	/next i
	
	/if (${Debug}) /echo <== MakeMacroSettingsIni -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Checks for and enables required macro plugins.
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB checkPlugins()
	/echo Checking plugins...

	| REQUIRED PLUGINS ==>
	
	|- Check MQ2EQBC.
	/if (!${Plugin[MQ2EQBC].Name.Length}) {
		/echo Plugin MQ2EQBC is not loaded, attempting to resolve...
		/plugin MQ2EQBC
		/delay 10s !${Select[${EQBC},NULL]}
		/if (${Select[${EQBC},NULL]}) {
			/echo ***WARNING*** Could not load MQ2EQBC, macro functionality may be limited.
			/echo Starting in Solo Mode...
		}
	}

	|- Auto-Connect to EQBCS.
	/if (${Bool[${EQBC}]}) {
		/if (!${EQBC.Connected}) {
			/bccmd connect
			/delay 100 ${EQBC.Connected}
			/delay 5
			/if (!${EQBC.Connected}) /echo ***WARNING*** Could not connect to EQBCS! Please open EQBCS and try again.  Macro functionality may be limited...
		}


		|- Check MQ2NetBots.
		/if (!${Plugin[MQ2NetBots].Name.Length}) {
			/echo Plugin MQ2NetBots is not loaded, attempting to resolve...
			/plugin MQ2NetBots
			/delay 3s ${NetBots}
			/if (!${NetBots}) {
				/echo ***WARNING*** Could not load MQ2NetBots! Macro functionality may be limited.
			}
		}
		
		|- Auto-Connect to NetBots.
		/if (${NetBots}) {
			/squelch /netbots on grab=on send=on
		}
		
		|- Check MQ2NetAdvPath.
		/if (!${Plugin[MQ2NetAdvPath].Name.Length}) {
			/echo Plugin MQ2NetAdvPath is not loaded, attempting to resolve...
			/plugin MQ2NetAdvPath
			/delay 3s ${NetAdvPath}
			/if (!${NetAdvPath}) {
				/echo ***WARNING*** Could not load MQ2NetAdvPath. Please ensure you're using a copy of MQ2 which was compiled with the MQ2NetAdvPath plugin and try again.
			}
		}
	}
	
	|- Check MQ2MoveUtils.
	/if (!${Plugin[MQ2MoveUtils].Name.Length}) {
		/echo Plugin MQ2MoveUtils is not loaded, attempting to resolve...
		/plugin MQ2MoveUtils
		/delay 3s !${Select[${Stick.Status},NULL]}
		/if (${Select[${Stick.Status},NULL]}) {
			/echo ***WARNING*** Could not load MQ2MoveUtils! Macro functionality may be limited.
			/if (!${NetAdvPath}) {
				/echo Follow and Assist stick DISABLED.
			} else {
				/echo Assist and NPC stick DISABLED.
				/echo Follow restricted to NetBots.
			}
		}
	}	

	|- Check MQ2Exchange.
	/if (!${Plugin[MQ2Exchange].Name.Length}) {
		/echo Plugin MQ2Exchange is not loaded, attempting to resolve...
		/plugin MQ2Exchange
		/delay 3s ${Plugin[MQ2Exchange].Name.Length}
		/if (!${Plugin[MQ2Exchange].Name.Length}) {
			/echo ***WARNING*** Could not load MQ2Exchange! Macro functionality may be limited.
			/echo Item swapping is DISABLED.
		}
	}

	|- Check MQ2Twist.
	/if (${Select[${Me.Class.ShortName},BRD]}) {
		/if (!${Plugin[MQ2Twist].Name.Length}) {
			/echo Plugin Unable to detect MQ2Twist, attempting to resolve...
			/plugin MQ2Twist
			/delay 3s !${Select[${Twist},NULL]}
			/if (${Select[${Twist},NULL]}) {
				/echo ***WARNING*** Could not load MQ2Twist! Macro functionality may be limited.
				/echo Bard melodies DISABLED.
			}
		}	
	}

	|- Check MQ2ItemDisplay.
	/if (!${Plugin[mq2itemdisplay].Name.Length}) /plugin MQ2ItemDisplay

	|- Check MQ2HUD.
	/if (!${Plugin[mq2hud].Name.Length}) /plugin MQ2HUD	
	
	| <== REQUIRED PLUGINS
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Generates a general Macro Settings file, to be used by multiple includes.
|---------------------------------------------------------------------------------------------------------------------------------------------------|
#EVENT MakeCharacterSettingsIni "[MQ2] Make bot ini"
SUB EVENT_MakeCharacterSettingsIni()
	/if (${Debug}) /echo |- EVENT_MakeCharacterSettingsIni ==>

	/echo Creating ${Me.CleanName}'${If[${Me.CleanName.Right[1].Equal[s]},,s]} settings file...
	
	/call IniToArray "${iniAdvanced},Includes,Include#" macroSetups
	
	/declare i int local
	
	/for i 1 to ${macroSetups.Size}
		/if (${macroSetups[${i}].Equal[class]}) /varset macroSetups[${i}] ${Me.Class.ShortName}
		/if (${Bool[${macroSetups[${i}]}]}) /call ${macroSetups[${i}]}_CharacterSettings
	/next i
	
	/if (${Debug}) /echo <== EVENT_MakeCharacterSettingsIni -|
/RETURN	

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Deletes and Recreates aliases associated with e3.mac.
|---------------------------------------------------------------------------------------------------------------------------------------------------|
#EVENT UpdateAliases "[MQ2] Update macro aliases"
SUB EVENT_UpdateAliases()
	/if (${Debug}) /echo |- EVENT_UpdateAliases ==>

	/echo Updating macro aliases....
	/echo (WARNING: Your clients will lock up for a few moments.  Stand fast, friend...)
	/echo Aliases may not work properly until you restart MQ2 on ALL clients.
	
	| Core aliases
	/squelch /alias /updateAliases /echo Update macro aliases
	/squelch /alias /makeCharIni /echo Make Character Ini
	/squelch /alias /makeSettingsIni /echo Make General Settings Ini
	/squelch /alias /makeAdvSettingsIni /echo Make Advanced Settings Ini
	/squelch /alias /e3help /echo Help
	/squelch /alias /debugon /varset Debug 1
	/squelch /alias /debugoff /varset Debug 0

	/call IniToArray "${iniAdvanced},Includes,Include#" macroSetups
	
	/declare i int local
	
	/for i 1 to ${macroSetups.Size}
		/if (${macroSetups[${i}].Equal[class]}) /varset macroSetups[${i}] ${Me.Class.ShortName}
		/if (${Bool[${macroSetups[${i}]}]}) /call ${macroSetups[${i}]}_Aliases
	/next i
	
	/call VetAAs_Aliases

	/if (${Debug}) /echo <== EVENT_UpdateAliases -|
/RETURN
