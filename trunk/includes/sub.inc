
Sub loadCustomVars
	/declare stickCommand string outer ${Ini[${iniGlobal},Global,stickCommand]}
/return

Sub joinChannel
	/join Team
	/doevents
	:channelLoop
	/if (${channel} == 0) {
		/doevents
		/goto :channelLoop
	}
/return

Sub isHolyShit
	/if (${Me.Combat} && !${Me.Casting.ID} && !${Cursor.ID}) {
		/return TRUE
	}
/return FALSE

Sub isDownShit
	/if (!${Me.Combat} && !${Me.Casting.ID} && !${Cursor.ID}) {
		/return TRUE
	}
/return FALSE

Sub instantBuff
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
			| en 1, l'item
			/varset myInstant ${myInstantLine.Token[1,/]}
			/varset myInstantSpell ${FindItem[=${myInstant}].Spell}
			/if (${Spell[${myInstantSpell}].Stacks} && !${Me.Buff[${myInstantSpell}].ID}) {
				/casting "${myInstant}" ${mySlot}
			}
		/next i
	}
/return

Sub checkSpellBuff
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
			| Si on a une target pour le spell, c'est que ce n'est pas un group spell
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
					| Verifier la presence de tous les membres du groupe avant de caster le bazar
					/call checkGroupNeedBuff ${Spell[${mySpell}].ID}
					/if (${Macro.Return}) {
						/chat ${channel} casting "${mySpell}"
						/casting "${mySpell}" ${myGem} -maxtries|3
					}
				}
			}
		/next i
	}
/return

Sub checkTargetNeedBuff(string targetName, int spellID)
	/declare i int local
	/declare targetBuffs string local
	/declare targetBuff	string local
	
	/varset targetBuffs ${NetBots[${targetName}].Buff}
	/if (${targetBuffs.Find[${spellID}]}) {
		/return FALSE
	}
	/for i 1 to ${targetBuffs.Count[ ]} - 1
		/varset targetBuff ${targetBuffs.Token[${i}, ]}
		/if (${targetBuff.Length}) {
			/if (!${Spell[${spellID}].WillStack[${Spell[${targetBuff}]}]}) {
				/chat ${channel} '${Spell[${spellID}].Name}' won't stack with '${Spell[${targetBuff}]}' !
				/return FALSE
			}
		}
	/next i
/return TRUE

Sub checkGroupNeedBuff(int spellID)

/return FALSE

Sub checkDiscipline
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
/return

Sub checkHolyShitBuff
	/declare i int local
	/declare j int local
	/declare myHSBList string local ${Ini[${iniSetup},HolyShitBuff]}
	/declare myHSBListCount int local ${myHSBList.Count[|]}
	/varcalc myHSBListCount ${myHSBListCount}-1
	/declare myHSBLine string local
	/declare myHSB string local | HolyShitBuff
	/declare myHSS string local | HolyShitSpell
	/declare mySlot string local
	/declare recastTime string local
	/declare itemCleanName string local
	/declare targetName string local
	/declare NewString string local

	/if (${myHSBListCount} > 0) {
		/for i 1 to ${myHSBListCount}
			/varset mySlot ${myHSBList.Token[${i},|]}
			/varset myHSBLine ${Ini[${iniSetup},HolyShitBuff,${mySlot}]}
			| en 1, l'item
			/varset myHSB ${myHSBLine.Token[1,/]}
			| en 2, le timer
			/varset recastTime ${myHSBLine.Token[2,/]}
			| en 3, la target
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
/return

Sub checkDownShitBuff
	/declare i int local
	/declare myDSBList string local ${Ini[${iniSetup},DownShitBuff]}
	/declare myDSBListCount int local ${myDSBList.Count[|]}
	/varcalc myDSBListCount ${myDSBListCount}-1
	/declare myDSBLine string local
	/declare myDSB string local | DownShitBuff
	/declare myDSS string local | DownShitSpell
	/declare mySlot string local
	/declare recastTime string local
	/declare itemCleanName string local
	/declare targetName string local

	/if (${myDSBListCount} > 0) {
		/for i 1 to ${myDSBListCount}
			/varset mySlot ${myDSBList.Token[${i},|]}
			/varset myDSBLine ${Ini[${iniSetup},DownShitBuff,${mySlot}]}
			| en 1, l'item
			/varset myDSB ${myDSBLine.Token[1,/]}
			| en 2, le timer
			/varset recastTime ${myDSBLine.Token[2,/]}
			| en 3, la target
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
/return

Sub EVENT_Dead
	|/call Wait4Rez
	/ssm max
/return

Sub EVENT_Camp
	/echo Camping...
	/end
/return

Sub EVENT_Hail(String Line, String Cible)
	/declare toTheZone string local
	/varset toTheZone ${Ini[${iniHail},General,${Cible.Lower}]}
	/if (${toTheZone.Length} && ${toTheZone.NotEqual[NULL]}) {
		/say ${toTheZone}
	} else {
		/echo ${Line}
	}
	/return

Sub EVENT_Task(String Line, String Tache)
	/tell ${myMain} ${Line}
/return

Sub EVENT_Assign(String Line, String Tache)
	/tell ${myMain} ${Line}
/return

Sub checkPlugIn
	/if (!${Plugin[MQ2Cast].Name.Length}) {
		/echo MQ2Cast is a REQUIRED PLUGIN
		/echo Please Load MQ2Cast before running this macro
		/end
	}
/return

Sub EVENT_Tell(Line, string From, string Msg)
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
/return

Sub EVENT_Group(Line, string From)
	/if (${From.Find[${myMain}]}) {
		/if (!${Me.Grouped}) {
			/invite
		}
	}
/return

Sub EVENT_DraekonVoice(string Line, string targetName)
	/moveto id ${Spawn[npc ${targetName}].ID}
/return


Sub EVENT_DraekonVoiceEnd(string Line, string targetName)
	/moveto id ${Spawn[npc Draekon].ID}
/return


Sub EVENT_DraekonNoGrass
	/if (${Me.Class.Name.Equal[Warrior]} || ${Me.Class.Name.Equal[Paladin]} || ${Me.Class.Name.Equal[Shadowknight]}) /return

	/declare EndX float local			-1609.88
	/declare EndY float local			308.23

	/call InternalMoveTo ${EndY} ${EndX}
/return


Sub EVENT_DraekonGrass
	/moveto id ${Spawn[npc Draekon].ID}
/return


Sub EVENT_DraekonBelly
	/moveto id ${Spawn[npc Draekon].ID}
/return

Sub InternalMoveTo(int y, int x)

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

/return

Sub isOneOfMyToons(String from)
	/if (${NetBots.Client.Find[${from}]}) {
		/return TRUE
	}
/return FALSE

Sub cleanToonName(String toonName)
	/if (${NetBots.Client.Find[${toonName}]}) {
		/return ${toonName}
	}
	/if (${NetBots.Client.Find[${toonName.Right[-2].Left[-1]}]}) {
		/return ${toonName.Right[-2].Left[-1]}
	}
/return ${toonName}

Sub sticking(String to)
    /if (${Spawn[${to}].ID} == ${Me.ID}) {
        /return
    }
	/call stopCurrentAction FALSE
    /target ${to}
    /face fast
    /delay 1s
    /stick ${stickCommand}
    /chat ${channel} Sticking to ${to}
/return

| Allow toon to gate to the Forest
Sub gate(String from, String who)
	/echo gate
    /call amIconcerned ${from} ${who}
    /if (${Macro.Return}) {
        /call stopCurrentAction TRUE
        /declare CharmOfQueenAdrianne string local ${FindItem[Charm of Queen Adrianne].Name}
        /if (${CharmOfQueenAdrianne.Length} > 0 && !${CharmOfQueenAdrianne.Equal[NULL]}) {
            /casting "${CharmOfQueenAdrianne}" charm
            /chat ${channel} Gating to the Forest !
        }
    }
/return

| stick off, attack off, interrupt, twist off (if specify)
Sub stopCurrentAction(bool evenBard)
	/echo stopCurrentAction ${evenBard}
    /if (${evenBard}) {
        /twist off
    }
	/target clear
    /stick off
    /attack off
    /interrupt
    /delay 1
/return

Sub amIMelee
	/if (${Me.Class.ShortName.Equal[WAR]) {
		/return TRUE
	}
	/if (${Me.Class.ShortName.Equal[ROG]) {
		/return TRUE
	}
	/if (${Me.Class.ShortName.Equal[BSK]) {
		/return TRUE
	}
	/if (${Me.Class.ShortName.Equal[MNK]) {
		/return TRUE
	}
	/if (${Me.Class.ShortName.Equal[BRD]) {
		/return TRUE
	}
	/if (${Me.Class.ShortName.Equal[PAL]) {
		/return TRUE
	}
	/if (${Me.Class.ShortName.Equal[SHD]) {
		/return TRUE
	}
	/if (${Me.Class.ShortName.Equal[RNG]) {
		/return TRUE
	}
	/if (${Me.Class.ShortName.Equal[BST]) {
		/return TRUE
	}
/return FALSE

Sub amINuker
	/if (${Me.Class.ShortName.Equal[WIZ]) {
		/return TRUE
	}
	/if (${Me.Class.ShortName.Equal[MAG]) {
		/return TRUE
	}
	/if (${Me.Class.ShortName.Equal[ENC]) {
		/return TRUE
	}
	/if (${Me.Class.ShortName.Equal[NEC]) {
		/return TRUE
	}
/return FALSE

Sub amIconcerned(String from, String who)
	/echo amIconcerned ${from} ${who}
	/if (${who.Equal[all]}) {
		/return TRUE
	}
	/if (${who.Equal[group]}) {
		/if (${Me.Grouped} && ${Group.Member[${from}]}) {
			/return TRUE
		}
	}
	/if (${who.Equal[caster]}) {
		| Can cast spells, including Bard 
		/if (${Me.Class.CanCast}) {
			/return TRUE
		}
	}
	/if (${who.Equal[melee]}) {
		| Monk/Rogue/Bard/Ranger/Warrior/Paladin/ShadowKnight?
		/call amIMelee
		/return ${Macro.Return}
	}
	/if (${who.Equal[priest]}) {
		| Cleric/Druid/Shaman? 
		/if (${Me.Class.HealerType}) {
			/return TRUE
		}
	}
	/if (${who.Equal[nuker]}) {
		| Wizard/Magician/Necromancer/Enchanter?
		/call amINuker
		/return ${Macro.Return}
	}
	/if (${who.Equal[${Me.Class.Name}]} || ${who.Equal[${Me.Class.ShortName}]}) {
		/return TRUE
	}
	/if (${who.Equal[${Me.CleanName}]}) {
		/return TRUE
	}
/return FALSE

Sub makeGroup
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
		/return
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
		/return
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
		/return
	}
/return

Sub LPS
	| ----------------------------------------------------------------------
	| Declare loops / second variables
	| ----------------------------------------------------------------------
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
/return

| Allow toon to the guild hall
Sub goToGuildHall(String from, String who)
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
/return

Sub test()
	| Fermer toutes les fenetres ouvertes d'abord !
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
/return

Sub endMacro
	/deletevar * global
	/disband
	/raiddisband
	/end
/return

