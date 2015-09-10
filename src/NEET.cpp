#include <Windows.h>
#include <iostream>
#include <fstream>
#include <thread>
#include "hLib.h"
#include "Secure.h"

using namespace std;

/*
 *	@variable		Instiantiates 'Secure' object, pass integer parameter for scanning delay (seconds).
 */
	static Secure *PROTECT = new Secure(35);
/*
 *	@variable		Instiantiates 'Bypass' object, pass integer parameter for bypass type/level.
 *	@param1			{ _hackShield = 0, _noCrc = 1 }
 */
	static Bypass *bypass = new Bypass(bTypes::hackShield);
/*
 *	@variable		Instiantiates 'Library' class, allows calls for specified hacks and cheats.
 */
	static Library *lib = new Library();
/*
 *	@variable		Instiantiates 'Speed' class, allows calls specifically to modify 'QueryPerformanceCounter'.
 */
	static Speed *speed = new Speed();

	static ofstream GOAL ("P_MODULE.ini");

	enum LOG { _CARAT = 0, _LEVEL = 1 };
/*
 *	@structure		Allows use of native method 'FindWindow' using the WINAPI.
 */
HWND WINAPI FindWindow(
  _In_opt_  LPCTSTR lpClassName,
  _In_opt_  LPCTSTR lpWindowName
);

class P_Tools {

public:

	string CONFIG;

public:
	/*
	 *	INITIALIZE_MAIN
	 *	@function		Perform protection/security scanner, and initialize hackshield bypass.
	 *	@return			void
	 */
		VOID doFUNCS() {

			PROTECT->_SCAN();
			bypass->Initialize();
		}
	/*
	 *	CHECK_CLIENT_INITIALIZED
	 *	@function		Checks if client initialized by scanning for var->pointer.
	 *	@return			bool
	 */
		bool isGAME() {

			if (lib->getChannel() > 0) {
				return true;
			}
			return false;
		}
	/*
	 *	LOG_STAT_VARS
	 *	@function		Logs specified statistics to be read.
	 *	@param1			Used to decide what statistic to log.
	 *	@return			void
	 */
		void logINFO(int Type) {

			switch (Type) {
			case _CARAT:
				{
				int mCARAT = NULL;

				label_CARAT:
				if (mCARAT == NULL) {
					mCARAT = lib->getCarat();
					goto label_CARAT;
				}
				GOAL << mCARAT;
				GOAL.close();
				}
				break;
			case _LEVEL:
				{
				int mLEVEL = NULL;

				label_LEVEL:
				if (mLEVEL == NULL) {
					mLEVEL = lib->getLevel();
					goto label_LEVEL;
				}

				ofstream GOAL ("P_MODULE.ini");
				GOAL << mLEVEL;
				GOAL.close();
				}
				break;
			}
		}
	/*
	 *	GET_MODULE_SETTINGS
	 *	@function		Check specified module initialization settings.
	 *	@return			void
	 *	 >types			$PyE_C, $PyE_C_G, $PyE_E, $PyE_E_G : G=Goal
	 */
		void getMOD() {

			std::ifstream in("CONFIG.ini");
			string str((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());

			CONFIG = str;
		}
	/*
	 *	INITIALIZE_PYE_MODULE
	 *	@function		Perform initialization dependent on 'getMOD', specified settings.
	 *	@param1			Module settings.
	 *	@return			void
	 */
		void doMOD() {

			if (CONFIG != "") {
				if (CONFIG == "$PyE_C") {
					lib->doCARAT();
					GOAL.close();
				}
				if (CONFIG == "$PyE_E") {
					lib->doEXP();
					GOAL.close();
				}
				if (CONFIG == "$PyE_C_G") {
					lib->doCARAT();
					logINFO(_CARAT);
				}
				if (CONFIG == "$PyE_E_G") {
					lib->doEXP();
					logINFO(_LEVEL);
				}
			}
			else
				ExitProcess(0);
		}
};

/*
*	RECURSIVE_SET_CHANNEL
*	@function		Recursively sets channel to empty var.
*	@return			void
*/
	VOID doCHAN() {

		while (true) { lib->doCHANNEL(4); Sleep(1500); }
	}
/*
*	RECURSIVE_PROTECTION_SCAN
*	@function		Begin background protection/security scan.
*	@return			void
*/
	VOID scanTHREAD() {

		PROTECT->InitScan();
	}

void Main(void) {

	//INSTANTIATE_TOOLS
		P_Tools *tools = new P_Tools();
	
	//INITIALIZE_MODULE
		tools->doFUNCS();

	//GET_MODULE_TYPE
		//IF_GOALS_ENABLED_CLEAR_OLD_LOGS
		tools->getMOD();
		if ( tools->CONFIG == "$PyE_C_G" || "$PyE_E_G" ) {
			//SET_GOAL_DATA
			ofstream GOAL ( "P_MODULE.ini" );
			GOAL << NULL;
		}

	//WAIT_FOR_WINDOW
label_Window:
		if ( FindWindow( "Rumble Fighter", "Rumble Fighter" ) == NULL ) {
			Sleep( 500 );
			goto label_Window;
		}

	//WAIT_FOR_CLIENT
label_Game:
		if ( !tools->isGAME() ) {
			Sleep( 1000 );
			goto label_Game;
		}

	//LOAD_MODULE
		lib->Initialize(tools->CONFIG);

	//ENABLE_HACKS
		tools->doMOD();

	//DO_CHANNEL_HACK
		thread CHAN( &doCHAN );
		CHAN.detach();

	//ENABLE_SPEED
		speed->setSPEED( 500 );
		speed->doSPEED();

	//CREATE_SCAN_THREAD
		thread SECURE( &scanTHREAD );
		SECURE.detach();
}