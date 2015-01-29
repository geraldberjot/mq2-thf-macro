|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- e3_Macro_BegForBuffs.inc	v2.0 ( originally designed to work with e3.mac )						-|
|- Adapted for THF by Conradd (macro from Killians of PEQ)																		-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Allows chat senders to request buffs by their full name, or alias listed in the BuffAliases_Ini.	-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|

|- Make an event to change buff request toggle
|- Adding restricted spells
|- add a -group function, to cast on all group members

**** Revamp begforbuffs to work off castReturns set in Casting, rather than checking range mana etc before hand. (this could make it easier to add items and abilities to the begforbuffs function.)

|- Specify chat channels to monitor.
#chat group
#chat tell

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Request spells by name or alias.										-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- @param ChatChannel: The channel a chat command was given through.	-|
|- @param ChatSender: The PC who sent the chat message.					-|
|- @param ChatText: The message that was sent.							-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB event_Chat(ChatChannel, ChatSender, ChatText)
/if (${Debug} || ${Debug_BegForBuffs}) /echo |- Event_Chat ==>

	|- Make sure that ChatText is not an integer, this will cause an unparsable calculation.
	/if (!${Int[${ChatText}]}) {
	
		/declare whatToCast string local
		/declare spellBeingCast string local
	
		|- If the ChatText is not found in the spellbook, check SpellAliases_Ini.
		:Retry_Alias

		/if (${Me.Book[${ChatText}]}) {
			
			/varset whatToCast ${Spell[${ChatText}]}/GiveUpTimer|${Math.Calc[${Spell[${ChatText}].RecastTime}+30]}s
			/varset spellBeingCast ${Spell[${ChatText}]}
			
		} else /if (${FindItemCount[=${ChatText}]}) {
			
			/varset whatToCast ${FindItem[=${ChatText}]}
			/varset spellBeingCast ${FindItem[=${ChatText}].Spell}

		} else {
			
			|- Replace :'s with ;'s to adjust for inis
			/if (${ChatText.Find[:]}) /varset ChatText ${ChatText.Replace[:,;]}
			
			|- Check the ini for an alias
			/if (${Ini[${SpellAliases_Ini},${Me.Class},${ChatText}].Length}) {
			
				|- Disable follow before porting.
				/if (${ChatText.Find[Port]}) {
					/if (${Stick.Active}) /squelch /stick off
					/delay 30 !${Stick.Active}
				}

				|- Reset variables using data from SpellAliases_Ini
				/varset ChatText ${Ini[${SpellAliases_Ini},${Me.Class},${ChatText}].Replace[;,:]}
				/if (${Debug} || ${Debug_BegForBuffs}) /echo |- Event_Chat -|- :Retry_Alias
				/goto :Retry_Alias
			}
		}
		
		|- If I found something to cast.
		/if (${Bool[${whatToCast}]}) {
		
			|- Verify the event
			/call verifyEvent "${ChatSender}" "${ChatText}"
			/if (${Defined[ChatSender]}) /varset ChatSender ${Macro.Return.Arg[1,_]}
		
			|- If the ChatSender IS a NetBot
			/if (${Bool[${Macro.Return.Arg[2,_]}]}) {
				
				:allowAccess
				
				|- Check zone
				/if (!${SpawnCount[pc ${ChatSender}]}) {
					/if (${ChatChannel.Equal[TELL]}) {
						/if (${Ini[${iniGeneral},User Defined Responses,Buff Request-Out of Zone].Length}) {
							/tell ${ChatSender} ${Ini[${iniGeneral},User Defined Responses,Buff Request-Out of Zone]}
						} else {
							/tell ${ChatSender} We're not in the same zone, fool!
						}
					}
				} else {
				
					|- Check the request was a beneficial spell
					/if (${Spell[${ChatText}].SpellType.Left[10].NotEqual[Beneficial]}) {
						/if (${ChatChannel.Equal[TELL]}) {
							/if (${Ini[${iniGeneral},User Defined Responses,Buff Request-Not Beneficial].Length}) {
								/tell ${ChatSender} ${Ini[${iniGeneral},User Defined Responses,Buff Request-Not Beneficial]}
							} else {
								/tell ${ChatSender} You're only allowed to request happy spells, fool!
							}
						} else {
							/if (${Ini[${iniGeneral},User Defined Responses,Buff Request-Not Beneficial].Length}) {
								/g ${Ini[${iniGeneral},User Defined Responses,Buff Request-Not Beneficial]}
							} else {
								/g You're only allowed to request happy spells, fool!
							}
						}
					} else {
				
						|- Check restricted spells list
						/if (${Select[${ChatText},${Ini[${iniToon},Buffs,Restricted Spells]}]} || ${ChatText.Find[Gate]} || ${ChatText.Find[Gate]} || ${ChatText.Find[Evacuate]} || ${ChatText.Find[portal]} || ${ChatText.Find[Relocation]} || ${ChatText.Find[Succor]} || ${ChatText.Find[Circle of]} || ${ChatText.Find[Ring of]} || ${ChatText.Find[Wind of]}) {
							/if (${NetBots[${ChatSender}].ID}) {
								/goto :castRestricted
							} else {
								/if (${ChatChannel.Equal[TELL]}) {
									/if (${Ini[${iniGeneral},User Defined Responses,Buff Request-Restricted Spell].Length}) {
										/tell ${ChatSender} ${Ini[${iniGeneral},User Defined Responses,Buff Request-Restricted Spell]}
									} else {
										/tell ${ChatSender} You're only allowed to request happy spells, fool!
									}
								} else {
									/if (${Ini[${iniGeneral},User Defined Responses,Buff Request-Restricted Spell].Length}) {
										/g ${Ini[${iniGeneral},User Defined Responses,Buff Request-Restricted Spell]}
									} else {
										/g You're only allowed to request happy spells, fool!
									}
								}
							}
						} else {

							:castRestricted
						
							|- Check for self only spells
							/if (${Spell[${spellBeingCast}].TargetType.Find[self]}) {
								/if (${ChatChannel.Equal[TELL]}) {
									/if (${Ini[${iniGeneral},User Defined Responses,Buff Request-Self Buff].Length}) {
										/tell ${ChatSender} ${Ini[${iniGeneral},User Defined Responses,Buff Request-Self Buff]}
									} else {
										/tell ${ChatSender} That's a self only buff, fool!
									}
								} else {
									/if (${Ini[${iniGeneral},User Defined Responses,Buff Request-Self Buff].Length}) {
										/g ${Ini[${iniGeneral},User Defined Responses,Buff Request-Self Buff]}
									} else {
										/g That's a self only buff, fool!
									}
								}
							} else {
							
								|- Check range.
								/if (${Spell[${spellBeingCast}].MyRange} && ${Spawn[pc ${ChatSender}].Distance} > ${Spell[${spellBeingCast}].MyRange}) {
									/if (${ChatChannel.Equal[TELL]}) {
										/if (${Ini[${iniGeneral},User Defined Responses,Buff Request-Out of Range].Length}) {
											/tell ${ChatSender} ${Ini[${iniGeneral},User Defined Responses,Buff Request-Out of Range]}
										} else {
											/tell ${ChatSender} You're out of range, fool!
										}
									} else {
										/if (${Ini[${iniGeneral},User Defined Responses,Buff Request-Out of Range].Length}) {
											/g ${Ini[${iniGeneral},User Defined Responses,Buff Request-Out of Range]}
										} else {
											/g You're out of range, fool!
										}
									}
								} else {
								
									|- Check mana.
									/if (${whatToCast.Arg[1,/].Equal[${spellBeingCast}]} && ${Spell[${spellBeingCast}].Mana} > ${Me.CurrentMana}) {
										/if (${ChatChannel.Equal[TELL]}) {
											/if (${Ini[${iniGeneral},User Defined Responses,Buff Request-Out of Mana].Length}) {
												/tell ${ChatSender} ${Ini[${iniGeneral},User Defined Responses,Buff Request-Out of Mana]}
											} else {
												/tell ${ChatSender} I don't have enough mana to cast ${spellBeingCast}, Try again later.
											}
										} else {
											/if (${Ini[${iniGeneral},User Defined Responses,Buff Request-Out of Mana].Length}) {
												/g ${Ini[${iniGeneral},User Defined Responses,Buff Request-Out of Mana]}
											} else {
												/g I don't have enough mana to cast ${spellBeingCast}, Try again later.
											}
										}
									} else {

										|- Cast the requested spell, on the ChatSender.
										/if (${Target.ID}) /declare oldTarget int local ${Target.ID}
										/if (${ChatChannel.Equal[TELL]}) {
											/if (${Ini[${iniGeneral},User Defined Responses,Buff Request-Casting Buff].Length}) {
												/tell ${ChatSender} ${Ini[${iniGeneral},User Defined Responses,Buff Request-Casting Buff]}
											} else {
												/tell ${ChatSender} ${spellBeingCast} inc...
											}
										} else {
											/if (${Ini[${iniGeneral},User Defined Responses,Buff Request-Casting Buff].Length}) {
												/g ${Ini[${iniGeneral},User Defined Responses,Buff Request-Casting Buff]}
											} else {
												/g ${spellBeingCast} inc...
											}
										}
										
										/call thf_Casting ${Spawn[pc =${ChatSender}].ID} "${whatToCast}"

										|- Switch back to your old target
										/if (${oldTarget}) {
											/call TrueTarget ${oldTarget}
										}
									}
								}		
							}
						}
					}
				}
				
			|- If the ChatSender is NOT a NetBot	
			} else {
		
				|- Check Restricted PCs
				/if (${Select[${ChatSender},${Ini[${iniGeneral},Buff Requests,Restricted PCs (When Requests [On])]}]}) {
					/if (${Ini[${iniGeneral},User Defined Responses,Buff Request-Restricted PC].Length}) {
						/tell ${ChatSender} ${Ini[${iniGeneral},User Defined Responses,Buff Request-Restricted PC]}
					} else {
						/tell ${ChatSender} You are an asshole.  I will not buff you, because you are an asshole.
					}
				} else {
				
					|- Check iniGeneral > [Buff Requests] > Allow Buff Requests (On/Off)
					/call iniToVar "${iniGeneral},Buff Requests,Allow Buff Requests (On/Off)" allow_BuffRequests bool local
					/if (${Bool[${Macro.Return}]}) {
						/goto :allowAccess
					} else {
						
						|- Check iniGeneral > [Buff Requests] > Allowed PCs (When Requests [Off])
						/if (${Select[${ChatSender},${Ini[${iniGeneral},Buff Requests,Allowed PCs (When Requests [Off])]}]} || ${Select[Guild,${Ini[${iniGeneral},Buff Requests,Allowed PCs (When Requests [Off])]}]} && ${Spawn[pc ${ChatSender}].Guild.Equal[${Me.Guild}]} || ${Select[Raid,${Ini[${iniGeneral},Buff Requests,Allowed PCs (When Requests [Off])]}]} && ${Raid.Member[${ChatSender}].Level}) {
							/goto :allowAccess
						} else {
							/if (${ChatChannel.Equal[TELL]}) {
								/if (${Ini[${iniGeneral},User Defined Responses,Buff Requests Off].Length}) {
									/tell ${ChatSender} ${Ini[${iniGeneral},User Defined Responses,Buff Requests Off]}
								} else {
									/tell ${ChatSender} Sorry, I'm not buffing right now.
								}
							}
						}
					}
				}
			}
		}
	}
	
/if (${Debug} || ${Debug_BegForBuffs}) /echo <== Event_Chat -|
/RETURN
	
	

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Relays buff requests from /tell and /group to EQBC.	-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- @param line: 'Name tells you, 'Buff me'.'			-|
|- @param buffTarget: The target to cast buffs on.		-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
#EVENT buffRequestRelay "#1# tells you, 'Buff #*#'"
#EVENT buffRequestRelay "#1# tells you, 'Buff #*# |-|#2#'"
SUB event_buffRequestRelay(line, ChatSender, eventParams)
/if (${Debug} || ${Debug_BegForBuffs}) /echo |- event_buffRequestRelay ==>

	|- Verify the event
	/call verifyEvent "${ChatSender}" "${ChatText}"
	/if (${Defined[ChatSender]}) /varset ChatSender ${Macro.Return.Arg[1,_]}

	|-    |-----------Valid User----------|
	/if (${Bool[${Macro.Return.Arg[2,_]}]}) {
	
		:allowAccess

		|- Check zone
		/if (!${SpawnCount[pc ${ChatSender}]}) {
			/if (${Ini[${iniGeneral},User Defined Responses,Buff Request-Out of Zone].Length}) {
				/tell ${ChatSender} ${Ini[${iniGeneral},User Defined Responses,Buff Request-Out of Zone]}
			} else {
				/tell ${ChatSender} We're not in the same zone, fool!
			}
		} else {
		
			|- Check range.
			/if (${Spawn[pc ${ChatSender}].Distance} > 200) {
				/if (${Ini[${iniGeneral},User Defined Responses,Buff Request-Out of Range].Length}) {
					/tell ${ChatSender} ${Ini[${iniGeneral},User Defined Responses,Buff Request-Out of Range]}
				} else {
					/tell ${ChatSender} You're out of range, fool!
				}
			} else {
		
				|- -------------------Find buffTarget------------------- |
				/declare buffTarget int local
				/declare castName string local
				/declare gemNumber string local gem${DefaultGem}
				/declare groupList string local
				
				/declare i int local
				/declare x int local

				/if (${Int[${line.Arg[3, ]}]}) {
					/varset buffTarget ${line.Arg[3, ]}
				} else /if (${line.Find[Buff me]}) {
					/varset buffTarget ${Spawn[pc =${ChatSender}].ID}
				} else /if (${line.Find[Buff pet]} || ${line.Find[Buff my pet]}) {
					/varset buffTarget ${Spawn[pet ${Spawn[pc =${ChatSender}].Pet}].ID}
				} else  /if (${SpawnCount[pc ${line.Arg[3, ]}]}) {
					/varset buffTarget ${Spawn[pc =${line.Arg[3, ]}].ID}
				} else /if (${line.Find['s pet]}) {
					/varset buffTarget ${Spawn[pet ${Spawn[pc ${line.Arg[3, ].Arg[1,']}].Pet}].ID}
				} else /if (${line.Find[s pet]}) {
					/varset buffTarget ${Spawn[pet ${Spawn[pc ${line.Arg[3, ].Left[${Math.Calc[${line.Arg[3, ].Length} - 1].Int}]}].Pet}].ID}
				}
		
				|- -------------------Verify the buffTarget------------------- |
				/if (${GroupBuffs.Size}) {
					/tell ${ChatSender} Casting buffs on ${Spawn[id ${buffTarget}].CleanName}...
					/bc ${If[${Defined[eventParams]},Buff ${buffTarget} -${eventParams},Buff ${buffTarget}]}
					
				|- If I don't have buffs, but others might.
				} else /if (!${GroupBuffs.Size} && ${NetBots.Counts}>1) {
					/tell ${ChatSender} Requesting buffs on ${Spawn[id ${buffTarget}].CleanName}...
					/bc ${If[${Defined[eventParams]},Buff ${buffTarget} -${eventParams},Buff ${buffTarget}]}
					
				|- If I'm not a buffer, and no other bots are on.
				} else /if (!${GroupBuffs.Size} && ${NetBots.Counts}<=1) {
					/tell ${ChatSender} Sorry, I'm not currently set up to cast any buffs, try again later. (This is an automated response)
				}
			}
		}
	} else {
	
		|- Check Restricted PCs
		/if (${Select[${ChatSender},${Ini[${iniGeneral},Buff Requests,Restricted PCs (When Requests [On])]}]}) {
			/if (${Ini[${iniGeneral},User Defined Responses,Buff Request-Restricted PC].Length}) {
				/tell ${ChatSender} ${Ini[${iniGeneral},User Defined Responses,Buff Request-Restricted PC]}
			} else {
				/tell ${ChatSender} You are a giant ass.  I will not buff you, because you are an ass.
			}
		} else {
			
			|- If user is not valid, check Allow Buff Requests
			/call iniToVar "${iniGeneral},Buff Requests,Allow Buff Requests (On/Off)" allow_BuffRequests bool local
			
			/if (${Bool[${Macro.Return}]}) {
				/goto :allowAccess
			} else {
			
				/if (${Select[${ChatSender},${Ini[${iniGeneral},Buff Requests,Allowed PCs (When Requests [Off])]}]} || ${Select[Guild,${Ini[${iniGeneral},Buff Requests,Allowed PCs (When Requests [Off])]}]} && ${Spawn[pc ${ChatSender}].Guild.Equal[${Me.Guild}]} || ${Select[Raid,${Ini[${iniGeneral},Buff Requests,Allowed PCs (When Requests [Off])]}]} && ${Raid.Member[${ChatSender}].Level}) {
					/goto :allowAccess
				} else {
					/if (${Ini[${iniGeneral},User Defined Responses,Buff Requests Off].Length}) {
						/tell ${ChatSender} ${Ini[${iniGeneral},User Defined Responses,Buff Requests Off]}
					} else {
						/tell ${ChatSender} Sorry, I'm not buffing right now.
					}	
				}
			}
		}
	}

/if (${Debug} || ${Debug_BegForBuffs}) /echo <== event_buffRequestRelay -|
/RETURN



#EVENT portRelay "<#*#> Port to #1#"
SUB event_portRelay(line, destination)
/if (${Debug} || ${Debug_BegForBuffs}) /echo |- EVENT_portRelay ==>

	/if (${Group} && ${Me.CleanName.Equal[${Group.Leader}]}) {
		/g Port ${destination}
	}

/if (${Debug} || ${Debug_BegForBuffs}) /echo <== EVENT_portRelay -|
/RETURN



#EVENT makeSpellList "[MQ2] Make Spell List"
#EVENT makeSpellList "<#*#> Make Spell Lists"
SUB EVENT_makeSpellList
/if (${Debug} || ${Debug_BegForBuffs}) /echo |- EVENT_makeSpellList ==>

	/if (${Me.Class.CanCast} && ${Me.Class.ShortName.NotEqual[BRD]}) {
		/if (${Bool[${Ini[${SpellList_Ini}]}]}) {
			/echo Creating a list of ${Me.Class} spells...
		} else {
			/echo Adding spells to [${SpellList_Ini}] please wait...
		}

		/call Build_Alphabetized_Ini "${SpellList_Ini}"
		
		/declare i int local
		/declare popupVar string local .
		/declare spellName string local
		/declare miscTimer timer local 15
		
		/for i 1 to 720
		
			/if (!${miscTimer}) {
				/if (${popupVar.Length} == 6) /varset popupVar . 
				/popup ${popupVar}
				/varset popupVar ${popupVar}.
				/varset miscTimer 5
			}
		
			/if (${Bool[${Me.Book[${i}]}]} && ${Spell[${Me.Book[${i}]}].SpellType.NotEqual[Detrimental]} && ${Spell[${Me.Book[${i}]}].TargetType.NotEqual[Self]} && ${Spell[${Me.Book[${i}]}].TargetType.NotEqual[Pet]}) {
				/varset spellName ${Me.Book[${i}]}
				/if (!${Bool[${Ini[${SpellList_Ini},${spellName.Left[1]},${spellName}]}]}) /call writeToIni "${SpellList_Ini},${spellName.Left[1]},${spellName}"
			}
		
		/next i

		/echo Finished adding spells.  Please edit [${SpellList_Ini}], and then use /importspellaliases to import your changes.
	}

/if (${Debug} || ${Debug_BegForBuffs}) /echo <== EVENT_makeSpellList -|
/RETURN



#EVENT import_SpellAliases "[MQ2] Import Spell Aliases"
#EVENT import_SpellAliases "<#*#> Import Spell Aliases"
SUB EVENT_import_SpellAliases
/if (${Debug} || ${Debug_BegForBuffs}) /echo |- EVENT_import_SpellAliases ==>

	/if (!${Bool[${Ini[${SpellList_Ini}]}]}) {
		/echo You must create a spell list file using, /MakeSpellList.  After editing the newly created file, use /importspellaliases to import your changes.
	} else {
		
		/declare i int local
		/declare e int local
		
		/for i 1 to 26
		
			/if (${Bool[${Ini[${SpellList_Ini},${Ini[${SpellList_Ini}].Arg[${i},|]}]}]}) {
			
				/varset e 1
				:next_Alias
				/if (${Bool[${Ini[${SpellList_Ini},${Ini[${SpellList_Ini}].Arg[${i},|]}].Arg[${e},|]}]}) {
					/if (${Bool[${Ini[${SpellList_Ini},${Ini[${SpellList_Ini}].Arg[${i},|]},${Ini[${SpellList_Ini},${Ini[${SpellList_Ini}].Arg[${i},|]}].Arg[${e},|]}]}]}) /call writeToIni "${SpellAliases_Ini},${Me.Class},${Ini[${SpellList_Ini},${Ini[${SpellList_Ini}].Arg[${i},|]},${Ini[${SpellList_Ini},${Ini[${SpellList_Ini}].Arg[${i},|]}].Arg[${e},|]}]}" "${If[${Ini[${SpellList_Ini},${Ini[${SpellList_Ini}].Arg[${i},|]}].Arg[${e},|].Find[;]},${Ini[${SpellList_Ini},${Ini[${SpellList_Ini}].Arg[${i},|]}].Arg[${e},|].Replace[;,:]},${Ini[${SpellList_Ini},${Ini[${SpellList_Ini}].Arg[${i},|]}].Arg[${e},|]}]}" 1
					/varcalc e ${e} + 1
					/goto :next_Alias
				}

		/next i
		
		/echo New spell aliases have been imported.
	}
	
/if (${Debug} || ${Debug_BegForBuffs}) /echo <== EVENT_import_SpellAliases -|
/RETURN



#EVENT createSpellAlias "[#1#] Set Spell Alias #2# #3#"
#EVENT createSpellAlias "<#1#> Set Spell Alias #2# #3#"
SUB EVENT_createSpellAlias(line, ChatSender, newAlias, spellName)
/if (${Debug} || ${Debug_BegForBuffs}) /echo |- EVENT_createSpellAlias ==>

	/if (${ChatSender.Equal[${Me.CleanName}]} || ${ChatSender.Equal[MQ2]}) {
		/channel Adding spell alias [${newAlias}] to cast [${spellName}] for [${Me.Class}s].
		/call writeToIni "${SpellAliases_Ini},${Me.Class},${newAlias}" "${spellName}" 1
	}
	
/if (${Debug} || ${Debug_BegForBuffs}) /echo <== EVENT_createSpellAlias -|
/RETURN



|---------------------------------------------------------------------------------------------------------------------------------------------------|-|
|- Setup BegForBuff variables.	-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|-|
SUB begForBuffs_Setup
/call iniToVar "${iniGeneral},Debug,Debug BegForBuffs (On/Off)" Debug_BegForBuffs bool outer
/if (${Debug} || ${Debug_BegForBuffs}) /echo |- setup_BegForBuffs ==>

	|- ---------------------------------------------------------------------------------------------------------------Add SpellAlias File Path.
	/if (!${Ini[${MacroData_Ini},File Paths,Spell Aliases File].Length}) /call writeToIni "${MacroData_Ini},File Paths,Spell Aliases File" "e3 Macro Inis\Spell Aliases.ini" 1
	
	|- ---------------------------------------------------------------------------------------------------------------Add Spell List File Path.
	/if (!${Ini[${MacroData_Ini},File Paths,Spell List Files].Length}) {
		/noparse /ini "e3 Macro Inis\e3 Data.ini" "File Paths" "Spell List Files" "Macro Inis\Spell List-${Me.Class.ShortName}.ini"
	}
	
	|- ----------------------------------------------------------------------------------------------------------------Import SpellAliases_Ini.
	/if (!${Ini[${MacroData_Ini},File Paths,Spell Aliases File].Length}) {
		/echo ERROR: Could not find designated file path for [Spell Aliases], please review [${MacroData_Ini}] and restart.
		/endmacro
	} else {
		/declare SpellAliases_Ini string outer ${Ini[${MacroData_Ini},File Paths,Spell Aliases File]}
	}
	
	|- ----------------------------------------------------------------------------------------------------------------Import SpellList_Ini.
	/if (!${Ini[${MacroData_Ini},File Paths,Spell List Files].Length}) {
		/echo ERROR: Could not find designated file path for [Spell List Files], please review [${MacroData_Ini}] and restart.
		/endmacro
	} else {
		/declare SpellList_Ini string outer ${Ini[${MacroData_Ini},File Paths,Spell List Files]}
	}

	/if (!${Ini[${SpellAliases_Ini}].Length}) /call MakeSpellAliases_Ini

/if (${Debug} || ${Debug_BegForBuffs}) /echo <== setup_BegForBuffs -|
/RETURN



SUB begForBuffs_Background_Events
/RETURN



SUB begForBuffs_MacroSettings
/if (${Debug}) /echo |- _MacroSettings ==>

	/call writeToIni "${iniGeneral},Debug,Debug BegForBuffs (On/Off)" Off

	/call writeToIni "${iniGeneral},Buff Requests,Allow Buff Requests (On/Off)" On
	/call writeToIni "${iniGeneral},Buff Requests,Restricted PCs (When Requests [On])"
	/call writeToIni "${iniGeneral},Buff Requests,Allowed PCs (When Requests [Off])" "Guild,Raid"
	/call writeToIni "${iniGeneral},User Defined Responses,Buff Request-Out of Zone"
	/call writeToIni "${iniGeneral},User Defined Responses,Buff Request-Out of Range"
	/call writeToIni "${iniGeneral},User Defined Responses,Buff Request-Out of Mana"
	/call writeToIni "${iniGeneral},User Defined Responses,Buff Request-Self Buff"
	/call writeToIni "${iniGeneral},User Defined Responses,Buff Request-Not Beneficial"
	/call writeToIni "${iniGeneral},User Defined Responses,Buff Request-Restricted Spell"
	/call writeToIni "${iniGeneral},User Defined Responses,Buff Request-Casting Buff"
	/call writeToIni "${iniGeneral},User Defined Responses,Buff Request-Restricted PC"
	/call writeToIni "${iniGeneral},User Defined Responses,Buff Requests Off"
	
	/call writeToIni "${iniToon},Buffs,Restricted Spells"

/if (${Debug}) /echo <== _MacroSettings -|
/RETURN



SUB begForBuffs_CharacterSettings
/if (${Debug} || ${Debug_BegForBuffs}) /echo |- _CharacterSettings ==>

/if (${Debug} || ${Debug_BegForBuffs}) /echo <== _CharacterSettings -|
/RETURN



SUB begForBuffs_Aliases
/if (${Debug} || ${Debug_BegForBuffs}) /echo |- _Aliases ==>

	/noparse /squelch /alias /giveme /tell ${Target.CleanName}
	/noparse /squelch /alias /addalias /echo Set Spell Alias

/if (${Debug} || ${Debug_BegForBuffs}) /echo <== _Aliases -|
/RETURN



|----------------------------|
|- Build SpellAliases_Ini.	-|
|----------------------------|
|- Needs to be updated
SUB MakeSpellAliases_Ini

	/echo Creating spell aliases file...
	
	|- Beastlord Aliases.
	/call writeToIni "${SpellAliases_Ini},Beastlord,Beastlord Spells"
	/call writeToIni "${SpellAliases_Ini},Beastlord,SD" "Spiritual Dominion"
	/call writeToIni "${SpellAliases_Ini},Beastlord,SV" "Spiritual Vigor"
	
	|- Clerc Aliases.
	/call writeToIni "${SpellAliases_Ini},Cleric,Cleric Spells"
	/call writeToIni "${SpellAliases_Ini},Cleric,Kazad" "Kazad's Mark"
	/call writeToIni "${SpellAliases_Ini},Cleric,Virtue" "Virtue"
	/call writeToIni "${SpellAliases_Ini},Cleric,Virt" "Virtue"
	/call writeToIni "${SpellAliases_Ini},Cleric,HoV" "Hand of Virtue"	
	/call writeToIni "${SpellAliases_Ini},Cleric,BoR" "Blessing of Reverence"	
	/call writeToIni "${SpellAliases_Ini},Cleric,CHeal" "Complete Heal"	
	/call writeToIni "${SpellAliases_Ini},Cleric,HoT" "Holy Elixir"
	/call writeToIni "${SpellAliases_Ini},Cleric,Heal" "Holy Light"
	/call writeToIni "${SpellAliases_Ini},Cleric,QHeal" "Supernal Remedy"
	/call writeToIni "${SpellAliases_Ini},Cleric,GroupHeal" "Word of Replenishment"
	/call writeToIni "${SpellAliases_Ini},Cleric,GroupHoT" "Ethereal Elixir"	
	/call writeToIni "${SpellAliases_Ini},Cleric,Temp" "Temperance"
	
	|- Druid Aliases.
	/call writeToIni "${SpellAliases_Ini},Druid,Druid Spells"
	/call writeToIni "${SpellAliases_Ini},Druid,FoE" "Flight of Eagles"
	/call writeToIni "${SpellAliases_Ini},Druid,DS" "Legacy of Bracken"
	/call writeToIni "${SpellAliases_Ini},Druid,BoT9" "Blessing of the Nine"	
	/call writeToIni "${SpellAliases_Ini},Druid,Heal" "Sylvan Infusion"
	/call writeToIni "${SpellAliases_Ini},Druid,Regen" "Blessing of Replenishment"
	/call writeToIni "${SpellAliases_Ini},Druid,CoS" "Circle of Seasons"
	/call writeToIni "${SpellAliases_Ini},Druid,Str" "Nature's Might"
	/call writeToIni "${SpellAliases_Ini},Druid,GroupWolf" "Share Form of the Great Wolf"
	/call writeToIni "${SpellAliases_Ini},Druid,GroupInvis" "Foliage Shield"
	/call writeToIni "${SpellAliases_Ini},Druid,Evac" "Succor"
	
	/call writeToIni "${SpellAliases_Ini},Druid,GPortNK" "Circle of Karana"
	/call writeToIni "${SpellAliases_Ini},Druid,GPortLavastorm" "Circle of Lavastorm"
	/call writeToIni "${SpellAliases_Ini},Druid,GPortSRo" "Circle of Ro"
	/call writeToIni "${SpellAliases_Ini},Druid,GPortMisty" "Circle of Misty"
	/call writeToIni "${SpellAliases_Ini},Druid,GPortFeerrott" "Circle of Feerrott"
	/call writeToIni "${SpellAliases_Ini},Druid,GPortCommons" "Circle of Commons"
	/call writeToIni "${SpellAliases_Ini},Druid,GPortEK" "Succor: East"
	/call writeToIni "${SpellAliases_Ini},Druid,GPortSureFall" "Circle of Surefall Glade"
	/call writeToIni "${SpellAliases_Ini},Druid,GPortButcher" "Circle of Butcher"
	/call writeToIni "${SpellAliases_Ini},Druid,GPortSteamfont" "Circle of Steamfont"
	/call writeToIni "${SpellAliases_Ini},Druid,GPortSkyfire" "Wind of the North"
	/call writeToIni "${SpellAliases_Ini},Druid,GPortEJ" "Wind of the South"
	/call writeToIni "${SpellAliases_Ini},Druid,GPortDawnshroud" "Circle of Dawnshroud"
	/call writeToIni "${SpellAliases_Ini},Druid,GPortDSP" "Circle of Dawnshroud"	
	/call writeToIni "${SpellAliases_Ini},Druid,GPortTwilight" "Circle of Twilight"
	/call writeToIni "${SpellAliases_Ini},Druid,GPortGrimling" "Circle of Grimling"
	/call writeToIni "${SpellAliases_Ini},Druid,GPortNexus" "Circle of the Nexus"
	/call writeToIni "${SpellAliases_Ini},Druid,GPortStonebrunt" "Circle of Stonebrunt"
	/call writeToIni "${SpellAliases_Ini},Druid,GPortTox" "Circle of Toxxulia"
	/call writeToIni "${SpellAliases_Ini},Druid,GPortBarindu" "Circle of Barindu"
	/call writeToIni "${SpellAliases_Ini},Druid,GPortNatimbi" "Circle of Natimbi"
	/call writeToIni "${SpellAliases_Ini},Druid,GPortPoK" "Circle of Knowledge"
	/call writeToIni "${SpellAliases_Ini},Druid,GPortKnowledge" "Circle of Knowledge"
	/call writeToIni "${SpellAliases_Ini},Druid,GPortCS" "Circle of Cobalt Scar"
	/call writeToIni "${SpellAliases_Ini},Druid,GPortWL" "Circle of Wakening Lands"
	/call writeToIni "${SpellAliases_Ini},Druid,GPortGD" "Circle of Great Divide"
	/call writeToIni "${SpellAliases_Ini},Druid,GPortCombines" "Circle of the Combines"
	/call writeToIni "${SpellAliases_Ini},Druid,GPortDreadlands" "Circle of the Combines"
	/call writeToIni "${SpellAliases_Ini},Druid,GPortDL" "Circle of the Combines"

	|- Enchanter Aliases.
	/call writeToIni "${SpellAliases_Ini},Enchanter,Enchanter Spells"
	/call writeToIni "${SpellAliases_Ini},Enchanter,Haste" "Vallon's Quickening"
	/call writeToIni "${SpellAliases_Ini},Enchanter,VQ" "Vallon's Quickening"
	/call writeToIni "${SpellAliases_Ini},Enchanter,VoQ" "Voice Of Quellious"
	/call writeToIni "${SpellAliases_Ini},Enchanter,C5" "Voice Of Quellious"
	/call writeToIni "${SpellAliases_Ini},Enchanter,KEI" "Koadic's Endless Intellect"
	/call writeToIni "${SpellAliases_Ini},Enchanter,C4" "Koadic's Endless Intellect"
	/call writeToIni "${SpellAliases_Ini},Enchanter,GoD" "Guard of Druzzil"
	/call writeToIni "${SpellAliases_Ini},Enchanter,CHA" "Radiant Visage"
	
	|- Magician Aliases.
	/call writeToIni "${SpellAliases_Ini},Magician,Magician Spells"
	/call writeToIni "${SpellAliases_Ini},Magician,DS" "Maelstrom of Ro"
	/call writeToIni "${SpellAliases_Ini},Magician,CoH" "Call of the Hero"

	|- Necromancer Aliases.
	/call writeToIni "${SpellAliases_Ini},Necromancer,Necromancer Spells"
	/call writeToIni "${SpellAliases_Ini},Necromancer,DMF" "Dead Men Floating"
	/call writeToIni "${SpellAliases_Ini},Necromancer,IVU" "Invisibility versus Undead"
	/call writeToIni "${SpellAliases_Ini},Necromancer,SummonCorpse" "Conjure Corpse"
	/call writeToIni "${SpellAliases_Ini},Necromancer,Sac" "Sacrifice"
	/call writeToIni "${SpellAliases_Ini},Necromancer,Bind" "Bind Affinity"

	|- Paladin Aliases.
	/call writeToIni "${SpellAliases_Ini},Paladin,Paladin Spells"
	/call writeToIni "${SpellAliases_Ini},Paladin,BSS" "Brell's Stalwart Shield"

	|- Ranger Aliases.
	/call writeToIni "${SpellAliases_Ini},Ranger,Ranger Spells"
	/call writeToIni "${SpellAliases_Ini},Ranger,SoTP" "Spirit of the Predator"
	/call writeToIni "${SpellAliases_Ini},Ranger,Pred" "Spirit of the Predator"
	/call writeToIni "${SpellAliases_Ini},Ranger,SoT" "Stringth of Tunare"
	/call writeToIni "${SpellAliases_Ini},Ranger,Tunare" "Strength of Tunare"
	/call writeToIni "${SpellAliases_Ini},Ranger,Rathe" "Call of the Rathe"
	/call writeToIni "${SpellAliases_Ini},Ranger,CoR" "Call of the Rathe"
	/call writeToIni "${SpellAliases_Ini},Ranger,SoE" "Spirit of Eagle"	
	/call writeToIni "${SpellAliases_Ini},Ranger,SoW" "Spirit of Wolf"
	/call writeToIni "${SpellAliases_Ini},Ranger,DS" "Shield of Spikes"
	/call writeToIni "${SpellAliases_Ini},Ranger,CoS" "Circle of Summer"	
	/call writeToIni "${SpellAliases_Ini},Ranger,CoW" "Circle of Winter"	

	|- Shadow Knight Aliases.
	/call writeToIni "${SpellAliases_Ini},Shadow Knight,Shadow Knight Spells"
	/call writeToIni "${SpellAliases_Ini},Shadow Knight,Voice" "Voice of Thule"
	/call writeToIni "${SpellAliases_Ini},Shadow Knight,VoT" "Voice of Thule"
	/call writeToIni "${SpellAliases_Ini},Shadow Knight,Thule" "Voice of Thule"
	
	|- Shaman Aliases.
	/call writeToIni "${SpellAliases_Ini},Shaman,Shaman Spells"
	/call writeToIni "${SpellAliases_Ini},Shaman,HoT" "Breath of Trushar"
	/call writeToIni "${SpellAliases_Ini},Shaman,Heal" "Daluda's Mending"
	/call writeToIni "${SpellAliases_Ini},Shaman,Fo7" "Focus of the Seventh"
	/call writeToIni "${SpellAliases_Ini},Shaman,Focus" "Focus of Soul"
	/call writeToIni "${SpellAliases_Ini},Shaman,Haste" "Talisman of Alacrity"
	/call writeToIni "${SpellAliases_Ini},Shaman,Billy" "Spirit of Bih`li"
	/call writeToIni "${SpellAliases_Ini},Shaman,Bih`li" "Spirit of Bih`li"
	/call writeToIni "${SpellAliases_Ini},Shaman,SoB" "Spirit of Bih`li"
	/call writeToIni "${SpellAliases_Ini},Shaman,Regen" "Blessing of Replenishment"
	/call writeToIni "${SpellAliases_Ini},Shaman,Agi" "Talisman of the Wrulan"
	/call writeToIni "${SpellAliases_Ini},Shaman,Dex" "Talisman of the Raptor"
	/call writeToIni "${SpellAliases_Ini},Shaman,Sta" "Talisman of the Boar"
	/call writeToIni "${SpellAliases_Ini},Shaman,Str" "Talisman of the Diaku"
	/call writeToIni "${SpellAliases_Ini},Shaman,Tribunal" "Talisman of the Tribunal"
	/call writeToIni "${SpellAliases_Ini},Shaman,ToT" "Talisman of the Tribunal"	
	/call writeToIni "${SpellAliases_Ini},Shaman,Jag" "Spirit of the Jaguar"
	/call writeToIni "${SpellAliases_Ini},Shaman,Shrink" "Shrink"
	/call writeToIni "${SpellAliases_Ini},Shaman,GroupShrink" "Tiny Terror"
	/call writeToIni "${SpellAliases_Ini},Shaman,GShrink" "Tiny Terror"
	/call writeToIni "${SpellAliases_Ini},Shaman,RGC" "Remove Greater Curse"
	/call writeToIni "${SpellAliases_Ini},Shaman,Cha" "Glamour"

	|- Wizard Aliases.
	/call writeToIni "${SpellAliases_Ini},Wizard,Wizard Spells"
	/call writeToIni "${SpellAliases_Ini},Wizard,Decess" "Decession"
	/call writeToIni "${SpellAliases_Ini},Wizard,Evac" "Decession"
	/call writeToIni "${SpellAliases_Ini},Wizard,TL" "Teleport"
	/call writeToIni "${SpellAliases_Ini},Wizard,DS" "O'Keils Radiation"
	
	/call writeToIni "${SpellAliases_Ini},Wizard,GPortWK" "West Portal"
	/call writeToIni "${SpellAliases_Ini},Wizard,GPortNRo" "Ro Portal"
	/call writeToIni "${SpellAliases_Ini},Wizard,GPortCommons" "Common Portal"
	/call writeToIni "${SpellAliases_Ini},Wizard,GPortCazic" "Cazic Portal"
	/call writeToIni "${SpellAliases_Ini},Wizard,GPortNek" "Nek Portal"
	/call writeToIni "${SpellAliases_Ini},Wizard,GPortNektulos" "Nek Portal"	
	/call writeToIni "${SpellAliases_Ini},Wizard,GPortNK" "North Portal"
	/call writeToIni "${SpellAliases_Ini},Wizard,GPortGfay" "Fay Portal"
	/call writeToIni "${SpellAliases_Ini},Wizard,GPortEJ" "Markar's Relocation"
	/call writeToIni "${SpellAliases_Ini},Wizard,GPortEmerald" "Markar's Relocation"
	/call writeToIni "${SpellAliases_Ini},Wizard,GportSkyfire" "Tishan's Relocation"
	/call writeToIni "${SpellAliases_Ini},Wizard,GportDL" "Combine Portal"
	/call writeToIni "${SpellAliases_Ini},Wizard,GportDreadlands" "Combine Portal"
	/call writeToIni "${SpellAliases_Ini},Wizard,GportCombines" "Combine Portal"
	/call writeToIni "${SpellAliases_Ini},Wizard,GportDawnshroud" "Dawnshroud Portal"
	/call writeToIni "${SpellAliases_Ini},Wizard,GportDSP" "Dawnshroud Portal"
	/call writeToIni "${SpellAliases_Ini},Wizard,GportTwilight" "Twilight Portal"
	/call writeToIni "${SpellAliases_Ini},Wizard,GportGrimling" "Grimling Portal"
	/call writeToIni "${SpellAliases_Ini},Wizard,GportNexus" "Nexus Portal"
	/call writeToIni "${SpellAliases_Ini},Wizard,GroupTL" "Translocate: Group"
	/call writeToIni "${SpellAliases_Ini},Wizard,GportTx" "Tox Portal"
	/call writeToIni "${SpellAliases_Ini},Wizard,GportBarindu" "Barindu Portal"
	/call writeToIni "${SpellAliases_Ini},Wizard,GportNatimbi" "Natimbi Portal"
	/call writeToIni "${SpellAliases_Ini},Wizard,GportPoK" "Knowledge Portal"
	/call writeToIni "${SpellAliases_Ini},Wizard,GportKnowledge" "Knowledge Portal"
	/call writeToIni "${SpellAliases_Ini},Wizard,GPortSky" "Alter Plane: Sky"
	/call writeToIni "${SpellAliases_Ini},Wizard,GPortHate" "Alter Plane: Hate"
	/call writeToIni "${SpellAliases_Ini},Wizard,GPortCS" "Cobalt Scar Portal"
	/call writeToIni "${SpellAliases_Ini},Wizard,GPortWL" "Wakening Lands Portal"
	/call writeToIni "${SpellAliases_Ini},Wizard,GPortGD" "Great Divide Portal"
	/call writeToIni "${SpellAliases_Ini},Wizard,GPortIceclade" "Iceclad Portal"
	
	/call writeToIni "${SpellAliases_Ini},Wizard,TLDawnshroud" "Translocate: Dawnshroud"
	/call writeToIni "${SpellAliases_Ini},Wizard,TLCazic" "Translocate: Cazic"
	/call writeToIni "${SpellAliases_Ini},Wizard,TLNRo" "Translocate: Ro"
	/call writeToIni "${SpellAliases_Ini},Wizard,TLWC" "Translocate: West"
	/call writeToIni "${SpellAliases_Ini},Wizard,TLNek" "Translocate: Nek"
	/call writeToIni "${SpellAliases_Ini},Wizard,TLCommon" "Translocate: Common"
	/call writeToIni "${SpellAliases_Ini},Wizard,TLFay" "Translocate: Fay"
	/call writeToIni "${SpellAliases_Ini},Wizard,TLCombine" "Translocate: Combine"
	/call writeToIni "${SpellAliases_Ini},Wizard,TLTwilight" "Translocate: Twilight"
	/call writeToIni "${SpellAliases_Ini},Wizard,TLGrimling" "Translocate: Grimling"
	/call writeToIni "${SpellAliases_Ini},Wizard,TLNexus" "Translocate: Nexus"
	/call writeToIni "${SpellAliases_Ini},Wizard,TLGroup" "Translocate"
	/call writeToIni "${SpellAliases_Ini},Wizard,TLTox" "Translocate: Tox"
	/call writeToIni "${SpellAliases_Ini},Wizard,TLNatimbi" "Translocate: Natimbi"
	/call writeToIni "${SpellAliases_Ini},Wizard,TLBarindu" "Translocate: Barindu"
	/call writeToIni "${SpellAliases_Ini},Wizard,TLPoK" "Translocate: Knowledge"
	/call writeToIni "${SpellAliases_Ini},Wizard,TLCS" "Translocate: Cobalt Scar"
	/call writeToIni "${SpellAliases_Ini},Wizard,TLWL" "Translocate: Wakening Lands"
	/call writeToIni "${SpellAliases_Ini},Wizard,TLGD" "Translocate: Great Divide"
	/call writeToIni "${SpellAliases_Ini},Wizard,TLIceclad" "Translocate: Iceclad"
	
/RETURN