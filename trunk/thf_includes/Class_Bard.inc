|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- thf_Bard.inc v1.0													-|
|- Adapted for THF by Conradd (macro from Killians of PEQ)											-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Contains Bard specific functions related to,							-|
|-		Setup and Ini entry information									-|
|-		MainLoop, and AssistLoop hooks									-|
|-		Over-all functionality / flow									-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|

#EVENT saveMelody "<#1#> SaveMelody #2# #3#"
#EVENT saveMelody "<#1#> Save Melody #2# #3#"
#EVENT saveMelody "[MQ2] SaveMelody #2# #3#"
#EVENT saveMelody "[MQ2] Save Melody #2# #3#"
SUB EVENT_saveMelody(line, ChatSender, melodyName, songsToSave)
/if (${outerDebug}) /echo |- EVENT_saveMelody ==>

	|- If no ChatSender is defined, make one.	
	/if (!${Defined[ChatSender]}) /declare ChatSender string local ${Me.CleanName}

	|- If I'm the ChatSender.
	/if (${ChatSender.Equal[${Me.CleanName}]}) {

		/declare i int local

		|- Check if the melody already exists, and should not be overwritten.
		/if (${Ini[${iniToon},${melodyName} Melody].Length}) {	
			/if (${outerDebug}) /echo |- EVENT_saveMelody -|- Overwriting [${melodyName}] melody...
			/for i 1 to 5
				/if (${Ini[${iniToon},${melodyName} Melody,Song#${i}].Length}) /call writeToIni "${iniToon},${melodyName} Melody,Song#${i}" *PLACEHOLDER 1
			/next i
		}
		
		/echo Saving [${melodyName}] melody...
		
		/varset i 1
	
		:check_nextSong

		|- If the gem number is not recognized
		/if (!${Int[${songsToSave.Arg[${i}, ]}]} || ${Int[${songsToSave.Arg[${i}, ]}]} < 1 || ${Int[${songsToSave.Arg[${i}, ]}]} > 9) {
			/if (${songsToSave.Arg[${i}, ].NotEqual[-Overwrite]}) {
				/echo [${songsToSave.Arg[${i}, ]}] is not a valid gem number.  Please use the globalFollowing syntax when saving melodies, "Save Melody [NAME] [# # # # #] (-Overwrite)".  Ex.  /echo Save Melody Raid 1 2 3 4 5 -Overwrite
			}
		} else {
			/echo Song#${i}=${Me.Gem[${songsToSave.Arg[${i}, ]}]}/gem|${songsToSave.Arg[${i}, ]}
			/call writeToIni "${iniToon},${melodyName} Melody,Song#${i}" "${Me.Gem[${songsToSave.Arg[${i}, ]}]}/gem|${songsToSave.Arg[${i}, ]}" ${If[${Ini[${iniToon},${melodyName} Melody,Song#${i}].Length},1,0]}
		}
		/varcalc i ${i} + 1
		/if (${songsToSave.Arg[${i}, ].Length}) /goto :check_nextSong
	}

/if (${outerDebug}) /echo <== EVENT_saveMelody -|
/RETURN

#EVENT startMelody "<#1#> Melody #2#"
#EVENT startMelody "[#1#] Melody #2#"
SUB EVENT_startMelody(String line, String from, melodyName)
	/if (${outerDebug}) /echo |- EVENT_startMelody ==>

	|- Verify the event
	/call amIConcerned ${from} ${Me.CleanName} FALSE
	/if (${Macro.Return}) {
		/if (${melodyName.Find[ /]}) /varset melodyName ${melodyName.Arg[1, /]}
		|- If I don't have the melodyName listed in my ini.
		/if (!${Ini[${iniToon},${melodyName} Melody,Song#1].Length}) {
			
			|- If Stop, end, or pause were used, stop the melody.
			/if (${Select[${melodyName},Stop,End,Pause]}) {
				/varset playingMelody FALSE
				/varset songSet
				/channel Ending melody.
				/twist end
			} else {
				/channel [${melodyName}] is not a known melody.
			}
		} else {
		
			/declare songList string local
			/declare retryTimer timer local 100
			/declare spellName string local
			/declare i int local
			
			|- For 1 to 5 (maximum melody size)
			/for i 1 to 5
			
			/if (${Ini[${iniToon},${melodyName} Melody,Song#${i}].Length} && ${Ini[${iniToon},${melodyName} Melody,Song#${i}].NotEqual[PLACEHOLDER]}) {
				
				/varset spellName ${Ini[${iniToon},${melodyName} Melody,Song#${i}].Arg[1,/]}
				
				|- Check that the song is in my book
				/if (!${Me.Book[${spellName}]}) {	
					/echo I don't have [${spellName}] in my spellbook, playing the melody without it.
				} else {
				
					|- If the song is not memmed, use thf_Castinging.inc functions to mem it.
					/if (!${Me.Gem[${spellName}]}) {
						/call argueString gem "${Ini[${iniToon},${melodyName} Melody,Song#${i}]}"
						/varset pendingCast ${spellName}
						/varset giveUpTimer 100
						/call memSpell "${Macro.Return}"
					}
					
					|- Update songList with the corresponding gem#
					/varset songList ${songList} ${Me.Gem[${spellName}]}
				}
				/next i
			}
			
			|- If I have a songList, start singing the melody
			/if (${Bool[${songList}]}) {
				
				|- Twist the newly built songList.
				/varset playingMelody TRUE
				/varset songSet ${songList}
				/channel Playing [${melodyName}] melody...
				/twist ${songSet}
			}
		}
	}
	
/if (${outerDebug}) /echo <== EVENT_startMelody -|
/RETURN



SUB restart_Melody
/if (${outerDebug}) /echo |- check_Melody ==>

	/if (!${Me.Casting.ID}) /twist ${Twist.List}

/if (${outerDebug}) /echo <== check_Melody -|
/RETURN



SUB BRD_Setup
/if (${outerDebug}) /echo |- BRD_Setup ==>

	/declare returnTwist bool outer 0
	
	/if (!${Defined[playingMelody]}) /declare playingMelody bool global
	/if (!${Defined[songSet]}) /declare songSet string global
	
	/call iniToVar "${iniToon},Bard,Auto-Melody" autoMelody string outer
	
	|- Start Auto-Melody at start-up.  Currently disabled to check auto start in check_Zone sub.
	|- /if (${Defined[autoMelody]} && !${songSet.Length}) /startmelody ${autoMelody} /only|ME
	
/if (${outerDebug}) /echo <== BRD_Setup -|
/RETURN



SUB BRD_checkBackgroundEvents
	
	/doevents saveMelody
	/doevents startMelody
	
	|- Check melody
	/if (${playingMelody} && !${Twist.Twisting} && !${Bool[${Me.Casting}]}) /call restart_Melody
	
	|- Check Auto-Melody
	|- The call for Auto-Melody is located in Background.inc -> check_Zone.  This is so Auto-Melody will only be checked when you zone, or load the macro.

/RETURN



SUB BRD_MacroSettings
/if (${outerDebug}) /echo |- BRD_MacroSettings ==>

	|- Adv Settings
	/call writeToIni "${iniTHF},Bard,Functions" "check_Burns/check_CombatBuffs/check_Buffs"

/if (${outerDebug}) /echo <== BRD_MacroSettings -|
/RETURN



SUB BRD_CharacterSettings
/if (${outerDebug}) /echo |- BRD_CharacterSettings ==>

	/call writeToIni "${iniToon},Bard,Auto-Melody"

/if (${outerDebug}) /echo <== BRD_CharacterSettings -|
/RETURN



SUB BRD_Aliases
/if (${outerDebug}) /echo |- BRD_Aliases ==>

	/squelch /alias /SaveMelody /echo Save Melody
	/squelch /alias /PlayMelody /bc  Melody

/if (${outerDebug}) /echo <== BRD_Aliases -|
/RETURN