
SUB loadCustomVars
	/declare stickCommand string outer ${Ini[${iniGlobal},Global,stickCommand]}
/RETURN

SUB joinChannel
	/join Team
	/doevents
	:channelLoop
	/if (${channel} == 0) {
		/doevents
		/goto :channelLoop
	}
/RETURN

SUB isHolyShit
	/if (${Me.Combat} && !${Me.Casting.ID} && !${Cursor.ID}) {
		/RETURN TRUE
	}
/RETURN FALSE

SUB isDownShit
	/if (!${Me.Combat} && !${Me.Casting.ID} && !${Cursor.ID}) {
		/RETURN TRUE
	}
/RETURN FALSE

SUB instantBuff
	/declare i int local
	/declare myInstantList string local ${Ini[${iniSetup},InstantBuff]}
	/declare myInstantListCount int local ${myInstantList.Count[|]}
	/varcalc myInstantListCount ${myInstantListCount}-1
	/declare myInstantLine string local
	/declare myInstant string local
	/declare mySlot string local
	/declare myInstantSpell string local

	/if (${myInstantListCount} > 0) {
		/for i 1 to ${myInstantListCount}
			/varset mySlot ${myInstantList.Token[${i},|]}
			/varset myInstantLine ${Ini[${iniSetup},InstantBuff,${mySlot}]}
			|- en 1, l'item
			/varset myInstant ${myInstantLine.Token[1,/]}
			/varset myInstantSpell ${FindItem[=${myInstant}].Spell}
			/if (${Spell[${myInstantSpell}].Stacks} && !${Me.Buff[${myInstantSpell}].ID}) {
				/casting "${myInstant}" ${mySlot}
			}
		/next i
	}
/RETURN

SUB checkSpellBuff
	/declare i int local
	/declare targetID int local
	/declare mySpellList string local ${Ini[${iniSetup},SpellBuff]}
	/declare mySpellListCount int local
	/varcalc mySpellListCount ${mySpellList.Count[|]}-1
	/declare mySpellLine string local
	/declare mySpell string local
	/declare myGem string local
	/declare targetName string local

	/if (${mySpellListCount} > 0) {
		/for i 1 to ${mySpellListCount}
			/varset myGem ${mySpellList.Token[${i},|]}
			/varset mySpellLine ${Ini[${iniSetup},SpellBuff,${myGem}]}
			/varset mySpell ${mySpellLine.Token[1,/]}
			/varset targetName ${mySpellLine.Token[2,/]}
			|- Si on a une target pour le spell, c'est que ce n'est pas un group spell
			/if (${targetName.Length}) {
				/varset targetID ${Spawn[pc ${targetName} los radius ${Spell[${mySpell}].MyRange}].ID}
				/if (${Me.CurrentMana} > ${Spell[${mySpell}].Mana} && ${Me.SpellReady[${mySpell}]} && ${targetID} && !${Me.Moving}) {
					/call checkTargetNeedBuff ${targetName} ${Spell[${mySpell}].ID}
					/if (${Macro.Return}) {
						/if (${targetID}) {
							/chat ${channel} casting "${mySpell}" on [${targetName}]
							/casting "${mySpell}" ${myGem} -maxtries|3 -targetid|${Spawn[pc ${targetName}].ID}
							/delay 3s
						}
					}
				}
			} else {
				/if (${Spell[${mySpell}].Mana} > ${Me.CurrentMana} && ${Spell[${mySpell}].Stacks} && !${Me.Moving}) {
					|- Verifier la presence de tous les membres du groupe avant de caster le bazar
					/call checkGroupNeedBuff ${Spell[${mySpell}].ID}
					/if (${Macro.Return}) {
						/chat ${channel} casting "${mySpell}"
						/casting "${mySpell}" ${myGem} -maxtries|3
					}
				}
			}
		/next i
	}
/RETURN

SUB checkTargetNeedBuff(string targetName, int spellID)
	/declare i int local
	/declare targetBuffs string local
	/declare targetBuff	string local
	
	/varset targetBuffs ${NetBots[${targetName}].Buff}
	/if (${targetBuffs.Find[${spellID}]}) {
		/RETURN FALSE
	}
	/for i 1 to ${targetBuffs.Count[ ]} - 1
		/varset targetBuff ${targetBuffs.Token[${i}, ]}
		/if (${targetBuff.Length}) {
			/if (!${Spell[${spellID}].WillStack[${Spell[${targetBuff}]}]}) {
				/chat ${channel} '${Spell[${spellID}].Name}' won't stack with '${Spell[${targetBuff}]}' !
				/RETURN FALSE
			}
		}
	/next i
/RETURN TRUE

SUB checkGroupNeedBuff(int spellID)

/RETURN FALSE

SUB checkDiscipline
	/declare i int local
	/declare myDiscList string local ${Ini[${iniSetup},Discipline]}
	/declare myDiscListCount int local
	/varcalc myDiscListCount ${myDiscList.Count[|]}-1
	/declare myDisc string local
	/declare myNumber string local
	
	/if (${myDiscListCount} > 0) {
		/for i 1 to ${myDiscListCount}
			/varset myNumber ${myDiscList.Token[${i},|]}
			/varset myDisc ${Ini[${iniSetup},Discipline,${myNumber}]}
			/if (${Me.CombatAbilityReady[${myDisc}]} && ${Target.PctHPs}<99) {
				/disc ${myDisc}
			}
		/next i
	}
/RETURN

SUB checkHolyShitBuff
	/declare i int local
	/declare j int local
	/declare myHSBList string local ${Ini[${iniSetup},HolyShitBuff]}
	/declare myHSBListCount int local ${myHSBList.Count[|]}
	/varcalc myHSBListCount ${myHSBListCount}-1
	/declare myHSBLine string local
	/declare myHSB string local |- HolyShitBuff
	/declare myHSS string local |- HolyShitSpell
	/declare mySlot string local
	/declare recastTime string local
	/declare itemCleanName string local
	/declare targetName string local
	/declare NewString string local

	/if (${myHSBListCount} > 0) {
		/for i 1 to ${myHSBListCount}
			/varset mySlot ${myHSBList.Token[${i},|]}
			/varset myHSBLine ${Ini[${iniSetup},HolyShitBuff,${mySlot}]}
			|- en 1, l'item
			/varset myHSB ${myHSBLine.Token[1,/]}
			|- en 2, le timer
			/varset recastTime ${myHSBLine.Token[2,/]}
			|- en 3, la target
			/varset targetName ${myHSBLine.Token[3,/]}
			/varset itemCleanName ${myHSB.Replace[ ,].Replace[',].Replace[.,]}
			/if (${itemCleanName.Find[,]}) {
				/varset NewString
				/for j 1 to ${itemCleanName.Length}
					/if (${itemCleanName.Mid[${j},1].NotEqual[,]}) {
						/varset NewString ${NewString}${itemCleanName.Mid[${j},1]}
					}
				/next j
				/varset itemCleanName ${NewString}
			}
			/if (!${Defined[${itemCleanName}Timer]}) /declare ${itemCleanName}Timer timer outer 0
			/varset myHSS ${FindItem[=${myHSB}].Spell}
			/if (${Spell[${myHSS}].Stacks} && !${Me.Buff[${myHSS}].ID} && ${${itemCleanName}Timer} <= 0 && !${Me.Moving}) {
				/echo Casting "${myHSB}" ${mySlot}
				/casting "${myHSB}" ${mySlot}
				/varset ${itemCleanName}Timer ${recastTime}
			} else {
				/if (${${itemCleanName}Timer} <= 0 && !${Me.Moving} && ${itemCleanName.Lower.Equal[artoriastheabysswalker]}) {
					/echo Casting "${myHSB}" ${mySlot}
					/casting "${myHSB}" ${mySlot}
					/varset ${itemCleanName}Timer ${recastTime}
				}
			}
		/next i
	}
/RETURN

SUB checkDownShitBuff
	/declare i int local
	/declare myDSBList string local ${Ini[${iniSetup},DownShitBuff]}
	/declare myDSBListCount int local ${myDSBList.Count[|]}
	/varcalc myDSBListCount ${myDSBListCount}-1
	/declare myDSBLine string local
	/declare myDSB string local |- DownShitBuff
	/declare myDSS string local |- DownShitSpell
	/declare mySlot string local
	/declare recastTime string local
	/declare itemCleanName string local
	/declare targetName string local

	/if (${myDSBListCount} > 0) {
		/for i 1 to ${myDSBListCount}
			/varset mySlot ${myDSBList.Token[${i},|]}
			/varset myDSBLine ${Ini[${iniSetup},DownShitBuff,${mySlot}]}
			|- en 1, l'item
			/varset myDSB ${myDSBLine.Token[1,/]}
			|- en 2, le timer
			/varset recastTime ${myDSBLine.Token[2,/]}
			|- en 3, la target
			/varset targetName ${myDSBLine.Token[3,/]}
			/varset itemCleanName ${myDSB.Replace[ ,].Replace[',].Replace[.,]}
			/if (!${Defined[${itemCleanName}Timer]}) /declare ${itemCleanName}Timer timer outer 0
			/varset myDSS ${FindItem[=${myDSB}].Spell}
			/if (${Spell[${myDSS}].Stacks} && !${Me.Buff[${myDSS}].ID} && ${${itemCleanName}Timer} <= 0 && !${Me.Moving}) {
				/casting "${myDSB}" ${mySlot} -targetid|${Spawn[pc ${targetName}].ID}
				/varset ${itemCleanName}Timer ${recastTime}
				/delay 3s
			}
		/next i
	}
/RETURN

SUB EVENT_Dead
	|/call Wait4Rez
	/ssm max
/RETURN

SUB EVENT_Camp
	/echo Camping...
	/end
/RETURN

SUB EVENT_Hail(String Line, String Cible)
	/declare toTheZone string local
	/varset toTheZone ${Ini[${iniHail},General,${Cible.Lower}]}
	/if (${toTheZone.Length} && ${toTheZone.NotEqual[NULL]}) {
		/say ${toTheZone}
	} else {
		/echo ${Line}
	}
	/RETURN

SUB EVENT_Task(String Line, String Tache)
	/tell ${myMain} ${Line}
/RETURN

SUB EVENT_Assign(String Line, String Tache)
	/tell ${myMain} ${Line}
/RETURN

SUB checkPlugIn
	/if (!${Plugin[MQ2Cast].Name.Length}) {
		/echo MQ2Cast is a REQUIRED PLUGIN
		/echo Please Load MQ2Cast before running this macro
		/end
	}
/RETURN

SUB EVENT_Tell(Line, string From, string Msg)
	/if (${From.Find[${myMain}]}) {
		/if (${Msg.Equal[loot]}) {
			/call LootMobs
		}
		/if (${Msg.Lower.Find[find].NotEqual[NULL]}) {
			
		}
	} else {
		/if (${Me.CleanName.NotEqual[${myMain}]}) {
			/chat ${channel} ${Line}
		}
	}
/RETURN

SUB EVENT_Group(Line, string From)
	/if (${From.Find[${myMain}]}) {
		/if (!${Me.Grouped}) {
			/invite
		}
	}
/RETURN

SUB EVENT_DraekonVoice(string Line, string targetName)
	/moveto id ${Spawn[npc ${targetName}].ID}
/RETURN


SUB EVENT_DraekonVoiceEnd(string Line, string targetName)
	/moveto id ${Spawn[npc Draekon].ID}
/RETURN


SUB EVENT_DraekonNoGrass
	/if (${Me.Class.Name.Equal[Warrior]} || ${Me.Class.Name.Equal[Paladin]} || ${Me.Class.Name.Equal[Shadowknight]}) /RETURN

	/declare EndX float local			-1609.88
	/declare EndY float local			308.23

	/call InternalMoveTo ${EndY} ${EndX}
/RETURN


SUB EVENT_DraekonGrass
	/moveto id ${Spawn[npc Draekon].ID}
/RETURN


SUB EVENT_DraekonBelly
	/moveto id ${Spawn[npc Draekon].ID}
/RETURN

SUB InternalMoveTo(int y, int x)

	/declare currentX int local 0
	/declare currentY int local 0
	/declare currentZ int local 0
	/moveto loc ${y} ${x}
:waitUntilMovingFinished
	/if (${Math.Distance[${y},${x},${Me.Z}]} > 15 || ${Math.Distance[${currentY},${currentX},${currentZ}]} > 5) {
		/varset currentX ${Me.X}
		/varset currentY ${Me.Y}
		/varset currentZ ${Me.Z}
		/delay 1
		/doevents
		/goto :waitUntilMovingFinished
	}

/RETURN

SUB isOneOfMyToons(String from)
	/if (${NetBots.Client.Find[${from}]}) {
		/RETURN TRUE
	}
/RETURN FALSE

SUB cleanToonName(String toonName)
	/if (${NetBots.Client.Find[${toonName}]}) {
		/RETURN ${toonName}
	}
	/if (${NetBots.Client.Find[${toonName.Right[-2].Left[-1]}]}) {
		/RETURN ${toonName.Right[-2].Left[-1]}
	}
/RETURN ${toonName}

SUB sticking(String to)
    /if (${Spawn[${to}].ID} == ${Me.ID}) {
        /RETURN
    }
	/call stopCurrentAction FALSE
    /target ${to}
    /face fast
    /delay 1s
    /stick ${stickCommand}
    /chat ${channel} Sticking to ${to}
/RETURN

|- Allow toon to gate to the Forest
SUB gate(String from, String who)
	/echo gate
    /call amIconcerned ${from} ${who}
    /if (${Macro.Return}) {
        /call stopCurrentAction TRUE
        /declare CharmOfQueenAdrianne string local ${FindItem[Charm of Queen Adrianne].Name}
        /if (${CharmOfQueenAdrianne.Length} > 0 && !${CharmOfQueenAdrianne.Equal[NULL]}) {
			/if (${from.Equal[${Me.CleanName}]}) {
				/delay 10s
			}
            /casting "${CharmOfQueenAdrianne}" charm
            /chat ${channel} Gating to the Forest !
        }
    }
/RETURN

|- stick off, attack off, interrupt, twist off (if specify)
SUB stopCurrentAction(bool evenBard)
	/echo stopCurrentAction ${evenBard}
    /if (${evenBard}) {
        /twist off
    }
	/target clear
    /stick off
    /attack off
    /interrupt
    /delay 1
/RETURN

SUB amIMelee
	/if (${Me.Class.ShortName.Equal[WAR]) {
		/RETURN TRUE
	}
	/if (${Me.Class.ShortName.Equal[ROG]) {
		/RETURN TRUE
	}
	/if (${Me.Class.ShortName.Equal[BSK]) {
		/RETURN TRUE
	}
	/if (${Me.Class.ShortName.Equal[MNK]) {
		/RETURN TRUE
	}
	/if (${Me.Class.ShortName.Equal[BRD]) {
		/RETURN TRUE
	}
	/if (${Me.Class.ShortName.Equal[PAL]) {
		/RETURN TRUE
	}
	/if (${Me.Class.ShortName.Equal[SHD]) {
		/RETURN TRUE
	}
	/if (${Me.Class.ShortName.Equal[RNG]) {
		/RETURN TRUE
	}
	/if (${Me.Class.ShortName.Equal[BST]) {
		/RETURN TRUE
	}
/RETURN FALSE

SUB amINuker
	/if (${Me.Class.ShortName.Equal[WIZ]) {
		/RETURN TRUE
	}
	/if (${Me.Class.ShortName.Equal[MAG]) {
		/RETURN TRUE
	}
	/if (${Me.Class.ShortName.Equal[ENC]) {
		/RETURN TRUE
	}
	/if (${Me.Class.ShortName.Equal[NEC]) {
		/RETURN TRUE
	}
/RETURN FALSE

SUB amIconcerned(String from, String who)
	/echo amIconcerned ${from} ${who}
	/if (${who.Equal[all]}) {
		/RETURN TRUE
	}
	/if (${who.Equal[group]}) {
		/if (${Me.Grouped} && ${Group.Member[${from}]}) {
			/RETURN TRUE
		}
	}
	/if (${who.Equal[caster]}) {
		|- Can cast spells, including Bard 
		/if (${Me.Class.CanCast}) {
			/RETURN TRUE
		}
	}
	/if (${who.Equal[melee]}) {
		|- Monk/Rogue/Bard/Ranger/Warrior/Paladin/ShadowKnight?
		/call amIMelee
		/RETURN ${Macro.Return}
	}
	/if (${who.Equal[priest]}) {
		|- Cleric/Druid/Shaman? 
		/if (${Me.Class.HealerType}) {
			/RETURN TRUE
		}
	}
	/if (${who.Equal[nuker]}) {
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

SUB makeGroup
	/if (${Me.CleanName.Equal[conradd]}) {
		/invite tiarra
		/delay 1
		/invite tifania
		/delay 1
		/invite brocelia
		/delay 1
		/invite gaya
		/delay 1
		/invite houdi
		/RETURN
	}
	/if (${Me.CleanName.Equal[vivyana]}) {
		/invite arsen
		/delay 1
		/invite aiguisee
		/delay 1
		/invite fandril
		/delay 1
		/invite shoalin
		/delay 1
		/invite korale
		/RETURN
	}
	/if (${Me.CleanName.Equal[namea]}) {
		/invite margona
		/delay 1
		/invite mirlen
		/delay 1
		/invite elmunster
		/delay 1
		/invite malefica
		/delay 1
		/invite aede
		/RETURN
	}
/RETURN

SUB LPS
	|- ---------------------------------------------------------------------
	|- Declare loops / second variables
	|- ---------------------------------------------------------------------
	/if (!${Defined[looptest]}) 	/declare looptest 	int outer 0
	/if (!${Defined[loops]}) 		/declare loops 		int outer 0
	/if (!${Defined[looptime]}) 	/declare looptime 	int outer ${Time.Second}
	/if (!${Defined[loopsecmax]}) 	/declare loopsecmax int outer 0
	/if (!${Defined[loopsecmin]}) 	/declare loopsecmin int outer 10000
	/if (!${Defined[loopcount]}) 	/declare loopcount 	int outer 0

	/if (${Time.Second}>=${Math.Calc[${looptime}+1]}) {
		/if (${looptest}>${loopsecmax}) /varset loopsecmax ${looptest}
		/if (${looptest}<${loopsecmin} && ${looptest}) /varset loopsecmin ${looptest}
		/varset loops ${looptest}
		/varset looptest 0
		/varset looptime ${Time.Second}
	} else /if (${Time.Second}==0 && ${looptime}!=0) {
		/echo LPS:${looptest} loop/sec min:${loopsecmin} max:${loopsecmax}
		/varset looptest 0
		/varset looptime 0
		/varset loopsecmax 0
		/varset loopsecmin 10000
	}
/RETURN

|- Allow toon to the guild hall
SUB goToGuildHall(String from, String who)
	/call gate ${from} ${who}
    /call amIconcerned ${from} ${who}
    /if (${Macro.Return}) {
        /call stopCurrentAction TRUE
        /declare CharmOfQueenAdrianne string local ${FindItem[Charm of Queen Adrianne].Name}
        /if (${CharmOfQueenAdrianne.Length} > 0 && !${CharmOfQueenAdrianne.Equal[NULL]}) {
            /casting "${CharmOfQueenAdrianne}" charm
            /chat ${channel} Gating to the Forest !
        }
    }
/RETURN

SUB test()
	|/call putLinkInChatWindow
/RETURN

SUB putLinkInChatWindow
	/notify InventoryWindow InvSlot13 rightmouseheld
	/delay 1
	/keypress Enter
	/shiftkey /keypress . chat
	/nomodkey /keypress c chat
	/nomodkey /keypress h chat
	/nomodkey /keypress q chat
	/nomodkey /keypress t chat
	/nomodkey /keypress space chat
	/shiftkey /keypress ${channel} chat
	/keypress Space chat
	/delay 1
	/notify ItemDisplayWindow IconButton leftmouseup
	/delay 1
	/keypress Enter chat
	/delay 1
	/notify ItemDisplayWindow 0 close 
/RETURN

SUB endMacro
	/deletevar * global
	/disband
	/raiddisband
	/end
/RETURN

SUB halloween
	/if (!${Defined[Halloween]})		/declare Halloween 			bool outer false
	/if (!${Defined[HalloweenZones]})	/declare HalloweenZones[7] 	int outer -1
	
	/declare NormalZoneNpcID 				int local 0
	/declare HalloweenMerchantID 			int local 0
	/declare HalloweenRaidTokenNpcID 		int local 0
	/declare MistmooreTrickyAdvisorID 		int local 59
	/declare MistmooreTrickiestAdvisorID 	int local 0
	/declare LesserFaydarkFirenzellaID 		int local 0
	/declare LesserFaydarkFarmerID	 		int local 0
	/declare PapillonID	 					int local 0
	
	/declare i int local
	
	|- Test dans LF
	/varset Halloween true
	/varset HalloweenZones[1] 368
	/varset HalloweenZones[2] 1
	/varset HalloweenZones[3] 0
	
	/if (!${Halloween}) {
		/varset Halloween true
		/varset HalloweenZones[1] 0
		/call gate ${Me.CleanName} ${Me.CleanName}
		/delay 11s
		/call halloween
	} else /if (${HalloweenZones[1]} == 0) {
		|- On commence le manège
		/echo Partie THF
		|- On est dans THF, il faut récupérer les costumes
		/echo Milieu de cave
		/call MoveTo -Loc|-40,-340 |- On se met au milieu de la cave
		/echo Milieu de THF
		/call MoveTo -Loc|19,22 |- On sort de la cave, milieu de THF
		/echo Marchand de costume
		/varset HalloweenMerchantID ${Spawn[Jacque Magnifico].ID} |- le nom du marchand de costume
		/call MoveTo -SpawnID|${HalloweenMerchantID}
		|- On achète les costumes au marchand
			|- On ouvre la fenêtre de trade
			/target id ${HalloweenMerchantID}
			/stick 5
			/delay 30
			/nomodkey /click right target
			/delay 10
			|- On achète
			/call Buy "Fairy Costume Kit" 5
			/call Buy "Gargoyle Costume Kit" 1
			/call Buy "Scarecrow Costume Kit" 2
		|- On va chercher la quête
		/echo Fitzina
		/call MoveTo -Loc|19,22 |- On sort de la cave, milieu de THF
		/varset HalloweenRaidTokenNpcID ${Spawn[Fitzina].ID}
		/call MoveTo -SpawnID|${HalloweenRaidTokenNpcID} |- l'ID de Fitzina
		/casting "Fairy Costume Kit" item -maxtries|3
		/delay 1s
		/target id ${HalloweenRaidTokenNpcID}
		/delay 1s
		/hail
		/call MoveTo -Loc|19,22 |- On sort de la cave, milieu de THF
		/varset NormalZoneNpcID ${Spawn[Elyse].ID} |- le nom de la fée des "normal zones"
		/call MoveTo -SpawnID|${NormalZoneNpcID}
		/target ID ${NormalZoneNpcID}
		/delay 1s
		/varset HalloweenZones[1] ${Zone.ID}
		/varset HalloweenZones[2] 0
		/delay 5
		/say mistmoore
	} else /if (${HalloweenZones[2]} == 0) {
		|- In mistmoore
		/echo a tricky advisor
		/call MoveTo -SpawnID|${MistmooreTrickyAdvisorID}
		/delay 1s
		/casting "Gargoyle Costume Kit" item -maxtries|3
		/delay 6s
		/target ID ${MistmooreTrickyAdvisorID}
		/delay 1s
		/for i 1 to 12
			/hail
			/delay 1
		/next i
		/varset MistmooreTrickiestAdvisorID ${Spawn[trickiest advisor].ID}
		/call MoveTo -SpawnID|${MistmooreTrickiestAdvisorID}
		/delay 1
		/target ID ${MistmooreTrickiestAdvisorID}
		/delay 1
		/hail
		/delay 1
		/call MoveTo -SpawnID|${MistmooreTrickyAdvisorID}
		/call MoveTo -SpawnID|${Spawn[Eirod Haerod].ID} |- la nana à l'entrée de mistmoore
		/varset HalloweenZones[2] ${Zone.ID}
		/varset HalloweenZones[3] 0
		/call MoveTo -Loc|-350,124 |- la zone de Lesser Faydark
		/keypress forward hold
	} else /if (${HalloweenZones[3]} == 0) {
		/
		|- In Lesser Faydark
		/call MoveTo -Loc|-949,3183 |- on s'approche du druid ring
		/varset PapillonID ${Spawn[papillon].ID}
		/if (${PapillonID}) {
			/call waitFor -NPC|${PapillonID}
		}
		/varset LesserFaydarkFirenzellaID ${Spawn[Firenzella].ID}
		/call MoveTo -SpawnID|${LesserFaydarkFirenzellaID}
		/target ID ${LesserFaydarkFirenzellaID}
		/casting "Scarecrow Costume Kit" item -maxtries|3
		/hail
		/call MoveTo -Loc|-949,3183 |- on s'approche du druid ring
		
		/varset LesserFaydarkFarmerID ${Spawn[shady farmer].ID}
		/call MoveTo -SpawnID|${LesserFaydarkFarmerID}
		/target ID ${LesserFaydarkFarmerID}
		/hail
		/autoinventory
		
		/varset LesserFaydarkFarmerID ${Spawn[tricky farmer].ID}
		/call MoveTo -SpawnID|${LesserFaydarkFarmerID}
		/target ID ${LesserFaydarkFarmerID}
		/hail
		/autoinventory

		/varset LesserFaydarkFarmerID ${Spawn[frightening farmer].ID}
		/call MoveTo -SpawnID|${LesserFaydarkFarmerID}
		/target ID ${LesserFaydarkFarmerID}
		/hail
		/autoinventory

		/varset LesserFaydarkFarmerID ${Spawn[ghoulie farmer].ID}
		/call MoveTo -SpawnID|${LesserFaydarkFarmerID}
		/target ID ${LesserFaydarkFarmerID}
		/hail
		/autoinventory
		
		/call MoveTo -SpawnID|${LesserFaydarkFirenzellaID}
		/target ID ${LesserFaydarkFirenzellaID}
		/face fast
		/call putItemOnCursor() |- Le premier truc à lui donner
		/delay 5
		/nomodkey /click left Target
		/delay 5
		/call putItemOnCursor() |- Le deuxième truc à lui donner
		/delay 5
		/nomodkey /click left Target
		/delay 5
		/call putItemOnCursor() |- Le troisième truc à lui donner
		/delay 5
		/nomodkey /click left Target
		/delay 5
		/call putItemOnCursor() |- Le quatrieme truc à lui donner
		/delay 5
		/nomodkey /click left Target
		/delay 5
		/notify GiveWnd GVW_Give_Button leftmouseup
		/delay 5
		/varset HalloweenZones[3] ${Zone.ID}
		/varset HalloweenZones[4] 0
	} else /if (${HalloweenZones[4]} == 0) {
		
	}
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Triangulates distance between to points using the Pythagoras theorem.	-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- @param loc#1: Location 'A'												-|
|- @param loc#2: Location 'B'												-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB Triangulate_Distance(string loc#1, string loc#2)
/RETURN ${Math.Sqrt[${Math.Calc[${loc#2.Arg[2,,]}-${loc#1.Arg[2,,]}]}*${Math.Calc[${loc#2.Arg[2,,]}-${loc#1.Arg[2,,]}]}+${Math.Calc[${loc#2.Arg[1,,]}-${loc#1.Arg[1,,]}]}*${Math.Calc[${loc#2.Arg[1,,]}-${loc#1.Arg[1,,]}]}]}

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- MoveTo																							-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Attempts to utilize MQ2MoveUtils /moveto function, to move to a given destination.				-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- @param1 destination: Destinations can be provided as an ID, or loc, using the globalFollowing syntax:	-|
|-																									-|
|- Move to a Spawn ID use, -SpawnID|#																-|
|-		Ex.  /call MoveTo -SpawnID|${Target.ID}														-|
|-																									-|
|- Move to a Location use, -Loc|Y,X																	-|
|-		Ex.  /call MoveTo -Loc|${Target.Y},${Target.X}												-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB MoveTo(destinationInfo, int distCheck)
	/if (${outerDebug}) /echo |- MoveTo ==>

	/declare X_Loc float local
	/declare Y_Loc float local
	/if (!${Defined[distCheck]}) /declare distCheck int local 10

	|-Set MoveTo variables per destinationType.
	/if (${destinationInfo.Find[-SpawnID]}) {
		/call TrueTarget ${destinationInfo.Arg[2,|]}
		/varset Y_Loc ${Target.Y}
		/varset X_Loc ${Target.X}
	} else /if (${destinationInfo.Find[-Loc]}) {
		/varset Y_Loc ${destinationInfo.Arg[2,|].Arg[1,,]}
		/varset X_Loc ${destinationInfo.Arg[2,|].Arg[2,,]}
	} else {
		/echo [${destinationInfo}] does not specify a valid destinationType.  Use -SpawnID|#, or, -Loc|Y,X to note your intended destinationType.
	}

	|-Check if target is too far.
	/call Triangulate_Distance ${Me.Loc.Replace[ ,]} ${Y_Loc},${X_Loc}
	/if (${Int[${Macro.Return}]} > 1500) {
		/echo [${Spawn[id ${destination}].CleanName}] is too far away.
	} else {
		|-Check if target is already close enough
		/if (${Int[${Macro.Return}]} < 10) {
			/if (${outerDebug}) /echo I am already at the destination.
		} else {
			|- Declare a timer based on how far you have to move (1s for every 50 units + 10).
			/declare retryTimer timer local ${Math.Calc[${Macro.Return} / 50 + 10].Int}s
			
			|- Engage MQ2MoveUtils.
			:retry_MoveToLoc
			/if (${Me.Casting.ID} && ${Me.Class.ShortName.NotEqual[BRD]}) /interrupt
			/squelch /moveto loc ${Y_Loc} ${X_Loc} dist ${If[${Defined[distCheck]},${distCheck},10]}
			/delay 10 ${Me.Moving} || ${MoveTo.Moving}
			
			:movingTo_Loop
			
			|- Debug Echo
			/if (${outerDebug} && !${outerSpamTimer_MTLoop}) {
				/echo |- MoveToSpawn -|- :MoveTo_Loop
				/call createTimer outerSpamTimer_MTLoop 50
			}

			|/call check_BackgroundEvents
			/call Triangulate_Distance ${Me.Loc.Replace[ ,]} ${Y_Loc},${X_Loc}
			
			/if (${MoveTo.Moving}) {
				/if (${Macro.Return} > ${distCheck} && ${retryTimer}) {
					/goto :movingTo_Loop
				} else {
					/squelch /stick off
					/if (${outerDebug} && !${outerSpamTimer_MTFAIL}) {
						/echo I have failed to move to the intended target.
						/call createTimer outerSpamTimer_MTFAIL 30
					}
				}
			} else {
				/if (${Macro.Return} > ${Math.Calc[${distCheck} + ${Math.Calc[${distCheck} / 4].Int}].Int}) {
					/if (${retryTimer}) {
						/goto :retry_MoveToLoc
					} else {
						/if (${outerDebug} && !${outerSpamTimer_MTFAIL}) {
							/echo I have failed to move to the intended target.
							/call createTimer outerSpamTimer_MTFAIL 30
						}
					}
				} else {
					/squelch /stick off
					/if (${outerDebug}) /echo I have arrived at my intended destination.
					/delay 30 !${Me.Moving}
				}
			}
		}
	}

	/if (${outerDebug}) /echo <== MoveTo -|
/RETURN

|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- Reliably targets specified Target.IDs.					-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
|- @param TargetID: The ID of the spawn you wish to target.	-|
|---------------------------------------------------------------------------------------------------------------------------------------------------|
SUB TrueTarget(TargetID)
	/if (${outerDebug}) /echo |- TrueTarget${If[${TargetID}, [${TargetID}],]} ==>

	|- Check if the target is in zone.
	/if (${SpawnCount[id ${TargetID}]}) {
		|- Check server for range restrictions.
		/if (${MacroQuest.Server.Equal[PEQTGC]} && ${SpawnCount[id ${TargetID} radius 200]} || ${MacroQuest.Server.NotEqual[PEQTGC]}) {
		
			/declare retryTimer timer local 30
			
			:Target_Loop
			/squelch /target id ${TargetID}
			/delay 2 ${Target.ID} == ${TargetID}
			
			|- Check that I have the correct target ID.
			/if (${Target.ID} != ${TargetID} && ${SpawnCount[id ${TargetID}]}) {
				/if (${retryTimer}) {
					/goto :Target_Loop
				} else {
					/echo Failed to target [${Spawn[id ${TargetID}].CleanName} - ${TargetID}]
				}
			}
		}
	}

	/if (${outerDebug}) /echo <== TrueTarget -|
/RETURN

SUB putItemOnCursor(String itemName)
	/declare i int local
	/declare itemToFind item local ${FindItem[=${itemName}]
	/if (${itemToFind} == NULL) /RETURN
	/itemnotify ${itemToFind.InvSlot.Pack} rightmouseup
	/delay 5
	/itemnotify ${InvSlot[${itemToFind.InvSlot}]} leftmouseup
/RETURN

SUB waitFor(String command)
	/if (${command.Find[-NPC]}) {
		/declare npcID int local ${command.Arg[2,|]}
		:stillUP
		/if (${Spawn[id ${npcID}].ID}) {
			/delay 6s
			/goto :stillUP
		}
	} else {
		/echo [${command}] does not specify a valid waiting cause.  Use -NPC|#.
		/chat ${channel} [${command}] does not specify a valid waiting cause.  Use -NPC|#.
	}
/RETURN