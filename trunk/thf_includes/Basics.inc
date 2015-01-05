|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Basics.inc
|- Adapted for THF by Conradd (macro from Killians of PEQ)
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Contains basic and miscellaneous EVENTs and functions of thf.mac
|---------------------------------------------------------------------------------------------------------------------------------------------------|

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Engages, and sets variables related to Follow.
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Only toons in the same zone will engage follow.
|- Toons will wait to engage follow until, they can see the globalFollowTarget, and it is within the MaxResponseDist.
|- If toons are assisting, they will back off and follow until re-engage.
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- @param paramLine: The paramLine that called the event
|- @param paramSender: The PC who sent the command
|- @param paramArgs: Who to follow
|---------------------------------------------------------------------------------------------------------------------------------------------------|
#EVENT Follow "[#1#] Follow #2#"
SUB EVENT_Follow(String paramLine, String paramSender, String paramArgs)
	/if (${outerDebug} || ${Debug_Basics}) {
		/echo |- EVENT_Follow ==>
		/echo paramSender=${paramSender} |- paramArgs=${paramArgs}
	}

	|- Verify the event
	/call amIConcerned ${paramSender} ${paramArgs} TRUE
	/if (${Macro.Return}) {
		|- Turn globalFollowing on and set the globalFollowTarget
		/varset globalFollowing TRUE
		/varset globalFollowTarget ${Spawn[pc ${paramSender}].CleanName}
		
		|- If I am the follow target
		|- =>
		/if (${globalFollowTarget.Equal[${Me}]}) {

			|- Turn off follow plugins.
			/if (${Stick.Active}) /squelch /stick off

			|- create a list of bots globalFollowing you.
			/varset globalFollowers 
			/declare i int local
			/for i 1 to ${NetBots.Counts}
				/if (${NetBots.Client.Arg[${i}, ].NotEqual[${Me.CleanName}]} && ${SpawnCount[pc =${NetBots.Client.Arg[${i}, ]}]} && ${Spawn[pc =${NetBots.Client.Arg[${i}, ]}].Distance}<${MaxResponseDist}) /varset globalFollowers ${globalFollowers}${NetBots.Client.Arg[${i}, ]},
			/next i
		|- <=
		} else {
		
		|- If I am not the follow target
		|- =>
		
			|- Check Range
			/if (${Spawn[=${globalFollowTarget}].Distance} > ${MaxResponseDist}) {
				/channel [${globalFollowTarget}] is too far away.
			} else {
			
				|- Check LoS.
				/if (!${Spawn[=${globalFollowTarget}].paramLineOfSight}) {
					/channel I cannot see [${globalFollowTarget}].
				} else {
				
					|- Stop assisting so we don't try to stick to the AssistTarget.	
					/if (${Assisting} || ${Me.Combat}) /call EVENT_BackOff
					
					|- Start globalFollowing.
					/if (${Verbosity}) /channel globalFollowing ${globalFollowTarget}.
					/call AcquireFollow
				}
			}
		|- <=
		}
	}
	
	/if (${outerDebug} || ${Debug_Basics}) /echo <== EVENT_Follow -|
/RETURN



|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Engages follow plugins on specified follow targets.
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB AcquireFollow
	/if (${outerDebug} || ${Debug_Basics}) /echo |- AcquireFollow ==>

	|- Check for globalFollowTarget in zone.
	/if (${SpawnCount[=${globalFollowTarget}]}) {
		|- Check range to the globalFollowTarget
		/if (${SpawnCount[=${globalFollowTarget} radius ${MaxResponseDist}]}) {
			|- Check that I'm not the follow target.
			/if (${globalFollowTarget.NotEqual[${Me.CleanName}]}) {
				|- Turn off any previous follow plugins.
				/if (${Stick.Active}) /squelch /stick off
				
				/declare globalFollowTargetID int local
				
				|- Check Target ID
				/if (${SpawnCount[pc =${globalFollowTarget}]}) {
					/varset globalFollowTargetID ${Spawn[pc =${globalFollowTarget}].ID}
				} else /if (${SpawnCount[npc ${globalFollowTarget}]}) {
					/varset globalFollowTargetID ${Spawn[npc =${globalFollowTarget}].ID}
				} else /if (${SpawnCount[pet ${globalFollowTarget}]}) {
					/varset globalFollowTargetID ${Spawn[pet =${globalFollowTarget}].ID}
				}

				|- Check LoS
				/if (${Spawn[id ${globalFollowTargetID}].paramLineOfSight}) {
					/call TrueTarget ${globalFollowTargetID}
					/squelch /stick ${stickCommand}
					/channel Sticking to [ ${globalFollowTarget} ]
				}

				|- Clear my target
				/if (${Target.ID} == ${globalFollowTargetID}) /squelch /target clear
			}
		}
	}

	/if (${outerDebug} || ${Debug_Basics}) /echo <== AcquireFollow -|
/RETURN



|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Disengages follow on bots who are in the same zone as the paramSender.
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Use, /stop, or, 'stop' in /bc.
|---------------------------------------------------------------------------------------------------------------------------------------------------|
#EVENT Stop	"[#1#] Stop #2#"
#EVENT Stop	"<#1#> Stop #2#"
SUB EVENT_Stop(paramLine, paramSender, paramArgs)
	/if (${outerDebug} || ${Debug_Basics}) /echo |- EVENT_Stop ==>

	|- Check if I'm globalFollowing.
	/if (${globalFollowing}) {
		
		|- Verify the event
		/call amIConcerned ${paramSender} ${paramArgs} TRUE
		/if (${Macro.Return}) {
	
			|- Wait to get close to the globalFollowTarget.
			/if (!${Spawn[${globalFollowTarget}].Moving} && ${Spawn[${globalFollowTarget}].Distance} > ${LeashLength}) /delay 100 ${Spawn[${globalFollowTarget}].Distance} < ${LeashLength} || ${Spawn[${globalFollowTarget}].Moving}
		
			|- Stop globalFollowing
			/if (${Stick.Active}) /squelch /stick off
			/varset globalFollowTarget NULL
			/varset globalFollowing FALSE
		}
	}

	/if (${outerDebug} || ${Debug_Basics}) /echo <== EVENT_Stop -|
/RETURN STOP



#EVENT MoveHere "<#1#> Move to #2#"
SUB EVENT_MoveHere(paramLine, paramSender, paramArgs)
/if (${outerDebug} || ${Debug_Basics}) /echo |- EVENT_MoveHere ==>

	|- Verify the event
	/if (${Defined[paramSender]}) /call verifyEvent "${paramSender}" "${paramLine}"
	/if (${Defined[paramSender]}) /varset paramSender ${Macro.Return.Arg[1,_]}
	|-    |-----------Valid User----------|-    |--------Included Bot-----------|-    |-----------In Zone-------------|-    |------------In Range------------|
	/if (${Bool[${Macro.Return.Arg[2,_]}]} && ${Bool[${Macro.Return.Arg[3,_]}]} && ${Bool[${Macro.Return.Arg[4,_]}]} && ${Bool[${Macro.Return.Arg[5,_]}]}) {
	
		|- Check if the paramSender is me.
		/if (${paramSender.Equal[${Me}]}) {
			/if (${outerDebug} || ${Debug_Basics}) /echo |- EVENT_MoveHere -|- I am the paramSender, skipping.
		} else {
		
			|- Set WhereTo.
			/declare moveTo_Name string local
			/declare WhereTo string local ${If[${paramArgs.Arg[1, ].Find[/]} || ${paramArgs.Arg[1, ].Equal[me]} || ${paramArgs.Arg[1, ].Equal[here]},${paramSender},${paramArgs.Arg[1, ]}]}
			
			|- Change WhereTo to a spawnID
			/if (!${Int[${WhereTo}]}) {
				/if (${SpawnCount[pc =${WhereTo}]}) {
					/varset WhereTo ${Spawn[pc =${WhereTo}].ID}
				} else /if (${SpawnCount[pet =${WhereTo}]}) {
					/varset WhereTo ${Spawn[pet =${WhereTo}].ID}
				} else /if (${SpawnCount[npc =${WhereTo}]}) {
					/varset WhereTo ${Spawn[npc =${WhereTo}].ID}
				}
			}
			
			|- Convert SpawnID to Loc
			/varset moveTo_Name ${Spawn[id ${WhereTo}].CleanName}
			/varset WhereTo ${Spawn[id ${WhereTo}].Loc.Replace[ ,]}
			
			|- Stop assisting so I don't try to stick to the AssistTarget.	
			/if (${Assisting} || ${Me.Combat}) /call EVENT_BackOff
			
			/declare retryTimer timer local 150
			
			:retry_MoveTo
			/call MoveTo -Loc|${WhereTo}
			
			|- Check if I have moved to the Target.
			/call Triangulate_Distance ${WhereTo} ${Me.Loc.Replace[ ,]}
			/if (${Macro.Return} > 10) {
				/if  (${retryTimer}) {
					/goto :retry_MoveTo
				} else {
					/echo |- EVENT_MoveHere -|- Failed to move to the specified target.
				}
			} else {
				/if (${outerDebug} || ${Debug_Basics}) /echo |- EVENT_MoveHere -|- I have arrived at the specified target.
			}
			
			|- If I'm globalFollowing, reaquire follow.
			/if (${globalFollowing}) {
				/if (${moveTo_Name.Equal[${globalFollowTarget}]}) {
					/call AcquireFollow
				} else {
					/call EVENT_Stop "<${Me.CleanName}> Stop" ${Me.CleanName}
				}
			}
		}
	}

/if (${outerDebug} || ${Debug_Basics}) /echo <== EVENT_MoveHere -|
/RETURN



|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Commands bots to bark a message at a target.															-|
|- Targets can be designated by name, or ID, using -ID|- or -Name|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- @param paramLine: '<Name> Bark at MobName BarkMsg'														-|
|- @param msgTarget: The NPC you wish to bark at.														-|
|- @param barkIt: What to bark at the NPC.																-|
|- @param paramSender: The command sender that requested bark.											-|
|- @param selectedBots: Bots specified to bark at npc.													-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Targets must be within the MaxResponseDist (default 250units).										-|
|- Use, '/bark MESSAGE', or, the command 'Bark at NAME MESSAGE' can be given in /bc, /g, or /tell.		-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
#EVENT bark "<#1#> Bark at #2# #3#"
#EVENT bark "#1# tells you, 'Bark at #2# #3#'"
#EVENT bark "#1# tells the group, 'Bark at #2# #3#'"
#EVENT bark "#1# you tell your party, 'Bark at #2# #3#'"
SUB EVENT_bark(paramLine, paramSender, msgTarget, barkMsg)
/if (${outerDebug} || ${Debug_Basics}) /echo |- EVENT_Bark ==>

	|- Verify the event
	/call verifyEvent "${paramSender}" "${paramLine}"
	/if (${Defined[paramSender]}) /if (${Defined[paramSender]}) /varset paramSender ${Macro.Return.Arg[1,_]}
	|-    |-----------Valid User----------|-    |---------Included Bot----------|-    |-------------In Zone-----------|-    |------------In Range------------|
	/if (${Bool[${Macro.Return.Arg[2,_]}]} && ${Bool[${Macro.Return.Arg[3,_]}]} && ${Bool[${Macro.Return.Arg[4,_]}]} && ${Bool[${Macro.Return.Arg[5,_]}]}) {
		
		|- If msgTarget is not an ID, find TargetID.
		/if (!${Int[${msgTarget}]}) {
			/if (${SpawnCount[npc =${msgTarget}]}) {
				/varset msgTarget ${Spawn[npc =${msgTarget}].ID}
			} else /if (${SpawnCount[pc =${msgTarget}]}) {
				/varset msgTarget ${Spawn[npc =${msgTarget}].ID}
			} else /if (${SpawnCount[pet =${msgTarget}]}) {
				/varset msgTarget ${Spawn[npc =${msgTarget}].ID}
			} else {
				/echo Could not find a SpawnID for [${msgTarget}].
				/varset msgTarget NULL
			}
		}
		
		/declare barkTarget string outer ${Spawn[id ${msgTarget}].CleanName}
		/declare barkReturned bool outer FALSE
		/declare retryTimer timer local 150
		/declare startingLoc string local
		
		/if (${Stick.Active}) /squelch /stick off
		/if (${Me.Casting.ID} && ${Me.Class.ShortName.NotEqual[BRD]}) /delay 300 !${Me.Casting.ID}
		
		:retry_Bark
		/if (${outerDebug} && !${outerSpamTimer_retryBark}) {
			/echo |- EVENT_bark -|- :retry_Bark loop
			/call createTimer outerSpamTimer_retryBark 20
		}
		
		|- Target the msgTarget
		/call TrueTarget ${msgTarget}
		/if (${Target.Distance} > 10) {
			/call MoveTo -Loc|${Target.Y},${Target.X}
			/if (${retryTimer}) {
				/goto :retry_Bark
			} else {
				/echo |- EVENT_Bark -|- Failed to move to the intended target.
			}
		} else {
		
			|- Wait a moment, then bark the message.
			/delay ${Math.Rand[30]}
			
			/varset startingLoc ${Me.Loc.Replace[ ,]}
			
			|- Reset barkMsg
			/if (${barkMsg.Find[-]} && ${barkMsg.Find[|]}) {
				/varset barkMsg ${barkMsg.Arg[1,-]}
				/varset barkMsg ${barkMsg.Left[${Math.Calc[${barkMsg.Length} - 1].Int}]}
			}
			
			/say ${barkMsg}
			
			|- Wait up to 3 seconds for something to happen.
			/delay 30 ${Zone.ID} != ${outerCurrentZone} || ${Me.Loc.Replace[ ,].NotEqual[${startingLoc}]} || ${Window[ConfirmationDialogBox].Open} || ${Window[LargeDialogWindow].Open}

			|- Click Yes to a confirmation box.
			/if (${Window[ConfirmationDialogBox].Open}) {
				/notify ConfirmationDialogBox Yes_Button leftmouseup
				/delay 10
			} else /if (${Window[LargeDialogWindow].Open}) {
				/notify LargeDialogWindow LDW_YesButton leftmouseup
				/delay 10
			}
			
			|- Check to see if the bark was successful.
			/call Triangulate_Distance ${Me.Loc.Replace[ ,]} ${startingLoc}
			
			/doevents barkReturn

			/if (${Zone.ID} == ${outerCurrentZone} && ${Macro.Return} < 50 && !${barkReturned}) {
				/if (${retryTimer}) {
					/goto :retry_Bark
				} else {
					/channel Nothing seems to have happened...
				}
			}
		}
		/deletevar barkTarget
		/deletevar barkReturned
	}

/if (${outerDebug} || ${Debug_Basics}) /echo <== EVENT_Bark -|
/RETURN



|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Attempts to detected NPC responses to PC hails in /say.	-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- @param paramLine: '|BarkTarget|- says, 'Something''			-|
|- @param returnBark: What the NPC replied.					-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
#EVENT barkReturn "|${barkTarget}|- #*#"
SUB EVENT_barkReturn(paramLine, returnBark)
/if (${outerDebug} || ${Debug_Basics}) /echo |- EVENT_barkReturn ==>
	
	/varset barkReturned TRUE
	
/if (${outerDebug} || ${Debug_Basics}) /echo <== EVENT_barkReturn -|
/RETURN



|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Moves to a given location, and clicks the nearest object.				-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- @param paramSender: The command sender.									-|
|- @param locToStart: Where to stand when attempting to click the object.	-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
#EVENT clickIt "[#1#] Click object #2# at #3#"
#EVENT clickIt "<#1#> Click object #2# at #3#"
SUB EVENT_clickIt(paramLine, paramSender, paramArgs, locToStart)
	/if (${outerDebug} || ${Debug_Basics}) /echo |- EVENT_clickIt ==>

	|- Verify the event
	/call amIConcerned ${paramSender} ${paramArgs} FALSE
	/if (${Macro.Return}) {

		|- Check LoS.
		/if (!${paramLineOfSight[${Me.Loc.Replace[ ,]}:${locToStart}]}) {
			/channel I cannot see the object to click.
		} else {
			
			/if (${locToStart.Find[ ]}) /varset locToStart ${locToStart.Arg[1, ]}
			
			|- Check range to object.
			/call Triangulate_Distance ${locToStart} ${Me.Loc.Replace[ ,]}
			/if (${Macro.Return} > ${Math.Calc[${MaxResponseDist}*3].Int}) {
				/channel That object is too far away to click.
			} else {
					
				/declare retryTimer timer local 300
				/declare miscTimer timer local
				/declare startZone int local ${Zone.ID}

				/if (${Stick.Active}) /squelch /stick off
				/if (${Me.Casting.ID} && ${Me.Class.ShortName.NotEqual[BRD]}) /delay 300 !${Me.Casting.ID}
		
				:retryClick
				
				|- Check if I need to move closer.
				/call Triangulate_Distance ${locToStart} ${Me.Loc.Replace[ ,]}
				/if (${Macro.Return} > 10) {
					/if (${retryTimer}) {
						/call MoveTo -Loc|${locToStart}
						/goto :retryClick
					} else {
						/channel I failed to move to the inteded object
					}
				} else {
				
					|- Switch to first person camera.
					/keypress First_Person_Camera
				
					|- Face object and attempt to click it for up to 5 seconds.
					/varset miscTimer 50
					/squelch /doortarget
					/squelch /face fast door
					/squelch /look -75

					:keypress
					
					|- Shifts look angle up and down while attempting to click.
					/look ${If[${Me.Look} < 75,${Math.Calc[${Me.Look} + 5]},-75]}
					/if (${Target.ID}) /squelch /target clear
					/keypress U | numpad 9 and numpad 3 or Page-Up and Page-Down -> up and down
					/delay 2
					
					|- Check if I have zoned, or moved.
					/call Triangulate_Distance ${locToStart} ${Me.Loc.Replace[ ,]}
					/if (${Zone.ID} == ${outerCurrentZone} && ${Macro.Return} < 50) {

						|- Check keypressTimer.
						/if (${miscTimer}) {
							/goto :keypress
						} else {
						
							|- Check the retryTimer.
							/if (${retryTimer}) {
								/if (${outerDebug} || ${Debug_Basics}) /echo |- EVENT_clickIt -|- Click attempt failed, retrying.
								/keypress back hold
								/delay 5
								/keypress forward
								/delay ${Math.Rand[30]}
								
								/if (${retryTimer}) /goto :retryClick
						
							|- If the retryTimer has expired, and I have not clicked the object.
							} else {
								/channel I have failed to click the object.
							}
						}
					}
				}
			}
		}
	}

	/if (${outerDebug} || ${Debug_Basics}) /echo <== EVENT_clickIt -|
/RETURN



|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Points faces bots to your current heading, and runs through a zone paramLine.
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- @param paramSender: The command sender.
|- @param locToStart: Where to stand when trying to click the door.
|- @param snapHeading: The direction to face when trying to click the door.
|---------------------------------------------------------------------------------------------------------------------------------------------------|
#EVENT runThruZone "[#1#] Run through zone #2# at #3#"
#EVENT runThruZone "<#1#> Run through zone #2# at #3#"
SUB EVENT_runThruZone(paramLine, paramSender, paramArgs, locToStart)
	/if (${outerDebug} || ${Debug_Basics}) /echo |- EVENT_runThruZone ==>

	|- Verify the event
	/call amIConcerned ${paramSender} ${paramArgs} FALSE
	/if (${Macro.Return}) {

		/declare startHeading float local ${locToStart.Arg[1, ].Arg[3,,]}
		/declare retryTimer timer local 300
		/declare miscTimer timer local
		/declare startZone int local ${Zone.ID}
	
		|- Check that you can see the location to start from.
		/if (!${paramLineOfSight[${Me.Loc.Replace[ ,]}:${locToStart}]}) {
			/channel I cannot see the starting location.
		} else {
		
			|- Check range to object.
			/call Triangulate_Distance ${locToStart} ${Me.Loc.Replace[ ,]}
			/if (${Macro.Return} > ${Math.Calc[${MaxResponseDist}*3].Int}) {
				/channel I am to far away from that zone paramLine.
			} else {
			
				/if (${Stick.Active}) /squelch /stick off
				/if (${Me.Casting.ID} && ${Me.Class.ShortName.NotEqual[BRD]}) /delay 300 !${Me.Casting.ID}
			
				:retryRTZ
			
				|- Check if I need to move closer.
				/call Triangulate_Distance ${locToStart} ${Me.Loc.Replace[ ,]}
				/if (${Macro.Return} > 10) {
					/if (${retryTimer}) {
						/call MoveTo -Loc|${locToStart}
						/goto :retryRTZ
					} else {
						/channel I failed to move to the inteded zone paramLine.
					}
				} else {
				
					|- Face the zone and run foward for 15 seconds.
					/face fast heading ${Math.Calc[${startHeading}*-1]}
					/delay 5
					/keypress forward hold
					
					|- Wait to zone.
					/delay 100 !${Zone.ID} || ${Zone.ID} != ${startZone}
					/if (!${Zone.ID}) /delay 600 ${Zone.ID}
	
					|- Check if I zoned.
					/if (${Zone.ID} == ${startZone}) {
						/if (${retryTimer}) {
							/goto :retryRTZ
						} else {
							/channel I failed to run through the zoneparamLine in [${Zone}].
							/call MoveTo -Loc|${locToStart}
						}
					}
				}
			}
		}
	}

	/if (${outerDebug} || ${Debug_Basics}) /echo <== EVENT_runThruZone -|
/RETURN



|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Drops invis on connected NetBots.
|---------------------------------------------------------------------------------------------------------------------------------------------------|
#EVENT dropInvis "[#1#] Drop Invis #2#"
#EVENT dropInvis "[#1#] Drop Invisibility #2#"
SUB EVENT_dropInvis(paramLine, paramSender, paramArgs)
	/if (${outerDebug} || ${Debug_Basics}) /echo |- EVENT_dropInvis ==>

	|- Verify the event
	/call amIConcerned ${paramSender} ${paramArgs} FALSE
	/if (${Macro.Return}) {
	
		/if (${Me.Invis}) {
		
			/if (${Target.ID}) /declare oldTarget int local ${Target.ID}
			
			/squelch /target clear
			/attack on
			/delay 30 ${Me.Combat}
			/attack off 
			
			/if (${Defined[oldTarget]}) /call TrueTarget ${oldTarget}
		}
	}

/if (${outerDebug} || ${Debug_Basics}) /echo <== EVENT_dropInvis -|
/RETURN



|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Uses /exit on all bots, except the paramSender.	-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Use, /bc Exit, or, -[Exit]- in any channel.		-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
#EVENT Exit "<#1#> Exit #2#"
SUB EVENT_Exit(paramLine, paramSender, paramArgs)
	/if (${outerDebug} || ${Debug_Basics}) /echo |- EVENT_Exit ==>

	|- Verify the event
	/call amIConcerned ${paramSender} ${paramArgs} FALSE
	/if (${Macro.Return}) {

		/channel Bye !
		/deletevar * global
		/disband
		/raiddisband
		/camp desktop
		/end
	}

	/if (${outerDebug} || ${Debug_Basics}) /echo <== EVENT_Exit -|
/RETURN



|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Attempts to load the spellset listed in e3_Settings.ini.	-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
#EVENT LoadSpells "[#1#] Load Spells #2#"
#EVENT LoadSpells "<#1#> Load Spells #2#"
SUB EVENT_LoadSpells(paramLine, paramSender)
	/if (${outerDebug} || ${Debug_Basics}) /echo |- EVENT_LoadSpells ==>

	|- Verify the event
	/call amIConcerned ${paramSender} ${paramArgs} FALSE
	/if (${Macro.Return}) {

		|- Check if I can cast, and have a spellset.	
		/if (${Me.Class.CanCast} && ${Defined[Default_SpellSet]}) {
	
			/declare spellsetMem_Timer timer local 300
			/channel Loading spells...
			/memspellset ${Default_SpellSet}
			:Memloop
			/if (${outerDebug} || ${Debug_Basics}) /echo |- EVENT_LoadSpells -|- :Memloop
			
			/call Background_Events
			/if (${outerDebug} || ${Debug_Basics}) /echo spellsetMem_Timer = [${spellsetMem_Timer}]
			/if (${Window[SpellBookWnd].Open} && ${spellsetMem_Timer}) /goto :Memloop
			/if (${Me.Sitting}) /stand	
		}
	}

	/if (${outerDebug} || ${Debug_Basics}) /echo <== EVENT_LoadSpells -|
/RETURN


	
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Attempts to remove specified buffs on NetBots.	-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Use, /bc Remove [Name], or /bc Remove 'Name'.	-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
#EVENT RemoveBuff "[#1#] Remove buff #2# (#3#)"
#EVENT RemoveBuff "<#1#> Remove buff #2# (#3#)"
SUB EVENT_RemoveBuff(paramLine, paramSender, paramArgs, buffToRemove)
	/if (${outerDebug} || ${Debug_Basics}) /echo |- EVENT_RemoveBuff ==>

	|- Verify the event
	/call amIConcerned ${paramSender} ${paramArgs} FALSE
	/if (${Macro.Return}) {

		/declare badBuff string local ${buffToRemove}
		/declare i int local

		/for i 0 to 29
			
			/if (${Bool[${Window[BuffWindow].Child[BW_Buff${i}_Button].Tooltip}]} && ${Window[BuffWindow].Child[BW_Buff${i}_Button].Tooltip.Find[${badBuff}]}) {
				/varset badBuff ${Window[BuffWindow].Child[BW_Buff${i}_Button].Tooltip.Arg[1,(]}
				/varset badBuff ${badBuff.Left[${Math.Calc[${badBuff.Length}-1]}]}
				/if (${outerDebug} || ${Debug_Basics}) /echo |- EVENT_RemoveBuff -|- Removing [${badBuff}]
				/if (${Bool[${badBuff}]}) /nomodkey /notify BuffWindow Buff${Math.Calc[${Me.Buff[${badBuff}].ID}-1].Int} leftmouseup
			}
			
		/next i
	}

	/if (${outerDebug} || ${Debug_Basics}) /echo <== EVENT_RemoveBuff -|
/RETURN



|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Checks for specified buffs and debuffs on NetBots.		-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Use, /bc Check Buff [Name], or /bc Check buff 'Name'.	-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
#EVENT check_Buffs "[#1#] Check buff #2# (#3#)"
#EVENT check_Buffs "<#1#> Check buff #2# (#3#)"
SUB EVENT_check_Buffs(paramLine, paramSender, paramArgs, buffToCheck)
	/if (${outerDebug} || ${Debug_Basics}) /echo |- EVENT_check_Buffs ==>

	|- Verify the event
	/call amIConcerned ${paramSender} ${paramArgs} FALSE
	/if (${Macro.Return}) {
	
		/declare badBuff string local ${buffToCheck}
		/declare i int local

		/for i 0 to 29
			
			/if (${Bool[${Window[BuffWindow].Child[BW_Buff${i}_Button].Tooltip}]} && ${Window[BuffWindow].Child[BW_Buff${i}_Button].Tooltip.Find[${badBuff}]}) {
				/varset badBuff ${Window[BuffWindow].Child[BW_Buff${i}_Button].Tooltip.Arg[1,(]}
				/varset badBuff ${badBuff.Left[${Math.Calc[${badBuff.Length}-1]}]}
				/if (${outerDebug} || ${Debug_Basics}) /echo |- EVENT_RemoveBuff -|- Removing [${badBuff}]
				/channel I have [${Math.Calc[${Me.Buff[${badBuff}].Duration} * 6 / 60].Int} minutes] left on [${badBuff}].
			}
			
		/next i
	}

	/if (${outerDebug} || ${Debug_Basics}) /echo <== EVENT_check_Buffs -|
/RETURN



|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Searches inventory, and banks, for indicated items.	-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Use, /bc Find 'Name', or, /echo Find 'Name'.			-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
#EVENT FindItems "[#1#] Find Item #2# (#3#)"
#EVENT FindItems "<#1#> Find Item #2# (#3#)"
SUB EVENT_FindItems(paramLine, paramSender, paramArgs, itemToFind)
	/if (${outerDebug} || ${Debug_Basics}) /echo |- EVENT_FindItems ==>

	|- Verify the event
	/call amIConcerned ${paramSender} ${paramArgs} FALSE
	/if (${Macro.Return}) {
	
		|- Declare counting variables.
		/declare i int local
		/declare e int local
		/declare itemName string local

		/if (${FindItemCount[${itemToFind}]} || ${FindItemBankCount[${itemToFind}]} || ${Cursor.Name.Equal[${itemToFind}]}) {
			
			/channel >>> Found (${Math.Calc[${FindItemCount[${itemToFind}]} + ${FindItemBankCount[${itemToFind}]}].Int}) examples of [${itemToFind}] in the following slots ... <<<
			
			|- Check cursor.
			/if (${Cursor.Name.Equal[${itemToFind}]})  /channel >>> Found [${Cursor.Name}] -> [Cursor]. <<<
			
			|- Check Inventory.
			/if (${FindItemCount[${itemToFind}]}) {
			
				|- Search Equippped Items.
				/for i 1 to 22
					/varset itemName ${InvSlot[${i}].Item}
					/if (${itemName.Find[${itemToFind}]}) /channel [${InvSlot[${i}].Item}] -> (${InvSlot[${i}].Name})
				/next i
				
				|- Search packslots.
				/for i 1 to 8	
					/varset itemName ${Me.Inventory[pack${i}]}
					/if (${itemName.Find[${itemToFind}]}) /channel [${Me.Inventory[pack${i}]}] -> (InvSlot${i})
					
					/if (${Me.Inventory[pack${i}].Container}) {
						/for e 1 to ${Me.Inventory[pack${i}].Container}
							/varset itemName ${Me.Inventory[pack${i}].Item[${e}]}
							/if (${itemName.Find[${itemToFind}]}) /channel [${Me.Inventory[pack${i}].Item[${e}]}] -> (${Me.Inventory[pack${i}]} -> InvSlot${i} -> PackSlot${e})
						/next e
					}	
				/next i
			}
			
			/if (${FindItemBankCount[${itemToFind}]}) {
			
				|- Search bank packslots.
				/for i 1 to 26	
					/varset itemName ${Me.Bank[${i}]}
					/if (${itemName.Find[${itemToFind}]}) /channel [${Me.Bank[${i}]}] -> (BankSlot${i})
					
					/if (${Me.Bank[${i}].Container}) {
						/for e 1 to ${Me.Bank[${i}].Container}
							/varset itemName ${Me.Bank[${i}].Item[${e}]}
							/if (${itemName.Find[${itemToFind}]}) /channel [${Me.Bank[${i}].Item[${e}]}] -> (${Me.Bank[${i}]}] -> BankSlot${i} -> PackSlot${e})
						/next e
					}
				/next i
			}
		}
	}

	/if (${outerDebug} || ${Debug_Basics}) /echo <== EVENT_FindItems -|
/RETURN



|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Checks for bots who are out of range, or out of zone.	-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Use, /count, or, /bc Count bots							-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
#EVENT CountBots "[#*#] Count"
#EVENT CountBots "<#*#> Count"
#EVENT CountBots "[#*#] Count Bots"
#EVENT CountBots "<#*#> Count Bots"
#EVENT CountBots "[#*#] RollCall"
#EVENT CountBots "<#*#> RollCall"
#EVENT CountBots "[#*#] Roll Call"
#EVENT CountBots "<#*#> Roll Call"
#EVENT CountBots "[#*#] HeadCount"
#EVENT CountBots "<#*#> HeadCount"
#EVENT CountBots "[#*#] Head Count"
#EVENT CountBots "<#*#> Head Count"
SUB EVENT_CountBots
/if (${outerDebug} || ${Debug_Basics}) /echo |- EVENT_CountBots ==>

	/if (!${NetBots.Counts}) {
		/echo You must be connected to MQ2EQBC, and, MQ2NetBots, to use this function.
	} else {
		/declare i int local
		
		/declare InRangeCount int local
		
		/declare OutOfRange string local
		/declare OutOfRangeCount int local
		
		/declare OutOfZone string local
		/declare OutOfZoneCount int local
		
		/for i 1 to ${NetBots.Counts}
		
			/if (!${SpawnCount[pc ${NetBots.Client[${i}]}]}) {
				/varset OutOfZone ${OutOfZone}${NetBots.Client[${i}]},
				/varcalc OutOfZoneCount ${OutOfZoneCount} + 1
			} else /if (${SpawnCount[pc ${NetBots.Client[${i}]}]} && !${SpawnCount[pc ${NetBots.Client[${i}]} radius ${MaxResponseDist}]}) {
				/varset OutOfRange ${OutOfRange}${NetBots.Client[${i}]},
				/varcalc OutOfRangeCount ${OutOfRangeCount} + 1
			} else {
				/varcalc InRangeCount ${InRangeCount} + 1
			}
		
		/next i
	}
	
	/if (${NetBots.Counts} < 2) {
		/echo ------------------------------------------
		/echo No other NetBots are connected.
		/echo ------------------------------------------
	} else {
		/echo ------------------------------------------
		/echo [${NetBots.Counts}] NetBots connected.
		/echo ------------------------------------------
	}
	
	/if (${InRangeCount} > 1) {
		/echo [${InRangeCount}] NetBots are present.
		/echo ------------------------------------------
	}
	
	/if (${OutOfRange.Length}) {
		/varset i 1
		/echo [${OutOfRangeCount}] NetBots out of range:
		:next_OutOfRange
		/echo ----- ${OutOfRange.Arg[${i},,]}, ${Int[${Spawn[pc ${OutOfRange.Arg[${i},,]}].Distance}]} units away.
		/varcalc i ${i} + 1
		/if (${OutOfRange.Arg[${i},,].Length}) /goto :next_OutOfRange
		/echo ------------------------------------------
	}
	
	/if (${OutOfZone.Length}) {
		/varset i 1

		/echo [${OutOfZoneCount}] NetBots out of zone:
		:next_OutOfZone
		/echo ----- ${OutOfZone.Arg[${i},,]} is in, ${Zone[${NetBots[${OutOfZone.Arg[${i},,]}].Zone}]}.
		/varcalc i ${i} + 1
		/if (${OutOfZone.Arg[${i},,].Length}) /goto :next_OutOfZone
		/echo ------------------------------------------
	}
	
/if (${outerDebug} || ${Debug_Basics}) /echo <== EVENT_CountBots -|
/RETURN



|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Preforms item swaps, listed in iniToons.	-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
#EVENT Swap_Items "<#*#> Swap #1#"
#EVENT Swap_Items "[MQ2] Swap #1#"
SUB EVENT_Swap_Items(paramLine, IniEntry)
/if (${outerDebug} || ${Debug_Basics}) /echo |- Event Swap_Items ==>

	|- If I don't have a matching Ini entry.
	/if (!${Ini[${iniToon},Swap Items,${IniEntry}].Length}) {
		/echo |- Swap_Items -|- I have no listing for [${IniEntry}].
	} else {
	
		/if (${Cursor.ID}) /call ClearCursor
		/if (${Bool[${Me.Casting}]} && ${Me.Class.ShortName.NotEqual[BRD]}) /delay 300 !${Bool[${Me.Casting}]}
	
		/declare i int local 1
		
		/declare itemToSwap string local
		/declare slotToSwap string local
		
		:SwapLoop
		/if (${outerDebug} || ${Debug_Basics}) /echo |- EVENT_Swap_Items -|- Swaploop.
		
		/varset itemToSwap ${Ini[${iniToon},Swap Items,${IniEntry}].Arg[${i},/].Arg[1,|]}
		/varset slotToSwap ${Ini[${iniToon},Swap Items,${IniEntry}].Arg[${i},/].Arg[2,|]}
		
		|- If I don't have the item in my inventory.
		/if (!${FindItemCount[${itemToSwap}]}) {
			/echo |- Event Swap_Items -|- #*# Error #*# [${itemToSwap}] was not found.
			/varcalc i + 1
			/if (${Ini[${iniToon},Swap Items,${IniEntry}].Arg[${i},/].Length}) /goto :SwapLoop
		} else {
		
			|- If I'm swapping in a 2 hander, while dual wielding.
			/if (${slotToSwap.Equal[Mainhand]} && ${FindItem[${itemToSwap}].Type.Left[2].Equal[2H]} && ${Bool[${InvSlot[Offhand].Item}]}) /unequip Offhand
		
			|- Swap in item.
			/echo Equipping [${itemToSwap}]
			/call SwapItem "${itemToSwap}" ${slotToSwap}
			
			/varcalc i ${i} + 1
			/if (${Ini[${iniToon},Swap Items,${IniEntry}].Arg[${i},/].Length}) /goto :SwapLoop
		}
	}

/if (${outerDebug} || ${Debug_Basics}) /echo <== Swap_Items -|
/RETURN



#EVENT useItem "[MQ2] Use Item #1#"
SUB EVENT_useItem(paramLine, itemToUse)
/if (${outerDebug} || ${Debug_Basics}) /echo |- EVENT_useItem ==>

	/if (${Bool[${Cursor}]}) /call ClearCursor

	:TryAgain
	|- if I cant find the item
	/if (!${FindItemCount[=${itemToUse}]}) {
	
		|- check ini for an alias
		/if (${Ini[${iniToon},Use Items,${itemToUse}].Length}) {
			/varset itemToUse ${Ini[${iniToon},Use Items,${itemToUse}]}
			/goto :TryAgain
		} else {
			/echo Could not find [${itemToUse}].
		}
	} else {
		
		|- check item timers
		/if (${FindItem[=${itemToUse}].Timer} || ${itemTimer_${FindItem[=${itemToUse}].ID}}) {
			/echo [${itemToUse}] is not ready! [${If[${FindItem[=${itemToUse}].Timer},${FindItem[=${itemToUse}].Timer} ticks,${Math.Calc[${itemTimer_${FindItem[=${itemToUse}].ID}}/10].Int} sec]}]
		} else {

			|- cast the item
			/if (${Me.Casting.ID}) /call Interrupt
			/if (${Cursor.ID}) /call clearCursor
			/if (${Target.ID}) /declare oldTarget int local ${Target.ID}

			/call thf_Casting ${Me.ID} "${itemToUse}" item
			/if (${oldTarget}) /call TrueTarget ${oldTarget}
		}
	}

/if (${outerDebug} || ${Debug_Basics}) /echo <== EVENT_useItem -|
/RETURN



|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Ends the macro before a bot finishes camping.	-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
#EVENT EndMacro "It will take about 5 more seconds to prepare your camp."
SUB EVENT_EndMacro
	/echo User is camping, ending macro.
	/endmacro
/RETURN



|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Engages MedBreak mode, on bots who can cast.	-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
#EVENT medOn "[#1#] MedBreak On#*#"
#EVENT medOn "<#1#> MedBreak On#*#"
#EVENT medOn "#1# tells you, 'MedBreak On#*#'"
#EVENT medOn "#1# tells the group, 'MedBreak On#*#'"
#EVENT medOn "#1# you tell your party, 'MedBreak On#*#'"
SUB EVENT_medOn(paramLine, paramSender)
/if (${outerDebug} || ${Debug_Basics}) /echo |- EVENT_medOn ==>

	|- Verify the event
	/call verifyEvent "${paramSender}" "${paramLine}"
	/if (${Defined[paramSender]}) /varset paramSender ${Macro.Return.Arg[1,_]}
	|-    |-----------Valid User----------|-    |--------Included Bot-----------|-    |-----------In Zone-------------|
	/if (${Bool[${Macro.Return.Arg[2,_]}]} && ${Bool[${Macro.Return.Arg[3,_]}]} && ${Bool[${Macro.Return.Arg[4,_]}]}) {
	
		|- Check that I'm a caster, and not a bard
		/if (${Me.Class.CanCast} && ${Me.Class.ShortName.NotEqual[BRD]}) {
	
			|- Set medbreak variables
			/varset medBreak TRUE
			/if (${paramLine.Find[ Hold]}) {
				/varset medBreak_Hold TRUE
			} else {
				/varset medBreak_Hold FALSE
			}
			
			|- Check range
			/if (${Spawn[pc ${paramSender}].Distance} < ${MaxResponseDist}) {
				/channel Meditating...
			} else {
				/channel I am too far away from you to meditate, right now.  I'll start meditating when you're in range.
			}
		}
	}
	
/if (${outerDebug} || ${Debug_Basics}) /echo <== EVENT_medOn -|
/RETURN



|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- disengages MedBreak mode, on bots who can cast.	-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
#EVENT medOff "[#1#] MedBreak Off#*#"
#EVENT medOff "<#1#> MedBreak Off#*#"
#EVENT medOff "#1# tells you, 'MedBreak Off#*#'"
#EVENT medOff "#1# tells the group, 'MedBreak Off#*#'"
#EVENT medOff "#1# you tell your party, 'MedBreak Off#*#'"
SUB EVENT_medOff(paramLine, paramSender)
/if (${outerDebug} || ${Debug_Basics}) /echo |- EVENT_medOff ==>

	|- Verify the event
	/call verifyEvent "${paramSender}" "${paramLine}"
	/if (${Defined[paramSender]}) /varset paramSender ${Macro.Return.Arg[1,_]}
	|-    |-----------Valid User----------|-    |--------Included Bot-----------|-    |-----------In Zone-------------|
	/if (${Bool[${Macro.Return.Arg[2,_]}]} && ${Bool[${Macro.Return.Arg[3,_]}]} && ${Bool[${Macro.Return.Arg[4,_]}]}) {

		|- If medbreak is engaged
		/if (${medBreak}) {
	
			|- Reset variables, and stand up
			/channel Ending Medbreak.
			/varset medBreak FALSE
			/varset medBreak_Hold FALSE
			/if (${Me.Sitting}) /stand
		}
	}
	
/if (${outerDebug} || ${Debug_Basics}) /echo <== EVENT_medOff -|
/RETURN



|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- MedBreak hook for Tertiary loop.
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Automatically disengages and re-engages MedBreak when conditions are met.
|- Ends MedBreak mode at 99% mana, unless, 'MedBreak hold command is given.
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB check_MedBreak
	/if (${outerDebug} || ${Debug_Basics}) /echo |- check_MedBreak ==>

	|- If I'm globalFollowing, check distance to follow target
	/if (${globalFollowing} && ${SpawnCount[pc ${globalFollowTarget}]} && ${Spawn[pc ${globalFollowTarget}].Distance} > ${LeashLength}  && !${Me.Moving}) {
		/call check_Follow
		/delay 30 ${Me.Moving}
	}
	
	|- If I'm not moving disengage any active follow plugins
	/if (!${Me.Moving}) {
		/if (${Stick.Active}) /squelch /stick off
	}
	
	|- If End MedBreak in Combat (On/Off)=On, and a netbot is in combat, call medOff
	/if (${medOn_combatBreak} && ${outerCombatTimer}) /call EVENT_medOff "NULL" "${Me.CleanName}"
	
	/if (!${Me.Feigning}) {
	
		|- do ***Meditating*** popup
		/if (!${medbreak_Popup_Timer}) {
			/popup ***Meditating***
			/varset medbreak_Popup_Timer 30
		}
		
		|- sit down, if we are not sitting
		/if (!${Me.Sitting} && !${Me.Casting.ID} && !${Me.Moving}) /sit
		
		|- end MedBreak when we are full mana, unless hold is engaged
		/if (${medBreak} && ${Me.PctMana} >= 99 && !${medBreak_Hold}) {
			/channel I am at full mana, ending MedBreak.
			/if (${Me.Sitting}) /stand
			/varset medBreak FALSE
			/varset medBreak_Hold FALSE
		}
	}
	
/if (${outerDebug} || ${Debug_Basics}) /echo <== check_MedBreak -|
/RETURN



#EVENT set_AAXP "<#1#> Set AA XP #2#"
#EVENT set_AAXP "[#1#] Set AA XP #2#"
SUB EVENT_set_AAXP(paramLine, paramSender, paramArgs)
/if (${outerDebug} || ${Debug_Basics}) /echo |- set_AAXP ==>

	|- Verify the event
	/call verifyEvent "${paramSender}" "${paramLine}"
	/if (${Defined[paramSender]}) /varset paramSender ${Macro.Return.Arg[1,_]}
	|-    |-----------Valid User----------|-    |--------Included Bot-----------|
	/if (${Bool[${Macro.Return.Arg[2,_]}]} && ${Bool[${Macro.Return.Arg[3,_]}]}) {	

		|- reset paramArgs 
		/varset paramArgs ${Int[${paramArgs}]}
	
		/echo Setting AA XP [${paramArgs}%].
		/alternateadv ON ${paramArgs}
	}
	
/if (${outerDebug} || ${Debug_Basics}) /echo <== set_AAXP -|
/RETURN



#EVENT fixCorpses "<#*#> Fix Corpses"
SUB EVENT_fixCorpses
/if (${outerDebug} || ${Debug_Basics}) /echo |- EVENT_fixCorpses ==>

	/echo Attempting to reset corpse positions...
	/hidecorpse all
	/delay 2
	/hidecorpse none
	/delay 2
	/hidecorpse looted

/if (${outerDebug} || ${Debug_Basics}) /echo <== EVENT_fixCorpses -|
/RETURN



|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Consents a PC to drag your corpse.	-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
#EVENT Consent "#1# tells you, 'Consent #2#'"
#EVENT Consent "<#1#> Consent #2#"
SUB EVENT_Consent(paramLine, paramSender, ConsentTarget)
/if (${outerDebug} || ${Debug_Basics}) /echo |- EVENT_Consent ==>

	|- Verify the event
	/call verifyEvent "${paramSender}" "${paramLine}"
	/if (${Defined[paramSender]}) /varset paramSender ${Macro.Return.Arg[1,_]}
	|-    |-----------Valid User----------|-    |--------Included Bot-----------|
	/if (${Bool[${Macro.Return.Arg[2,_]}]} && ${Bool[${Macro.Return.Arg[3,_]}]}) {
		
		/if (${ConsentTarget.NotEqual[${Me.CleanName}]}) {
			/if (${Verbosity}) /channel Consenting ${ConsentTarget}
			/consent ${ConsentTarget}
		}
	}

/if (${outerDebug} || ${Debug_Basics}) /echo <== EVENT_Consent -|
/RETURN



|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Adds a PC name to your list of consentTargets.  This is used is corpse drag functions.	-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
#EVENT addConsent "You have been given permission to drag #1#'s corpse in all zones."
SUB EVENT_addConsent(paramLine, consentTarget)
/if (${outerDebug} || ${Debug_Basics}) /echo |- EVENT_addConsent ==>

	/if (!${Select[${consentTarget},${consentTargets}]}) {
		/varset consentTargets ${consentTargets},${consentTarget}
	}

/if (${outerDebug} || ${Debug_Basics}) /echo <== EVENT_addConsent -|
/RETURN



|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Clears your list of PCs who have consented to have you move their corpse.	-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
#EVENT flushConsent "<#*#> Flush consent"
#EVENT flushConsent "[MQ2] Flush consent"
SUB EVENT_flushConsent
	/echo Flushing my current consented targets.
	/if (${Defined[consentTargets]}) /varset consentTargets ${Me.CleanName}
/RETURN



#EVENT gatherCorpses "[MQ2] Ping corpses"
#EVENT gatherCorpses "[MQ2] Gather corpses"
#EVENT gatherCorpses "[MQ2] Gather corpse"
SUB EVENT_gatherCorpses
/if (${outerDebug} || ${Debug_Basics}) /echo |- EVENT_gatherCorpses ==>

	/declare miscTimer timer local
	/declare i int local
	/for i 1 to ${SpawnCount['s radius 100]}
		/if (${Select[${Spawn[${NearestSpawn[${i},'s radius 100]}].CleanName.Arg[1,']},${consentTargets}]}) {
			
			/call TrueTarget ${Spawn[${NearestSpawn[${i},'s radius 100]}].ID}
			/varset miscTimer 30
			
			:tryAgain
			/corpse
			/delay 10 ${Target.Distance} < 20
			/if (${miscTimer} && ${Target.Distance} > 20) /goto :tryAgain
		}
	/next i
		
	/squelch /target clear

/if (${outerDebug} || ${Debug_Basics}) /echo <== EVENT_gatherCorpses -|
/RETURN



|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- All bots click yes or no to confirmation boxes.
|---------------------------------------------------------------------------------------------------------------------------------------------------|
#EVENT ClickYes	"[#1#] Click #2#"
#EVENT ClickYes	"<#1#> Click #2#"
#EVENT ClickYes	"#1# tells you, 'Click #2#'"
#EVENT ClickYes	"#1# you tell your party, 'Click #2#'"
#EVENT ClickYes	"#1# tells the group, 'Click #2#'"
#EVENT ClickYes	"<#1#> Click #2#"
#EVENT ClickYes	"#1# tells you, 'Click #2#'"
#EVENT ClickYes	"#1# you tell your party, 'Click #2#'"
#EVENT ClickYes	"#1# tells the group, 'Click #2#'"
SUB EVENT_ClickYes(paramLine, paramSender, paramArgs)
/if (${outerDebug} || ${Debug_Basics}) /echo |- EVENT_ClickYes ==>

	|- Verify the event
	/call verifyEvent "${paramSender}" "${paramLine}"
	/if (${Defined[paramSender]}) /if (${Defined[paramSender]}) /varset paramSender ${Macro.Return.Arg[1,_]}
	|-    |-----------Valid User----------|-    |--------Included Bot-----------|
	/if (${Bool[${Macro.Return.Arg[2,_]}]} && ${Bool[${Macro.Return.Arg[3,_]}]}) {
	
		/declare buttonToClick string local ${If[${paramArgs.Find[yes]},Yes,No]}
		/declare miscTimer timer local 100
		
		:clickButton
		/if (${Window[ConfirmationDialogBox].Open}) {
			/notify ConfirmationDialogBox ${buttonToClick}_Button leftmouseup
		} else /if (${Window[LargeDialogWindow].Open}) {
			/notify LargeDialogWindow LDW_${buttonToClick}Button leftmouseup
		}
		/if (${miscTimer}) {
			/if (${Window[ConfirmationDialogBox].Open} || ${Window[LargeDialogWindow].Open}) /goto :clickButton
		}
	}

/if (${outerDebug} || ${Debug_Basics}) /echo <== EVENT_ClickYes -|	
/RETURN



|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Attempt to gate on all bots.							-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Use, /bc gate, /tell 'Name' [Gate], or, /g [Gate].	-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
#EVENT gate "[#1#] Gate"
#EVENT gate	"<#1#> Gate"
#EVENT gate "#1# tells you, 'Gate'"
#EVENT gate "#1# you tell your party, 'Gate'"
#EVENT gate "#1# tells the group, 'Gate'"
#EVENT gate	"<#1#> Gate |/|#*#"
#EVENT gate "#1# tells you, 'Gate |/|#*#'"
#EVENT gate "#1# you tell your party, 'Gate |/|#*#'"
#EVENT gate "#1# tells the group, 'Gate |/|#*#'"
SUB EVENT_gate(paramLine, paramSender)
/if (${outerDebug} || ${Debug_Basics}) /echo |- EVENT_Gate ==>

	|- Verify the event
	/call verifyEvent "${paramSender}" "${paramLine}"
	/if (${Defined[paramSender]}) /if (${Defined[paramSender]}) /varset paramSender ${Macro.Return.Arg[1,_]}
	|-    |-----------Valid User----------|-    |--------Included Bot-----------|
	/if (${Bool[${Macro.Return.Arg[2,_]}]} && ${Bool[${Macro.Return.Arg[3,_]}]}) {

		|- Attempt to gate.
		/if (!${Me.Book[gate]}) {
			/if (${FindItem[Philter of Major Translocation].ID}) {
				/channel Preparing to gate...
				/call thf_Casting ${Me.ID} "Philter of Major Translocation"
			} else /if (${FindItem[Vial of Swirling Smoke].ID}) {
				/channel Preparing to gate...
				/call thf_Casting ${Me.ID} "Vial of Swirling Smoke"
			} else {
				/channel I am not able to gate at this time.
			}
		} else {
			/channel Preparing to gate...
			/call thf_Casting ${Me.ID} "Gate/GiveUpTimer|30s"
		}
	}

/if (${outerDebug} || ${Debug_Basics}) /echo <== EVENT_Gate -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Automatically accepts raid invites.	-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
#EVENT RaidAccept "#*#invites you to join a raid.#*#"
SUB EVENT_RaidAccept
	/raidaccept
/RETURN



|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Forces a raid request in guild.			-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Use, /tell 'Name' Request raid invite	-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
#EVENT Request_RaidInvite "#1# tells you, 'Request raid invite'"
SUB EVENT_Request_RaidInvite(paramLine, paramSender)
/if (${outerDebug} || ${Debug_Basics}) /echo |- Request_RaidInvite ==>

	|- Verify the event
	/call verifyEvent "${paramSender}" "${paramLine}"
	/if (${Defined[paramSender]}) /if (${Defined[paramSender]}) /varset paramSender ${Macro.Return.Arg[1,_]}
	|-    |-----------Valid User----------|
	/if (${Bool[${Macro.Return.Arg[2,_]}]}) {
		/gu Raid invite
	}

/if (${outerDebug} || ${Debug_Basics}) /echo <== Request_RaidInvite -|
/RETURN



|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Forces all bots to disband from their raid.											-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Use, /bc RaidDisband, /bc disband from raid, or /tell 'Name' [Force raid disband].	-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
#EVENT Force_RaidDisband "<#*#> RaidDisband"
#EVENT Force_RaidDisband "<#*#> Disband from raid"
#EVENT Force_RaidDisband "#*# tells you, '[Force Raid Disband]'"
SUB EVENT_Force_RaidDisband
/if (${outerDebug} || ${Debug_Basics}) /echo |- Force_RaidDisband ==>

	|- Verify the event
	/call verifyEvent "${paramSender}" "${paramLine}"
	/if (${Defined[paramSender]}) /if (${Defined[paramSender]}) /varset paramSender ${Macro.Return.Arg[1,_]}
	|-    |-----------Valid User----------|-    |--------Included Bot-----------|
	/if (${Bool[${Macro.Return.Arg[2,_]}]} && ${Bool[${Macro.Return.Arg[3,_]}]}) {
		/bcaa //raiddisband
	}

/if (${outerDebug} || ${Debug_Basics}) /echo <== Force_RaidDisband -|
/RETURN



|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Automatically accepts group invites.	-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
#EVENT Invite "#1# invites you to join a group."
SUB EVENT_Invite(paramLine, paramSender)
/if (${outerDebug}) /echo |- EVENT_Invite ==>

	|- Verify the event
	/call verifyEvent "${paramSender}" "${paramLine}"
	/if (${Defined[paramSender]}) /if (${Defined[paramSender]}) /varset paramSender ${Macro.Return.Arg[1,_]}
	|-    |-----------Valid User----------|
	/if (${Bool[${Macro.Return.Arg[2,_]}]}) {
	
		|- Wait to finish casting
		/if (${Me.Casting.ID} && ${Me.Class.ShortName.NotEqual[BRD]}) /delay 300 !${Me.Casting.ID}
	
		|- If the groupleader was valid, disband a current group, and join the new one.
		|- /if (${Group} || ${Raid.Members}) /call EVENT_Disband
		
		/declare retryTimer timer local 150
		
		:retry_Invite
		/if (${outerDebug}) /echo |- EVENT_Invite -|- retry_Invite loop.
		
		/call Background_Events
		/invite
		/delay 5 ${Group}
		
		/if (!${Group}) {
			/if (${retryTimer}) {
				/goto :retry_Invite
			} else {
				/echo Failed to join the group.
			}
		}
	}	

/if (${outerDebug}) /echo <== EVENT_Invite -|
/RETURN



|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Disband bots from group and raid.	-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Use, /bc disband						-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
#EVENT Disband "#1# tells you, 'Incoming group invite...'"
#EVENT Disband "<#1#> Disband all"
SUB EVENT_Disband(paramLine, paramSender)
/if (${outerDebug}) /echo |- EVENT_Disband ==>

	|- Verify the event
	/call verifyEvent "${paramSender}" "${paramLine}"
	/if (${Defined[paramSender]}) /if (${Defined[paramSender]}) /varset paramSender ${Macro.Return.Arg[1,_]}
	|-    |-----------Valid User----------|
	/if (${Bool[${Macro.Return.Arg[2,_]}]}) {
	
		|- Wait to stop casting.
		/if (${Me.Casting.ID} && ${Me.Class.ShortName.NotEqual[BRD]}) /delay 300 !${Me.Casting.ID}
	
		|- Disband from a raid.
		/if (${Raid.Members}) {
			/call TrueTarget ${Me.ID}
			/raiddisband
			/delay 10 !${Raid.Members}
		}
		
		|- Disband from a group.
		/if (${Group}) {
			/call TrueTarget ${Me.ID}
			/disband
			/delay 10 !${Group}
		}
		
		/squelch /target clear
	}

/if (${outerDebug}) /echo <== EVENT_Disband -|
/RETURN



|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Groupleaders save current group formations to the Groups.ini file, to be recalled later.	-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Use, /savegroup 'Name', /bc Save Group 'Name', or /bc SetGroup 'Name'					-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
#EVENT saveGroup "<#1#> Save Group #2#"
SUB EVENT_saveGroup(paramLine, paramSender, paramArgs)
/if (${outerDebug}) /echo |- EVENT_saveGroup ==>

	|- Verify the event
	/call verifyEvent "${paramSender}" "${paramLine}"
	/if (${Defined[paramSender]}) /varset paramSender ${Macro.Return.Arg[1,_]}
	|-    |-----------Valid User----------|-    |--------Included Bot-----------|
	/if (${Bool[${Macro.Return.Arg[2,_]}]} && ${Bool[${Macro.Return.Arg[3,_]}]}) {
		|- Check that I'm the group leader.
		/if (!${Select[${Group.Leader},${Me}]}) {
			/if (${outerDebug}) /echo I am not a group leader, skipping.
		} else {
			
			/echo Preparing to save...
			/declare groupName string local ${paramArgs.Arg[1,/]]}
			/if (${groupName.Right[1].Equal[ ]}) /varset groupName ${groupName.Left[${Math.Calc[${groupName.Length}-1]}]}
			/delay ${Math.Calc[${NetBots[${Me.CleanName}]} * 3].Int}
			/declare uniqueID int local
			/declare i int local
			/declare e int local
			
			|- Set a random ID to associate with out group name.
			/for i 1 to ${NetBots.Counts}
				/if (${NetBots.Client[${i}].Equal[${Me.CleanName}]}) {
					/varset uniqueID ${i}
				}
			/next i
			
			|- Check to see if existing entries need to be overwritten.
			/if (${Ini[${iniGroup}].Find[${groupName}]}) {
				/echo Overwriting existing saved group(s)...
				/varset i 1

				:next_groupName
				/if (${Ini[${iniGroup}].Arg[${i},|].Length}) {
					/if (${Ini[${iniGroup}].Arg[${i},|].Arg[2,_].Equal[${groupName}]}) {
						/for e 0 to 5
							/call writeToIni "${iniGroup},${Ini[${iniGroup}].Arg[${i},|]},GroupMember#${e}" "DEPRECATED" 1
						/next e
					}
					/varcalc i ${i}+1
					/goto :next_groupName
				}
			}
			
			|- Delay for a moment to allow other bots to catch up, so they don't overwrite our soon to be saved group.
			/delay 20
			|- Save the group configuration
			/echo Group configuration(s) saved. Use [/group ${groupName}] to recall this group setup.
			/for i 0 to 5
				|- Save group members 0-5.
				/if (${Bool[${Group.Member[${i}]}]}) {
					/call writeToIni "${iniGroup},${MacroQuest.Server}_${groupName}_${uniqueID},GroupMember#${i}" ${Group.Member[${i}]} 1
				|- If the group has less that 5 members, set placeholders
				} else {
					/call writeToIni "${iniGroup},${MacroQuest.Server}_${groupName}_${uniqueID},GroupMember#${i}" PLACEHOLDER 1
				}
			/next i
		}
	}

/if (${outerDebug}) /echo <== EVENT_saveGroup -|
/RETURN



|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Lists all saved group setups, on your current server. -|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
#EVENT listGroups "<#1#> List Groups"
#EVENT listGroups "<#1#> ListGroups"
SUB EVENT_listGroups
/if (${outerDebug}) /echo |- listGroups ==>

	|- Check iniGroup for groups on this server.
	/if (!${Ini[${iniGroup}].Find[${MacroQuest.Server}]}) {
		/echo You don't have any group setups saved on, [${MacroQuest.Server}].
	} else {
	
		/declare i int local 1
		/declare e int local
		
		:next_groupListing

		/if (${Ini[${iniGroup}].Arg[${i},|].Arg[1,_].Find[${MacroQuest.Server}]}) {
			/echo
			/echo --- Group: [${Ini[${iniGroup}].Arg[${i},|].Arg[2,_]}] ---
			
			|- Reset 'e' the group member counting variable.
			/varset e 1
			
			:next_memberListing
			
			|- Check that the entry is not a PLACEHOLDER.
			/if (${Ini[${iniGroup},${Ini[${iniGroup}].Arg[${i},|]},${Ini[${iniGroup},${Ini[${iniGroup}].Arg[${i},|]}].Arg[${e},|]}].NotEqual[PLACEHOLDER]}) {
				/echo Group member#${e}: ${Ini[${iniGroup},${Ini[${iniGroup}].Arg[${i},|]},${Ini[${iniGroup},${Ini[${iniGroup}].Arg[${i},|]}].Arg[${e},|]}]}
				/varcalc e ${e} + 1
				/if (${Ini[${iniGroup},${Ini[${iniGroup}].Arg[${i},|]},${Ini[${iniGroup},${Ini[${iniGroup}].Arg[${i},|]}].Arg[${e},|]}].Length}) /goto :next_memberListing
			}
		}	

		/varcalc i ${i} + 1
		/if (${Ini[${iniGroup}].Arg[${i},|].Length}) /goto :next_groupListing
		/echo
	}

/if (${outerDebug}) /echo <== listGroups -|
/RETURN



|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Recalls groups saved in your Groups.ini.		-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Use, /group 'Name', or, /bc Group 'Name'.	-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
#EVENT groupUp "<#1#> Group #2#"
SUB EVENT_groupUp(paramLine, paramSender, paramArgs)
/if (${outerDebug}) /echo |- EVENT_groupUp ==>

	|- Verify the event
	/call verifyEvent "${paramSender}" "${paramLine}"
	/if (${Defined[paramSender]}) /varset paramSender ${Macro.Return.Arg[1,_]}
	|-    |-----------Valid User----------|-    |--------Included Bot-----------|
	/if (${Bool[${Macro.Return.Arg[2,_]}]} && ${Bool[${Macro.Return.Arg[3,_]}]}) {
		
		/declare groupName string local ${paramArgs.Arg[1,/]}
		/if (${groupName.Right[1].Equal[ ]}) /varset groupName ${groupName.Left[${Math.Calc[${groupName.Length}-1]}]}
		/declare groupFound string local
		/declare groupMembers[5] string local
		/declare groupSize int local
		
		/declare i int local 1
		/declare e int local
		
		|- Check for groupName in iniGroup.
		:nextGroupListing
		/if (${Ini[${iniGroup}].Arg[${i},|].Arg[2,_].Equal[${groupName}]} && ${Ini[${iniGroup},${Ini[${iniGroup}].Arg[${i},|]},GroupMember#0].NotEqual[DEPRECATED]}) {
			|- Disband from a current group
			/if (${Group}) /call EVENT_Disband "PLACEHOLDER" "${Me.CleanName}"
				
			|- check to see if I am the group leader(GroupMember#0)
			/if (${Ini[${iniGroup},${Ini[${iniGroup}].Arg[${i},|]},GroupMember#0].Equal[${Me.CleanName}]}) {
				/echo Recalling [${groupName}]...
				/delay 20
				|- Invite group members while ignoring placeholders, and set 
				/for e 1 to 5
					/varset groupMembers[${e}] ${Ini[${iniGroup},${Ini[${iniGroup}].Arg[${i},|]},GroupMember#${e}]}
					/if (${Ini[${iniGroup},${Ini[${iniGroup}].Arg[${i},|]},GroupMember#${e}].NotEqual[PLACEHOLDER]}) {
						/varcalc groupSize ${groupSize}+1
						|- Alert PC of incoming group invite.
						/invite ${groupMembers[${e}]}
					}
				/next e

				/delay 100 ${Group} == ${groupSize}
				/if (${Group} != ${groupSize}) {
					/for e 1 to 5
						/if (${groupMembers[${e}].NotEqual[PLACEHOLDER]} && !${Group.Member[${groupMembers[${e}]}]}) /channel [${groupMembers[${e}]}] Did not join the group...
					/next e
				}
				/goto :skip
			}
		}
		/varcalc i ${i}+1
		/if (${Ini[${iniGroup}].Arg[${i},|].Length}) /goto :nextGroupListing
	}
:skip
/if (${outerDebug}) /echo <== EVENT_groupUp -|
/RETURN



|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Written by Stryd of PEQ -|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
#EVENT reportMana "<#1#> Report mana"
SUB EVENT_reportMana(paramLine,paramSender)
/if (${Me.Class.CanCast}) {
	/if (${outerDebug} || ${Debug_Basics}) /echo |- EVENT_reportMana ==>
		|- Verify the event
		/call verifyEvent "${paramSender}" "${paramLine}"
		/if (${Defined[paramSender]}) /varset paramSender ${Macro.Return.Arg[1,_]}
		|-   |-----------Valid User----------|-    |-----------In Zone-------------|	|--------Included Bot-----------|
		/if (${Bool[${Macro.Return.Arg[2,_]}]} && ${Bool[${Macro.Return.Arg[4,_]}]} && ${Bool[${Macro.Return.Arg[3,_]}]}) {   	
			/channel ${Me.PctMana}m${If[${Me.PctMana}!=100, - Full Mana in ${Math.Calc[((${Me.MaxMana}-${Me.CurrentMana})/${Me.ManaRegen})*6]}s,]}
		}
	/if (${outerDebug} || ${Debug_Basics}) /echo <== EVENT_reportMana [${Me.PctMana}m] -|
}
/RETURN
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Check if I should answer to the order
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB amIconcerned(string from, string who, bool sameZone)
	/if (${outerDebug} || ${Debug_Basics}) {
		/echo |- amIconcerned ==>
		/echo from=${from} |- who=${who}
	}
	
	/if (${sameZone}) {
		/if (!${NetBots[${from}].InZone}) {
			/RETURN FALSE
		}
	}
	/if (${who.Lower.Equal[all]}) {
		/RETURN TRUE
	}
	/if (${who.Lower.Equal[group]}) {
		/if (${Me.Grouped} && ${Group.Member[${from}]}) {
			/RETURN TRUE
		}
	}
	/if (${who.Lower.Equal[caster]}) {
		|- Can cast spells, including Bard 
		/if (${Me.Class.CanCast}) {
			/RETURN TRUE
		}
	}
	/if (${who.Lower.Equal[melee]}) {
		/call amIMelee
		/RETURN ${Macro.Return}
	}
	/if (${who.Lower.Equal[priest]}) {
		|- Cleric/Druid/Shaman? 
		/if (${Me.Class.HealerType}) {
			/RETURN TRUE
		}
	}
	/if (${who.Lower.Equal[nuker]}) {
		|- Wizard/Magician/Necromancer/Enchanter?
		/call amINuker
		/RETURN ${Macro.Return}
	}
	/if (${who.Equal[${Me.Class.Name}]} || ${who.Equal[${Me.Class.ShortName}]}) {
		/RETURN TRUE
	}
	/if (${who.Equal[${Me.CleanName}]}) {
		/RETURN TRUE
	}
/RETURN FALSE

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- TRUE if I'm a Berserk, Beastlord, Monk, Paladin, Ranger, Rogue, ShadowKnight or Warrior
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB amIMelee()
	/if (${Select[${Me.Class.ShortName},BRD,BER,BST,MNK,PAL,RNG,ROG,SHD,WAR]}) {
		/RETURN TRUE
	}
/RETURN FALSE

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- TRUE if I'm a Wizard, Magician, Enchanter or a Necromancer
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB amINuker()
	/if (${Select[${Me.Class.ShortName},WIZ,MAG,ENC,NEC]}) {
		/RETURN TRUE
	}
/RETURN FALSE

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- TRUE if I'm a Beastlord, Magician or a Necromancer (Shaman are not a pet class in THF)
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB amIPetClass()
	/if (${Select[${Me.Class.ShortName},BST,MAG,NEC]}) {
		/RETURN TRUE
	}
/RETURN FALSE

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- TRUE if [@Param from] is within the Netbots client list
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB isOneOfMyToons(String from)
	/if (${NetBots.Client.Find[${from}]}) {
		/RETURN TRUE
	}
/RETURN FALSE

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Remove every unwanted characters from the toon name
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB cleanToonName(String toonName)
	/declare name string local ${toonName}
	/if (${NetBots.Client.Find[${name}]}) {
		/RETURN ${name}
	}
	/varset name ${toonName.Right[-2].Left[-1]}
	/if (${NetBots.Client.Find[${name}]}) {
		/RETURN ${name}
	}
/RETURN ${name}

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- stick off, attack off, interrupt, twist off (if specify)
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB stopCurrentAction(bool evenBard)
	/if (${outerDebug} || ${Debug_Basics}) {
		/echo |- stopCurrentAction ==>
		/echo evenBard=${evenBard}
	}
    /if (${evenBard}) {
        /twist off
    }
    /interrupt
	/target clear
    /stick off
    /attack off
    /delay 1
	/if (${outerDebug} || ${Debug_Basics}) /echo <== stopCurrentAction -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- 
|---------------------------------------------------------------------------------------------------------------------------------------------------|
#EVENT Channels "Channels: #*#"
SUB EVENT_Channels(String line)
	/if (${outerDebug} || ${Debug_Basics}) /echo |- EVENT_Channels ==>

	/declare i int local
	/declare max int local ${line.Count[,]}
	/varcalc max ${max} + 1
	/declare chnl string local
	/for i 1 to ${max}
		/varset chnl ${line.Token[${i},,]}
		/if (${chnl.Find[Team]} > 0) {
			/varset channelNumber ${chnl.Token[1,=]}
			/if (${outerDebug} || ${Debug_Basics}) /echo channel=${channelNumber}
		}
	/next i
	
	/if (${outerDebug} || ${Debug_Basics}) /echo <== EVENT_Channels -|
/RETURN

SUB joinChannel()
	/if (${outerDebug} || ${Debug_Basics}) /echo |- joinChannel ==>
	/join ${channel}
	/doevents Channels
	:channelLoop
	/if (${channelNumber} == 0) {
		/doevents Channels
		/delay 1
		/goto :channelLoop
	}
	/if (${outerDebug} || ${Debug_Basics}) /echo <== joinChannel -|
/RETURN


|---------------------------------------------------------------------------------------------------------------------------------------------------|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|---------------------------------------------------------------------------------------------------------------------------------------------------|

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB basics_Setup
/call iniToVar "${iniGeneral},Debug,Debug Basics (On/Off)" Debug_Basics bool outer
/if (${outerDebug} || ${Debug_Basics}) /echo |- basics_Setup ==>

	/if (!${Defined[globalFollowing]}) /declare globalFollowing bool global FALSE
	/if (!${Defined[globalFollowTarget]}) /declare globalFollowTarget string global NULL
	/if (!${Defined[globalFollowers]}) /declare globalFollowers string global NULL

	/if (!${Defined[consentTargets]}) /declare consentTargets string global ${Me.CleanName}

	/declare MeleeClasses string outer BRD,BER,BST,MNK,PAL,RNG,ROG,SHD,WAR
	/declare PriestClasses string outer CLR,DRU,SHM
	/declare CasterClasses string outer ENC,MAG,NEC,WIZ
	
	/declare medBreak bool outer FALSE
	/declare medBreak_Hold bool outer FALSE
	/declare medbreak_Popup_Timer timer outer
	
	/declare MaxResponseDist int outer 250
	/if (${Ini[${iniGeneral},General,Max Response Distance].Length} && ${Int[${Ini[${iniGeneral},General,Max Response Distance]}]}) /call iniToVar "${iniGeneral},General,Max Response Distance" MaxResponseDist int outer

	/declare LeashLength int outer 100
	/if (${Ini[${iniGeneral},General,Leash Length].Length} && ${Int[${Ini[${iniGeneral},General,Leash Length]}]}) /call iniToVar "${iniGeneral},General,Leash Length" LeashLength int outer
	
	/if (!${Ini[${iniGeneral},General,End MedBreak in Combat(On/Off)].Length}) /call iniToVar "${iniGeneral},General,End MedBreak in Combat(On/Off)" medOn_combatBreak bool outer
	
	|- Add Groups_Ini file path
	/if (!${Ini[${iniToon},File Paths,Saved Groups].Length}) /call writeToIni "${iniToon},File Paths,Saved Groups" "thf_inis\Saved Groups.ini" 1
	
	|- Import Groups_Ini.
	/if (!${Ini[${iniToon},File Paths,Saved Groups].Length}) {
		/echo ERROR: Could not find designated file path for [Saved Groups], please review [${iniToon}] and restart.
		/endmacro
	} else {
		/declare iniGroup string outer ${Ini[${iniToon},File Paths,Saved Groups]}
	}
	
/if (${outerDebug} || ${Debug_Basics}) /echo <== basics_Setup -|
/RETURN



SUB basics_Background_Events

	/doevents Follow
	/doevents MoveHere
	/doevents bark
	/doevents clickIt
	/doevents runThruZone
	/doevents dropinvis
	/doevents add_AutoDestroyItem
	/doevents remove_AutoDestroyItem
	/doevents Stop
	/doevents barkReturn
	/doevents Exit
	/doevents RemoveBuff
	/doevents check_Buffs
	/doevents FindItems
	/doevents CountBots
	/doevents EndMacro
	/doevents Consent
	/doevents Invite
	/doevents Disband
	/doevents saveGroup
	/doevents listGroups
	/doevents groupUp
	/doevents checkPCOnparamLine
	/doevents medOn
	/doevents medOff
	/doevents fixCorpses
	/doevents reportMana
	/doevents addConsent

/RETURN



SUB basics_MacroSettings
	/if (${outerDebug} || ${Debug_Basics}) /echo |- basics_MacroSettings ==>
	
	/call writeToIni "${iniGeneral},Debug,Debug Basics (On/Off)"

	/call writeToIni "${iniGeneral},General,Max Response Distance" 500
	/call writeToIni "${iniGeneral},General,Leash Length" 250
	/call writeToIni "${iniGeneral},General,End MedBreak in Combat(On/Off)" On

	/if (${outerDebug} || ${Debug_Basics}) /echo <== basics_MacroSettings -|
/RETURN



SUB basics_CharacterSettings
/if (${outerDebug} || ${Debug_Basics}) /echo |- basics_CharacterSettings ==>

	/if (!${Ini[${iniToon},Swap Items].Length}) /call writeToIni "${iniToon},Swap Items,(Example) Two Hander" "Lodestar|mainhand"
	/if (!${Ini[${iniToon},Swap Items].Length}) /call writeToIni "${iniToon},Swap Items,(Example) One Handers" "Heartwood Blade|mainhand/Chaotic Black Scimitar|offhand"
	/if (!${Ini[${iniToon},Use Items].Length}) /call writeToIni "${iniToon},Use Items,(Example) ShinkItem" "Bracelet of the Shadow Hive"

/if (${outerDebug} || ${Debug_Basics}) /echo <== basics_CharacterSettings -|
/RETURN



SUB basics_Aliases
/if (${outerDebug} || ${Debug_Basics}) /echo |- basics_Aliases ==>

	/noparse /squelch /alias /followon /bc Follow ${Me.CleanName}
	/noparse /squelch /alias /followme /bc Follow ${Me.CleanName}
	/noparse /squelch /alias /followit /bc Follow ${Target.CleanName}
	/squelch /alias /followoff /bc Stop
	/squelch /alias /stop /bc Stop
	/squelch /alias /mtm /bc Move to me
	/noparse /squelch /alias /mtt /bc Move to ${Target.ID}
	/squelch /alias /movetome /bc Move to me
	/noparse /squelch /alias /movetotarget /bc Move to ${Target.ID}
	/noparse /squelch /alias /Bark /bc Bark at ${Target.ID}
	/noparse /squelch /alias /ClickIt /bc Click object at ${Me.Loc.Replace[ ,]}
	/noparse /squelch /alias /RunThruZone /bc Run through zone at ${Me.Loc.Replace[ ,]},${Me.Heading.Degrees}
	/noparse /squelch /alias /RunThroughZone /bc Run through zone at ${Me.Loc.Replace[ ,]},${Me.Heading.Degrees}
	/noparse /squelch /alias /RTZ /bc Run through zone at ${Me.Loc.Replace[ ,]},${Me.Heading.Degrees}
	/squelch /alias /ClickYes /bc Click yes
	/squelch /alias /ClkYes /bc Click yes
	/squelch /alias /Yes /bc Click yes
	/squelch /alias /ClickNo /bc Click No
	/squelch /alias /ClkNo /bc Click No
	/squelch /alias /No /bc Click No
	/squelch /alias /DropInvis /bc Drop Invisibility
	/squelch /alias /BotExit /bc Exit
	/squelch /alias /Gate /bc Gate
	/squelch /alias /Evac /bc Evac
	/squelch /alias /LoadSpells /bc Load Spells
	/squelch /alias /RemoveBuff /bc Remove Buff
	/squelch /alias /CheckBuff /bc Check Buff
	/squelch /alias /FindItem /bc Find Item
	/noparse /squelch /alias /FindIt /bc Find Item ${Cursor}
	/squelch /alias /CountBots /echo Head Count
	/squelch /alias /Count /echo Head Count
	/squelch /alias /BotCount /echo Head Count
	/squelch /alias /RollCall /echo Head Count
	/squelch /alias /Swap /echo Swap
	/squelch /alias /useitem /echo Use Item
	/squelch /alias /castitem /echo Use Item
	/noparse /squelch /alias /ConsentMe /bc Consent ${Me.CleanName}
	/squelch /alias /BotConsent /bc Consent
	/squelch /alias /Group /bc Group
	/squelch /alias /GroupUp /bc Group
	/squelch /alias /SaveGroup /bc Save Group
	/squelch /alias /SaveGroups /bc Save Group
	/squelch /alias /listgroups /bc List Groups
	/squelch /alias /DisbandAll /bc Disband all
	/squelch /alias /DisbandBots /bc Disband all
	/squelch /alias /medon /bc MedBreak On
	/squelch /alias /medoff /bc MedBreak Off
	/squelch /alias /SetAAXP /bc Set AA XP
	/squelch /alias /PortTo /bc Port to
	/squelch /alias /FixCorpses /bc Fix Corpses
	/squelch /alias /ResetCorpses /bc Fix Corpses
	/squelch /alias /gathercorpses /echo Gather Corpses
	/squelch /alias /mana /bc Report Mana

/if (${outerDebug} || ${Debug_Basics}) /echo <== basics_Aliases -|
/RETURN

|- -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
|- -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
|- -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
|**

|- Add Filter File Path
/if (!${Ini[${iniToon},File Paths,Filter Settings].Length}) /call writeToIni "${iniToon},File Paths,Filter Settings" "Macro Inis\Filter Settings.ini"

|- Filter Settings
/if (!${Ini[${iniToon},File Paths,Filter Settings].Length}) {
	/echo ERROR: Could not find designated file path for [Filter Settings].  Please review review settings in [${iniToon} > File Paths].
	/endmacro
} else {
	/declare filterSettings_ini string outer ${Ini[${iniToon},File Paths, Filter Settings]}
}


	/squelch /alias /filters /echo Server Filters: 
	/squelch /alias /savefilters /echo Save Filters: 


|- Usage: /filters refName
|- Must have created an entry in the Filter Settings.ini file for refName by using /savefilters refName
#EVENT serverFilters "[MQ2] Server Filters: #1#"
SUB EVENT_serverFilters(paramLine,refName)
/if (${outerDebug} || ${Debug_Basics}) /echo |- EVENT_serverFilters [${refName}] ==>
/if (${Ini[${filterSettings_ini},${refName}].Length}) {
	/call openOptions Filters
	/declare filterList string local Window[OptionsWindow].Child[OFP_FilterList]
	/declare settingNum int local
	/declare i int local
	/call iniToArray2 "${filterSettings_ini}" "${refName}" "filterSettings" "outer"
	/for i 1 to 32
		/varset settingNum 1
		|- If the current filter setting is not equal to the ini's filter setting...
		/if (${${filterList}.List[${i},2].NotEqual[${filterSettings[${i},2]}]}) {
			|- Select the setting to be adjusted.
			/notify OptionsWindow OFP_FilterList listselect ${i}
			|- Find the position number of the desired setting in the combo box.
			:findSetting
			/if (${Window[OptionsWindow].Child[OFP_FilterComboBox].List[${settingNum}].Equal[${filterSettings[${i},2]}]}) {
				|- Select the desired setting in the combo box
				/notify OptionsWindow OFP_FilterComboBox listselect ${settingNum}
			} else {
				|- Loop until the desired setting is found
				/varcalc settingNum ${settingNum}+1
				/goto :findSetting
			}
		}
	/next i
	/deletevar filterSettings
} else {
	/echo ${refName} is not a valid saved filter setting! Use "/savefilters ${refName}" to save your desired filter settings for future use.
}
|- Close the options window
/keypress ALT+O
/if (${outerDebug} || ${Debug_Basics}) /echo <== EVENT_serverFilters [${filterSetting}] -|
/RETURN



|- Usage: /savefilters refName
|- Saves current chat filter settings to the Filter Settings.ini file
#EVENT saveFilters "[MQ2] Save Filters: #1#"
SUB EVENT_saveFilters(paramLine,refName)
/if (${outerDebug} || ${Debug_Basics}) /echo |- EVENT_saveFilters [${refName}] ==>
/call openOptions Filters
/declare filterNames[32] string local
/declare filterSettings[32] string local
/declare i int local
/declare filterList string local Window[OptionsWindow].Child[OFP_FilterList]
/for i 1 to 32
	/ini "${filterSettings_ini}" "${refName}" "${${filterList}.List[${i},1]}" "${${filterList}.List[${i},2]}"
/next i
/if (${outerDebug} || ${Debug_Basics}) /echo <== EVENT_saveFilters -|
/RETURN


|- Opens the options window and opens to a specified tabName. If given no tabName, opens to General tab.
SUB openOptions(string tabName)
/if (${outerDebug} || ${Debug_Basics}) /echo |- openOptions [${tabName}] ==>
|- open the options window. this happens even if no tab is given. default tab is general.
/declare tabNum int local 1
/declare pageName string local OptionsGeneralPage
:optionsOpen
/if (!${Window[OptionsWindow].Open}) /keypress ALT+O
/delay 20 ${Window[OptionsWindow].Open}
/if (!${Window[OptionsWindow].Open}) {
	/echo Could not open the options window using ALT+O. Please make sure this is the correct keybind. Trying again!
	/goto :optionsOpen
}
/if (${tabName.Equal[General]}) {
	|- use defaults above
} else /if (${tabName.Equal[Display]}) {
	/varset tabNum 2
	/varset pageName OptionsDisplayPage
} else /if (${tabName.Equal[Mouse]}) {
	/varset tabNum 3
	/varset pageName OptionsMousePage
} else /if (${tabName.Equal[Keys]}) {
	/varset tabNum 4
	/varset pageName OptionsKeyboardPage
} else /if (${tabName.Equal[Chat]} || ${tabName.Equal[Filters]}) {
	/varset tabNum 5
	/varset pageName OptionsChatPage
} else /if (${tabName.Equal[Colors]}) {
	/varset tabNum 6
	/varset pageName OptionsColorPage
} else /if (${tabName.Equal[Mail]}) {
	/varset tabNum 7
	/varset pageName OptionsMailPage
} else /if (${tabName.Equal[Voice]}) {
	/varset tabNum 8
	/varset pageName OptionsVoicePage
}
:openTab
/if (!${Window[OptionsWindow].Child[${pageName}].Open}) /notify OptionsWindow OPTW_OptionsSubwindows tabselect ${tabNum}
/delay 20 ${Window[OptionsWindow].Child[${pageName}].Open}
/if (!${Window[OptionsWindow].Child[${pageName}].Open}) {
	/echo Could not open ${tabName} tab. Retrying...
	/goto :openTab
}
/if (${outerDebug} || ${Debug_Basics}) /echo <== openOptions -|
/RETURN









#EVENT corpseDrag "[MQ2] drag corpses"
#EVENT corpseDrag "[MQ2] start corpse drag"
SUB EVENT_corpseDrag
/if (${outerDebug}) /echo |- EVENT_corpseDrag ==>

	/declare i int local 1
	
	:next_consentTarget
	/if (${Me.Running}) /keypress "ctrl r" release
	/if (${consentTargets.Arg[${i},,].Length}) {

		/if (${SpawnCount[${consentTargets.Arg[${i},,]}'s radius 100]} && !${SpawnCount[${consentTargets.Arg[${i},,]}'s radius 10]}) {
		
			/call TrueTarget ${Spawn[${consentTargets.Arg[${i},,]}'s].ID}
			/corpse
			/delay 1
		}
		
		
		/varcalc i ${i} + 1
		/goto :next_consentTarget
	} else {
		/varset i 1
		/goto :next_consentTarget
	}
	
/if (${outerDebug}) /echo <== EVENT_corpseDrag -|
/RETURN

|- need some way to turn off corpse drag; but, I'm bored, and dont want to do it.

#EVENT add_AutoDestroyItem "[MQ2] Add Auto Destroy Item #1#"
SUB EVENT_add_AutoDestroyItem(paramLine, WhatToAdd)
/if (${outerDebug} || ${Debug_Basics}) /echo |- add_AutoDestroyItem ==>

	/if (!${Cursor.ID}) {
		/echo You must be holding the item you wish to add to the auto destroy list.
	} else {
		/if (${WhatToAdd.NotEqual[${Cursor.Name}]}) {
			/echo ERROR: The item on your cursor does not match the auto destroy request.  Please try again.
		} else {
			/if (${Select[${WhatToAdd},${Ini[${Macro_Ini},Background,Auto-Destroy List]}]}) {
				/echo [${WhatToAdd}] is already on the Auto-Destroy List.
			} else {
				/echo Adding [${WhatToAdd}] to the Auto-Destroy list.
				/if (!${Ini[${Macro_Ini},Background,Auto-Destroy List].Length}) {
					/ini "${Macro_Ini}" "Background" "Auto-Destroy List" "${WhatToWrite}"
				} else {
					/ini "${Macro_Ini}" "Background" "Auto-Destroy List" "${Ini[${Macro_Ini},Background,Auto-Destroy List]},${WhatToWrite}"
				}
			}
		}
	}

/if (${outerDebug} || ${Debug_Basics}) /echo <== add_AutoDestroyItem -|
/RETURN

#EVENT remove_AutoDestroyItem "[MQ2] Remove Auto Destroy Item #1#"
SUB EVENT_remove_AutoDestroyItem(paramLine, WhatToAdd)
/if (${outerDebug} || ${Debug_Basics}) /echo |- remove_AutoDestroyItem ==>

	/if (!${Select[${WhatToAdd},${Ini[${Macro_Ini},Background,Auto-Destroy List]}]}) {
		/echo [${WhatToAdd}] is not on the Auto-Destroy List.
	} else {
		/echo this function is not yet finished
	}

/if (${outerDebug} || ${Debug_Basics}) /echo <== remove_AutoDestroyItem -|
/RETURN