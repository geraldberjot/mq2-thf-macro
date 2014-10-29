|--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
|- thf_Setup.inc
|- Written by Conradd of THF from Killians of PEQ
|--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
|- Contains setup functions for thf.mac
|--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|

|- e3 Core Includes. ==>
#INCLUDE thf_Aliases.inc
#INCLUDE thf_Assists.inc
#INCLUDE thf_Background.inc
#INCLUDE thf_Basics.inc
#INCLUDE thf_BuffCheck.inc
#INCLUDE thf_Casting.inc
#INCLUDE thf_Functions.inc
#INCLUDE thf_Heals.inc
#INCLUDE thf_Help.inc
#INCLUDE thf_Inis.inc
#INCLUDE thf_Pets.inc
#INCLUDE thf_BegForBuffs.inc
#INCLUDE VetAAs.inc

#INCLUDE thf_Classes_Bard.inc
#INCLUDE thf_Classes_Berserker.inc
#INCLUDE thf_Classes_Cleric.inc
#INCLUDE thf_Classes_Druid.inc
#INCLUDE thf_Classes_Enchanter.inc
#INCLUDE thf_Classes_Magician.inc
#INCLUDE thf_Classes_Monk.inc
#INCLUDE thf_Classes_Rogue.inc
#INCLUDE thf_Classes_Shaman.inc
#INCLUDE thf_Classes_Warrior.inc
#INCLUDE thf_Classes_Wizard.inc

|--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
SUB initialize(modeSelect)

	/call checkPlugins

	/echo Initializing...

	|- Create a file to define file structure in the Macros folder.
	/if (!${Ini[e3 File Paths.ini].Length}) {
		/echo Welcome to THF, preforming first time setup...
		/call make_FilePathsIni
	}
	|- Create core settings files.
	/if (!${Ini[e3 File Paths.ini,File Paths,Macro Settings File].Length}) {
		/echo ERROR: Could not find designated file path for [Macro Settings File], please review [e3 File Paths.ini] and restart.
		/endmacro
	} else {
		/declare Macro_Ini string outer ${Ini[e3 File Paths.ini,File Paths,Macro Settings File]}
	}

	|- Generate Macro_Ini.
	/if (!${Ini[${Macro_Ini}].Length}) {
		/call EVENT_updateAliases
		/call make_MacroSettingsIni
	}
	|- Import Character_Ini file path.
	/if (!${Ini[e3 File Paths.ini,File Paths,Bot Settings Files].Length}) {
		/echo ERROR: Could not find designated file path for [Bot Settings Files], please review [e3 File Paths.ini] and restart.
		/endmacro
	} else {
		/declare Character_Ini string outer ${Ini[e3 File Paths.ini,File Paths,Bot Settings Files]}
	}
	
	|- Import Macro Data file path.
	/if (!${Ini[e3 File Paths.ini,File Paths,Macro Data File].Length}) {
		/echo ERROR: Could not find designated file path for [Macro Data File], please review [e3 File Paths.ini] and restart.
		/endmacro
	} else {
		/declare MacroData_Ini string outer ${Ini[e3 File Paths.ini,File Paths,Macro Data File]}
	}

	|- Generate Character_Ini.
	/if (!${Ini[${Character_Ini}].Length}) /call make_CharacterSettingsIni
	
	|- Import core settings.
	/if (${Ini[${Macro_Ini},General,Debug Mode (On/Off)].Length}) /call INItoVar "${Macro_Ini},General,Debug Mode (On/Off)" Debug bool outer

	/declare ChatToggle string outer /bc
	/if (${Ini[${Macro_Ini},General,Macro Output Channel].Length}) /call IniToVar "${Macro_Ini},General,Macro Output Channel" ChatToggle string outer
	
	/if (${Ini[${Macro_Ini},General,Verbosity (On/Off)].Length}) /call IniToVar "${Macro_Ini},General,Verbosity (On/Off)" Verbosity bool outer
	
	|- Setup core functions.
	
	/echo Setting up includes...
	
	/call assistSetup
	/call backgroundSetup
	/call basicsSetup
	/call buffCheckSetup
	/call castingSetup
	/call healSetup
	/call petSetup
	/call ${Me.Class.ShortName}_Setup
	/call begForBuffsSetup
	
	/if (${Ini[${MacroData_Ini},PendingExchanges-${MacroQuest.Server},${Me.CleanName}].Length} && ${Ini[${MacroData_Ini},PendingExchanges-${MacroQuest.Server},${Me.CleanName}].NotEqual[NONE]} && !${itemExchange_Timer}) {
		/echo Completing a pending exchange, [${Ini[${MacroData_Ini},PendingExchanges-${MacroQuest.Server},${Me.CleanName}].Arg[1,/]}] to [${Ini[${MacroData_Ini},PendingExchanges-${MacroQuest.Server},${Me.CleanName}].Arg[2,/]}]...
		/call completePendingExchange
	}
	
	/echo thf loaded.
	
	/if (${Debug}) {
		/echo
		/echo
		/echo *** Debug_Mode ON ***
		/echo The macro has paused so you can review setup information, type /mqpause to continue.
		/echo
		/echo
		
		|- If macro is in DebugMode, pause after setup.
		/mqpause
	}
	
/RETURN

|--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
|- Checks for and enables required macro plugins.
|--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
SUB checkPlugins
	/echo Checking for required plugins...

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
			/if (!${EQBC.Connected}) /echo ***WARNING*** Could not connect to EQBCS, please open EQBCS and try again.  Macro functionality may be limited...
		}

		|- Check MQ2NetBots.
		/if (!${Plugin[MQ2NetBots].Name.Length}) {
			/echo Plugin MQ2NetBots is not loaded, attempting to resolve...
			/plugin MQ2NetBots
			/delay 3s ${NetBots}
			/if (!${NetBots}) {
				/echo ***WARNING*** Could not load MQ2NetBots, macro functionality may be limited.
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
				/echo ***WARNING*** Could not load MQ2NetAdvPath, please ensure you're using a copy of MQ2 which was compiled with the MQ2NetAdvPath plugin and try again.
				/echo Attempting to load MQ2MoveUtils
			}
		}
	}
	
	|- Check MQ2MoveUtils.
	/if (!${Plugin[MQ2MoveUtils].Name.Length}) {
		/echo Plugin MQ2MoveUtils is not loaded, attempting to resolve...
		/plugin MQ2MoveUtils
		/delay 3s !${Select[${Stick.Status},NULL]}
		/if (${Select[${Stick.Status},NULL]}) {
			/echo ***WARNING*** Could not load MQ2MoveUtils, macro functionality may be limited.
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
			/echo ***WARNING*** Could not load MQ2Exchange, macro functionality may be limited.
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
				/echo ***WARNING*** Could not load MQ2Twist, macro functionality may be limited.
				/echo Bard melodies DISABLED.
			}
		}	
	}
	
	|- Check MQ2Cast.
	/if (!${Plugin[MQ2Cast].Name.Length}) {
		/echo Plugin MQ2Cast is not loaded, attempting to resolve...
		/plugin MQ2Cast
		/delay 3s ${Plugin[MQ2Cast].Name.Length}
		/if (!${Plugin[MQ2Cast].Name.Length}) {
			/echo ***ERROR*** Could not load MQ2Cast
			/endmacro
		}
	}

	| <== REQUIRED PLUGINS

	|- Check MQ2ItemDisplay.
	/if (!${Plugin[mq2itemdisplay].Name.Length}) /plugin MQ2ItemDisplay

	|- Check MQ2HUD.
	/if (!${Plugin[mq2hud].Name.Length}) /plugin MQ2HUD

/RETURN