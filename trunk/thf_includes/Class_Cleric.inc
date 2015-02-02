|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Cleric.inc v1.0
|- Adapted for THF by Conradd (from Killians of PEQ)
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Contains Cleric specific functions related to,
|-		Setup and Ini entry information
|-		MainLoop, and AssistLoop hooks
|-		Over-all functionality / flow
|---------------------------------------------------------------------------------------------------------------------------------------------------|

SUB check_Yaulp
	/if (${outerDebug}) /echo |- check_clrYaulp ==>
	
	/if (${AutoYaulp} && !${medBreak})  {	

		/declare castName string local ${yaulpSpell.Arg[1,/]}

		|- Auto-yaulp spell
		/if (!${Bool[${Me.Buff[${castName}]}]} && ${Me.PctMana} < 95) {
			/if (${Target.ID}) /declare tempTarget int local ${Target.ID}
			/squelch /target clear
			/call thf_Casting 0 "${yaulpSpell}/giveuptimer|${If[${Me.Gem[${castName}]} || ${FindItemCount[=${castName}]},0,80]}"
			/if (${tempTarget}) /call TrueTarget ${tempTarget}
		}
	}
	
	/if (${outerDebug}) /echo <== check_clrYaulp -|
/RETURN

SUB check_DivineArb
	/if (${outerDebug}) /echo |- check_DivineArb ==>

	/if (${DivineArb_Pct} && ${Group}>1) {
		/declare i int local

		/for i 0 to ${Group}
		
			/if (${SpawnCount[pc ${Group.Member[${i}]}]}) {
				
				/if (${Bool[${NetBots[${Group.Member[${i}]}]}]} && ${NetBots[${Group.Member[${i}]}].PctHPs} < ${DivineArb_Pct} || ${Spawn[pc ${Group.Member[${i}]}].PctHPs} < ${DivineArb_Pct}) {
					
					/if (${NetBots[${Group.Member[${i}]}].PctHPs}<${DivineArb_Pct}) {
						/if (${FindItemCount[Ancient Shield of the Divine]} && !${FindItem[Ancient Shield of the Divine].Timer} && !${itemTimer_${FindItem[=Ancient Shield of the Divine].ID}}) {
							/call thf_Casting ${Me.ID} "Ancient Shield of the Divine"
						} else /if (${Me.AltAbilityReady[Divine Arbitration]}) {
							/call thf_Casting ${Me.ID} "Divine Arbitration"
						}
					}
				}
			}
			
		/if (!${outerActionTaken}) /next i
	}

	/if (${outerDebug}) /echo <== check_DivineArb -|
/RETURN

SUB check_celestialRegen
	/if (${outerDebug}) /echo |- check_celestialRegen ==>

	/if (${celestialRegen_Pct} && ${Group}>2 && ${Me.AltAbilityReady[Celestial Regeneration]}) {
		/declare groupSize int local
		/declare averageHP int local
		/declare i int local
		
		|- Count group members in zone
		/for i 0 to ${Group}
			
			/if (${SpawnCount[pc ${Group.Member[${i}]}]}) {
				/varcalc averageHP ${averageHP} + ${If[${NetBots[${Group.Member[${i}]}].ID},${NetBots[${Group.Member[${i}]}].PctHPs},${Group.Member[${i}].PctHPs}]}
				/varcalc groupSize ${groupSize} + 1
			}
			
		/next i
		
		/if (${groupSize}) {
			/varcalc averageHP ${averageHP} / ${groupSize}
			/if (${averageHP} < ${celestialRegen_Pct}) /call thf_Casting ${Me.ID} "Celestial Regeneration"
		}
	}

	/if (${outerDebug}) /echo <== check_celestialRegen -|
/RETURN

#EVENT Rez "<#1#> Rez"
#EVENT Rez "<#1#> Rezz"
#EVENT Rez "<#1#> Rez me"
#EVENT Rez "<#1#> Rezz me"
#EVENT Rez "<#*#> RezOn #1#"
#EVENT Rez "<#*#> Rez On #1#"
SUB EVENT_Rez(string line, string ChatSender)

	/if (${Me.Class.ShortName.Equal[CLR]}) {
		/if (${ChatSender.Left[1].Compare[ ]}<0) /varset ChatSender ${ChatSender.Right[-2].Left[-1]}
		/if (${Me.Class.ShortName.Equal[CLR]} && ${Spawn[${ChatSender}'s].ID}) {
			/tell ${ChatSender} Wait4Rez
			/if (${Select[${Me.Race},High Elf]}) /em pops a toot!
	 
			/if (${FindItem[Water Sprinkler of Nem Ankh].ID}) {
				/call thf_Casting ${Spawn[${ChatSender}'s].ID} "Water Sprinkler of Nem Ankh"
			} else /if (${Me.Book[Reviviscence]}) {
				/call thf_Casting ${Spawn[${ChatSender}'s].ID} "Reviviscence/giveuptimer|45s"
			} else /if (${Me.Book[Ressurection]}) {
				/call thf_Casting ${Spawn[${ChatSender}'s].ID} "Resurrection/giveuptimer|45s"
			} else /if (${Me.Book[Restoration]}) {
				/call thf_Casting ${Spawn[${ChatSender}'s].ID} "Restoration/giveuptimer|45s"
			} else /if (${Me.Book[Resuscitate]}) {
				/call thf_Casting ${Spawn[${ChatSender}'s].ID} "Resuscitate/giveuptimer|45s"
			} else /if (${Me.Book[Renewal]}) {
				/call thf_Casting ${Spawn[${ChatSender}'s].ID} "Renewal/giveuptimer|45s"
			} else /if (${Me.Book[Revive]}) {
				/call thf_Casting ${Spawn[${ChatSender}'s].ID} "Revive/giveuptimer|45s"
			} else /if (${Me.Book[Reparation]}) {
				/call thf_Casting ${Spawn[${ChatSender}'s].ID} "Reparation/giveuptimer|45s"
			} else /if (${Me.Book[Reconstitution]}) {
				/call thf_Casting ${Spawn[${ChatSender}'s].ID} "Reconstitution/giveuptimer|45s"
			} else /if (${Me.Book[Reanimation]}) {
				/call thf_Casting ${Spawn[${ChatSender}'s].ID} "Reanimation/giveuptimer|45s"
			}
		} else /if (!${Spawn[${ChatSender}'s].ID}) {
			/tell ${ChatSender} I don't see any of your corpses in ${Zone}.
		}
	}
/RETURN

#EVENT AE_Rez "[MQ2] AE Rez"
#EVENT AE_Rez "[MQ2] Rez all"
SUB EVENT_AE_Rez
	/if (${Select[${Me.Class},Cleric,Paladin]}) {
		/if (${outerDebug} || ${outerouterDebugBasics}) /echo |- EVENT_AE_Rez ==>
	
		/if (!${SpawnCount['s radius 100]}) {
			/echo I couldn't summon any corpses to ressurect...
		} else {
		
			/declare playerName string local
			/declare corpseID int local
			/declare miscTimer timer local
			/declare lastChecked int outer
			/declare rezzable string outer
			/declare i int local
			
			/for i 1 to ${SpawnCount['s radius 100]}
			
				/varset playerName ${NearestSpawn[${i},'s radius 100].CleanName}
				/varset playerName ${playerName.Arg[1,']}
				/varset corpseID ${NearestSpawn[${i},'s radius 100].ID}
				/varset miscTimer 30
				/varset rezzable UNKNOWN
				
				/if (!${playerName.Find[_]}) {
				
					|- Check if the corpse is rezzable
					/call TrueTarget ${corpseID}
					/consider
					
					:waitforEvent
					/delay 5
					/doevents CorpseExpired
					/if (${lastChecked} != ${corpseID} && ${miscTimer}) /goto :waitforEvent
					
					/if (!${Bool[${rezzable}]}) {
						
						/if (${rezzable.Equal[UNKNOWN]}) {
							/say I could not determine whether [${Spawn[id ${corpseID}].CleanName}] is rezzable.  Attempting to rez.
							/goto :retry
						} else {
							/say I cannot ressurect [${Spawn[id ${corpseID}].CleanName}] at this time.
						}
					} else {
					
						:retry
						/if (${Target.Distance} > 35) {
							/if (${Select[${playerName},${consentTargets}]}) {
								/if (${miscTimer}) {
									/corpse 
									/delay 10 ${Target.Distance} > 10
									/goto :retry
								} else {
									/echo I failed to pull [${Spawn[id ${corpseID}].CleanName}],  skipping rez.
								}
							} else {
								/tell ${playerName} You're too far away to rez, and I don't have concent to move your corpse.  Please consent me, then tell me 'rez'.
							}
						} else {

							/tell ${playerName} Wait4Rez

							/if (${FindItem[Water Sprinkler of Nem Ankh].ID}) {
								/call thf_Casting ${corpseID} "Water Sprinkler of Nem Ankh" item
							} else /if (${Me.Book[Reviviscence]}) {
								/call thf_Casting ${corpseID} "Reviviscence" Gem${DefaultGem} 45s
							} else /if (${Me.Book[Ressurection]}) {
								/call thf_Casting ${corpseID} "Ressurection" Gem${DefaultGem} 45s
							} else /if (${Me.Book[Restoration]}) {
								/call thf_Casting ${corpseID} "Restoration" Gem${DefaultGem} 45s
							} else /if (${Me.Book[Resuscitate]}) {
								/call thf_Casting ${corpseID} "Resuscitate" Gem${DefaultGem} 45s
							} else /if (${Me.Book[Renewal]}) {
								/call thf_Casting ${corpseID} "Renewal" Gem${DefaultGem} 45s
							} else /if (${Me.Book[Revive]}) {
								/call thf_Casting ${corpseID} "Revive" Gem${DefaultGem} 45s
							} else /if (${Me.Book[Reparation]}) {
								/call thf_Casting ${corpseID} "Reparation" Gem${DefaultGem} 45s
							} else /if (${Me.Book[Reconstitution]}) {
								/call thf_Casting ${corpseID} "Reconstitution" Gem${DefaultGem} 45s
							} else /if (${Me.Book[Reanimation]}) {
								/call thf_Casting ${corpseID} "Reanimation" Gem${DefaultGem} 45s
							}
						}
					}
				}
			
			/next i
			
			/deletevar lastChecked
			/deletevar rezzable
			
			/chat ${channelNumber} All viable corpses nearby, have been ressurected.
		}

		/if (${outerDebug} || ${outerouterDebugBasics}) /echo <== EVENT_AE_Rez -|
	}
/RETURN

#EVENT CorpseExpired "This corpse #1# be resurrected."
SUB EVENT_CorpseExpired(line, corpseExpired)
	/if (${Defined[lastChecked]}) {
		/if (${outerDebug}) /echo |- EVENT_CorpseExpired ==>

			/varset lastChecked ${Target.ID}
			/if (${corpseExpired.Equal[cannot]}) {
				/varset rezzable FALSE
			} else {
				/varset rezzable TRUE
			}

		/if (${outerDebug}) /echo <== EVENT_CorpseExpire -|
	}
/RETURN

SUB CLR_Setup
	/if (${outerDebug}) /echo |- CLR_Setup ==>

	/call iniToVar "${iniToon},Cleric,Divine Arbitration (Pct)" DivineArb_Pct int outer
	/call iniToVar "${iniToon},Cleric,Celestial Regeneration (Pct)" celestialRegen_Pct int outer
	/call iniToVar "${iniToon},Cleric,Auto-Yaulp (On/Off)" AutoYaulp bool outer
	/call iniToVar "${iniToon},Cleric,Yaulp Spell" yaulpSpell string outer
	/if (${AutoYaulp}) {
		/if (!${Defined[yaulpSpell]} || !${yaulpSpell.Length} || !${Me.Book[${yaulpSpell.Arg[1,/]}]}) {
			/echo ERROR! ${iniToon} > [Cleric] > Yaulp Spell is not correctly defined.  Please check this entry, and restart the macro.
			/varset AutoYaulp FALSE
		}
	}

	/if (${outerDebug}) /echo <== CLR_Setup -|
/RETURN

SUB CLR_BackgroundEvents
/RETURN

SUB CLR_MacroSettings
	/if (${outerDebug}) /echo |- CLR_MacroSettings ==>
	
	|- Settings
	/call writeToIni "${iniTHF},Cleric,Functions" "check_DivineArb/check_Yaulp/check_celestialRegen/check_healChain/check_Heals/check_CombatBuffs/check_Debuffs/check_Nukes/check_Buffs" FALSE

	/if (${outerDebug}) /echo <== CLR_MacroSettings -|
/RETURN

SUB CLR_CharacterSettings
	/if (${outerDebug}) /echo |- CLR_CharacterSettings ==>

	/call writeToIni "${iniToon},Cleric,Divine Arbitration (Pct)" 30 0
	/call writeToIni "${iniToon},Cleric,Celestial Regeneration (Pct)" 65 0
	/call writeToIni "${iniToon},Cleric,Auto-Yaulp (On/Off)" On 0
	/call writeToIni "${iniToon},Cleric,Yaulp Spell"

	/if (${outerDebug}) /echo <== CLR_CharacterSettings -|
/RETURN

SUB CLR_Aliases
	/if (${outerDebug}) /echo |- CLR_Aliases ==>

	/noparse /squelch /alias /RezIt /bc Rez on ${Target.CleanName.Arg[1,']}
	/squelch /alias /AERez /echo AE Rez

	/if (${outerDebug}) /echo <== CLR_Aliases -|
/RETURN