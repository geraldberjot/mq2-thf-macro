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

#include thf_includes\Alerts.inc
#include thf_includes\Assists.inc
#include thf_includes\Background.inc
#include thf_includes\Basics.inc
#include thf_includes\Utilities.inc

#include thf_includes\class_Bard.inc
#include thf_includes\class_Beastlord.inc
#include thf_includes\class_Berserker.inc
#include thf_includes\class_Cleric.inc
#include thf_includes\class_Druid.inc
#include thf_includes\class_Enchanter.inc
#include thf_includes\class_Magician.inc
#include thf_includes\class_Monk.inc
#include thf_includes\class_Necromancer.inc
#include thf_includes\class_Paladin.inc
#include thf_includes\class_Ranger.inc
#include thf_includes\class_Rogue.inc
#include thf_includes\class_ShadowKnight.inc
#include thf_includes\class_Shaman.inc
#include thf_includes\class_Warrior.inc
#include thf_includes\class_Wizard.inc

SUB thfSetup(bool debug)
	/if (${outerDebug}) /echo |- thfSetup ==>

	|-- Debug
	/declare outerDebug bool outer FALSE
	/if (${Bool[${debug}]}) /varset outerDebug TRUE

	|- The file path for thf.ini will still need to be updated in corresponding includes because you must use /noparse to write variables to inis.
	/declare iniTHF string outer thf_inis\Thf.ini
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

	|-- Toon Settings
	/if (!${Ini[${iniTHF},Paths,Toon].Length}) {
		/echo ERROR: Could not find designated file path for [Toon].  Please review review settings in [${iniTHF} > Paths].
		/endmacro
	} else {
		/declare iniToon string outer ${Ini[${iniTHF},Paths,Toon]}
	}
	
	|- Create/Read Ini files
	|-- Macro Setting Inis
	/if (${Bool[${Ini[${iniGeneral}]}]}) {
		/call ReadMacroSettings
	} else {
		/call EVENT_MakeMacroSettingsIni
	}
	
	|-- Generate iniToon.
	/if (!${Bool[${Ini[${iniToon}]}]}) /call EVENT_MakeCharacterSettingsIni
	
	|-- Macro Aliases
	/call EVENT_UpdateAliases

	|- Import ini settings
	/echo Importing user settings...
	
	|- Complete a pending exchange
	/if (${Ini[${iniTHF},${MacroQuest.Server}-${Me.CleanName},Pending Exchange].Length} && ${Ini[${iniTHF},${MacroQuest.Server}-${Me.CleanName},Pending Exchange].NotEqual[NONE]} && !${itemExchange_Timer}) /call completePendingExchange
	
	/call checkToons
	
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
	/if (${outerDebug}) /echo |- makeThfIni ==>

	/noparse /ini "thf_inis\Thf.ini" "Paths" "Toon" "thf_inis\${MacroQuest.Server}-${Me.CleanName}.ini"
	/noparse /ini "thf_inis\Thf.ini" "Paths" "General" "thf_inis\General.ini"

	/if (${outerDebug}) /echo <== makeThfIni -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Launch every subroutine for each array element
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB loadAll(String tableau, String subToCall)
	/if (${outerDebug}) /echo |- makeThfIni ==>

	/echo Calling ${Me.Class.ShortName}_${subToCall}
	/call ${Me.Class.ShortName}_${subToCall}
	/declare i int local
	/for i 1 to ${${tableau}.Size}
		/if (${outerDebug}) /echo Calling ${${tableau}[${i}]}_${subToCall}
		/if (${Bool[${${tableau}[${i}]}]}) /call ${${tableau}[${i}]}_${subToCall}
	/next i

	/if (${outerDebug}) /echo <== makeThfIni -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Generate the general Macro Settings file
|---------------------------------------------------------------------------------------------------------------------------------------------------|
#EVENT MakeMacroSettingsIni "[MQ2] Make Macro ini"
SUB EVENT_MakeMacroSettingsIni()
	/if (${outerDebug}) /echo |- MakeMacroSettingsIni ==>

	/echo Creating macro settings files...

	/call writeToIni "${iniGeneral},General,Channel" team
	/call writeToIni "${iniGeneral},Debug,Debug Mode (On/Off)" Off
	/call writeToIni "${iniGeneral},General,Includes" assist/background/basics
	
	/call split "${iniGeneral},General,Includes" includes outer /
	
	/call loadAll ${includes} "MacroSettings"
	
	/if (${outerDebug}) /echo <== MakeMacroSettingsIni -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Read the general Macro Settings file
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB ReadMacroSettings()
	/if (${outerDebug}) /echo |- ReadMacroSettings ==>

	|-- Chat Channel
	/if (!${Defined[channelNumber]}) /declare channelNumber string outer
	/call iniToVar "${iniGeneral},General,Channel" channel string outer
	
	/call split "${iniGeneral},General,Includes" includes outer /
	
	/call loadAll includes Setup
	
	/if (${outerDebug}) /echo <== ReadMacroSettings -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Generates a general Macro Settings file, to be used by multiple includes.
|---------------------------------------------------------------------------------------------------------------------------------------------------|
#EVENT MakeCharacterSettingsIni "[MQ2] Make bot ini"
SUB EVENT_MakeCharacterSettingsIni()
	/if (${outerDebug}) /echo |- EVENT_MakeCharacterSettingsIni ==>

	/echo Creating ${Me.CleanName}'${If[${Me.CleanName.Right[1].Equal[s]},,s]} settings file...
	
	/call ${Me.Class.ShortName}_CharacterSettings
	/declare i int local
	/for i 1 to ${includes.Size}
		/if (${Bool[${includes[${i}]}]}) /call ${includes[${i}]}_CharacterSettings
	/next i
	
	/if (${outerDebug}) /echo <== EVENT_MakeCharacterSettingsIni -|
/RETURN	

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Deletes and Recreates aliases associated with thf.mac.
|---------------------------------------------------------------------------------------------------------------------------------------------------|
#EVENT UpdateAliases "[MQ2] Update macro aliases"
SUB EVENT_UpdateAliases()
	/if (${outerDebug}) /echo |- EVENT_UpdateAliases ==>

	/echo Updating macro aliases....
	/echo (WARNING: Your clients will lock up for a few moments.  Stand fast, friend...)
	/echo Aliases may not work properly until you restart MQ2 on ALL clients.
	
	| Core aliases
	/squelch /alias /updateAliases /echo Update macro aliases
	/squelch /alias /makeCharIni /echo Make Character Ini
	/squelch /alias /makeSettingsIni /echo Make General Settings Ini
	/squelch /alias /makeAdvSettingsIni /echo Make Advanced Settings Ini
	/squelch /alias /thfHelp /echo Help
	/squelch /alias /debugon /varset Debug 1
	/squelch /alias /debugoff /varset Debug 0

	/call IniToArray "${iniAdvanced},Includes,Include#" macroSetups
	
	/declare i int local
	
	/for i 1 to ${macroSetups.Size}
		/if (${Bool[${macroSetups[${i}]}]}) /call ${macroSetups[${i}]}_Aliases
	/next i
	
	|/call VetAAs_Aliases

	/if (${outerDebug}) /echo <== EVENT_UpdateAliases -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Checks for and enables required macro plugins.
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB checkPlugins()
	/echo Checking plugins...

	|- REQUIRED PLUGINS ==>
	
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

