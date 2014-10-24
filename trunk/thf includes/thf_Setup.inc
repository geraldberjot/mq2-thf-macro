|--------------------------------------------------------------------|
|- thf_Setup.inc													-|
|- Written by Conradd of THF										-|
|--------------------------------------------------------------------|
|- Contains setup functions for thf.mac								-|
|--------------------------------------------------------------------|

#include thf includes\ninjadvloot.inc
#include thf includes\wait4Rez.inc
#include thf includes\sub.inc
#include thf includes\events.inc

#include thf includes\thf_Alerts.inc
#include thf includes\thf_Assists.inc

#include thf includes\thf_Bard.inc
#include thf includes\thf_Beastlord.inc
#include thf includes\thf_Berserker.inc
#include thf includes\thf_Cleric.inc
#include thf includes\thf_Druid.inc
#include thf includes\thf_Enchanter.inc
#include thf includes\thf_Magician.inc
#include thf includes\thf_Monk.inc
#include thf includes\thf_Necromancer.inc
#include thf includes\thf_Paladin.inc
#include thf includes\thf_Ranger.inc
#include thf includes\thf_Rogue.inc
#include thf includes\thf_ShadowKnight.inc
#include thf includes\thf_Shaman.inc
#include thf includes\thf_Warrior.inc
#include thf includes\thf_Wizard.inc

|----------------------------------------------------|
|- Checks for and enables required macro plugins.	-|
|----------------------------------------------------|
SUB check_Plugins
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

|----------------------------------------------------|
|- Declare all the files.ini						-|
|----------------------------------------------------|
SUB declareInis
	/declare iniGlobal 	string outer "ini\global.ini"
	/declare iniSetup 	string outer "ini\setup_${Me.CleanName}.ini"
	/declare iniSpell 	string outer "ini\spells_${Me.CleanName}.ini"
	/declare iniHail 	string outer "ini\hail.ini"
/RETURN