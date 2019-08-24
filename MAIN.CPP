// Midnight Editor 06.2a: X-COM: Apocalypse
//
// Source code, Microsoft Visual C++ 4.0 or 5.0
//
// Copyright (c) 1997-98 Marcin Wichary 
//                       (mwichary@polbox.com or mwichary@friko2.onet.pl)
//
// Available at Midnight Editors Homepage: http://midnight.tme.szczecin.pl
//
// This code is intended for learning purposes only. The author retains
// all legal right to the code and the program. You may not create
// derivative works of this product. 
//
// Please look at README.TXT file for detailed informations on compilation.
//
// Best viewed at 800x600, font: Lucida Console 8, tab size: 4

// ------------------------------------------------------------------------


#include "main.h" 
#include "resource.h"

// Editor title - drawn on window caption
const char Title[]="Midnight Editor: X-COM: Apocalypse";
// Expected size of savegame - 0 if savegame changes its size
DWORD SaveExpectedSize=0;

//-------------------------------------------------------- Normal variables -
HINSTANCE CurInstance;          // Program instance
HWND MainDialog;                // Main window handle
HWND TabWindow;                 // Tab control handle
HWND Popup;                     // "Reading data" and "Saving data" pop up 
                                // windows handle

INT CurPage;                    // Current tab / child window number

HFONT hTitleFont;               // big font handle (Arial Bold)
HFONT hSmallFont;               // small font (Small Fonts)

HANDLE SaveFile;                // handle to savegame file

BOOL SaveLoaded=FALSE;          // is savegame currently loaded?
BOOL BackupPresent=FALSE;       // is this savegame backuped?
CHAR SaveName[MAX_PATH]="";     // current savegame name
CHAR NewSaveName[MAX_PATH]="";  // "about-to-load" savegame name
CHAR LastSaveName[MAX_PATH]=""; // last loaded savegame name
CHAR Executable[MAX_PATH]="";   // game executable file name
CHAR SaveDescription[80]="";    // current save description
CHAR SaveTime[80]="";           // current save in-game time

BOOL WrongValueReminder=TRUE;   // are "wrong value encountered" windows
                                // going to be displayed?

HBITMAP BPhotos[75];            // soldier photos handles
HBITMAP BOrgs[28];              // organization icons handles
HBITMAP BVehicles[33];          // vehicle icons handles

// structure used for controlling tab control
typedef struct tag_dlghdr { 
    HWND hwndTab;            // tab control handle
    HWND hwndDisplay;        // current child dialog box handle
    RECT rcDisplay;          // display rectangle for the tab control 
    DLGTEMPLATE *apRes[12];  // child windows templates
} DLGHDR; 

//----------------------------------------------------- Auxiliary variables -
// This values are used to remember selections whan changing tabs.
BYTE CurProject=0;           // current project selected
BYTE CurPerson=0;            // current person selected
BYTE CurAttribute=0;         // current attribute selected
BYTE CurAvVehicle=0;         // current vehicle on stock selected
BYTE CurFacility=0;          // current facility selected
BYTE CurVehicle=0;           // current vehicle selected 
BYTE CurEquipment=0;         // current equipment selected 
BYTE CurBase=0;              // current base selected
BYTE CurCategory=0;          // current category selected
BYTE CurItem=0;              // current item selected 
BYTE Cur1stOrganization=0;   // current first organization selected
BYTE Cur2ndOrganization=0;   // current second organization

//----------------------------------------------------------- Savegame data -
LONG Cash;                   // cash

BYTE WeekDay;                // date-time
BYTE Day;
BYTE Month;
WORD Year;
WORD Week;
BYTE Hour,Minute,Second;
 
BYTE Relations[2][28][28];   // relations between organizations

LONG Score[2][7];            // score

struct PROJECT               // projects data 
{
	BYTE Progress;           // progress (from 0 to 100)
	LONG RealProgress;       // real progress (multiplied in savegame)
	BYTE Finished;           // is finished?  
	BYTE Group;              // auxiliary variable
} Projects[99];

struct PERSON                // people data
{
	CHAR Name[27];           // name
	BYTE Photo;              // photo number
	BYTE Function;           // function (soldier, engineer etc.)
	BYTE Rank;               // rank
	BYTE IsAttributes[14];   // current attributes
	BYTE WasAttributes[14];  // former attributes
	WORD DaysService;        // days in service  
	WORD Kills;              // kills
	WORD Missions;           // missions
	BYTE Orders;             // orders
	LONG SavegameOffset;     // offset in savegame
} People[179];
BYTE NPeople;                // number of people

struct AVVEHICLE             // vehicles available on stock data
{
	LONG Price;              // price
	LONG Quantity;           // quantity
} AvVehicles[33];

struct BASE                  // bases data
{
	CHAR Name[22];           // base name
	WORD Items[255];         // items in data
	BYTE Facilities[64];     // facilities
	BYTE DaysLeft[64];       // days left to build facilities above
} Bases[8];
BYTE NBases;                 // number of bases

struct FACILITY              // facilities data
{
	BYTE CanBeBuilt;         // can this one be built?
	WORD CostToBuild;        // cost to build 
	BYTE DaysToBuild;        // days to build
	WORD MaintenanceCost;    // maintenance cost
	WORD Capacity;           // capacity
} Facilities[17];

struct VEHICLE               // vehicles data
{
	CHAR Name[27];           // vehicle name
	BYTE Type;               // vehicle type
	BYTE Owner;              // vehicle owner
	WORD Constitution;       // current constitution
	BYTE Equipment[33];      // equipment kind
	WORD EquipmentRounds[33];// equipment rounds/fuel (for above)
	LONG EquipmentNumber[33];// equipment number in savegame
	BYTE NEquipment;         // number of all equipments
	LONG SavegameOffset;     // offset in savegame
} Vehicles[79];
BYTE NVehicles;              // number of vehicles

//--------------------------------------------------------------- Constants -
const CHAR TWeekDays[7][10] = {"Monday","Tuesday","Wednesday","Thursday",
                               "Friday","Saturday","Sunday"};

const CHAR TMonths[12][12] = {"January","February","March","April","May",
                              "June","July","August","September",
							  "October","November","December"};

const CHAR TDays[31][5] = {"1st","2nd","3rd","4th","5th","6th","7th",
                           "8th","9th","10th","11th","12th","13th","14th",
						   "15th","16th","17th","18th","19th","20th",
						   "21st","22nd","23rd","24th","25th","26th",
						   "27th","28th","29th","30th","31st"};

const BYTE TCategory[99] = // categories ( 0 - biochemistry, 1 - physics )
	{1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,1,0,1,0,1,1,1,1,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,1};

const BYTE TTopics[99][3][50] = { // research topics
{"Dimension Gates","",""},{"The Alien Dimension","",""},
{"Dimension Probe","",""},{"Biotrans","",""},{"Explorer","",""},
{"Retaliator","",""},{"Annihilator","",""},{"Brainsucker Pod","",""},
{"Brainsucker Autopsy","",""},{"Brainsucker","",""},
{"Multiworm Egg Autopsy","",""},{"Multiworm Egg","",""},
{"Multiworm Autopsy","",""},{"Multiworm","",""},{"Hyperworm Autopsy","",""},
{"Hyperworms","",""},{"Chrysalis Autopsy","",""},{"Chrysalis","",""},
{"Anthropod Autopsy","",""},{"Anthropod","",""},{"Psimorph Autopsy","",""},
{"Psimorph","",""},{"Spitter Autopsy","",""},{"Spitter","",""},
{"Megaspawn Autopsy","",""},{"Megaspawn","",""},{"Popper Autopsy","",""},
{"Popper","",""},{"Skeletoid Autopsy","",""},{"Skeletoid","",""},
{"Micronoid Aggregate Autopsy","",""},{"Micronoid Aggregate","",""},
{"Queenspawn Autopsy","",""},{"Queenspawn","",""},{"Overspawn Autopsy","",""},
{"Overspawn","",""},{"The Alien Genetic Structure","",""},
{"The Alien Life Cycle","",""},{"The Real Alien Threat","",""},
{"Toxigun","Toxigun A-Clip",""},{"Toxigun B-Clip","",""},
{"Toxigun C-Clip","",""},
{"Heavy Launcher AG Missile","Minilauncher AG Missile","Alien Gas Grenade"},
{"Advanced Security Station","",""},{"Advanced Biochemistry Lab","",""},
{"Advanced Quantum Physics Lab","",""},{"Advanced Alien Containment","",""},
{"Advanced Workshop","",""},{"Disruptor Gun","",""},
{"Devastator Cannon","",""},{"Boomeroid","",""},{"Brainsucker Launcher","",""},
{"Entropy Launcher","",""},{"Entropy Pod","",""},
{"Dimension Missile Launcher","",""},
{"Dimension Missile","",""},{"Vortex Mine","",""},
{"Personal Disruptor Shield","",""},{"Personal Teleporter","",""},
{"Personal Cloaking Field","",""},{"Dimension Destabilizer","",""},
{"X-COM Disruptor Armor","",""},{"Light Disruptor Beam","",""},
{"Medium Disruptor Beam","",""},{"Heavy Disruptor Beam","",""},
{"Disruptor Bomb Launcher","",""},{"Stasis Bomb Launcher","",""},
{"Disruptor Multi-Bomb Launcher","",""},{"Advanced Control System","",""},
{"Bio-Transport Module","",""},{"Small Disruption Shield","",""},
{"Large Disruption Shield","",""},{"Cloaking Field","",""},
{"Teleporter","",""},{"Dimension Shifter","",""},
{"Alien Craft Propulsion","",""},{"Alien Craft Control Systems","",""},
{"Alien Craft Energy Source","",""},{"Alien Probe","",""},
{"Alien Scout Ship","",""},{"Alien Transporter","",""},
{"Alien Fast Attack Ship","",""},{"Alien Destroyer","",""},
{"Alien Assault Ship","",""},{"Alien Bomber","",""},
{"Alien Escort Ship","",""},{"Alien Battleship","",""},
{"Alien Mothership","",""},{"Incubator","",""},{"Spawning Chamber","",""},
{"Food Chamber","",""},{"Megapod Chamber","",""},
{"Sleeping Chamber","",""},{"Organic Factory","",""},
{"Alien Farm","",""},{"Control Chamber","",""},
{"Maintenance Factory","",""},{"Dimension Gate Generator","",""},
{"One way to win","",""}};

const CHAR TFunctions[4][50] = // people functions
{"Agent","Biochemist","Engineer","Quantum Physicist"};

const CHAR TRanks[7][50] = // soldier ranks
{"Rookie","Squaddie","Squad Leader","Sergeant","Captain","Colonel",
"Commander"};

const CHAR TAttributes[14][50] = // people attributes
{"Current health","Maximum health","Accuracy","Reactions","Maximum speed",
"Stamina","Bravery","Strength","Psi-energy","Psi-attack","Psi-defense",
"Biochemistry skill","Quantum physics skill","Engineering skill"};

const BYTE WasIncluded[14] = // is former value for this attribute remembered?
{0,1,1,1,1,1,1,1,1,1,1,0,0,0};

const WORD Margins[14][2] = // maximum and minimum values for attributes
{{0,255},{1,255},{0,100},{0,255},{0,255},{0,127},{0,25},{0,255},
{0,255},{0,255},{0,255},{0,255},{0,255},{0,255}};

const CHAR TVehicles[33][50] = // vehicle names
{"Alien Probe","Alien Scout","Alien Transporter",
"Alien Fast Attack Ship","Alien Destroyer","Alien Assault Ship",
"Alien Bomber","Alien Escort","Alien Battleship","Alien Mothership",
"Police Hovercar","Airtaxi","Rescue Transport","Construction Vehicle",
"Airtrans","Space Liner","Phoenix Hovercar","Hoverbike",
"Valkyrie Interceptor","Hawk Air Warrior","Dimension Probe","Biotrans",
"Explorer","Retaliator","Annihilator","Autotaxi","Autotrans","Police Car",
"Civilian Car","Stormdog","Wolfhound APC","Blazer Turbo Bike","Griffon AFV"};

const WORD MaxConstitution[33] = // maximum constitution for vehicles above
{80,320,600,700,800,1050,1100,900,2200,3200,40,25,35,60,60,800,70,25,
280,460,80,600,400,450,700,15,20,50,20,45,550,20,700};

const CHAR TCategories[4][50] = // categories for items in stores
{"Agent equipment","Airborne vehicle equipment/fuel",
"Road vehicle equipment/fuel","Alien containment"};

const CHAR TItems1[86][50] = // items in 1st category 
{"Megapol AP Grenade","Megapol Stun Grenade","Megapol Smoke Grenade",
"Marsec Proximity Mine","Marsec High Explosive","Megapol Lawpistol",
"» Megapol Lawpistol Clip","Marsec M4000 Machine Gun",
"» Marsec M4000 Gun Clip","Megapol Laser Sniper Gun","» Megapol Laser Pod",
"Megapol Auto Cannon","» Auto Cannon AP Clip","» Auto Cannon HE Clip",
"» Auto Cannon IN Clip","Megapol Plasma Gun","» Megapol Plasma Pod",
"Marsec Heavy Launcher","» Heavy Launcher AG Missile [or AA]",
"» Heavy Launcher HE Missile","» Heavy Launcher IN Missile",
"Marsec MiniLauncher","» MiniLauncher AG Missile [or AA]",
"» MiniLauncher HE Missile","» MiniLauncher IN Missile",
"Megapol Stun Grapple","Alien Gas Grenade","Tracker Gun Clip","Tracker Gun",
"Multi-Tracker","PSI-Grenade","ForceWeb","Toxigun","» Toxigun A-Clip",
"» Toxigun B-Clip","» Toxigun C-Clip","Dimension Destabiliser [or AA]",
"Mind Shield","Mind Bender","Alien Detector","Disruptor Gun [or AA]",
"Devastator Cannon [or AA]","Boomeroid [or AA]","Power Sword",
"Brainsucker Launcher [or AA]","Entropy Launcher [or AA]",
"Dimension Missile Launcher [or AA]","Dimension Missile [or AA]",
"Vortex Mine [or AA]","Personal Disruptor Shield [or AA]",
"Personal Teleporter [or AA]","Personal Cloaking Field [or AA]",
"Dimension Force Field","» Energy Pod","Medi-kit","Motion Scanner",
"» Brainsucker Pod [or AA]","» Entropy Pod [or AA]","Incendiary Grenade",
"Psimorph's Mindbender [or AA]","Megaspawn's Disruptor [or AA]",
"Megaspawn's Launcher [or AA]","Spitter's Vomit Funnel [or AA]",
"Multiworm's Spit [or AA]","Alien Egg's Vomit Tube [or AA]",
"Hyperworm's Bite [or AA]","Queenspawn's Tentacles [or AA]",
"Popper's Bomb [or AA]","Psiclone","Elerium","Alien Artifact",
"Megapol Leg Armor","Megapol Body Armor","Megapol Right Arm Armor",
"Megapol Left Arm Armor","Megapol Helmet","Marsec Leg Units",
"Marsec Body Unit","Marsec Right Arm Unit","Marsec Left Arm Unit",
"Marsec Head Unit","X-COM Leg Shields","X-COM Body Shield",
"X-COM Right Arm Shield","X-COM Left Arm Shield","X-COM Head Shield"};

const CHAR TItems2[49][50] = // items in 2nd category
{"Light Weapons Control","Medium Weapons Control","Heavy Weapons Control",
"Advanced Control System [or AA]","Cargo Module","Passenger Module",
"Bio-Transport Module [or AA]","Missile Evasion Matrix",
"Small Disruption Shield [or AA]","Large Disruption Shield [or AA]",
"Cloaking Field [or AA]","Teleporter [or AA]","Dimension Shifter [or AA]",
"Bolter 4000 Laser Gun","Lancer 7000 Laser Gun","Rendor Plasma Gun",
"» Elerium-115","Lineage Plasma Cannon","» Elerium-115",
"Plasma Multi-System","» Elerium-115","Light Disruptor Beam [or AA]",
"Medium Disruptor Beam [or AA]","Heavy Disruptor Beam [or AA]",
"40mm Auto Cannon","» Multi-Cannon Round","Janitor Missile Array",
"» Janitor Missile","Justice Missile Launcher","» Justice Missile",
"Prophet Missile Array","» Prophet Missile","Retribution Missile Launcher",
"» Retribution Missile","Disruptor Bomb Launcher [or AA]","» Disruptor Bomb",
"Stasis Bomb Launcher [or AA]","» Stasis Bomb",
"Disruptor Multi-Bomb Launcher [or AA]","» Disruptor Multi-Bomb",
"Laser Defense Array","Plasma Defense Array","SD Standard","SD Deluxe",
"SD Sports","SD Turbo","SD Elite","SD Special","» Fusion Powerfuel"};

const CHAR TItems3[28][50] = // items in 3rd category
{"Light Weapons Control","Medium Weapons Control",
"Heavy Weapons Control","Advanced Control System [or AA]","Cargo Module",
"Passenger Module","Bio-Transport Module [or AA]","Missile Evasion Matrix",
"Small Disruption Shield [or AA]","Large Disruption Shield [or AA]",
"Cloaking Field [or AA]","40mm Auto Cannon Turret",
"» Repeater 40mm Cannon Round","Airguard Anti-Air Cannon",
"» Airguard 52mm Cannon Round","GLM Array",
"» Ground Launched Missile","Plasma Turret Cannon","» Elerium-115",
"GLM Air defense","» Air Defense Missile","Rumble Cannon",
"Metro Roadhog","Metro Roadgrav","Metro Turbograv","Metro Powergrav",
"Metro Multipower Plus","» Fusion Powerfuel"};

const CHAR TItems4[30][50] = // items in 4th category
{"Alien Egg (Alive)","Brainsucker (Alive)","Multiworm (Alive)",
"Hyperworm (Alive)","Chrysalis (Alive)","Anthropod (Alive)",
"Skeletoid (Alive)","Spitter (Alive)","Popper (Alive)",
"Megaspawn (Alive)","Psimorph (Alive)","Queenspawn (Alive)",
"Micronoid Aggregate (Alive)","Brainsucker Pod (Alive)",
"Overspawn (Alive)","Alien Egg (Dead)","Brainsucker (Dead)",
"Multiworm (Dead)","Hyperworm (Dead)","Chrysalis (Dead)",
"Anthropod (Dead)","Skeletoid (Dead)","Spitter (Dead)",
"Popper (Dead)","Megaspawn (Dead)","Psimorph (Dead)",
"Queenspawn (Dead)","Micronoid Aggregate (Dead)","Brainsucker Pod (Dead)",
"Overspawn (Dead)"};

const LONG Items1Off[86] = // 1st category items offsets for base #1
{0x1e064,0x1e066,0x1e068,0x1e06a,0x1e06c,0x1e06e,0x1e070,0x1e072,0x1e074,
0x1e076,0x1e078,0x1e07a,0x1e07c,0x1e07e,0x1e080,0x1e082,0x1e084,0x1e086,
0x1e088,0x1e08a,0x1e08c,0x1e08e,0x1e090,0x1e092,0x1e094,0x1e096,0x1e098,
0x1e09a,0x1e09c,0x1e09e,0x1e0a0,0x1e0a2,0x1e0a4,0x1e0a6,0x1e0a8,0x1e0aa,
0x1e0ac,0x1e0ae,0x1e0b0,0x1e0b2,0x1e0b4,0x1e0b6,0x1e0b8,0x1e0ba,0x1e0bc,
0x1e0be,0x1e0c0,0x1e0c2,0x1e0c4,0x1e0c6,0x1e0c8,0x1e0ca,0x1e0cc,0x1e0ce,
0x1e0d0,0x1e0d2,0x1e0d4,0x1e0d6,0x1e0d8,0x1e0f8,0x1e0fa,0x1e0fc,0x1e0fe,
0x1e100,0x1e102,0x1e104,0x1e106,0x1e108,0x1e10a,0x1e10c,0x1e10e,0x1e0da,
0x1e0dc,0x1e0de,0x1e0e0,0x1e0e2,0x1e0e4,0x1e0e6,0x1e0e8,0x1e0ea,0x1e0ec,
0x1e0ee,0x1e0f0,0x1e0f2,0x1e0f4,0x1e0f6};

const LONG Items2Off[49] = // 2nd category items offsets for base #1
{0x1e048,0x1e04a,0x1e04c,0x1e04e,0x1e050,0x1e052,0x1e054,0x1e056,0x1e058,
0x1e05a,0x1e05c,0x1e05e,0x1e060,0x1e002,0x1e004,0x1e006,
0x1e112,0x1e008,0x1e112,0x1e00a,0x1e112,0x1e00c,0x1e00e,0x1e010,
0x1e012,0x1e116,0x1e014,0x1e118,0x1e016,0x1e11a,0x1e018,0x1e11c,
0x1e01a,0x1e11e,0x1e01c,0x1e120,0x1e01e,0x1e122,0x1e020,0x1e124,
0x1e022,0x1e024,0x1e026,0x1e028,0x1e02a,0x1e02c,0x1e02e,0x1e030,
0x1e110};

const LONG Items3Off[28] = // 3rd category items offsets for base #1
{0x1e048,0x1e04a,0x1e04c,0x1e04e,0x1e050,0x1e052,0x1e054,0x1e056,0x1e058,
0x1e05a,0x1e05c,0x1e032,0x1e126,0x1e034,0x1e128,0x1e036,0x1e12a,0x1e038,
0x1e112,0x1e03a,0x1e12c,0x1e03c,0x1e03e,0x1e040,0x1e042,0x1e044,0x1e046,
0x1e110};

const LONG Items4Off[30] = // 4th category items offsets for base #1
{0x1e186,0x1e188,0x1e18a,0x1e18c,0x1e18e,0x1e190,0x1e192,
0x1e194,0x1e196,0x1e198,0x1e19a,0x1e19c,0x1e19e,0x1e1a0,0x1e1a2,0x1e1a4,
0x1e1a6,0x1e1a8,0x1e1aa,0x1e1ac,0x1e1ae,0x1e1b0,0x1e1b2,0x1e1b4,0x1e1b6,
0x1e1b8,0x1e1ba,0x1e1bc,0x1e1be,0x1e1c0};

const CHAR TFacilities[17][50] = // facility names
{"Living Quarters","Stores","Cells","Medical Bay","Training Area",
"Psi-gym","Security Station","Advanced Security Station",
"Vehicle Repair Bay","Biochemistry Lab","Advanced Biochemistry Lab",
"Quantum Physics Lab","Advanced Quantum Physics Lab",
"Alien Containment","Advanced Alien Containment",
"Workshop","Advanced Workshop"};

const WORD FacilityDefs[17][4] = // facility defaults
{{5000,3,300,10},{1000,2,100,10000},{1200,2,100,10},{12000,4,800,10},
{8000,5,500,10},{15000,6,900,10},{2000,2,200,0},{8000,4,300,0},
{9000,5,500,1},{10000,5,100,5},{20000,8,200,10},{10000,5,100,5},
{25000,8,200,10},{7000,4,400,20},{14000,7,700,20},{6000,6,100,5},
{12000,9,200,10}};

const CHAR TEquipment[49][50] = // vehicle equipment
{"(none)","Bolter 4000 Laser Gun","Lancer 7000 Laser Gun",
"Rendor Plasma Gun","Lineage Plasma Cannon","Plasma Multi-System",
"Light Disruptor Beam","Medium Disruptor Beam","Heavy Disruptor Beam",
"40mm Auto Cannon","Janitor Missile Array","Justice Missile Launcher",
"Prophet Missile Array","Retribution Missile Launcher",
"Disruptor Bomb Launcher","Stasis Bomb Launcher",
"Disruptor Multi-Bomb Launcher","Laser Defense Array",
"Plasma Defense Array","SD Standard","SD Deluxe","SD Sports",
"SD Turbo","SD Elite","SD Special","40mm Auto Cannon Turret",
"Airguard Anti-Air Cannon","GLM Array","Plasma Turret Cannon",
"GLM Air defense","Rumble Cannon","Metro Roadhog","Metro Roadgrav",
"Metro Turbograv","Metro Powergrav","Metro Multipower Plus",
"Light Weapons Control","Medium Weapons Control",
"Heavy Weapons Control","Advanced Control System","Cargo Module",
"Passenger Module","Bio-Transport Module","Missile Evasion Matrix",
"Small Disruption Shield","Large Disruption Shield","Cloaking Field",
"Teleporter","Dimension Shifter"};

const CHAR TOrganizations[28][50] = // organizations
{"X-COM","Alien","Government","Megapol","Cult of Sirius",
"Marsec","Superdynamics","General Metro","Cyberweb","Transtellar",
"Solmine","Sensovision","Lifetree","Nutrivend","Evonet",
"Sanctuary Clinic","Nanotech","Energen","Synthemesh",
"Gravball League","Psyke","Diablo","Osiron","S.E.L.F.",
"Mutant Alliance","Extropians","Technocrats","Civilian"};

const char TProject[99][70] = { // project names
"Dimension Gates","The Alien Dimension","Dimension Probe",
"Bio-Transport","Explorer","Retaliator","Annihilator",
"Brainsucker Pods","Brainsucker Autopsy","Brainsucker",
"Multiworm Egg Autopsy","Multiworm Egg","Multiworm Autopsy",
"Multiworm","Hyperworm Autopsy","Hyperworm","Chrysalis Autopsy",
"Chrysalis","Anthropod Autopsy","Anthropod","Psimorph Autopsy",
"Psimorph","Spitter Autopsy","Spitter","Megaspawn Autopsy",
"Megaspawn","Popper Autopsy","Popper","Skeletoid Autopsy",
"Skeletoid","Micronoid Autopsy","Micronoid","Queenspawn Autopsy",
"Queenspawn","Overspawn Autopsy","Overspawn","The Alien Genetic Structure",
"The Alien Life Cycle","The Real Alien Threat","Biological Warfare",
"Toxin Type B","Toxin Type C","Alien Gas","Advanced Security Station",
"Advanced Biochemistry Lab","Advanced Quantum Physics Lab",
"Advanced Alien Containment","Advanced Workshop",
"Disruptor Gun","Devastator Cannon","Boomeroid",
"Brainsucker Launcher","Entropy Launcher","Entropy Pod",
"Dimension Missile Launcher","Dimension Missile","Vortex Mine",
"Personal Disruptor Shield","Personal Teleporter",
"Personal Cloaking Field","Dimension Destabiliser",
"Disruptor Armor","Light Disruptor Beam","Medium Disruptor Beam",
"Heavy Disruptor Beam","Disruptor Inversion Bomb",
"Stasis Field Bomb","Disruptor Multi-Bomb","X-COM Advanced Control System",
"Bio-Transport Module","Small Disruption Shield","Large Disruption Shield",
"Cloaking Field","Teleporter","Dimension Shifter",
"Alien Propulsion System","Alien Control System",
"Alien Energy Source","UFO type 1","UFO type 2","UFO type 3","UFO type 4",
"UFO type 5","UFO type 6","UFO type 7","UFO type 8","UFO type 9",
"UFO type 10","Alien building","Alien building","Alien building",
"Alien building","Alien building","Alien building","Alien building",
"Alien building","Alien building","Alien building","One way to win"};

const LONG PediaOff1[99][3] = { // ufopaedia offsets part 1
{0x6a5de,0,0},{0x6a5df,0,0},{0x6a619,0,0},{0x6a61a,0,0},
{0x6a61b,0,0},{0x6a61c,0,0},{0x6a61d,0,0},{0x6a5e1,0,0},
{0x6a5e4,0,0},{0x6a5e3,0,0},{0x6a5e6,0,0},{0x6a5e5,0,0},
{0x6a5e8,0,0},{0x6a5e7,0,0},{0x6a5ea,0,0},{0x6a5e9,0,0},
{0x6a5ec,0,0},{0x6a5eb,0,0},{0x6a5ee,0,0},{0x6a5ed,0,0},
{0x6a5f0,0,0},{0x6a5ef,0,0},{0x6a5f2,0,0},{0x6a5f1,0,0},
{0x6a5f4,0,0},{0x6a5f3,0,0},{0x6a5f6,0,0},{0x6a5f5,0,0},
{0x6a5f8,0,0},{0x6a5f7,0,0},{0x6a5fa,0,0},{0x6a5f9,0,0},
{0x6a5fc,0,0},{0x6a5fb,0,0},{0x6a5fe,0,0},{0x6a5fd,0,0},
{0x6a5ff,0,0},{0x6a600,0,0},{0x6a601,0,0},{0x6a689,0x6a68a,0},
{0x6a68b,0,0},{0x6a68c,0,0},{0x6a67b,0x6a67f,0x6a683},
{0x6a62f,0,0},{0x6a632,0,0},{0x6a634,0,0},{0x6a636,0,0},
{0x6a638,0,0},{0x6a691,0,0},{0x6a692,0,0},{0x6a693,0,0},
{0x6a695,0,0},{0x6a696,0,0},{0x6a6a2,0,0},{0x6a697,0,0},
{0x6a698,0,0},{0x6a699,0,0},{0x6a69a,0,0},{0x6a69b,0,0},
{0x6a69c,0,0},{0x6a68d,0,0},{0x6a6a6,0,0},{0x6a63e,0,0},
{0x6a63f,0,0},{0x6a640,0,0},{0x6a646,0,0},{0x6a647,0,0},
{0x6a648,0,0},{0x6a65f,0,0},{0x6a662,0,0},{0x6a664,0,0},
{0x6a665,0,0},{0x6a666,0,0},{0x6a667,0,0},{0x6a668,0,0},
{0x6a60c,0,0},{0x6a60d,0,0},{0x6a60e,0,0},{0x6a602,0,0},
{0x6a603,0,0},{0x6a604,0,0},{0x6a605,0,0},{0x6a606,0,0},
{0x6a607,0,0},{0x6a608,0,0},{0x6a609,0,0},{0x6a60a,0,0},
{0x6a60b,0,0},{0x6a5d4,0,0},{0x6a5d5,0,0},{0x6a5d6,0,0},
{0x6a5d7,0,0},{0x6a5d8,0,0},{0x6a5d9,0,0},{0x6a5da,0,0},
{0x6a5db,0,0},{0x6a5dc,0,0},{0x6a5dd,0,0},{0x6a5e0,0,0}};

const LONG PediaOff0[99][3] = // ufopaedia offsets part 2
{{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0x6abc0,0,0},
{0,0,0},{0,0,0},{0,0,0},{0x6abd7,0,0},{0,0,0},{0x6abd6,0,0},{0,0,0},
{0x6abd8,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0x6abd2,0,0},
{0,0,0},{0x6abd5,0,0},{0,0,0},{0x6abd3,0x6abd4,0},{0,0,0},{0x6abda,0,0},
{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0x6abd9,0,0},{0,0,0},{0,0,0},
{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0x6ab9a,0x6ab9e,0},
{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0x6abb0,0,0},{0x6abb1,0,0},
{0x6abb2,0,0},{0x6abb4,0,0},{0x6abb5,0,0},{0x6abc1,0,0},{0x6abb6,0,0},
{0x6abb7,0,0},{0x6abb8,0,0},{0x6abb9,0,0},{0x6abba,0,0},{0x6abbb,0,0},
{0x6abac,0,0},{0,0,0},{0x6c49d,0,0},{0x6c49e,0,0},{0x6c49f,0,0},
{0x6c4a5,0,0},{0x6c4a6,0,0},{0x6c4a7,0,0},{0x6c4be,0,0},{0x6c4c1,0,0},
{0x6c4c3,0,0},{0x6c4c4,0,0},{0x6c4c5,0,0},{0x6c4c6,0,0},{0x6c4c7,0,0},
{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},
{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},
{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}};

const LONG MaxProgress[99] = // maximum progress for projects
{5000,6000,12000,18000,24000,30000,36000,6000,7000,9000,8000,12000,9000,
15000,6000,9000,8000,12000,8000,12000,12000,20000,6000,9000,12000,20000,
6000,9000,10000,15000,20000,26000,15000,25000,20000,25000,25000,30000,
35000,26000,15000,15000,28000,18000,16000,16000,22000,24000,40000,38000,
40000,9000,9000,7000,42000,12000,46000,60000,130000,100000,85000,85000,
36000,46000,70000,60000,50000,80000,35000,9000,60000,105000,80000,150000,
190000,25000,30000,25000,8000,10000,12000,12000,14000,14000,16000,16000,
20000,20000,38000,42000,32000,46000,30000,44000,34000,40000,36000,48000,
35000};

//------------------------------------------------------- Control functions -

// Since up-down controls resolution is only 16 bits, which is too little
// for us (some larger numbers in savegames require 32 bits), we will
// "enhance" up-down control

WNDPROC DefEditProc = NULL; // default edit window procedure

// Since every control has accompanying 32-bit value, we will use it to
// store a pointer to MINMAX structure

struct MINMAX // structure defining minimum and maximum value for edit control
{
	LONG Min;
	LONG Max;
};

LRESULT APIENTRY EditProc(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	// Check if edit control's value doesn't exceed min/max borders if
	// control is losing focus or about to be destroyed
	if ((uMsg==WM_KILLFOCUS) || (uMsg==WM_DESTROY))
	{
		// Get the MINMAX pointer
		MINMAX *pMinMax=(MINMAX *)GetWindowLong(hDlg,GWL_USERDATA);
		// Get value
		LONG Value=GetDlgItemInt(TabWindow,GetDlgCtrlID(hDlg),NULL,TRUE);

		// Check if isn't too big or too small
		if (Value<pMinMax->Min) Value=pMinMax->Min; else
		if (Value>pMinMax->Max) Value=pMinMax->Max;

		// Write new value
		if (SaveLoaded)
		SetDlgItemInt(TabWindow,GetDlgCtrlID(hDlg),Value,TRUE);

		// Oh, and destroy MINMAX when it's "buddy" is going to die :)
		if (uMsg==WM_DESTROY) delete pMinMax;
	}
	// Now call the original edit window procedure
	return(CallWindowProc(DefEditProc,hDlg,uMsg,wParam,lParam));
}


// AdjustEdit performs various functions on edit control
VOID AdjustEdit(HWND hDlg,INT ControlNo,INT Length,
				LONG MaxValue,LONG MinValue=0)
{
	DWORD UpDownStyle;
	MINMAX *pMinMax;
	
	if (!SaveLoaded) // if save isn't loaded, set the edit to read only
	SendDlgItemMessage(hDlg,ControlNo,EM_SETREADONLY,(WPARAM)TRUE,0);
	else
	{
		// if minvalue is >= zero, limit edit input to digits only 
		// (if minvalue is less than zero, we would like to enter a
		// minus sing, wouldn't we?)
		if (MinValue>=0)
		SetWindowLong(GetDlgItem(hDlg,ControlNo),GWL_STYLE,
		GetWindowLong(GetDlgItem(hDlg,ControlNo),GWL_STYLE) | ES_NUMBER);

		// and set the edit length to specified value
		SendDlgItemMessage(hDlg,ControlNo,EM_SETLIMITTEXT,(WPARAM)Length,0);
	}
	// At the very first call, store the pointer to default edit window
	// procedure
	if (DefEditProc==NULL)
		DefEditProc=(WNDPROC)GetWindowLong(GetDlgItem(hDlg,ControlNo),
		                                   GWL_WNDPROC);
	// Set the pointer to new procedure
	SetWindowLong(GetDlgItem(hDlg,ControlNo),GWL_WNDPROC,(LONG)EditProc);

	// Create MINMAX structure and store a pointer to it
	pMinMax=new MINMAX;
	SetWindowLong(GetDlgItem(hDlg,ControlNo),GWL_USERDATA,(LONG)pMinMax);
	pMinMax->Min=MinValue;
	pMinMax->Max=MaxValue;

	// Now create up-down control
	UpDownStyle=WS_CHILD | WS_VISIBLE | UDS_ALIGNRIGHT 
		        | UDS_ARROWKEYS | UDS_WRAP;
	if (!SaveLoaded) UpDownStyle|=WS_DISABLED;
	// Up-down control's ID is the original edit control ID plus 1000
	CreateUpDownControl(UpDownStyle,0,0,0,0,TabWindow,ControlNo+1000,
					    CurInstance,GetDlgItem(hDlg,ControlNo),1,0,0);
}

// DisableControl disables specified control. It also can set its caption
// if required
VOID DisableControl(HWND hDlg,INT ControlNo,LPTSTR Caption=NULL)
{
	SetWindowLong(GetDlgItem(hDlg,ControlNo),GWL_STYLE,
	GetWindowLong(GetDlgItem(hDlg,ControlNo),GWL_STYLE) | WS_DISABLED );
	if (Caption!=NULL) SetWindowText(GetDlgItem(hDlg,ControlNo),Caption);
}

// EnableControl enables specified control. It also can set its caption
// if required
VOID EnableControl(HWND hDlg,INT ControlNo,LPTSTR Caption=NULL)
{
	SetWindowLong(GetDlgItem(hDlg,ControlNo),GWL_STYLE,
	GetWindowLong(GetDlgItem(hDlg,ControlNo),GWL_STYLE) & ~WS_DISABLED );
	if (Caption!=NULL) SetWindowText(GetDlgItem(hDlg,ControlNo),Caption);
}

//-------------------------------------------- drawing/destroying tab pages -

// KillPage gets all data from tab-control contents before it's destroyed
// (in order to display another page)
VOID KillPage()
{
	HWND hDlg=TabWindow; // handle to child window
	LONG i;              // temporary variables...
	HWND Temp;

	// We force the current control to lose control and get it again.
	// Why? If it accidentally happens to be an edit control, its value 
	// will be checked... (look EditProc function above)
	Temp=GetFocus(); 
	SetFocus(NULL);
	SetFocus(Temp);

	if (SaveLoaded) // do it all only if savegame is loaded
	switch (CurPage)
	{
	case 4:
		// get current cash
		Cash=GetDlgItemInt(hDlg,IDC_CASHEDIT,NULL,FALSE);

		Year=GetDlgItemInt(hDlg,IDC_YEAREDIT,NULL,FALSE);
		Hour=GetDlgItemInt(hDlg,IDC_HOUREDIT,NULL,FALSE);
		Minute=GetDlgItemInt(hDlg,IDC_MINUTEEDIT,NULL,FALSE);
		Second=GetDlgItemInt(hDlg,IDC_SECONDEDIT,NULL,FALSE);
		Week=GetDlgItemInt(hDlg,IDC_WEEKEDIT,NULL,FALSE);

		Month=(BYTE)SendDlgItemMessage(hDlg,IDC_MONTHCOMBO,CB_GETCURSEL,0,0);
		WeekDay=(BYTE)SendDlgItemMessage(hDlg,IDC_WEEKDAYCOMBO,
			                             CB_GETCURSEL,0,0);
		Day=(BYTE)SendDlgItemMessage(hDlg,IDC_DAYCOMBO,CB_GETCURSEL,0,0);

		break;
	case 5:
		// get building information
		Facilities[CurFacility].CanBeBuilt=
			(BYTE)SendDlgItemMessage(hDlg,IDC_CANBEBUILTCHECK,BM_GETCHECK,0,0);

		Facilities[CurFacility].CostToBuild=
			(WORD)GetDlgItemInt(hDlg,IDC_COSTTOBUILDEDIT,NULL,FALSE);
		Facilities[CurFacility].DaysToBuild=
			(BYTE)GetDlgItemInt(hDlg,IDC_DAYSTOBUILDEDIT,NULL,FALSE);
		Facilities[CurFacility].MaintenanceCost=
			(WORD)GetDlgItemInt(hDlg,IDC_MAINTENANCECOSTEDIT,NULL,FALSE);

		Facilities[CurFacility].Capacity=
			(WORD)GetDlgItemInt(hDlg,IDC_CAPACITYEDIT,NULL,FALSE);

		CurBase=(BYTE)SendDlgItemMessage(hDlg,IDC_BASECOMBO,CB_GETCURSEL,0,0);
		CurFacility=(BYTE)SendDlgItemMessage(hDlg,IDC_FACILITYLIST,
			                                 LB_GETCURSEL,0,0);
		break;
	case 6:
		// get stores information
		i=GetDlgItemInt(hDlg,IDC_QUANTITY,NULL,FALSE);

		if (CurCategory==0) 
			Bases[CurBase].Items[(Items1Off[CurItem]-0x1e002)/2]=(WORD)i;
		else
		if (CurCategory==1) 
			Bases[CurBase].Items[(Items2Off[CurItem]-0x1e002)/2]=(WORD)i;
		else
		if (CurCategory==2) 
			Bases[CurBase].Items[(Items3Off[CurItem]-0x1e002)/2]=(WORD)i;
		else
		if (CurCategory==3) 
			Bases[CurBase].Items[(Items4Off[CurItem]-0x1e002)/2]=(WORD)i;

		CurBase=(BYTE)SendDlgItemMessage(hDlg,IDC_BASECOMBO,CB_GETCURSEL,0,0);
		CurCategory=(BYTE)SendDlgItemMessage(hDlg,IDC_CATEGORYCOMBO,
			                                 CB_GETCURSEL,0,0);
		CurItem=(BYTE)SendDlgItemMessage(hDlg,IDC_ITEMLIST,LB_GETCURSEL,0,0);
		break;
	case 7:
		// get person information
		if (NPeople>0)
		{
			People[CurPerson].Photo=
				(BYTE)SendDlgItemMessage(hDlg,IDC_PHOTOLIST,LB_GETCURSEL,0,0);
			CurPerson=
				(BYTE)SendDlgItemMessage(hDlg,IDC_PERSONCOMBO,CB_GETCURSEL,0,0);
		}
		break;
	case 8:
		// get person information
		if (NPeople>0)
		{
			People[CurPerson].Rank=
				(BYTE)SendDlgItemMessage(hDlg,IDC_RANKCOMBO,CB_GETCURSEL,0,0);

			People[CurPerson].DaysService=
				(WORD)GetDlgItemInt(hDlg,IDC_DAYSSERVICEEDIT,NULL,FALSE);
			People[CurPerson].Kills=
				(WORD)GetDlgItemInt(hDlg,IDC_KILLSEDIT,NULL,FALSE);
			People[CurPerson].Missions=
				(WORD)GetDlgItemInt(hDlg,IDC_MISSIONSEDIT,NULL,FALSE);

			People[CurPerson].Orders=
				(BYTE)SendDlgItemMessage(hDlg,IDC_ORDERSSLIDER,TBM_GETPOS,0,0);

			People[CurPerson].WasAttributes[CurAttribute]=
				(BYTE)SendDlgItemMessage(hDlg,IDC_WASSLIDER,TBM_GETPOS,0,0);
			People[CurPerson].IsAttributes[CurAttribute]=
				(BYTE)SendDlgItemMessage(hDlg,IDC_ISSLIDER,TBM_GETPOS,0,0);

			CurPerson=(BYTE)SendDlgItemMessage(hDlg,IDC_PERSONCOMBO,
				                               CB_GETCURSEL,0,0);
		}
		break;
	case 9:
		// get available vehicles info
		AvVehicles[CurAvVehicle].Price=
			GetDlgItemInt(hDlg,IDC_PRICE,NULL,FALSE);
		AvVehicles[CurAvVehicle].Quantity=
			GetDlgItemInt(hDlg,IDC_QUANTITY,NULL,FALSE);

		CurAvVehicle=(BYTE)SendDlgItemMessage(hDlg,IDC_VEHICLELIST,
			                                  LB_GETCURSEL,0,0);

		break;
	case 10:
		// get vehicles information
		if (NVehicles>0)
		{
			if (Vehicles[CurVehicle].NEquipment>0)
			{
				Vehicles[CurVehicle].EquipmentRounds[CurEquipment]=
					(WORD)GetDlgItemInt(hDlg,IDC_ROUNDSEDIT,NULL,FALSE);

				CurEquipment=(BYTE)SendDlgItemMessage(hDlg,IDC_EQUIPMENTLIST,
					                                  LB_GETCURSEL,0,0);
			}
			Vehicles[CurVehicle].Constitution=
				(WORD)GetDlgItemInt(hDlg,IDC_CONSTITUTIONEDIT,NULL,FALSE);

			Vehicles[CurVehicle].Owner=
				(BYTE)SendDlgItemMessage(hDlg,IDC_OWNERCOMBO,CB_GETCURSEL,0,0);

			CurVehicle=(BYTE)SendDlgItemMessage(hDlg,IDC_VEHICLELIST,
				                                LB_GETCURSEL,0,0);
		}

		break;
	case 11:
		// get projects information
		Projects[CurProject].Finished=
			(BYTE)SendDlgItemMessage(hDlg,IDC_FINISHED,BM_GETCHECK,0,0);
		Projects[CurProject].Progress=(
			BYTE)SendDlgItemMessage(hDlg,IDC_PROGRESSSLIDER,TBM_GETPOS,0,0);

		CurProject=(BYTE)SendDlgItemMessage(hDlg,IDC_PROJECTLIST,
			                                LB_GETCURSEL,0,0);
		break;
	case 12:
		// get relations information
		Relations[0][Cur1stOrganization][Cur2ndOrganization]=
			(BYTE)SendDlgItemMessage(hDlg,IDC_RELATION11,TBM_GETPOS,0,0);
		Relations[1][Cur1stOrganization][Cur2ndOrganization]=
			(BYTE)SendDlgItemMessage(hDlg,IDC_RELATION12,TBM_GETPOS,0,0);
		Relations[0][Cur2ndOrganization][Cur1stOrganization]=
			(BYTE)SendDlgItemMessage(hDlg,IDC_RELATION21,TBM_GETPOS,0,0);
		Relations[1][Cur2ndOrganization][Cur1stOrganization]=
			(BYTE)SendDlgItemMessage(hDlg,IDC_RELATION22,TBM_GETPOS,0,0);

		Cur1stOrganization=(BYTE)SendDlgItemMessage(hDlg,
			                       IDC_1STORGANIZATIONCOMBO,CB_GETCURSEL,0,0);
		Cur2ndOrganization=(BYTE)SendDlgItemMessage(hDlg,
		                    	   IDC_2NDORGANIZATIONCOMBO,CB_GETCURSEL,0,0);
		break;
	case 13:
		// get score information
		for (i=0;i<=6;i++)
		{
			Score[0][i]=GetDlgItemInt(hDlg,IDC_WEEKEDIT1+i,FALSE,TRUE);
			Score[1][i]=GetDlgItemInt(hDlg,IDC_TOTALEDIT1+i,FALSE,TRUE);
		}
		break;
	}
}

// DrawPage prepares the page to be displayed - changes the fonts, 
// enables/disables appropriate controls, sets data etc.
VOID DrawPage(BOOL Redraw) // Redraw - needs immediate redrawing?
{
	HWND hDlg=TabWindow; // handle to child window

	INT i;				 // temporary variables
	DWORD j;

	switch (CurPage)
	{
	case 1:
		// set appropriate fonts
		SendDlgItemMessage(hDlg,IDC_TITLE,WM_SETFONT,(WPARAM)hTitleFont,0);
		SendDlgItemMessage(hDlg,IDC_GAMETITLE,WM_SETFONT,(WPARAM)hTitleFont,0);
		SendDlgItemMessage(hDlg,IDC_COPYRIGHT,WM_SETFONT,(WPARAM)hSmallFont,0);
		break;
	case 3: 
		if (SaveLoaded) 
		{
			// enable some controls if save's loaded
			EnableControl(hDlg,IDC_SAVENAME,SaveName);
			EnableControl(hDlg,IDC_SAVEDESCRIPTION,SaveDescription);
			EnableControl(hDlg,IDC_SAVETIME,SaveTime);
			EnableControl(hDlg,IDC_BACKUPBAKBUTTON);
			if (BackupPresent) EnableControl(hDlg,IDC_RESTOREBAKBUTTON);
				          else DisableControl(hDlg,IDC_RESTOREBAKBUTTON);
			DisableControl(hDlg,IDC_LASTSAVEBUTTON);
		}
		else 
		if (lstrlen(LastSaveName)!=0)
			EnableControl(hDlg,IDC_LASTSAVEBUTTON);
		break;
	case 4:
		// limit edit to 9 digits and set its maximum value to 999999999
		AdjustEdit(hDlg,IDC_CASHEDIT,9,999999999);
		AdjustEdit(hDlg,IDC_YEAREDIT,5,32767);
		AdjustEdit(hDlg,IDC_WEEKEDIT,5,32767);
		AdjustEdit(hDlg,IDC_HOUREDIT,2,23);
		AdjustEdit(hDlg,IDC_MINUTEEDIT,2,59);
		AdjustEdit(hDlg,IDC_SECONDEDIT,2,59);

		if (SaveLoaded)
		{
			SetDlgItemInt(hDlg,IDC_CASHEDIT,Cash,FALSE);
			SetDlgItemInt(hDlg,IDC_YEAREDIT,Year,FALSE);
			SetDlgItemInt(hDlg,IDC_WEEKEDIT,Week,FALSE);
			SetDlgItemInt(hDlg,IDC_HOUREDIT,Hour,FALSE);
			SetDlgItemInt(hDlg,IDC_MINUTEEDIT,Minute,FALSE);
			SetDlgItemInt(hDlg,IDC_SECONDEDIT,Second,FALSE);
			EnableControl(hDlg,IDC_STATIC1); EnableControl(hDlg,IDC_STATIC2);
			EnableControl(hDlg,IDC_STATIC3); EnableControl(hDlg,IDC_STATIC4);
			EnableControl(hDlg,IDC_STATIC5); EnableControl(hDlg,IDC_STATIC6); 
			EnableControl(hDlg,IDC_STATIC7); EnableControl(hDlg,IDC_STATIC8); 
			EnableControl(hDlg,IDC_STATIC9); EnableControl(hDlg,IDC_STATIC10); 
			EnableControl(hDlg,IDC_FINISH); EnableControl(hDlg,IDC_MAX);
			EnableControl(hDlg,IDC_DIG);
			EnableControl(hDlg,IDC_UPDATESAVETIME);
			// write bases names to combo control
			for (i=0;i<NBases;i++)
				SendDlgItemMessage(hDlg,IDC_BASECOMBO,CB_ADDSTRING,
				                   (WPARAM)0,(LPARAM)(LPTSTR)Bases[i].Name);
			// set current base selection
			SendDlgItemMessage(hDlg,IDC_BASECOMBO,CB_SETCURSEL,
				               (WPARAM)CurBase,0);

			// write days, weeks, months names to combo controls
			for (i=0;i<=30;i++)
				SendDlgItemMessage(hDlg,IDC_DAYCOMBO,CB_ADDSTRING,
				                   (WPARAM)0,(LPARAM)(LPTSTR)TDays[i]);
			for (i=0;i<=6;i++)
				SendDlgItemMessage(hDlg,IDC_WEEKDAYCOMBO,CB_ADDSTRING,
				                   (WPARAM)0,(LPARAM)(LPTSTR)TWeekDays[i]);
			for (i=0;i<=11;i++)
				SendDlgItemMessage(hDlg,IDC_MONTHCOMBO,CB_ADDSTRING,
				                   (WPARAM)0,(LPARAM)(LPTSTR)TMonths[i]);
			SendDlgItemMessage(hDlg,IDC_DAYCOMBO,CB_SETCURSEL,Day,0);
			SendDlgItemMessage(hDlg,IDC_WEEKDAYCOMBO,CB_SETCURSEL,WeekDay,0);
			SendDlgItemMessage(hDlg,IDC_MONTHCOMBO,CB_SETCURSEL,Month,0);
		} else
		{
			DisableControl(hDlg,IDC_BASECOMBO);
			DisableControl(hDlg,IDC_DAYCOMBO);
			DisableControl(hDlg,IDC_MONTHCOMBO);
			DisableControl(hDlg,IDC_WEEKDAYCOMBO);
		}
		break;
	case 5:
		// add facility names...
		for (i=0;i<17;i++)
			SendDlgItemMessage(hDlg,IDC_FACILITYLIST,LB_ADDSTRING,
			                   (WPARAM)0,(LPARAM)(LPTSTR)TFacilities[i]);
		SendDlgItemMessage(hDlg,IDC_FACILITYLIST,LB_SETCURSEL,
			               (WPARAM)CurFacility,0);

		AdjustEdit(hDlg,IDC_COSTTOBUILDEDIT,5,65535);
		AdjustEdit(hDlg,IDC_DAYSTOBUILDEDIT,3,255);
		AdjustEdit(hDlg,IDC_MAINTENANCECOSTEDIT,5,65535);
		AdjustEdit(hDlg,IDC_CAPACITYEDIT,5,65535);

		if (SaveLoaded)
		{
			EnableControl(hDlg,IDC_STATIC3); EnableControl(hDlg,IDC_STATIC4);
			EnableControl(hDlg,IDC_STATIC5); EnableControl(hDlg,IDC_STATIC6);
			EnableControl(hDlg,IDC_STATIC7);
			EnableControl(hDlg,IDC_CANBEBUILTCHECK);
			EnableControl(hDlg,IDC_FACILITYLIST);
			EnableControl(hDlg,IDC_COPYFACILITY);
			EnableControl(hDlg,IDC_RESTOREFACILITY);
		};
		break;
	case 6:
		if (SaveLoaded)
		{
			for (i=0;i<NBases;i++)
				SendDlgItemMessage(hDlg,IDC_BASECOMBO,CB_ADDSTRING,
				                   (WPARAM)0,(LPARAM)(LPTSTR)Bases[i].Name);
			SendDlgItemMessage(hDlg,IDC_BASECOMBO,CB_SETCURSEL,
				               (WPARAM)CurBase,0);
		}
		
		for (i=0;i<4;i++)
			SendDlgItemMessage(hDlg,IDC_CATEGORYCOMBO,CB_ADDSTRING,
			                   (WPARAM)0,(LPARAM)(LPTSTR)TCategories[i]);
		SendDlgItemMessage(hDlg,IDC_CATEGORYCOMBO,CB_SETCURSEL,
			               (WPARAM)CurCategory,0);

		AdjustEdit(hDlg,IDC_QUANTITY,5,65535);
		if (SaveLoaded)
		{
			EnableControl(hDlg,IDC_STATIC1); EnableControl(hDlg,IDC_STATIC2);
			EnableControl(hDlg,IDC_STATIC3); EnableControl(hDlg,IDC_STATIC4);
			EnableControl(hDlg,IDC_STATIC5);
			EnableControl(hDlg,IDC_COPYCATEGORY);
			EnableControl(hDlg,IDC_COPYTHISBASE);
			EnableControl(hDlg,IDC_COPYALLBASES);
			EnableControl(hDlg,IDC_ITEMLIST);
		} else
		{
			DisableControl(hDlg,IDC_CATEGORYCOMBO);
			DisableControl(hDlg,IDC_BASECOMBO);
		}
		break;
	case 7:
		for (i=0;i<75;i++)
		{
			j=SendDlgItemMessage(hDlg,IDC_PHOTOLIST,LB_ADDSTRING,
				                 (WPARAM)0,(LPARAM)(LPTSTR)"a");
			SendDlgItemMessage(hDlg,IDC_PHOTOLIST,LB_SETITEMDATA,j,i);
		}
		if (NPeople>0) // do it only if there are some people
		{
			for (i=0;i<NPeople;i++)
				SendDlgItemMessage(hDlg,IDC_PERSONCOMBO,CB_ADDSTRING,
				                   (WPARAM)0,(LPARAM)(LPTSTR)People[i].Name);
			SendDlgItemMessage(hDlg,IDC_PERSONCOMBO,CB_SETCURSEL,
				               (WPARAM)CurPerson,0);
		}

		if ((SaveLoaded) && (NPeople>0))
		{
			EnableControl(hDlg,IDC_PHOTOLIST);
			EnableControl(hDlg,IDC_STATIC1); EnableControl(hDlg,IDC_STATIC2);
			EnableControl(hDlg,IDC_STATIC3); EnableControl(hDlg,IDC_STATIC4);
			EnableControl(hDlg,IDC_FUNCTION);
			SendDlgItemMessage(hDlg,IDC_NAMEEDIT,EM_SETLIMITTEXT,(WPARAM)25,0);
		} else
		{
			DisableControl(hDlg,IDC_PERSONCOMBO);
			SendDlgItemMessage(hDlg,IDC_NAMEEDIT,EM_SETREADONLY,(WPARAM)TRUE,0);
		}
		break;
	case 8:
		if (NPeople>0)
		{
			for (i=0;i<NPeople;i++)
				SendDlgItemMessage(hDlg,IDC_PERSONCOMBO,CB_ADDSTRING,
				                   (WPARAM)0,(LPARAM)(LPTSTR)People[i].Name);
			SendDlgItemMessage(hDlg,IDC_PERSONCOMBO,CB_SETCURSEL,
				               (WPARAM)CurPerson,0);

			for (i=0;i<7;i++)
				SendDlgItemMessage(hDlg,IDC_RANKCOMBO,CB_ADDSTRING,
				                   (WPARAM)0,(LPARAM)(LPTSTR)TRanks[i]);

			for (i=0;i<14;i++)
				SendDlgItemMessage(hDlg,IDC_ATTRIBUTECOMBO,CB_ADDSTRING,
				                   (WPARAM)0,(LPARAM)(LPTSTR)TAttributes[i]);
			SendDlgItemMessage(hDlg,IDC_ATTRIBUTECOMBO,CB_SETCURSEL,
				               (WPARAM)CurAttribute,0);

		}

		AdjustEdit(hDlg,IDC_DAYSSERVICEEDIT,5,65535);
		AdjustEdit(hDlg,IDC_KILLSEDIT,5,65535); 
		AdjustEdit(hDlg,IDC_MISSIONSEDIT,5,65535);

		SendDlgItemMessage(hDlg,IDC_ORDERSSLIDER,TBM_SETRANGE,
			               (WPARAM)FALSE,(LPARAM)MAKELONG(0,5));
		SendDlgItemMessage(hDlg,IDC_ORDERSSLIDER,TBM_SETTICFREQ,
			               (WPARAM)1,(LPARAM)0);

		if ((SaveLoaded) && (NPeople>0))
		{
			EnableControl(hDlg,IDC_STATIC1); EnableControl(hDlg,IDC_STATIC2);
			EnableControl(hDlg,IDC_STATIC3); EnableControl(hDlg,IDC_STATIC5);
			EnableControl(hDlg,IDC_STATIC6); EnableControl(hDlg,IDC_STATIC7);
			EnableControl(hDlg,IDC_STATIC8); EnableControl(hDlg,IDC_STATIC9);
			EnableControl(hDlg,IDC_MAX); EnableControl(hDlg,IDC_ORDERSSLIDER);
			EnableControl(hDlg,IDC_COPYPERSON);
		} else
		{
			DisableControl(hDlg,IDC_PERSONCOMBO);
			DisableControl(hDlg,IDC_RANKCOMBO);
			DisableControl(hDlg,IDC_ATTRIBUTECOMBO);
		}
		break;
	case 9:
		for (i=0;i<33;i++)
		{
			j=SendDlgItemMessage(hDlg,IDC_VEHICLELIST,LB_ADDSTRING,
				                 (WPARAM)0,(LPARAM)(LPTSTR)TVehicles[i]);
			SendDlgItemMessage(hDlg,IDC_VEHICLELIST,LB_SETITEMDATA,j,i);
		}
		SendDlgItemMessage(hDlg,IDC_VEHICLELIST,LB_SETCURSEL,
			               (WPARAM)CurAvVehicle,0);

		AdjustEdit(hDlg,IDC_PRICE,9,999999999);
		AdjustEdit(hDlg,IDC_QUANTITY,5,99999);

		if (SaveLoaded)
		{
			EnableControl(hDlg,IDC_STATIC1); EnableControl(hDlg,IDC_STATIC2);
			EnableControl(hDlg,IDC_STATIC3); 
			EnableControl(hDlg,IDC_VEHICLELIST);
			EnableControl(hDlg,IDC_COPYAVVEHICLES);
		}
		break;
	case 10:
		if (NVehicles>0)
		{
			for (i=0;i<NVehicles;i++)
			{
				j=SendDlgItemMessage(hDlg,IDC_VEHICLELIST,LB_ADDSTRING,
					                 (WPARAM)0,
									 (LPARAM)(LPTSTR)Vehicles[i].Name);
				SendDlgItemMessage(hDlg,IDC_VEHICLELIST,LB_SETITEMDATA,
					               j,Vehicles[i].Type);
			}
			SendDlgItemMessage(hDlg,IDC_VEHICLELIST,LB_SETCURSEL,
				               (WPARAM)CurVehicle,0);

			for (i=0;i<28;i++)
			{
				j=SendDlgItemMessage(hDlg,IDC_OWNERCOMBO,CB_ADDSTRING,
					                 (WPARAM)0,
									 (LPARAM)(LPTSTR)TOrganizations[i]);
				SendDlgItemMessage(hDlg,IDC_OWNERCOMBO,CB_SETITEMDATA,j,i);
			}
		}
		AdjustEdit(hDlg,IDC_ROUNDSEDIT,5,32767);
		AdjustEdit(hDlg,IDC_CONSTITUTIONEDIT,5,32767);
		if ((SaveLoaded) && (NVehicles>0))
		{
			EnableControl(hDlg,IDC_STATIC1); EnableControl(hDlg,IDC_STATIC2);
			EnableControl(hDlg,IDC_STATIC3); EnableControl(hDlg,IDC_STATIC4);
			EnableControl(hDlg,IDC_STATIC5); EnableControl(hDlg,IDC_STATIC6);
			EnableControl(hDlg,IDC_STATIC7);
			EnableControl(hDlg,IDC_MAX);
			EnableControl(hDlg,IDC_VEHICLELIST);
			EnableControl(hDlg,IDC_EQUIPMENTLIST);
			SendDlgItemMessage(hDlg,IDC_CARNAMEEDIT,EM_SETLIMITTEXT,
				               (WPARAM)25,0);
		} else
		{
			DisableControl(hDlg,IDC_OWNERCOMBO);
			SendDlgItemMessage(hDlg,IDC_CARNAMEEDIT,EM_SETREADONLY,
				               (WPARAM)TRUE,0);
		};

		break;
	case 11:
		for (i=0;i<99;i++)
			SendDlgItemMessage(hDlg,IDC_PROJECTLIST,LB_ADDSTRING,(WPARAM)0,
			                   (LPARAM)(LPTSTR)TProject[i]);
		SendDlgItemMessage(hDlg,IDC_PROJECTLIST,LB_SETCURSEL,
			               (WPARAM)CurProject,0);

		SendDlgItemMessage(hDlg,IDC_PROGRESSSLIDER,TBM_SETRANGE,
			               (WPARAM)FALSE,(LPARAM)MAKELONG(0,100));
		SendDlgItemMessage(hDlg,IDC_PROGRESSSLIDER,TBM_SETTICFREQ,
			               (WPARAM)10,(LPARAM)0);

		if (SaveLoaded)
		{
			EnableControl(hDlg,IDC_PROJECTLIST);
			EnableControl(hDlg,IDC_STATIC1); EnableControl(hDlg,IDC_STATIC2);
			EnableControl(hDlg,IDC_STATIC3); EnableControl(hDlg,IDC_STATIC4);
			EnableControl(hDlg,IDC_CATEGORY);
			EnableControl(hDlg,IDC_TOPIC1); EnableControl(hDlg,IDC_TOPIC2);
			EnableControl(hDlg,IDC_TOPIC3);
			EnableControl(hDlg,IDC_PROGRESSPERC);
			EnableControl(hDlg,IDC_COPYPROJECT);
			EnableControl(hDlg,IDC_FINISHED);
			EnableControl(hDlg,IDC_PROGRESSSLIDER);
		}
		break;
	case 12:
		for (i=0;i<28;i++)
		{
			j=SendDlgItemMessage(hDlg,IDC_1STORGANIZATIONCOMBO,
				                 CB_ADDSTRING,(WPARAM)0,
								 (LPARAM)(LPTSTR)TOrganizations[i]);
			SendDlgItemMessage(hDlg,IDC_1STORGANIZATIONCOMBO,
				               CB_SETITEMDATA,j,i);
			j=SendDlgItemMessage(hDlg,IDC_2NDORGANIZATIONCOMBO,
				                 CB_ADDSTRING,(WPARAM)0,
								 (LPARAM)(LPTSTR)TOrganizations[i]);
			SendDlgItemMessage(hDlg,IDC_2NDORGANIZATIONCOMBO,
				               CB_SETITEMDATA,j,i);
		}
		SendDlgItemMessage(hDlg,IDC_1STORGANIZATIONCOMBO,CB_SETCURSEL,
			               (WPARAM)Cur1stOrganization,0);
		SendDlgItemMessage(hDlg,IDC_2NDORGANIZATIONCOMBO,CB_SETCURSEL,
			               (WPARAM)Cur2ndOrganization,0);

		SendDlgItemMessage(hDlg,IDC_INFO1,WM_SETFONT,(WPARAM)hSmallFont,0);
		SendDlgItemMessage(hDlg,IDC_INFO2,WM_SETFONT,(WPARAM)hSmallFont,0);
		SendDlgItemMessage(hDlg,IDC_INFO3,WM_SETFONT,(WPARAM)hSmallFont,0);
		SendDlgItemMessage(hDlg,IDC_INFO4,WM_SETFONT,(WPARAM)hSmallFont,0);
		SendDlgItemMessage(hDlg,IDC_SET1,WM_SETFONT,(WPARAM)hSmallFont,0);
		SendDlgItemMessage(hDlg,IDC_SET2,WM_SETFONT,(WPARAM)hSmallFont,0);
		SendDlgItemMessage(hDlg,IDC_SET3,WM_SETFONT,(WPARAM)hSmallFont,0);

		SendDlgItemMessage(hDlg,IDC_RELATION11,TBM_SETRANGE,
						   (WPARAM)FALSE,(LPARAM)MAKELONG(-100,100));
		SendDlgItemMessage(hDlg,IDC_RELATION11,TBM_SETTICFREQ,
						   (WPARAM)50,(LPARAM)-100);
		SendDlgItemMessage(hDlg,IDC_RELATION12,TBM_SETRANGE,
			               (WPARAM)FALSE,(LPARAM)MAKELONG(-100,100));
		SendDlgItemMessage(hDlg,IDC_RELATION12,TBM_SETTICFREQ,
			               (WPARAM)50,(LPARAM)-100);
		SendDlgItemMessage(hDlg,IDC_RELATION21,TBM_SETRANGE,
			               (WPARAM)FALSE,(LPARAM)MAKELONG(-100,100));
		SendDlgItemMessage(hDlg,IDC_RELATION21,TBM_SETTICFREQ,
			               (WPARAM)50,(LPARAM)-100);
		SendDlgItemMessage(hDlg,IDC_RELATION22,TBM_SETRANGE,
			               (WPARAM)FALSE,(LPARAM)MAKELONG(-100,100));
		SendDlgItemMessage(hDlg,IDC_RELATION22,TBM_SETTICFREQ,
			               (WPARAM)50,(LPARAM)-100);
		if (SaveLoaded)
		{
			EnableControl(hDlg,IDC_INFO1); EnableControl(hDlg,IDC_INFO2);
			EnableControl(hDlg,IDC_INFO3); EnableControl(hDlg,IDC_INFO4);
			EnableControl(hDlg,IDC_SET1); EnableControl(hDlg,IDC_SET2);
			EnableControl(hDlg,IDC_SET3);
			EnableControl(hDlg,IDC_COPYFIRST);
			EnableControl(hDlg,IDC_COPYSECOND);
			EnableControl(hDlg,IDC_STATIC1); EnableControl(hDlg,IDC_STATIC2);
			EnableControl(hDlg,IDC_STATIC3); EnableControl(hDlg,IDC_STATIC4);
			EnableControl(hDlg,IDC_VALUE11); EnableControl(hDlg,IDC_VALUE12);
			EnableControl(hDlg,IDC_VALUE21); EnableControl(hDlg,IDC_VALUE22);
			EnableControl(hDlg,IDC_RELATION11);
			EnableControl(hDlg,IDC_RELATION12);
			EnableControl(hDlg,IDC_RELATION21);
			EnableControl(hDlg,IDC_RELATION22);
		} else
		{
			DisableControl(hDlg,IDC_1STORGANIZATIONCOMBO);
			DisableControl(hDlg,IDC_2NDORGANIZATIONCOMBO);
		}

		break;
	case 13:
		for (i=0;i<7;i++)
		{
			AdjustEdit(hDlg,IDC_WEEKEDIT1+i,9,999999999,-99999999);
			AdjustEdit(hDlg,IDC_TOTALEDIT1+i,9,999999999,-99999999);
			if (SaveLoaded)
			{
				SetDlgItemInt(hDlg,IDC_WEEKEDIT1+i,Score[0][i],TRUE);
				SetDlgItemInt(hDlg,IDC_TOTALEDIT1+i,Score[1][i],TRUE);
			}
		}
		if (SaveLoaded)
		{
			EnableControl(hDlg,IDC_STATIC1); EnableControl(hDlg,IDC_STATIC2);
			EnableControl(hDlg,IDC_STATIC3); EnableControl(hDlg,IDC_STATIC4);
			EnableControl(hDlg,IDC_STATIC5); EnableControl(hDlg,IDC_STATIC6);
			EnableControl(hDlg,IDC_STATIC7); EnableControl(hDlg,IDC_STATIC8);
			EnableControl(hDlg,IDC_STATIC9);
		}
		break;
	}

	// enable save/revert buttons if save is loaded
	if (SaveLoaded)
	{
		EnableControl(MainDialog,IDC_SAVEBUTTON); 
		EnableControl(MainDialog,IDC_REVERTBUTTON);
	}
	// redraw window if necessary
	if (Redraw)
	{
		InvalidateRgn(MainDialog,NULL,FALSE);
		UpdateWindow(MainDialog);
	}
}

// React when new attribute is selected.
VOID UpdateAttributeChanged(BOOL PageInit)
{
	HWND hDlg=TabWindow;   // tab window handle (it's shorter name 
	                       // after all :) )
	CHAR TempChar[20];     // temporary variables...
	INT i;
	
	if (!PageInit) // if this function is not called when tab page 
		           // is activated...
	{
		KillPage(); // read current parameters
		// and check what new attribute the user has chosen
		CurAttribute=(BYTE)SendDlgItemMessage(hDlg,IDC_ATTRIBUTECOMBO,
			                                  CB_GETCURSEL,0,0);
	}

	if ((SaveLoaded) && (NPeople>0))
	{
		// enable several controls (actually needed only at page 
		// initialization)
		EnableControl(hDlg,IDC_ISSLIDER);
		EnableControl(hDlg,IDC_ISVALUE);
		// set slider's range
		SendDlgItemMessage(hDlg,IDC_ISSLIDER,TBM_SETRANGE,(WPARAM)FALSE,
			               (LPARAM)MAKELONG(Margins[CurAttribute][0],
						   Margins[CurAttribute][1]));
		// set slider's position
		SendDlgItemMessage(hDlg,IDC_ISSLIDER,TBM_SETPOS,(WPARAM)TRUE,
			           (LPARAM)People[CurPerson].IsAttributes[CurAttribute]);

		// show selected value
		i=SendDlgItemMessage(hDlg,IDC_ISSLIDER,TBM_GETPOS,0,0);
		if (CurAttribute==6) i=i*10; // bravery?
		_ltoa(i,TempChar,10);
		SetDlgItemText(hDlg,IDC_ISVALUE,TempChar);

		if (WasIncluded[CurAttribute])
		{
			EnableControl(hDlg,IDC_STATIC4,"&Was:"); 
			// We change the name of the control (although it's still the
			// same name), 'cause we want Windows to redraw it).
			EnableControl(hDlg,IDC_WASVALUE);
			EnableControl(hDlg,IDC_WASSLIDER);
			SendDlgItemMessage(hDlg,IDC_WASSLIDER,TBM_SETRANGE,(WPARAM)FALSE,
							   (LPARAM)MAKELONG(Margins[CurAttribute][0],
							   Margins[CurAttribute][1]));
			SendDlgItemMessage(hDlg,IDC_WASSLIDER,TBM_SETPOS,(WPARAM)TRUE,
				      (LPARAM)People[CurPerson].WasAttributes[CurAttribute]);

			i=SendDlgItemMessage(hDlg,IDC_WASSLIDER,TBM_GETPOS,0,0);
			if (CurAttribute==6) i=i*10;
			_ltoa(i,TempChar,10);
			SetDlgItemText(hDlg,IDC_WASVALUE,TempChar);
		} else
		{
			SendDlgItemMessage(hDlg,IDC_WASSLIDER,TBM_SETPOS,
				               (WPARAM)TRUE,(LPARAM)0);
			DisableControl(hDlg,IDC_STATIC4,"&Was:");
			DisableControl(hDlg,IDC_WASVALUE,"");
			DisableControl(hDlg,IDC_WASSLIDER);
		}
	}

};


// React when new person is selected.
VOID UpdatePersonChanged(BOOL PageInit)
{
	HWND hDlg=TabWindow;

	if (!PageInit)
	{
		KillPage();
		CurPerson=(BYTE)SendDlgItemMessage(hDlg,IDC_PERSONCOMBO,
			                               CB_GETCURSEL,0,0);
	}

	if ((SaveLoaded) && (NPeople>0))
	{
		if (CurPage==7)
		{
			SendDlgItemMessage(hDlg,IDC_PHOTOLIST,LB_SETCURSEL,
				               (WPARAM)People[CurPerson].Photo,0);
			SetDlgItemText(hDlg,IDC_NAMEEDIT,People[CurPerson].Name);
			SetDlgItemText(hDlg,IDC_FUNCTION,
				           TFunctions[People[CurPerson].Function]);
		} else
		if (CurPage==8)
		{
			SendDlgItemMessage(hDlg,IDC_RANKCOMBO,CB_SETCURSEL,
				               (WPARAM)People[CurPerson].Rank,0);
			SendDlgItemMessage(hDlg,IDC_ORDERSSLIDER,TBM_SETPOS,
				               (WPARAM)TRUE,(LPARAM)People[CurPerson].Orders);
			SetDlgItemInt(hDlg,IDC_DAYSSERVICEEDIT,
				          People[CurPerson].DaysService,FALSE);
			SetDlgItemInt(hDlg,IDC_KILLSEDIT,People[CurPerson].Kills,FALSE);
			SetDlgItemInt(hDlg,IDC_MISSIONSEDIT,People[CurPerson].Missions,
						  FALSE);
			UpdateAttributeChanged(TRUE);
		};
	}
}

// React when new stock vehicle is selected.
VOID UpdateAvVehicleChanged(BOOL PageInit)
{
	HWND hDlg=TabWindow;

	if (!PageInit)
	{
		KillPage();
		CurAvVehicle=(BYTE)SendDlgItemMessage(hDlg,IDC_VEHICLELIST,
			                                  LB_GETCURSEL,0,0);
	}

	if (SaveLoaded)
	{
		SetDlgItemInt(hDlg,IDC_PRICE,AvVehicles[CurAvVehicle].Price,FALSE);
		SetDlgItemInt(hDlg,IDC_QUANTITY,AvVehicles[CurAvVehicle].Quantity,
			          FALSE);
	}
}

// React when new equipment is selected.
VOID UpdateEquipmentChanged(BOOL PageInit)
{
	HWND hDlg=TabWindow;
	if (!PageInit)
	{
		KillPage();
		CurEquipment=(BYTE)SendDlgItemMessage(hDlg,IDC_EQUIPMENTLIST,
			                                  LB_GETCURSEL,0,0);
	}
	if (SaveLoaded)
	{
		SetDlgItemInt(hDlg,IDC_ROUNDSEDIT,
			          Vehicles[CurVehicle].EquipmentRounds[CurEquipment],
					  FALSE);
	}
}

// React when new vehicle is selected.
VOID UpdateVehicleChanged(BOOL PageInit)
{
	HWND hDlg=TabWindow;
	INT i;

	if (!PageInit)
	{
		KillPage();
		CurVehicle=(BYTE)SendDlgItemMessage(hDlg,IDC_VEHICLELIST,
			                                LB_GETCURSEL,0,0);
		CurEquipment=0;
	}

	if ((SaveLoaded) && (NVehicles>0))
	{
		SetDlgItemInt(hDlg,IDC_CONSTITUTIONEDIT,
			          Vehicles[CurVehicle].Constitution,FALSE);
		SetDlgItemText(hDlg,IDC_VEHTYPE,
			           TVehicles[Vehicles[CurVehicle].Type]);
		SendDlgItemMessage(hDlg,IDC_OWNERCOMBO,CB_SETCURSEL,
			               Vehicles[CurVehicle].Owner,0);
		SetDlgItemText(hDlg,IDC_CARNAMEEDIT,Vehicles[CurVehicle].Name);
		
		SendDlgItemMessage(hDlg,IDC_EQUIPMENTLIST,LB_RESETCONTENT,0,0);

		for (i=0;i<Vehicles[CurVehicle].NEquipment;i++) 
			SendDlgItemMessage(hDlg,IDC_EQUIPMENTLIST,LB_ADDSTRING,
			(WPARAM)0,
			(LPARAM)(LPTSTR)TEquipment[Vehicles[CurVehicle].Equipment[i]]);

		SendDlgItemMessage(hDlg,IDC_EQUIPMENTLIST,LB_SETCURSEL,
			               (WPARAM)CurEquipment,0);
		if (Vehicles[CurVehicle].NEquipment==0)
		{
			SendDlgItemMessage(hDlg,IDC_ROUNDSEDIT,EM_SETREADONLY,
				               (WPARAM)TRUE,0);
			SetDlgItemText(hDlg,IDC_ROUNDSEDIT,"");
		} else
		{
			SendDlgItemMessage(hDlg,IDC_ROUNDSEDIT,EM_SETREADONLY,
				               (WPARAM)FALSE,0);
			UpdateEquipmentChanged(TRUE);
		}
	}
}

// React when new facility is selected.
VOID UpdateFacilityChanged(BOOL PageInit)
{
	HWND hDlg=TabWindow;

	if (!PageInit)
	{
		KillPage();
		CurFacility=(BYTE)SendDlgItemMessage(hDlg,IDC_FACILITYLIST,
			                                 LB_GETCURSEL,0,0);
	}

	if (SaveLoaded)
	{
		SendDlgItemMessage(hDlg,IDC_CANBEBUILTCHECK,BM_SETCHECK,
			               (WPARAM)Facilities[CurFacility].CanBeBuilt,0);
		SetDlgItemInt(hDlg,IDC_COSTTOBUILDEDIT,
			          Facilities[CurFacility].CostToBuild,FALSE);
		SetDlgItemInt(hDlg,IDC_DAYSTOBUILDEDIT,
			          Facilities[CurFacility].DaysToBuild,FALSE);
		SetDlgItemInt(hDlg,IDC_MAINTENANCECOSTEDIT,
			          Facilities[CurFacility].MaintenanceCost,FALSE);
		SetDlgItemInt(hDlg,IDC_CAPACITYEDIT,
			          Facilities[CurFacility].Capacity,FALSE);
	}
}

// React when new item or new base is selected.
VOID UpdateItemOrBaseChanged(BOOL PageInit)
{
	HWND hDlg=TabWindow;
	LONG i;

	if (!PageInit)
	{
		KillPage();
		CurItem=(BYTE)SendDlgItemMessage(hDlg,IDC_ITEMLIST,LB_GETCURSEL,0,0);
		CurBase=(BYTE)SendDlgItemMessage(hDlg,IDC_BASECOMBO,CB_GETCURSEL,0,0);
	}

	if (SaveLoaded)
	{
		if (CurCategory==0) 
			i=Bases[CurBase].Items[(Items1Off[CurItem]-0x1e002)/2];
		else
		if (CurCategory==1) 
			i=Bases[CurBase].Items[(Items2Off[CurItem]-0x1e002)/2];
		else
		if (CurCategory==2) 
			i=Bases[CurBase].Items[(Items3Off[CurItem]-0x1e002)/2];
		else
		if (CurCategory==3) 
			i=Bases[CurBase].Items[(Items4Off[CurItem]-0x1e002)/2];

		SetDlgItemInt(hDlg,IDC_QUANTITY,i,FALSE);
	}
}

// React when new category is selected.
VOID UpdateCategoryChanged(BOOL PageInit)
{
	HWND hDlg=TabWindow;
	LONG i;

	if (!PageInit)
	{
		KillPage();
		CurCategory=(BYTE)SendDlgItemMessage(hDlg,IDC_CATEGORYCOMBO,
			                                 CB_GETCURSEL,0,0);
		CurItem=0;
	};

	SendDlgItemMessage(hDlg,IDC_ITEMLIST,LB_RESETCONTENT,0,0);

	if (CurCategory==0) for (i=0;i<86;i++) 
		SendDlgItemMessage(hDlg,IDC_ITEMLIST,LB_ADDSTRING,(WPARAM)0,
		                   (LPARAM)(LPTSTR)TItems1[i]);
	else 
	if (CurCategory==1) for (i=0;i<49;i++) 
		SendDlgItemMessage(hDlg,IDC_ITEMLIST,LB_ADDSTRING,(WPARAM)0,
		                   (LPARAM)(LPTSTR)TItems2[i]);
	else
	if (CurCategory==2) for (i=0;i<28;i++) 
		SendDlgItemMessage(hDlg,IDC_ITEMLIST,LB_ADDSTRING,(WPARAM)0,
		                   (LPARAM)(LPTSTR)TItems3[i]);
	else
	if (CurCategory==3) for (i=0;i<30;i++) 
		SendDlgItemMessage(hDlg,IDC_ITEMLIST,LB_ADDSTRING,(WPARAM)0,
		                   (LPARAM)(LPTSTR)TItems4[i]);

	SendDlgItemMessage(hDlg,IDC_ITEMLIST,LB_SETCURSEL,(WPARAM)CurItem,0);

	UpdateItemOrBaseChanged(TRUE);
}

// React when new project is selected.
VOID UpdateProjectChanged(BOOL PageInit)
{
	HWND hDlg=TabWindow;
	CHAR TempChar[20];

	if (!PageInit)
	{
		KillPage();
		CurProject=(BYTE)SendDlgItemMessage(hDlg,IDC_PROJECTLIST,
			                                LB_GETCURSEL,0,0);
	}

	SendDlgItemMessage(hDlg,IDC_FINISHED,BM_SETCHECK,
		               (WPARAM)Projects[CurProject].Finished,0);
	SendDlgItemMessage(hDlg,IDC_PROGRESSSLIDER,TBM_SETPOS,
		               (WPARAM)TRUE,(LPARAM)Projects[CurProject].Progress);

	_ltoa(SendDlgItemMessage(hDlg,IDC_PROGRESSSLIDER,TBM_GETPOS,0,0),
		                     TempChar,10);
	lstrcat(TempChar," %");
	SetDlgItemText(hDlg,IDC_PROGRESSPERC,TempChar);

	if (TCategory[CurProject]==0) 
		SetDlgItemText(hDlg,IDC_CATEGORY,"Biochemistry"); else 
		SetDlgItemText(hDlg,IDC_CATEGORY,"Quantum physics");

	SetDlgItemText(hDlg,IDC_TOPIC1,(CHAR *)TTopics[CurProject][0]);
	SetDlgItemText(hDlg,IDC_TOPIC2,(CHAR *)TTopics[CurProject][1]);
	SetDlgItemText(hDlg,IDC_TOPIC3,(CHAR *)TTopics[CurProject][2]);
}

// React when new organization is selected.
VOID UpdateOrganizationChanged(BOOL PageInit)
{
	HWND hDlg=TabWindow;
	INT i;

	if (!PageInit)
	{
		KillPage();
		Cur1stOrganization=(BYTE)SendDlgItemMessage(hDlg,
			                IDC_1STORGANIZATIONCOMBO,CB_GETCURSEL,0,0);
		Cur2ndOrganization=(BYTE)SendDlgItemMessage(hDlg,
			                IDC_2NDORGANIZATIONCOMBO,CB_GETCURSEL,0,0);
	}
		
	SendDlgItemMessage(hDlg,IDC_RELATION11,TBM_SETPOS,(WPARAM)TRUE,
		(LPARAM)(CHAR)Relations[0][Cur1stOrganization][Cur2ndOrganization]);
	SendDlgItemMessage(hDlg,IDC_RELATION12,TBM_SETPOS,(WPARAM)TRUE,
		(LPARAM)(CHAR)Relations[1][Cur1stOrganization][Cur2ndOrganization]);
	SendDlgItemMessage(hDlg,IDC_RELATION21,TBM_SETPOS,(WPARAM)TRUE,
		(LPARAM)(CHAR)Relations[0][Cur2ndOrganization][Cur1stOrganization]);
	SendDlgItemMessage(hDlg,IDC_RELATION22,TBM_SETPOS,(WPARAM)TRUE,
		(LPARAM)(CHAR)Relations[1][Cur2ndOrganization][Cur1stOrganization]);

	i=SendDlgItemMessage(hDlg,IDC_RELATION11,TBM_GETPOS,0,0);
	SetDlgItemInt(hDlg,IDC_VALUE11,i,TRUE);
	i=SendDlgItemMessage(hDlg,IDC_RELATION12,TBM_GETPOS,0,0);
	SetDlgItemInt(hDlg,IDC_VALUE12,i,TRUE);
	i=SendDlgItemMessage(hDlg,IDC_RELATION21,TBM_GETPOS,0,0);
	SetDlgItemInt(hDlg,IDC_VALUE21,i,TRUE);
	i=SendDlgItemMessage(hDlg,IDC_RELATION22,TBM_GETPOS,0,0);
	SetDlgItemInt(hDlg,IDC_VALUE22,i,TRUE);
}

//---------------------------------------------------------- Save functions -

// Here we have several read/write functions for different value sizes
// (byte, word, double word, and string). I am going to comment only
// first variation.

// Write 1-byte value 
BOOL WriteByteValue(LONG Offset,PBYTE Value)
{
	DWORD BytesWritten; // how many bytes went into savegame?
	BOOL OK=TRUE;       // it's everything ok?

	// Try to set file pointer to specified offset.
	if (SetFilePointer(SaveFile,Offset,NULL,FILE_BEGIN)==0xFFFFFFFF) OK=FALSE; else
	// Try to write the byte.
	if (!WriteFile(SaveFile,Value,1,&BytesWritten,NULL)) OK=FALSE; else
	// Check if we have written it.
	//if (BytesWritten==0) OK=FALSE;

	// If it isn't OK,
	if (!OK)
	{
		// display a message box,
		MessageBox(MainDialog,"Cannot write to savegame!","Midnight Editor",
			       MB_APPLMODAL | MB_OK | MB_ICONEXCLAMATION);
		// close the save handle,
		CloseHandle(SaveFile);
		// destroy "Writing data" window,
		DestroyWindow(Popup);
		// and return appropriate value.
		return(FALSE);
	};
	// Otherwise, report success.
	return(TRUE);
}

// Read 1-byte value
BOOL ReadByteValue(LONG Offset,PBYTE Value,BYTE Min=0,BYTE Max=255)
{
	DWORD BytesRead; // how many bytes were read?
	BOOL OK=TRUE;    // is everything ok?

	// Try to set file pointer to specified offset.
	if (SetFilePointer(SaveFile,Offset,NULL,FILE_BEGIN)==0xFFFFFFFF) OK=FALSE; else
	// Try to read from file.
	if (!ReadFile(SaveFile,Value,1,&BytesRead,NULL)) OK=FALSE; else
	// Check if we have read the byte.
	if (BytesRead==0) OK=FALSE;

	// If it isn't OK,
	if (!OK)
	{
		// display a warning message box,
		MessageBox(MainDialog,"Cannot read from savegame!","Midnight Editor",
			       MB_APPLMODAL | MB_OK | MB_ICONEXCLAMATION);
		// close the save handle,
		CloseHandle(SaveFile);
		// destroy "reading data" window,
		DestroyWindow(Popup);
		// and return appropriate value.
		return(FALSE);
	};

	// Otherwise check if value doesn't exceed specified boundaries.
	if ((*Value<Min) || (*Value>Max))
	{
		// Display a warning message box only if it's our "first time".
		if (WrongValueReminder)
		{
			if (MessageBox(MainDialog,
				"Midnight Editor encountered illegal value in the savegame. Further modifications can cause some unexpectable game behaviour. It is recommended to backup the savegame before making any changes. Also make sure you have selected the right file. Do you still want to open this savegame?",
				"Midnight Editor",MB_APPLMODAL | MB_YESNO | 
				MB_ICONEXCLAMATION )==IDNO) 
			{
				// If the user doesn't want to continue, we will close 
				// the handle
				CloseHandle(SaveFile);
				// destroy "reading data" window,
				DestroyWindow(Popup);
				// and report failure.
				return(FALSE); 
			}
			// Otherwise we do not want the message box to be displayed again.
			WrongValueReminder=FALSE;
		}
		// Oh, and "repair" the value.
		*Value=Min;
	};
	// Report success.
	return(TRUE);
}

// Write 2-bytes (word) value
BOOL WriteWordValue(LONG Offset,PWORD Value)
{
	DWORD BytesWritten;
	BOOL OK=TRUE;
	if (SetFilePointer(SaveFile,Offset,NULL,FILE_BEGIN)==0xFFFFFFFF) OK=FALSE; else
	if (!WriteFile(SaveFile,Value,2,&BytesWritten,NULL)) OK=FALSE; else
	if (BytesWritten!=2) OK=FALSE;
	if (!OK)
	{
		MessageBox(MainDialog,"Cannot write to savegame!","Midnight Editor",
			       MB_APPLMODAL | MB_OK | MB_ICONEXCLAMATION);
		CloseHandle(SaveFile);
		DestroyWindow(Popup);
		return(FALSE);
	};
	return(TRUE);
}

// Read 2-bytes (word) value
BOOL ReadWordValue(LONG Offset,PWORD Value,WORD Min=0,WORD Max=65535)
{
	DWORD BytesRead;
	BOOL OK=TRUE;
	if (SetFilePointer(SaveFile,Offset,NULL,FILE_BEGIN)==0xFFFFFFFF) OK=FALSE; else
	if (!ReadFile(SaveFile,Value,2,&BytesRead,NULL)) OK=FALSE; else
	if (BytesRead!=2) OK=FALSE;

	if (!OK)
	{
		MessageBox(MainDialog,"Cannot read from savegame!","Midnight Editor",
			       MB_APPLMODAL | MB_OK | MB_ICONEXCLAMATION);
		CloseHandle(SaveFile);
		DestroyWindow(Popup);
		return(FALSE);
	};

	if ((*Value<Min) || (*Value>Max))
	{
		if (WrongValueReminder)
		{
			if (MessageBox(MainDialog,
			    "Midnight Editor encountered illegal value in the savegame. Further modifications can cause some unexpectable game behaviour. It is recommended to backup the savegame before making any changes. Also make sure you have selected the right file. Do you still want to open this savegame?",
				"Midnight Editor",MB_APPLMODAL | MB_YESNO | 
				MB_ICONEXCLAMATION )==IDNO) 
			{ 
				CloseHandle(SaveFile);
				DestroyWindow(Popup);
				return(FALSE); 
			}
			WrongValueReminder=FALSE;
		}
		*Value=Min;
	};
	return(TRUE);
}

// Write 4-bytes (double word) value
BOOL WriteDWordValue(LONG Offset,PLONG Value)
{
	DWORD BytesWritten;
	BOOL OK=TRUE;
	if (SetFilePointer(SaveFile,Offset,NULL,FILE_BEGIN)==0xFFFFFFFF) OK=FALSE; else
	if (!WriteFile(SaveFile,Value,4,&BytesWritten,NULL)) OK=FALSE; else
	if (BytesWritten!=4) OK=FALSE;

	if (!OK)
	{
		MessageBox(MainDialog,"Cannot write to savegame!","Midnight Editor",
			       MB_APPLMODAL | MB_OK | MB_ICONEXCLAMATION);
		CloseHandle(SaveFile);
		DestroyWindow(Popup);
		return(FALSE);
	};
	return(TRUE);
}

// Read 4-bytes (double word) value
BOOL ReadDWordValue(LONG Offset,PLONG Value,LONG Min,LONG Max)
{
	DWORD BytesRead;
	BOOL OK=TRUE;
	if (SetFilePointer(SaveFile,Offset,NULL,FILE_BEGIN)==0xFFFFFFFF) OK=FALSE; else
	if (!ReadFile(SaveFile,Value,4,&BytesRead,NULL)) OK=FALSE; else
	if (BytesRead!=4) OK=FALSE;

	if (!OK)
	{
		MessageBox(MainDialog,"Cannot read from savegame!","Midnight Editor",
			       MB_APPLMODAL | MB_OK | MB_ICONEXCLAMATION);
		CloseHandle(SaveFile);
		DestroyWindow(Popup);
		return(FALSE);
	};

	if ((*Value<Min) || (*Value>Max))
	{
		if (WrongValueReminder)
		{
			if (MessageBox(MainDialog,
			    "Midnight Editor encountered illegal value in the savegame. Further modifications can cause some unexpectable game behaviour. It is recommended to backup the savegame before making any changes. Also make sure you have selected the right file. Do you still want to open this savegame?",
				"Midnight Editor",MB_APPLMODAL | MB_YESNO | 
				MB_ICONEXCLAMATION )==IDNO) 
			{ 
				CloseHandle(SaveFile); 
				DestroyWindow(Popup);
				return(FALSE); 
			}
			WrongValueReminder=FALSE;
		}
		*Value=Min;
	};
	return(TRUE);
}

// Write string value
BOOL WriteStringValue(LONG Offset,LPSTR Value,WORD Length)
{
	DWORD BytesWritten;
	BOOL OK=TRUE;
	if (SetFilePointer(SaveFile,Offset,NULL,FILE_BEGIN)==0xFFFFFFFF) OK=FALSE; else
	if (!WriteFile(SaveFile,Value,Length,&BytesWritten,NULL)) OK=FALSE; else
	if (BytesWritten!=Length) OK=FALSE;

	if (!OK)
	{
		MessageBox(MainDialog,"Cannot write to savegame!","Midnight Editor",
			       MB_APPLMODAL | MB_OK | MB_ICONEXCLAMATION);
		CloseHandle(SaveFile);
		DestroyWindow(Popup);
		return(FALSE);
	};
	return(TRUE);
}

// Read string value
BOOL ReadStringValue(LONG Offset,LPSTR Value,WORD Length)
{
	DWORD BytesRead;
	BOOL OK=TRUE;
	if (SetFilePointer(SaveFile,Offset,NULL,FILE_BEGIN)==0xFFFFFFFF) OK=FALSE; else
	if (!ReadFile(SaveFile,Value,Length,&BytesRead,NULL)) OK=FALSE; else
	if (BytesRead!=Length) OK=FALSE;
	if (!OK)
	{
		MessageBox(MainDialog,"Cannot read from savegame!","Midnight Editor",
			       MB_APPLMODAL | MB_OK | MB_ICONEXCLAMATION);
		CloseHandle(SaveFile);
		DestroyWindow(Popup);
		return(FALSE);
	};
	return(TRUE);
}

// Read string value with no error-reporting to user (used for descripting
// savegames in "Open savegame" dialog)
BOOL ReadStringValueNoError(LONG Offset,LPSTR Value,WORD Length)
{
	DWORD BytesRead;
	BOOL OK=TRUE;
	if (SetFilePointer(SaveFile,Offset,NULL,FILE_BEGIN)==0xFFFFFFFF) OK=FALSE; else
	if (!ReadFile(SaveFile,Value,Length,&BytesRead,NULL)) OK=FALSE; else
	if (BytesRead!=Length) OK=FALSE;
	return(OK);
}

// OK, now read the whole data...
BOOL ReadSavegameData()
{
	OFSTRUCT OpenFileStruct;    // structure used for opening file
	CHAR BackupName[MAX_PATH];  // backup file name
	INT i,j,k,l;                // temporary variables...
	CHAR *pBackslash;          
	CHAR TempName[255];        
	LONG Off;
	BYTE b;

	// Create "Reading data..." window
	Popup=CreateDialog(CurInstance,MAKEINTRESOURCE(IDD_READING),
		               MainDialog,NULL);
	// and show it.
	ShowWindow(Popup,SW_SHOWNORMAL);

	// Open savegame.
	SaveFile=(HANDLE)OpenFile(NewSaveName,&OpenFileStruct,
		      OF_PROMPT | OF_READ);

	// Check if it's really been opened.
	if (SaveFile==(HANDLE)-1) 
	{
		// If not - display warning message box,
		MessageBox(MainDialog,"Cannot open savegame!","Midnight Editor",
			       MB_APPLMODAL | MB_OK | MB_ICONEXCLAMATION);
		// close save handle,
		CloseHandle(SaveFile);
		// destroy "Reading data..." window,
		DestroyWindow(Popup);
		// and return failure.
		return(FALSE); 
	}
	
	// if all savegames should have the same size and it's different than 
	// the one user has selected...
	if (SaveExpectedSize!=0)
	if ((GetFileSize(SaveFile,(LPDWORD)NULL)!=SaveExpectedSize) 
	   && (WrongValueReminder))
	{
		// report this to him/her.
		if (MessageBox(MainDialog,
            "The savegame you selected seems to have illegal size. It is recommended to backup the savegame before making any changes. Also make sure you have selected the right file. Do you still want to open this savegame?",
            "Midnight Editor",
            MB_APPLMODAL | MB_YESNO | MB_ICONEXCLAMATION )==IDNO)
        {
			CloseHandle(SaveFile);
			DestroyWindow(Popup);
            return(FALSE);
        }
    };

	// OK, now read the whole savegame :)...
	if (!ReadStringValue(0x00,(LPTSTR)SaveTime,40)) return(FALSE);
	if (!ReadStringValue(0x28,(LPTSTR)SaveDescription,40)) return(FALSE);

	// ...cash...
	if (!ReadDWordValue(0x33d40,&Cash,0,999999999)) return(FALSE);

	// ...data...
	if (!ReadWordValue(0x51,&Year,0,32767)) return(FALSE);
	if (!ReadByteValue(0x53,&Month,0,11)) return(FALSE);
	if (!ReadByteValue(0x55,&WeekDay,0,6)) return(FALSE);
	if (!ReadByteValue(0x57,&Day,1,31)) return(FALSE);
	if (!ReadByteValue(0x59,&Hour,0,23)) return(FALSE);
	if (!ReadByteValue(0x5b,&Minute,0,59)) return(FALSE);
	if (!ReadByteValue(0x5d,&Second,0,59)) return(FALSE);
	if (!ReadWordValue(0x6ac9b,&Week,0,32767)) return(FALSE);
	Day--;

	// ...score...
	for (j=0;j<=6;j++)
	{
		if (!ReadDWordValue(0x6ab41+j*8,&Score[0][j],-99999999,999999999)) 
			return(FALSE);
		if (!ReadDWordValue(0x6ab41+4+j*8,&Score[1][j],-99999999-999999999,
			999999999+99999999)) return(FALSE);
		Score[1][j]+=Score[0][j];
	}

	// ...projects...
	for (i=0;i<99;i++)
	{
		if (!ReadDWordValue(0x67e56+i*10,&Projects[i].RealProgress,0,
			           MaxProgress[i])) return(FALSE);
		if (!ReadByteValue(0x67e5a+i*10,&Projects[i].Finished,0,1)) return(FALSE);
		Projects[i].Progress=
			(BYTE)(100*Projects[i].RealProgress/MaxProgress[i]);

		if (!ReadByteValue(0x67e54+i*10,&Projects[i].Group)) return(FALSE);
	}

	// ...people...
	NPeople=0;
	j=0;
	for (i=0;i<179;i++)
	{
		Off=0x20d6a+0xce*i;
		if (!ReadStringValue(Off,(LPTSTR)TempName,25)) return(FALSE);
		if (lstrlen(TempName)!=0)
		{
			People[j].SavegameOffset=Off;

			lstrcpy(People[j].Name,TempName);
			if (!ReadByteValue(Off+0xb5,&People[j].Photo,0,74)) return(FALSE);
			if (!ReadByteValue(Off+0x23,&People[j].Function,0,34)) return(FALSE);
			if (!ReadByteValue(Off+0x22,&People[j].Rank,0,6)) return(FALSE);

			if (!ReadByteValue(Off+0x3c,&People[j].IsAttributes[0])) return(FALSE);

			if (!ReadByteValue(Off+0x2f,&People[j].WasAttributes[1],1,255)) return(FALSE);
			if (!ReadByteValue(Off+0x3b,&People[j].IsAttributes[1],1,255)) return(FALSE);

			if (!ReadByteValue(Off+0x34,&People[j].WasAttributes[2],0,100)) return(FALSE);
			if (!ReadByteValue(Off+0x46,&People[j].IsAttributes[2],0,100)) return(FALSE);
			People[j].WasAttributes[2]=100-People[j].WasAttributes[2];
			People[j].IsAttributes[2]=100-People[j].IsAttributes[2];

			if (!ReadByteValue(Off+0x31,&People[j].WasAttributes[3])) return(FALSE);
			if (!ReadByteValue(Off+0x3f,&People[j].IsAttributes[3])) return(FALSE);
			
			if (!ReadByteValue(Off+0x2e,&People[j].WasAttributes[4])) return(FALSE);
			if (!ReadByteValue(Off+0x3a,&People[j].IsAttributes[4])) return(FALSE);

			if (!ReadByteValue(Off+0x30,&People[j].WasAttributes[5],0,254)) return(FALSE);
			if (!ReadByteValue(Off+0x3e,&People[j].IsAttributes[5],0,254)) return(FALSE);
			People[j].WasAttributes[5]=(BYTE)(People[j].WasAttributes[5]/2);
			People[j].IsAttributes[5]=(BYTE)(People[j].IsAttributes[5]/2);

			if (!ReadByteValue(Off+0x33,&People[j].WasAttributes[6],0,25)) return(FALSE);
			if (!ReadByteValue(Off+0x41,&People[j].IsAttributes[6],0,25)) return(FALSE);

			if (!ReadByteValue(Off+0x32,&People[j].WasAttributes[7])) return(FALSE);
			if (!ReadByteValue(Off+0x40,&People[j].IsAttributes[7])) return(FALSE);
			if (!ReadByteValue(Off+0x35,&People[j].WasAttributes[8])) return(FALSE);
			if (!ReadByteValue(Off+0x42,&People[j].IsAttributes[8])) return(FALSE);
			if (!ReadByteValue(Off+0x36,&People[j].WasAttributes[9])) return(FALSE);
			if (!ReadByteValue(Off+0x43,&People[j].IsAttributes[9])) return(FALSE);
			if (!ReadByteValue(Off+0x37,&People[j].WasAttributes[10])) return(FALSE);
			if (!ReadByteValue(Off+0x44,&People[j].IsAttributes[10])) return(FALSE);
			if (!ReadByteValue(Off+0x49,&People[j].IsAttributes[11])) return(FALSE);
			if (!ReadByteValue(Off+0x4a,&People[j].IsAttributes[12])) return(FALSE);
			if (!ReadByteValue(Off+0x4b,&People[j].IsAttributes[13])) return(FALSE);

			if (!ReadWordValue(Off+0x26,&People[j].DaysService)) return(FALSE);
			if (!ReadWordValue(Off+0x28,&People[j].Kills)) return(FALSE);
			if (!ReadWordValue(Off+0x2a,&People[j].Missions)) return(FALSE);

			if (!ReadByteValue(Off+0x2d,&People[j].Orders,0,6)) return(FALSE);
			if (People[j].Orders==5) People[j].Orders=4;
			if (People[j].Orders==6) People[j].Orders=5;

			j++;
		}
	}
	NPeople=j;

	// ..stock vehicles...
	for (i=0;i<33;i++)
	{
		if (!ReadDWordValue(0x691ca+i*22,&AvVehicles[i].Price,0,999999999)) return(FALSE);
		if (!ReadDWordValue(0x691d6+i*22,&AvVehicles[i].Quantity,0,99999)) return(FALSE);
	}

	// ...bases...
	NBases=0;
	j=0;
	for (i=0;i<8;i++)
	{
		Off=0x1df6c+0x2be*i;
		if (!ReadStringValue(Off,(LPTSTR)TempName,18)) return(FALSE);
		if (lstrlen(TempName)!=0)
		{
			lstrcpy(Bases[i].Name,TempName);

			for (k=0;k<255;k++)
				if (!ReadWordValue(Off+0x96+k*2,&Bases[i].Items[k])) return(FALSE);

			Bases[i].Items[55]=(WORD)(Bases[i].Items[55]/16);
			Bases[i].Items[57]=(WORD)(Bases[i].Items[57]/36);
			Bases[i].Items[59]=(WORD)(Bases[i].Items[59]/30);
			Bases[i].Items[61]=(WORD)(Bases[i].Items[61]/24);
			Bases[i].Items[62]=(WORD)(Bases[i].Items[62]/24);
			Bases[i].Items[63]=(WORD)(Bases[i].Items[63]/24);
			Bases[i].Items[65]=(WORD)(Bases[i].Items[65]/42);
			Bases[i].Items[71]=(WORD)(Bases[i].Items[71]/5);
			Bases[i].Items[72]=(WORD)(Bases[i].Items[72]/5);
			Bases[i].Items[73]=(WORD)(Bases[i].Items[73]/5);
			Bases[i].Items[82]=(WORD)(Bases[i].Items[82]/16);
			Bases[i].Items[83]=(WORD)(Bases[i].Items[83]/16);
			Bases[i].Items[84]=(WORD)(Bases[i].Items[84]/16);
			Bases[i].Items[106]=(WORD)(Bases[i].Items[106]/3);

			for (k=0;k<30;k++)
			{
				Bases[i].Items[194+k]+=Bases[i].Items[194+30+k];
				Bases[i].Items[194+30+k]=0;
			}

			for (k=0;k<64;k++)
			{
				if (!ReadByteValue(Off+0x14+k,&Bases[i].Facilities[k])) return(FALSE);
				if (!ReadByteValue(Off+0x54+k,&Bases[i].DaysLeft[k])) return(FALSE);
			}

			j++;
		}
	}
	NBases=j;

	// ...facilities...
	for (i=0;i<17;i++)
	{
		if (!ReadWordValue(0x20c5a+i*16,&Facilities[i].CostToBuild)) return(FALSE);
		if (!ReadByteValue(0x20c60+i*16,&Facilities[i].DaysToBuild)) return(FALSE);
		if (!ReadWordValue(0x20c62+i*16,&Facilities[i].MaintenanceCost)) return(FALSE);
		if (!ReadWordValue(0x20c64+i*16,&Facilities[i].Capacity)) return(FALSE);
		if (!ReadByteValue(0x20c67+i*16,&Facilities[i].CanBeBuilt,0,1)) return(FALSE);
	}

	// ...vehicles...
	NVehicles=0;
	j=0;
	for (i=0;i<79;i++)
	{
		Off=0x8174+0x276*i;
		if (!ReadStringValue(Off,(LPTSTR)TempName,25)) return(FALSE);
		if (lstrlen(TempName)!=0)
		{
			Vehicles[j].SavegameOffset=Off;
			lstrcpy(Vehicles[j].Name,TempName);

			if (!ReadWordValue(Off+0x128,&Vehicles[i].Constitution,0,32767)) return(FALSE);
			if (!ReadByteValue(Off-2,&Vehicles[i].Type,0,32)) return(FALSE);
			if (!ReadByteValue(Off+0x126,&Vehicles[i].Owner,0,0x1b)) return(FALSE);

			l=0;
			Vehicles[j].NEquipment=0;
			for (k=0;k<33;k++)
			{
				if (!ReadByteValue(Off+0x187+k,&b)) return(FALSE);
				if ((b!=0) && (b!=255))
				{
					Vehicles[i].Equipment[l]=b;
					Vehicles[i].EquipmentNumber[l]=k;
					if (!ReadWordValue(Off+0x1b4+k*2,
						&Vehicles[i].EquipmentRounds[l],0,32767)) return(FALSE);

					l++;
				}
			}
			Vehicles[j].NEquipment=l;
			
			j++;
		}
	}
	NVehicles=j;

	//...relations... OK, that's all. 
	for (i=0;i<28;i++)
	for (j=0;j<28;j++)
	for (k=0;k<2;k++)
	if (!ReadByteValue(0x224+784*k+i+28*j,&Relations[k][i][j])) return(FALSE);

	// Check if the savegame was backuped...
	lstrcpy(BackupName,NewSaveName);
	lstrcat(BackupName,".bak");
	BackupPresent=OpenFile(BackupName,&OpenFileStruct,OF_EXIST)!=HFILE_ERROR;

	// reset some variables 
    CurPerson=0;
	CurBase=0;
	CurVehicle=0;

	// close handle, destroy "Reading..." window
	CloseHandle(SaveFile);
	DestroyWindow(Popup);

	// change the main dialog title
	strcpy(TempName,Title);
	strcat(TempName," - ");
	pBackslash=strrchr(NewSaveName,'\\');
	if (pBackslash==NULL) strcat(TempName,NewSaveName); 
	else strcat(TempName,pBackslash+1);
	strcat(TempName," (");
	strcat(TempName,SaveDescription);
	strcat(TempName,")");
	SetWindowText(MainDialog,TempName);

	return(TRUE);
}

// Write all data to savegame. It's pretty much the same as reading.
BOOL WriteSavegameData()
{
	OFSTRUCT OpenFileStruct;
	INT i,j,k;
	BYTE a,b;
	LONG Off;

	Popup=CreateDialog(CurInstance,MAKEINTRESOURCE(IDD_WRITING),
		               MainDialog,NULL);
	ShowWindow(Popup,SW_SHOWNORMAL);

	SaveFile=(HANDLE)OpenFile(SaveName,&OpenFileStruct,OF_PROMPT | OF_WRITE);

	if (SaveFile==(HANDLE)-1) 
	{
		MessageBox(MainDialog,"Cannot open savegame!",
				   "Midnight Editor", MB_APPLMODAL | MB_OK | 
				   MB_ICONEXCLAMATION);
		CloseHandle(SaveFile);
		DestroyWindow(Popup);
		return(FALSE); 
	}

	if (!WriteStringValue(0x00,(LPTSTR)SaveTime,40)) return(FALSE);

	if (!WriteDWordValue(0x33d40,&Cash)) return(FALSE);

	Day++;
	if (!WriteWordValue(0x51,&Year)) return(FALSE);
	if (!WriteByteValue(0x53,&Month)) return(FALSE);
	if (!WriteByteValue(0x55,&WeekDay)) return(FALSE);
	if (!WriteByteValue(0x57,&Day)) return(FALSE);
	if (!WriteByteValue(0x59,&Hour)) return(FALSE);
	if (!WriteByteValue(0x5b,&Minute)) return(FALSE);
	if (!WriteByteValue(0x5d,&Second)) return(FALSE);
	if (!WriteWordValue(0x6ac9b,&Week)) return(FALSE);
	Day--;

	for (j=0;j<=6;j++)
	{
		Score[1][j]-=Score[0][j];
		for (i=0;i<=1;i++)
		if (!WriteDWordValue(0x6ab41+i*4+j*8,&Score[i][j])) return(FALSE);
		Score[1][j]+=Score[0][j];
	}

	for (i=0;i<99;i++)
	{
		Projects[i].RealProgress=Projects[i].Progress*MaxProgress[i]/100;
		if (!WriteDWordValue(0x67e56+i*10,&Projects[i].RealProgress)) return(FALSE);
		if (!WriteByteValue(0x67e5a+i*10,&Projects[i].Finished)) return(FALSE);

		if (Projects[i].Finished==1) 
		{
			a=0x00; b=0x63;
		} else
		{
			if ((Projects[i].Group==0x63) || (Projects[i].Group=0xff)) 
			{ a=0xff; b=0xff; } else { a=0x00; b=Projects[i].Group; };
		}

		if (!WriteByteValue(0x67e53+i*10,&a)) return(FALSE);
		if (!WriteByteValue(0x67e54+i*10,&b)) return(FALSE);


		for (j=0;j<3;j++)
		if (PediaOff1[i][j]!=0)
		{
			if (!WriteByteValue(PediaOff1[i][j],&Projects[i].Finished)) return(FALSE);
		}

		for (j=0;j<3;j++)
		if (PediaOff0[i][j]!=0)
		{
			a=!Projects[i].Finished;
			if (!WriteByteValue(PediaOff0[i][j],&a)) return(FALSE);
		}
	}
	
	if (NPeople>0)
	for (j=0;j<NPeople;j++)
	{
		Off=People[j].SavegameOffset;
		if (!WriteStringValue(Off,(LPSTR)&People[j].Name,25)) return(FALSE);

		if (!WriteByteValue(Off+0xb5,&People[j].Photo)) return(FALSE);
		if (!WriteByteValue(Off+0x23,&People[j].Function)) return(FALSE);
		if (!WriteByteValue(Off+0x22,&People[j].Rank)) return(FALSE);

		if (!WriteByteValue(Off+0x3c,&People[j].IsAttributes[0])) return(FALSE);

		if (!WriteByteValue(Off+0x2f,&People[j].WasAttributes[1])) return(FALSE);
		if (!WriteByteValue(Off+0x3b,&People[j].IsAttributes[1])) return(FALSE);

		People[j].WasAttributes[2]=100-People[j].WasAttributes[2];
		People[j].IsAttributes[2]=100-People[j].IsAttributes[2];
		if (!WriteByteValue(Off+0x34,&People[j].WasAttributes[2])) return(FALSE);
		if (!WriteByteValue(Off+0x46,&People[j].IsAttributes[2])) return(FALSE);
		People[j].WasAttributes[2]=100-People[j].WasAttributes[2];
		People[j].IsAttributes[2]=100-People[j].IsAttributes[2];

		if (!WriteByteValue(Off+0x31,&People[j].WasAttributes[3])) return(FALSE);
		if (!WriteByteValue(Off+0x3f,&People[j].IsAttributes[3])) return(FALSE);
			
		if (!WriteByteValue(Off+0x2e,&People[j].WasAttributes[4])) return(FALSE);
		if (!WriteByteValue(Off+0x3a,&People[j].IsAttributes[4])) return(FALSE);

		People[j].WasAttributes[5]=(BYTE)(People[j].WasAttributes[5]*2);
		People[j].IsAttributes[5]=(BYTE)(People[j].IsAttributes[5]*2);
		if (!WriteByteValue(Off+0x30,&People[j].WasAttributes[5])) return(FALSE);
		if (!WriteByteValue(Off+0x3e,&People[j].IsAttributes[5])) return(FALSE);
		People[j].WasAttributes[5]=(BYTE)(People[j].WasAttributes[5]/2);
		People[j].IsAttributes[5]=(BYTE)(People[j].IsAttributes[5]/2);

		if (!WriteByteValue(Off+0x33,&People[j].WasAttributes[6])) return(FALSE);
		if (!WriteByteValue(Off+0x41,&People[j].IsAttributes[6])) return(FALSE);

		if (!WriteByteValue(Off+0x32,&People[j].WasAttributes[7])) return(FALSE);
		if (!WriteByteValue(Off+0x40,&People[j].IsAttributes[7])) return(FALSE);
		if (!WriteByteValue(Off+0x35,&People[j].WasAttributes[8])) return(FALSE);
		if (!WriteByteValue(Off+0x42,&People[j].IsAttributes[8])) return(FALSE);
		if (!WriteByteValue(Off+0x36,&People[j].WasAttributes[9])) return(FALSE);
		if (!WriteByteValue(Off+0x43,&People[j].IsAttributes[9])) return(FALSE);
		if (!WriteByteValue(Off+0x44,&People[j].IsAttributes[10])) return(FALSE);
		if (!WriteByteValue(Off+0x49,&People[j].IsAttributes[11])) return(FALSE);
		if (!WriteByteValue(Off+0x4a,&People[j].IsAttributes[12])) return(FALSE);
		if (!WriteByteValue(Off+0x4b,&People[j].IsAttributes[13])) return(FALSE);

		if (!WriteWordValue(Off+0x26,&People[j].DaysService)) return(FALSE);
		if (!WriteWordValue(Off+0x28,&People[j].Kills)) return(FALSE);
		if (!WriteWordValue(Off+0x2a,&People[j].Missions)) return(FALSE);

		if (People[j].Orders==5) People[j].Orders=6;
		if (!WriteByteValue(Off+0x2d,&People[j].Orders)) return(FALSE);
		if (People[j].Orders==6) People[j].Orders=5;
	}
	
	for (i=0;i<33;i++)
	{
		if (!WriteDWordValue(0x691ca+i*22,&AvVehicles[i].Price)) return(FALSE);
		if (!WriteDWordValue(0x691d2+i*22,&AvVehicles[i].Price)) return(FALSE);
		if (!WriteDWordValue(0x691d6+i*22,&AvVehicles[i].Quantity)) return(FALSE);
	}

	for (i=0;i<NBases;i++)
	{
		Off=0x1df6c+0x2be*i;

		Bases[i].Items[55]=(WORD)(Bases[i].Items[55]*16);
		Bases[i].Items[57]=(WORD)(Bases[i].Items[57]*36);
		Bases[i].Items[59]=(WORD)(Bases[i].Items[59]*30);
		Bases[i].Items[61]=(WORD)(Bases[i].Items[61]*24);
		Bases[i].Items[62]=(WORD)(Bases[i].Items[62]*24);
		Bases[i].Items[63]=(WORD)(Bases[i].Items[63]*24);
		Bases[i].Items[65]=(WORD)(Bases[i].Items[65]*42);
		Bases[i].Items[71]=(WORD)(Bases[i].Items[71]*5);
		Bases[i].Items[72]=(WORD)(Bases[i].Items[72]*5);
		Bases[i].Items[73]=(WORD)(Bases[i].Items[73]*5);
		Bases[i].Items[82]=(WORD)(Bases[i].Items[82]*16);
		Bases[i].Items[83]=(WORD)(Bases[i].Items[83]*16);
		Bases[i].Items[84]=(WORD)(Bases[i].Items[84]*16);
		Bases[i].Items[106]=(WORD)(Bases[i].Items[106]*3);
		
		for (j=0;j<255;j++)
			if (!WriteWordValue(Off+0x96+j*2,&Bases[i].Items[j])) return(FALSE);

		Bases[i].Items[55]=(WORD)(Bases[i].Items[55]/16);
		Bases[i].Items[57]=(WORD)(Bases[i].Items[57]/36);
		Bases[i].Items[59]=(WORD)(Bases[i].Items[59]/30);
		Bases[i].Items[61]=(WORD)(Bases[i].Items[61]/24);
		Bases[i].Items[62]=(WORD)(Bases[i].Items[62]/24);
		Bases[i].Items[63]=(WORD)(Bases[i].Items[63]/24);
		Bases[i].Items[65]=(WORD)(Bases[i].Items[65]/42);
		Bases[i].Items[71]=(WORD)(Bases[i].Items[71]/5);
		Bases[i].Items[72]=(WORD)(Bases[i].Items[72]/5);
		Bases[i].Items[73]=(WORD)(Bases[i].Items[73]/5);
		Bases[i].Items[82]=(WORD)(Bases[i].Items[82]/16);
		Bases[i].Items[83]=(WORD)(Bases[i].Items[83]/16);
		Bases[i].Items[84]=(WORD)(Bases[i].Items[84]/16);
		Bases[i].Items[106]=(WORD)(Bases[i].Items[106]/3);

		for (j=0;j<64;j++)
		{
			if (!WriteByteValue(Off+0x14+j,&Bases[i].Facilities[j])) return(FALSE);
			if (!WriteByteValue(Off+0x54+j,&Bases[i].DaysLeft[j])) return(FALSE);
		}

	}

	for (i=0;i<17;i++)
	{
		if (!WriteWordValue(0x20c5a+i*16,&Facilities[i].CostToBuild)) return(FALSE);
		if (!WriteByteValue(0x20c60+i*16,&Facilities[i].DaysToBuild)) return(FALSE);
		if (!WriteWordValue(0x20c62+i*16,&Facilities[i].MaintenanceCost)) return(FALSE);
		if (!WriteWordValue(0x20c64+i*16,&Facilities[i].Capacity)) return(FALSE);
		if (!WriteByteValue(0x20c67+i*16,&Facilities[i].CanBeBuilt)) return(FALSE);
	}

	for (i=0;i<NVehicles;i++)
	{
		Off=Vehicles[i].SavegameOffset;
		if (!WriteWordValue(Off+0x128,&Vehicles[i].Constitution)) return(FALSE);
		if (!WriteByteValue(Off+0x126,&Vehicles[i].Owner)) return(FALSE);

		for (j=0;j<Vehicles[i].NEquipment;j++)
			if (!WriteWordValue(Off+0x1b4+Vehicles[i].EquipmentNumber[j]*2,
			    &Vehicles[i].EquipmentRounds[j])) return(FALSE);
	}

	for (i=0;i<28;i++)
	for (j=0;j<28;j++)
	for (k=0;k<2;k++)
	if (!WriteByteValue(0x224+784*k+i+28*j,&Relations[k][i][j])) return(FALSE);

	CloseHandle(SaveFile);

	DestroyWindow(Popup);

	return(TRUE);
}

// procedure used for "Open" dialog
BOOL CALLBACK OpenSaveProc(HWND hDlg,UINT uMsg,WORD wParam,LONG lParam)
{
	CHAR TempFilename[MAX_PATH];
	CHAR SaveTime[45];
	CHAR SaveDescription[45];
	OFSTRUCT OpenFileStruct;
	BOOL OK;

	switch (uMsg)
    {
	case WM_NOTIFY:
		switch (((LPNMHDR)lParam)->code)
		{
		case CDN_SELCHANGE: // user has selected different file
			OK=FALSE;       // it's not OK
			if (CommDlg_OpenSave_GetFilePath(GetParent(hDlg),
				TempFilename,MAX_PATH)!=0) // what is the file name?
			{
				SaveFile=(HANDLE)OpenFile(TempFilename,&OpenFileStruct,
					      OF_READ);
				if (SaveFile!=(HANDLE)-1) // try to open this file
				{
					if ((SaveExpectedSize==0) ||
					(GetFileSize(SaveFile,(LPDWORD)NULL)==SaveExpectedSize))
					// check if it has right size
					{
						if (ReadStringValueNoError(0x00,(LPTSTR)SaveTime,40))
						// try to read save time
						{
							if (ReadStringValueNoError(0x28,(LPTSTR)SaveDescription,
								                 40))
							// and description
							{
								// if all above succeeded, it's perfect!
								OK=TRUE;
								// update time and description
								EnableControl(hDlg,IDC_STATIC1,"Description:");
								EnableControl(hDlg,IDC_STATIC2,"Game time:");
								SetDlgItemText(hDlg,IDC_SAVEDESCRIPTION,
									           SaveDescription);
								SetDlgItemText(hDlg,IDC_SAVETIME,SaveTime);
							}
						}
					}
				}
			}
			// if not OK, then disable controls
			if (!OK)
			{
				DisableControl(hDlg,IDC_STATIC1);
				DisableControl(hDlg,IDC_STATIC2);
				SetDlgItemText(hDlg,IDC_SAVEDESCRIPTION,"");
				SetDlgItemText(hDlg,IDC_SAVETIME,"");
			}
			break;
		}

		break;

	default:
		break;
    }
    return(FALSE);
}


VOID OpenSavegame(BOOL LastSavegame)
{
	OPENFILENAME of;         // structure used for invoking "Open" dialog
	BOOL Proceed;
	CHAR *pBackslash;
	CHAR Directory[MAX_PATH];
	
	if (LastSavegame)        // are we supposed to read previous save?
	{
		Proceed=TRUE;
		lstrcpy(NewSaveName,LastSaveName);
	} else
	{
		NewSaveName[0]=0;
	
		// initialize structure
		of.lStructSize=sizeof(OPENFILENAME);
		of.hwndOwner=MainDialog;
		of.hInstance=CurInstance;
		of.lpstrFilter="Savegame Files (savegame.0?)\000SAVEGAME.0?\000All Files (*.*)\000*.*\000\000";
		of.lpstrCustomFilter=NULL;
		of.nMaxCustFilter=0;
		of.nFilterIndex=0;
		of.lpstrFile=NewSaveName;
		of.nMaxFile=MAX_PATH;
		of.lpstrFileTitle=NULL;
		of.nMaxFileTitle=0;
		// obtain directory name by cutting last save's name to last backslash
		strcpy(Directory,"");
		pBackslash=strrchr(LastSaveName,'\\');
		if (pBackslash!=NULL)
		{
			strncpy(Directory,LastSaveName,pBackslash-LastSaveName+1);
			Directory[pBackslash-LastSaveName+1]='\0';
		}
		of.lpstrInitialDir=Directory;
		of.lpstrTitle="Open savegame";
		of.Flags=OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST 
			     | OFN_ENABLETEMPLATE | OFN_ENABLEHOOK | OFN_EXPLORER;
		of.lpTemplateName=MAKEINTRESOURCE(IDD_OPENTEMPLATE);
		of.lpfnHook=(LPOFNHOOKPROC)OpenSaveProc;
		of.nFileOffset=0;
		of.nFileExtension=0;
		of.lpstrDefExt=NULL;
		of.lCustData=0;
		// OK, invoke dialog
		Proceed=(GetOpenFileName(&of));
	}
	if (Proceed) // if everything's OK, read data from file
	{
		WrongValueReminder=TRUE;

		if (ReadSavegameData())
		{
			lstrcpy(SaveName,NewSaveName);
			SaveLoaded=TRUE;
			DrawPage(TRUE);
		}
	} 
}

// Backup savegame.
VOID BackupSavegame()
{
	CHAR BackupName[MAX_PATH];

	lstrcpy(BackupName,SaveName); lstrcat(BackupName,".bak");

	if (CopyFile(SaveName,BackupName,FALSE))
	{
		MessageBox(MainDialog,
				   "The savegame was successfully backuped.",
					   "Midnight Editor",
					   MB_APPLMODAL | MB_OK | MB_ICONINFORMATION);
		BackupPresent=TRUE;
		DrawPage(TRUE); // redraw page
	} else
	{
		MessageBox(MainDialog,
				   "Midnight Editor couldn't backup the savegame!",
					   "Midnight Editor",
					   MB_APPLMODAL | MB_OK | MB_ICONEXCLAMATION);
	}
}

// Restore savegame.
VOID RestoreSavegame()
{
	CHAR BackupName[MAX_PATH];
	BOOL Alright;
	
	lstrcpy(BackupName,SaveName);
	lstrcat(BackupName,".bak");
	
	Alright=FALSE;
	if (CopyFile(BackupName,SaveName,FALSE))
	{
		WrongValueReminder=FALSE;
		if (ReadSavegameData()) Alright=TRUE;
		DrawPage(TRUE);
	}
	
	if (Alright)
		MessageBox(MainDialog,
				   "The savegame was successfully restored.",
				   "Midnight Editor",
				   MB_APPLMODAL | MB_OK | MB_ICONINFORMATION);
	else
		MessageBox(MainDialog,
				   "Midnight Editor couldn't restore the savegame!",
				   "Midnight Editor",
				   MB_APPLMODAL | MB_OK | MB_ICONEXCLAMATION);
}

// read configuration from ME.INI file
VOID ReadConfiguration()
{
	GetPrivateProfileString("Midnight Editor 06","LastSavegame","",
						    LastSaveName,MAX_PATH,"ME.INI");
	GetPrivateProfileString("Midnight Editor 06","Executable","",
		                    Executable,MAX_PATH,"ME.INI");
}
 
// write configuration to ME.INI file
VOID WriteConfiguration()
{
	if (SaveLoaded)
	WritePrivateProfileString("Midnight Editor 06","LastSavegame",
	                          SaveName,"ME.INI");
	if (strlen(Executable)!=0)
	WritePrivateProfileString("Midnight Editor 06","Executable",
	                          Executable,"ME.INI");
}

//------------------------------------------------------- Child dialog procs -

// Reads dialog from resource.
DLGTEMPLATE * WINAPI DoLockDlgRes(LPCSTR lpszResName) 
{ 
    HRSRC hrsrc=FindResource(NULL, lpszResName, RT_DIALOG); 
    HGLOBAL hglb=LoadResource(CurInstance, hrsrc); 

    return (DLGTEMPLATE *) LockResource(hglb); 
} 

// Positions the tab window after initialization.
VOID WINAPI OnChildDialogInit() 
{ 
    HWND hwndParent=GetParent(TabWindow); 
    DLGHDR *pHdr=(DLGHDR *) GetWindowLong( 
        hwndParent, GWL_USERDATA); 

    SetWindowPos(TabWindow, HWND_TOP, 
        pHdr->rcDisplay.left, pHdr->rcDisplay.top, 
        0, 0, SWP_NOSIZE); 
} 


// Handles all events coming to tab child window.
BOOL CALLBACK TabWindowProc(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	INT i,j,k;               // temporary variables
	CHAR TempChar[40];

 	switch (uMsg)
	{
	case WM_INITDIALOG:      // At window initialization...
		TabWindow=hDlg;      // update pointer
		OnChildDialogInit(); 
		DrawPage(FALSE);     // draw the page
		switch (CurPage)
		{
			case 5: UpdateFacilityChanged(TRUE); break;
			case 6: UpdateCategoryChanged(TRUE); break;
			case 7:
			case 8: UpdatePersonChanged(TRUE); break;
			case 9: UpdateAvVehicleChanged(TRUE); break;
			case 10: UpdateVehicleChanged(TRUE); break;
			case 11: UpdateProjectChanged(TRUE); break;
			case 12: UpdateOrganizationChanged(TRUE); break;
		}
		break;

    case WM_MEASUREITEM:     // returns item's width/height for self-drawn
		                     // list boxes and combos
	{
        LPMEASUREITEMSTRUCT lpmis=(LPMEASUREITEMSTRUCT)lParam; 
		switch (lpmis->CtlID)
		{
		case IDC_PHOTOLIST:
			lpmis->itemWidth=72;
			lpmis->itemHeight=100;
			break;
		case IDC_1STORGANIZATIONCOMBO:
		case IDC_2NDORGANIZATIONCOMBO:
		case IDC_OWNERCOMBO:
		case IDC_VEHICLELIST:
			lpmis->itemHeight=22+4;
			break;
		}
	}
        break; 

	case WM_DRAWITEM:        // draws item for self-drawn list boxes/combos
	{
		HDC hdc;
		RECT Temp;
		INT x,y;
		COLORREF clrBackground;
		COLORREF clrForeground;
		BOOL Disabled;
		TEXTMETRIC tm;
		HBITMAP hbmIcon;
		CHAR String1[5],String2[5];
		LPDRAWITEMSTRUCT lpdis=(LPDRAWITEMSTRUCT)lParam;

		switch (lpdis->CtlID)
		{
		case IDC_PHOTOLIST:   // photo list
			hbmIcon=BPhotos[lpdis->itemData]; // retrieve bitmap number
		
			// set text color depending on state of list box
			clrForeground=SetTextColor(lpdis->hDC, 
				    GetSysColor(lpdis->itemState & ODS_SELECTED ? 
					COLOR_HIGHLIGHTTEXT : COLOR_WINDOWTEXT)); 
 
			// is list box disabled?
			Disabled=lpdis->itemState & ODS_DISABLED;

			// set background color depending on list box state
			if (!Disabled)
			clrBackground=SetBkColor(lpdis->hDC, 
                GetSysColor(lpdis->itemState & ODS_SELECTED ? 
                COLOR_HIGHLIGHT : COLOR_WINDOW)); 
			else 
			clrBackground=SetBkColor(lpdis->hDC,GetSysColor(COLOR_3DFACE));

			// determine x and y position
			x=LOWORD(GetDialogBaseUnits())/4; 
			y=(lpdis->rcItem.bottom+lpdis->rcItem.top-64)/2; 
 
			// set appropriate text
			if (!Disabled)
			{
				_ltoa(lpdis->itemData+1,String1,10);
				if (lstrlen(String1)==1) lstrcpy(String2,"#0"); 
			                    else lstrcpy(String2,"#");
				lstrcat(String2,String1);

				SelectObject(lpdis->hDC,hSmallFont);
				SetTextAlign(lpdis->hDC,TA_TOP | TA_CENTER);
			} else lstrcpy(String2,"");

			// draw this text
			ExtTextOut(lpdis->hDC, lpdis->rcItem.left+35,lpdis->rcItem.top+68,
				ETO_CLIPPED | ETO_OPAQUE, &lpdis->rcItem, 
				String2,lstrlen(String2),NULL);

			SetTextColor(lpdis->hDC, clrForeground); 
			SetBkColor(lpdis->hDC, clrBackground); 
		
			hdc=CreateCompatibleDC(lpdis->hDC); 
			if (hdc==NULL) break; 
 
			// draw icon
			if (!Disabled)
			{
				SelectObject(hdc, hbmIcon); 
				BitBlt(lpdis->hDC,lpdis->rcItem.left+4,lpdis->rcItem.top+4,
					   64,64,hdc,0,0,SRCCOPY); 
			}
			DeleteDC(hdc); 

			GetClientRect(GetDlgItem(hDlg,IDC_PHOTOLIST),&Temp);

			lpdis->rcItem.bottom=Temp.bottom;
	
			// draw focus rectangle
			if (lpdis->itemState & ODS_FOCUS) 
				DrawFocusRect(lpdis->hDC,&lpdis->rcItem); 
			break;
		case IDC_1STORGANIZATIONCOMBO: // pretty much the same as above
		case IDC_2NDORGANIZATIONCOMBO:
		case IDC_OWNERCOMBO:
		case IDC_VEHICLELIST:
			if (lpdis->CtlID==IDC_VEHICLELIST)
			hbmIcon=BVehicles[lpdis->itemData]; else
			hbmIcon=BOrgs[lpdis->itemData];

			clrForeground=SetTextColor(lpdis->hDC,
				          GetSysColor(lpdis->itemState & ODS_SELECTED ?
						  COLOR_HIGHLIGHTTEXT : COLOR_WINDOWTEXT));

			Disabled=lpdis->itemState & ODS_DISABLED;

			clrBackground=SetBkColor(lpdis->hDC,GetSysColor(
						  lpdis->itemState & ODS_SELECTED ? COLOR_HIGHLIGHT :
						  COLOR_WINDOW));

			GetTextMetrics(lpdis->hDC,&tm);
			y=(lpdis->rcItem.bottom+lpdis->rcItem.top-tm.tmHeight)/2;
			x=LOWORD(GetDialogBaseUnits())/4;

			if (lpdis->CtlID==IDC_VEHICLELIST)
			SendMessage(lpdis->hwndItem,LB_GETTEXT,lpdis->itemID,
				        (LPARAM)(LPCSTR)TempChar); else
			SendMessage(lpdis->hwndItem,CB_GETLBTEXT,lpdis->itemID,
				        (LPARAM)(LPCSTR)TempChar);

			if (!Disabled)
			{
				ExtTextOut(lpdis->hDC,32+2*(x+4),y,ETO_CLIPPED | ETO_OPAQUE,
					       &lpdis->rcItem,TempChar,lstrlen(TempChar),NULL);
			}

			SetTextColor(lpdis->hDC,clrForeground);
			SetBkColor(lpdis->hDC,clrBackground);

			hdc=CreateCompatibleDC(lpdis->hDC);
			if (hdc==NULL) break;

			if (!Disabled)
			{
				SelectObject(hdc,hbmIcon);
				if (lpdis->CtlID==IDC_VEHICLELIST)
				BitBlt(lpdis->hDC,x+3,lpdis->rcItem.top+1,32,24,hdc,0,0,
				       SRCCOPY); else
				BitBlt(lpdis->hDC,x+5,lpdis->rcItem.top+2,30,22,hdc,0,0,
				       SRCCOPY);
			}
			DeleteDC(hdc);

			if (lpdis->itemState & ODS_FOCUS) 
				DrawFocusRect(lpdis->hDC,&lpdis->rcItem);

			break;
		}
	}
		break;		

	case WM_HSCROLL: // react when user moved the scroll bar
		switch (GetDlgCtrlID((HWND)lParam))
		{
		case IDC_PROGRESSSLIDER:
			_ltoa(SendDlgItemMessage(hDlg,IDC_PROGRESSSLIDER,TBM_GETPOS,
				                     0,0),TempChar,10);
			lstrcat(TempChar," %");
			SetDlgItemText(hDlg,IDC_PROGRESSPERC,TempChar);
			break;
		case IDC_ISSLIDER:
			i=SendDlgItemMessage(hDlg,IDC_ISSLIDER,TBM_GETPOS,0,0);
			if (CurAttribute==6) i=i*10;
			_ltoa(i,TempChar,10);
			SetDlgItemText(hDlg,IDC_ISVALUE,TempChar);
			break;
		case IDC_WASSLIDER:
			i=SendDlgItemMessage(hDlg,IDC_WASSLIDER,TBM_GETPOS,0,0);
			if (CurAttribute==6) i=i*10;
			_ltoa(i,TempChar,10);
			SetDlgItemText(hDlg,IDC_WASVALUE,TempChar);
			break;
		case IDC_RELATION11:
			i=SendDlgItemMessage(hDlg,IDC_RELATION11,TBM_GETPOS,0,0);
			SetDlgItemInt(hDlg,IDC_VALUE11,i,TRUE);
			break;
		case IDC_RELATION12:
			i=SendDlgItemMessage(hDlg,IDC_RELATION12,TBM_GETPOS,0,0);
			SetDlgItemInt(hDlg,IDC_VALUE12,i,TRUE);
			break;
		case IDC_RELATION21:
			i=SendDlgItemMessage(hDlg,IDC_RELATION21,TBM_GETPOS,0,0);
			SetDlgItemInt(hDlg,IDC_VALUE21,i,TRUE);
			break;
		case IDC_RELATION22:
			i=SendDlgItemMessage(hDlg,IDC_RELATION22,TBM_GETPOS,0,0);
			SetDlgItemInt(hDlg,IDC_VALUE22,i,TRUE);
			break;
		}
		break;

	case WM_NOTIFY:
		switch (((NMHDR FAR *)lParam)->code)
		{
		case UDN_DELTAPOS: // react when user pressed up-down button
			LONG Value;
			if (!(GetWindowLong(GetDlgItem(hDlg,(int)wParam-1000),GWL_STYLE) 
				& ES_READONLY))
			{
				MINMAX *pMinMax=(MINMAX *)
					GetWindowLong(GetDlgItem(hDlg,(int)wParam-1000),
					              GWL_USERDATA);

				Value=GetDlgItemInt(hDlg,(int)wParam-1000,NULL,TRUE);
				Value+=((NM_UPDOWN FAR *)lParam)->iDelta;
				// check if value doesn't exceed boundaries
				if (Value<pMinMax->Min) Value=pMinMax->Min; else
				if (Value>pMinMax->Max) Value=pMinMax->Max; 
				SetDlgItemInt(hDlg,(int)wParam-1000,Value,TRUE);
			}
			break;
		}
		break;
		
	case WM_COMMAND:
		if (HIWORD(wParam)==CBN_SELCHANGE) // react when user changed combo
			                               // selection
		{
			switch (CurPage)
			{
			case 6:
				if ((int)LOWORD(wParam)==IDC_BASECOMBO)
				UpdateItemOrBaseChanged(FALSE);
				if ((int)LOWORD(wParam)==IDC_CATEGORYCOMBO)
				UpdateCategoryChanged(FALSE);
				break;
			case 7:
				UpdatePersonChanged(FALSE);
				break;
			case 8:
				if ((INT)LOWORD(wParam)==IDC_PERSONCOMBO) 
					UpdatePersonChanged(FALSE); else
				if ((INT)LOWORD(wParam)==IDC_ATTRIBUTECOMBO) 
					UpdateAttributeChanged(FALSE);
				break;
			case 12:
				UpdateOrganizationChanged(FALSE);
				break;
			}
		};
		if (HIWORD(wParam)==LBN_SELCHANGE) // react when user changed list
			                               // box selection
		{
			switch (CurPage)
			{
			case 5:
				if ((int)LOWORD(wParam)==IDC_FACILITYLIST)
					UpdateFacilityChanged(FALSE);
				break;
			case 9:
				UpdateAvVehicleChanged(FALSE);
				break;
			case 6:
				if ((int)LOWORD(wParam)==IDC_ITEMLIST)
					UpdateItemOrBaseChanged(FALSE);
				break;
			case 10:
				if ((int)LOWORD(wParam)==IDC_VEHICLELIST)
					UpdateVehicleChanged(FALSE);
				if ((int)LOWORD(wParam)==IDC_EQUIPMENTLIST)
					UpdateEquipmentChanged(FALSE);
				break;
			case 11:
				UpdateProjectChanged(FALSE);
				break;
			}
		};
		if (HIWORD(wParam)==EN_UPDATE) 
			// react when user modified text in edit control (either person
			// or vehicle name)
		{
			if (CurPage==7)
			{
				GetDlgItemText(hDlg,IDC_NAMEEDIT,
					(LPTSTR)People[CurPerson].Name,25);
				if (lstrlen(People[CurPerson].Name)==0) 
					lstrcpy(People[CurPerson].Name,"No Name");
				SendDlgItemMessage(hDlg,IDC_PERSONCOMBO,CB_DELETESTRING,
					               (WPARAM)CurPerson,0);
				SendDlgItemMessage(hDlg,IDC_PERSONCOMBO,CB_INSERTSTRING,
					               (WPARAM)CurPerson,
								   (LPARAM)(LPTSTR)People[CurPerson].Name);
				SendDlgItemMessage(hDlg,IDC_PERSONCOMBO,CB_SETCURSEL,
					               (WPARAM)CurPerson,0);
			}
			if ((int)LOWORD(wParam)==IDC_CARNAMEEDIT)
			{
				GetDlgItemText(hDlg,IDC_CARNAMEEDIT,
					           (LPTSTR)Vehicles[CurVehicle].Name,25);
				if (lstrlen(Vehicles[CurVehicle].Name)==0) 
					lstrcpy(Vehicles[CurVehicle].Name,"No Name");
				SendDlgItemMessage(hDlg,IDC_VEHICLELIST,LB_DELETESTRING,
					               (WPARAM)CurVehicle,0);
				SendDlgItemMessage(hDlg,IDC_VEHICLELIST,LB_INSERTSTRING,
					               (WPARAM)CurVehicle,
								   (LPARAM)(LPTSTR)Vehicles[CurVehicle].Name);
				SendDlgItemMessage(hDlg,IDC_VEHICLELIST,LB_SETITEMDATA,
					               (WPARAM)CurVehicle,
								   Vehicles[CurVehicle].Type);
				SendDlgItemMessage(hDlg,IDC_VEHICLELIST,LB_SETCURSEL,
					               (WPARAM)CurVehicle,0);
			}
		};
		// handle messages from buttons
		switch (GET_WM_COMMAND_ID(wParam, lParam))
		{
		case IDC_OPENBUTTON: // open savegame
			OpenSavegame(FALSE);
			break;
		case IDC_LASTSAVEBUTTON: // open last savegame
			OpenSavegame(TRUE);
			SendMessage(hDlg,WM_NEXTDLGCTL,0,(LPARAM)FALSE);
			break;
		case IDC_BACKUPBAKBUTTON: // backup savegame
			BackupSavegame();
			break;
		case IDC_RESTOREBAKBUTTON: // restore savegame
			RestoreSavegame();
			break;
		case IDC_MAIL: // send mail
			ShellExecute(NULL,"open","mailto:mwichary@polbox.com","","",
				         SW_SHOW);
			break;
		case IDC_WWW: // open WWW page
			ShellExecute(NULL,"open","http://midnight.tme.szczecin.pl",
				         "","",SW_MAXIMIZE);
			break;
		case IDC_UPDATESAVETIME: // update savegame time
			{
				CHAR Temp[40];

				KillPage();
				strcpy(SaveTime,"");
				strcat(SaveTime,TWeekDays[WeekDay]);
				strcat(SaveTime,", ");
				strcat(SaveTime,TDays[Day]);
				strcat(SaveTime," ");
				strcat(SaveTime,TMonths[Month]);
				strcat(SaveTime,", ");
				itoa(Year,Temp,10); strcat(SaveTime,Temp);
				strcat(SaveTime," ");
				itoa(Hour,Temp,10); strcat(SaveTime,Temp);
				strcat(SaveTime,":");
				if (Minute<10) strcat(SaveTime,"0");
				itoa(Minute,Temp,10); strcat(SaveTime,Temp);
				strcat(SaveTime,":");
				if (Second<10) strcat(SaveTime,"0");
				itoa(Second,Temp,10); strcat(SaveTime,Temp);
				if (strlen(SaveTime)>39) SaveTime[39]='\0';
			}
			break;
		case IDC_COPYPROJECT: // copy project data to others
			KillPage();
			for (i=0;i<99;i++) Projects[i]=Projects[CurProject];
			break;
		case IDC_COPYAVVEHICLES: // copy stock vehicle data to others
			KillPage();
			for (i=0;i<33;i++) AvVehicles[i]=AvVehicles[CurAvVehicle];
			break;
		case IDC_COPYFACILITY: // copy facility data to others
			KillPage();
			for (i=0;i<17;i++) Facilities[i]=Facilities[CurFacility];
			break;
		case IDC_COPYCATEGORY: // copy to all items in this category
			KillPage();
			k=GetDlgItemInt(hDlg,IDC_QUANTITY,NULL,FALSE);
			if (CurCategory==0) for (i=0;i<86;i++) 
				Bases[CurBase].Items[(Items1Off[i]-0x1e002)/2]=k;
			else
			if (CurCategory==1) for (i=0;i<49;i++) 
				Bases[CurBase].Items[(Items2Off[i]-0x1e002)/2]=k;
			else
			if (CurCategory==2) for (i=0;i<28;i++) 
				Bases[CurBase].Items[(Items3Off[i]-0x1e002)/2]=k;
			else
			if (CurCategory==3) for (i=0;i<30;i++) 
				Bases[CurBase].Items[(Items4Off[i]-0x1e002)/2]=k;
			break;
		case IDC_COPYTHISBASE: // copy to all items in this base
			KillPage();

			k=GetDlgItemInt(hDlg,IDC_QUANTITY,NULL,FALSE);
			for (i=0;i<255;i++)
				Bases[CurBase].Items[i]=k;
			break;
		case IDC_COPYALLBASES: // copy to all items in all bases
			KillPage();
			k=GetDlgItemInt(hDlg,IDC_QUANTITY,NULL,FALSE);
			for (i=0;i<255;i++)
			for (j=0;j<NBases;j++)
				Bases[j].Items[i]=k;
			break;
		case IDC_COPYPERSON: // copy this person's data to others
			KillPage();
			for (i=0;i<NPeople;i++) 
			{
				People[i].Rank=People[CurPerson].Rank;
				People[i].DaysService=People[CurPerson].DaysService;
				People[i].Kills=People[CurPerson].Kills;
				People[i].Missions=People[CurPerson].Missions;
				People[i].Orders=People[CurPerson].Orders;
				for (j=0;j<14;j++)
				{
					People[i].WasAttributes[j]=
						People[CurPerson].WasAttributes[j];
					People[i].IsAttributes[j]=
						People[CurPerson].IsAttributes[j];
				}
			}
			break;
		case IDC_FINISH: // finish all facilities in this base
			i=SendDlgItemMessage(hDlg,IDC_BASECOMBO,CB_GETCURSEL,0,0);
			for (j=0;j<64;j++)
				Bases[i].DaysLeft[j]=0;
			break;
		case IDC_DIG: // dig more corridors
			i=SendDlgItemMessage(hDlg,IDC_BASECOMBO,CB_GETCURSEL,0,0);
			for (j=0;j<=7;j++)
			for (k=0;k<=7;k++)
				if (Bases[i].Facilities[8*k+j]<=15)
				Bases[i].Facilities[8*k+j]=1*(k!=0)+4*(k!=7)+8*(j!=0)+2*(j!=7);
			break;
		case IDC_MAX: // maximize buttons
			if (CurPage==4) // cash
				SetDlgItemInt(hDlg,IDC_CASHEDIT,999999999,FALSE); else
			if (CurPage==8) // person's characteristics
			{
				i=SendDlgItemMessage(hDlg,IDC_PERSONCOMBO,CB_GETCURSEL,0,0);
				for (j=0;j<14;j++)
				{
					People[i].IsAttributes[j]=(BYTE)Margins[j][1];
					if (WasIncluded[j]) People[i].WasAttributes[j]=
						                (BYTE)Margins[j][1];
				};
				UpdatePersonChanged(TRUE);
			} else
			if (CurPage==10) // constitution
				SetDlgItemInt(hDlg,IDC_CONSTITUTIONEDIT,
				              MaxConstitution[Vehicles[CurVehicle].Type],
							  FALSE);
			break;
		case IDC_RESTOREFACILITY: // restore facility data
			i=SendDlgItemMessage(hDlg,IDC_FACILITYLIST,LB_GETCURSEL,0,0);
			SetDlgItemInt(hDlg,IDC_COSTTOBUILDEDIT,FacilityDefs[i][0],FALSE);
			SetDlgItemInt(hDlg,IDC_DAYSTOBUILDEDIT,FacilityDefs[i][1],FALSE);
			SetDlgItemInt(hDlg,IDC_MAINTENANCECOSTEDIT,
				          FacilityDefs[i][2],FALSE);
			SetDlgItemInt(hDlg,IDC_CAPACITYEDIT,FacilityDefs[i][3],FALSE);
			break;

		case IDC_SET1: // set relation to either hostile,
		case IDC_SET2: // neutral,
		case IDC_SET3: // or friendly
			switch (GET_WM_COMMAND_ID(wParam, lParam))
			{
				case IDC_SET1: i=-100; break;
				case IDC_SET2: i=0; break;
				case IDC_SET3: i=100; break;
			}
			SendDlgItemMessage(hDlg,IDC_RELATION11,TBM_SETPOS,
							   (WPARAM)TRUE,(LPARAM)(CHAR)i);
			SendDlgItemMessage(hDlg,IDC_RELATION12,TBM_SETPOS,
				               (WPARAM)TRUE,(LPARAM)(CHAR)i);
			SendDlgItemMessage(hDlg,IDC_RELATION21,TBM_SETPOS,
				               (WPARAM)TRUE,(LPARAM)(CHAR)i);
			SendDlgItemMessage(hDlg,IDC_RELATION22,TBM_SETPOS,
				               (WPARAM)TRUE,(LPARAM)(CHAR)i);
			SetDlgItemInt(hDlg,IDC_VALUE11,i,TRUE);
			SetDlgItemInt(hDlg,IDC_VALUE12,i,TRUE);
			SetDlgItemInt(hDlg,IDC_VALUE21,i,TRUE);
			SetDlgItemInt(hDlg,IDC_VALUE22,i,TRUE);
			break;
		case IDC_COPYFIRST: // copy first->second to first->all
			KillPage();
			for (i=0;i<28;i++)
			for (j=0;j<2;j++)
			if (i!=Cur1stOrganization)
			{
				Relations[j][Cur1stOrganization][i]=
					Relations[j][Cur1stOrganization][Cur2ndOrganization];
				Relations[j][i][Cur1stOrganization]=
					Relations[j][Cur2ndOrganization][Cur1stOrganization];
			}
			break;
		case IDC_COPYSECOND: // copy second->first to second->all
			KillPage();
			for (i=0;i<28;i++)
			for (j=0;j<2;j++)
			if (i!=Cur2ndOrganization)
			{
				Relations[j][i][Cur2ndOrganization]=
					Relations[j][Cur1stOrganization][Cur2ndOrganization];
				Relations[j][Cur2ndOrganization][i]=
					Relations[j][Cur2ndOrganization][Cur1stOrganization];
			}
			break;
		}
		break;

	default:
		return(FALSE);
	}

	return(TRUE);
}

// if new tab is selected...
VOID WINAPI OnSelChanged(HWND hDlg) 
{ 
    DLGHDR *pHdr=(DLGHDR *)GetWindowLong(hDlg,GWL_USERDATA); 
 
    // destroy the current child dialog box, if any
    if (pHdr->hwndDisplay!=NULL) 
	{
		KillPage();
		DestroyWindow(pHdr->hwndDisplay); 
	}

	// update current page number	
    CurPage=TabCtrl_GetCurSel(pHdr->hwndTab)+1; 
 
    // create the new child dialog box
    pHdr->hwndDisplay=CreateDialogIndirect(CurInstance,
				      pHdr->apRes[CurPage-1],hDlg,TabWindowProc);
}
  
// if tab is initialized...
VOID WINAPI OnTabbedDialogInit(HWND hDlg) 
{ 
    DLGHDR *pHdr=(DLGHDR *)LocalAlloc(LPTR,sizeof(DLGHDR)); 
    TC_ITEM tie;
    RECT Rect1,Rect2; 
	
	// save a pointer to the DLGHDR structure
    SetWindowLong(hDlg,GWL_USERDATA,(LONG)pHdr); 
 
    // create the tab control
	pHdr->hwndTab=GetDlgItem(hDlg,IDC_MAINTAB);
 
    // add a tab for each child window
    tie.mask=TCIF_TEXT; 
    tie.pszText="About"; TabCtrl_InsertItem(pHdr->hwndTab,0,&tie); 
    tie.pszText="Information"; TabCtrl_InsertItem(pHdr->hwndTab,1,&tie); 
    tie.pszText="Savegame"; TabCtrl_InsertItem(pHdr->hwndTab,2,&tie); 
    tie.pszText="General"; TabCtrl_InsertItem(pHdr->hwndTab,3,&tie); 
    tie.pszText="Facilities"; TabCtrl_InsertItem(pHdr->hwndTab,4,&tie); 
    tie.pszText="Stores"; TabCtrl_InsertItem(pHdr->hwndTab,5,&tie); 
    tie.pszText="People (1)"; TabCtrl_InsertItem(pHdr->hwndTab,6,&tie); 
    tie.pszText="People (2)"; TabCtrl_InsertItem(pHdr->hwndTab,7,&tie); 
    tie.pszText="Stock vehicles"; TabCtrl_InsertItem(pHdr->hwndTab,8,&tie); 
    tie.pszText="Vehicles"; TabCtrl_InsertItem(pHdr->hwndTab,9,&tie); 
    tie.pszText="Research"; TabCtrl_InsertItem(pHdr->hwndTab,10,&tie); 
    tie.pszText="Organizations"; TabCtrl_InsertItem(pHdr->hwndTab,11,&tie); 
    tie.pszText="Score"; TabCtrl_InsertItem(pHdr->hwndTab,12,&tie); 
 
    // lock the resources for child dialog boxes
    pHdr->apRes[0]=DoLockDlgRes(MAKEINTRESOURCE(IDD_ABOUT)); 
    pHdr->apRes[1]=DoLockDlgRes(MAKEINTRESOURCE(IDD_INFORMATION)); 
    pHdr->apRes[2]=DoLockDlgRes(MAKEINTRESOURCE(IDD_SAVEGAME)); 
    pHdr->apRes[3]=DoLockDlgRes(MAKEINTRESOURCE(IDD_GENERAL)); 
    pHdr->apRes[4]=DoLockDlgRes(MAKEINTRESOURCE(IDD_FACILITIES)); 
    pHdr->apRes[5]=DoLockDlgRes(MAKEINTRESOURCE(IDD_STORES)); 
    pHdr->apRes[6]=DoLockDlgRes(MAKEINTRESOURCE(IDD_PEOPLE1)); 
    pHdr->apRes[7]=DoLockDlgRes(MAKEINTRESOURCE(IDD_PEOPLE2)); 
    pHdr->apRes[8]=DoLockDlgRes(MAKEINTRESOURCE(IDD_AVVEHICLES)); 
    pHdr->apRes[9]=DoLockDlgRes(MAKEINTRESOURCE(IDD_VEHICLES)); 
    pHdr->apRes[10]=DoLockDlgRes(MAKEINTRESOURCE(IDD_RESEARCH)); 
    pHdr->apRes[11]=DoLockDlgRes(MAKEINTRESOURCE(IDD_ORGANIZATIONS)); 
    pHdr->apRes[12]=DoLockDlgRes(MAKEINTRESOURCE(IDD_SCORE)); 

	// calculate the display rectangle
	GetWindowRect(pHdr->hwndTab,&Rect1);
	CopyRect(&Rect2,&Rect1);
	TabCtrl_AdjustRect(pHdr->hwndTab,FALSE,&Rect1);
	pHdr->rcDisplay.left=Rect1.left-Rect2.left+6;	
	pHdr->rcDisplay.top=Rect1.top-Rect2.top+9;

    // simulate selection of the first item
    OnSelChanged(hDlg);
} 

//------------------------------------------------------- Game running procs -

BOOL CALLBACK RunGameProc(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	CHAR *pSearchStr;
	CHAR TempString[MAX_PATH];
	HINSTANCE EXEInstance;
	CHAR NewExecutable[MAX_PATH];
	OPENFILENAME of;

	switch (uMsg)
	{
	case WM_INITDIALOG:
		SendDlgItemMessage(hDlg,IDC_EXENAME,EM_SETLIMITTEXT,
			               (WPARAM)MAX_PATH,0);
		if (strlen(Executable)==0)
		{
			// we determine the executable's name (if it is not present)
			// from current or last save game name
			strcpy(NewExecutable,"c:\\xcoma\\xcomapoc.pif");
			if ((strlen(LastSaveName)!=0) || (strlen(SaveName)!=0))
			{
				if (strlen(SaveName)==0) strcpy(TempString,LastSaveName);
					                    else strcpy(TempString,SaveName);
				_strupr(TempString);
				pSearchStr=strstr(TempString,"\\SAVEGAME\\");
				if (pSearchStr!=NULL)
				{
					strncpy(NewExecutable,TempString,pSearchStr-TempString);
					NewExecutable[pSearchStr-TempString]='\0';
					strcat(NewExecutable,"\\xcomapoc.pif");
					_strlwr(NewExecutable);
				}

			}
		} else strcpy(NewExecutable,Executable);
		SetDlgItemText(hDlg,IDC_EXENAME,NewExecutable);
		break;

	case WM_COMMAND:
		if (HIWORD(wParam)==EN_CHANGE)
		{
			GetDlgItemText(hDlg,IDC_EXENAME,TempString,MAX_PATH);
			// if executable name edit control is empty, we disable
			// "OK" button and make "Cancel" button default.
			if (strlen(TempString)==0) 
			{
				SetWindowLong(GetDlgItem(hDlg,IDCANCEL),GWL_STYLE,
				GetWindowLong(GetDlgItem(hDlg,IDCANCEL),GWL_STYLE) 
				              | BS_DEFPUSHBUTTON);
				SetWindowLong(GetDlgItem(hDlg,IDOK),GWL_STYLE,
				GetWindowLong(GetDlgItem(hDlg,IDOK),GWL_STYLE) 
				              & ~BS_DEFPUSHBUTTON);
				DisableControl(hDlg,IDOK,"OK");
				EnableControl(hDlg,IDCANCEL,"Cancel");
			}
			else
			{
				SetWindowLong(GetDlgItem(hDlg,IDCANCEL),GWL_STYLE,
				GetWindowLong(GetDlgItem(hDlg,IDCANCEL),GWL_STYLE) 
				              & ~BS_DEFPUSHBUTTON);
				SetWindowLong(GetDlgItem(hDlg,IDOK),GWL_STYLE,
				GetWindowLong(GetDlgItem(hDlg,IDOK),GWL_STYLE) 
				              | BS_DEFPUSHBUTTON);
				EnableControl(hDlg,IDOK,"OK");
				EnableControl(hDlg,IDCANCEL,"Cancel");
			}
		} else
		switch (GET_WM_COMMAND_ID(wParam,lParam))
		{
		case IDC_BROWSE: // display "Browse" dialog
			of.lStructSize=sizeof(OPENFILENAME);
			of.hwndOwner=hDlg;
			of.hInstance=CurInstance;
			of.lpstrFilter="Programs (*.bat;*.com;*.exe;*.pif)\000*.BAT;*.COM;*.EXE;*.PIF\000All Files (*.*)\000*.*\000\000";
			of.lpstrCustomFilter=NULL;
			of.nMaxCustFilter=0;
			of.nFilterIndex=0;
			of.lpstrFile=TempString;
			of.nMaxFile=MAX_PATH;
			of.lpstrFileTitle=NULL;
			of.nMaxFileTitle=0;
			of.lpstrInitialDir="";
			of.lpstrTitle="Browse";
			of.Flags=OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST;
			of.lpTemplateName=NULL;
			of.lpfnHook=NULL;
			of.nFileOffset=0;
			of.nFileExtension=0;
			of.lpstrDefExt=NULL;
			of.lCustData=0;
			if (GetOpenFileName(&of)) 
				SetDlgItemText(hDlg,IDC_EXENAME,TempString);
			break;

		case IDOK:
			GetDlgItemText(hDlg,IDC_EXENAME,NewExecutable,MAX_PATH);
			strcpy(TempString,NewExecutable);
			pSearchStr=strrchr(TempString,'\\');
			if (pSearchStr!=NULL) TempString[pSearchStr-TempString+1]='\0'; 
			else strcpy(TempString,"");
			EXEInstance=ShellExecute(NULL,"open",NewExecutable,"",
				                     TempString,SW_SHOWDEFAULT);
			if ((INT)EXEInstance<=32)
			{
				switch ((INT)EXEInstance)
				{
				case ERROR_FILE_NOT_FOUND:
					strcpy(TempString,"Cannot find the specified file!");
					break;
				case ERROR_PATH_NOT_FOUND:
					strcpy(TempString,"Cannot find the specified directory!");
					break;
				default:
					strcpy(TempString,"Cannot load the specified file!");
					break;
				};
				MessageBox(hDlg,TempString,"Midnight Editor",
					       MB_APPLMODAL | MB_OK | MB_ICONEXCLAMATION); 
			} else
			{ 
				strcpy(Executable,NewExecutable);
				EndDialog(hDlg,TRUE); 
			}
			break;
		case IDCANCEL:
			EndDialog(hDlg,FALSE); 
			break;
		}
		break;
	default:
		return(FALSE);
	}

	return(TRUE);
}

//-------------------------------------------------------- Main dialog procs -

BOOL CALLBACK MainDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	INT i;

	switch (uMsg)
	{
	case WM_INITDIALOG: 
		// read configuration from ME.INI file
		ReadConfiguration();

		// initialize fonts
		hTitleFont=CreateFont(25,0,0,0,FW_BOLD,0,0,0,0,0,0,0,0,"Arial");
		hSmallFont=CreateFont(10,0,0,0,FW_NORMAL,0,0,0,0,0,0,0,0,"Small Fonts");
		
		MainDialog=hDlg;
		
		// set window's caption
		SetWindowText(hDlg,Title);

		// load bitmaps
		for (i=0;i<75;i++) 
			BPhotos[i]=LoadBitmap(CurInstance,MAKEINTRESOURCE(IDB_BITMAP1+i));
		for (i=0;i<28;i++) 
			BOrgs[i]=LoadBitmap(CurInstance,MAKEINTRESOURCE(IDB_ORG0+i));
		for (i=0;i<33;i++) 
			BVehicles[i]=LoadBitmap(CurInstance,MAKEINTRESOURCE(IDB_VEHICLE0+i));

		// set icon
		SendMessage(hDlg,WM_SETICON,(WPARAM)TRUE,
			        (LPARAM)LoadIcon(CurInstance,MAKEINTRESOURCE(IDI_ICON1)));

		OnTabbedDialogInit(hDlg);

		break;

	case WM_DESTROY:
		// delete all fonts and bitmaps
		DeleteObject(hTitleFont);
		DeleteObject(hSmallFont);
		for (i=0;i<75;i++) DeleteObject(BPhotos[i]);
		for (i=0;i<28;i++) DeleteObject(BOrgs[i]);
		for (i=0;i<33;i++) DeleteObject(BVehicles[i]);
		// write configuration
		WriteConfiguration();
		break;

	case WM_NOTIFY:
		switch (((NMHDR FAR *)lParam)->code)
		{
		case TCN_SELCHANGE: // user has selected new tab
			OnSelChanged(hDlg);
			break;
		}
		break;

	case WM_COMMAND:
		switch (GET_WM_COMMAND_ID(wParam, lParam))
		{
		case IDC_REVERTBUTTON: // revert to saved button
		{
			DLGHDR *pHdr=(DLGHDR *)GetWindowLong(hDlg,GWL_USERDATA);

			WrongValueReminder=FALSE;
			lstrcpy(NewSaveName,SaveName);
			ReadSavegameData();
	
			DestroyWindow(pHdr->hwndDisplay); 
			pHdr->hwndDisplay=NULL;
			OnSelChanged(MainDialog);
		}
			break;
		case IDC_SAVEBUTTON: // save button
			KillPage();
			WriteSavegameData();
			break;
		case IDC_RUNGAME: // run the game button
			DialogBox(CurInstance,MAKEINTRESOURCE(IDD_RUNTHEGAME),
				      MainDialog,(DLGPROC)RunGameProc);
			break;
		case IDCANCEL: // close editor
			EndDialog(hDlg,TRUE);
			break;
		}
		break;

	default:
		return(FALSE);
	}

	return(TRUE);
}

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,
				   LPSTR lpCmdLine,INT nCmdShow)
{
	CurInstance=hInstance;
	InitCommonControls(); // needed for up-down controls, tab windows and
	                      // trackbars

	DialogBox(hInstance,MAKEINTRESOURCE(IDD_MAINDIALOG),NULL,MainDlgProc);
	return(FALSE);
}
