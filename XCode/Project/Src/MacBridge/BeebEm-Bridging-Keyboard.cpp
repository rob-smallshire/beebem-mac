//
//  BeebEm-Bridging-Keyboard.cpp
//  BeebEm
//
//  Created by Commander Coder on 12/04/2024.
//

#include <windows.h>


enum {
	aEventJoystick1Axis              = 0x0000,
	aEventJoystick1Button            = 0x0001,
	aEventJoystick2Axis              = 0x1000,
	aEventJoystick2Button            = 0x1001,
};


#include <Carbon/Carbon.h>
#include <AudioToolbox/AudioServices.h>
#include "BeebWin.h"
#include "6502core.h"
#include "Disc8271.h"
#include "Disc1770.h"
#include "Econet.h"
#include "Scsi.h"
#include "Sasi.h"
#include "IDE.h"
#include "Sound.h"
#include "Music5000.h"
#include "Tube.h"
#include "Master512CoPro.h"
#include "Z80mem.h"
#include "Z80.h"
#include "SprowCoPro.h"
#include "Teletext.h"
#include "SysVia.h"
extern BeebWin* mainWin;


// map from APPLE input: https://eastmanreference.com/complete-list-of-applescript-key-codes
// to WINDOWS input: https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes

// which is then translated to beeb input:
/* from b-Em
 
 *       0x00      0x01  0x02  0x03 0x04 0x05 0x06 0x07 0x08 0x09    0x0a   0x0b   0x0c
 * 0x00  Shift     Ctrl  <------- starup up DIP swicthes ------->
 * 0x10  Q         3#    4$    5%   f4   8(   f7   -
 *    ^~   Left    KP 6   KP 7
 * 0x20  f0        W     E     T    7'   I    9)   0    _£   Down    KP 8   KP 9
 * 0x30  1!        2"    D     R    6&   U    O    P    [{   Up      KP +   KP -   KP Return
 * 0x40  CapsLck   A     X     F    Y    J    K    @    :*   Return  KP /   KP Del KP .
 * 0x50  ShiftLck  S     C     G    H    N    L    ;+   ]}   Delete  KP #   KP *   KP ,
 * 0x60  Tab       Z     SPC   V    B    M    ,<   .>   /?   Copy    KP 0   KP 1   KP 3
 * 0x70  ESC       f1    f2    f3   f5   f6   f8   f9   \|   Right   KP 4   KP 5   KP 2
 */

int kmap[] =
{
VK_KEY_A, //[0]
VK_KEY_S,
VK_KEY_D,
VK_KEY_F,
VK_KEY_H,
VK_KEY_G,
VK_KEY_Z,
VK_KEY_X,
VK_KEY_C,
VK_KEY_V,
VK_OEM_MINUS, //VK_KEY_§ (next to 1)
VK_KEY_B, //[11]
VK_KEY_Q,
VK_KEY_W,
VK_KEY_E,
VK_KEY_R,
VK_KEY_Y,
VK_KEY_T,
VK_KEY_1,
VK_KEY_2,
VK_KEY_3,
VK_KEY_4,
VK_KEY_6,
VK_KEY_5,
VK_OEM_PLUS,
VK_KEY_9, //[25]
VK_KEY_7,
VK_OEM_MINUS,
VK_KEY_8,
VK_KEY_0,
VK_OEM_6, //]
VK_KEY_O,
VK_KEY_U,
VK_OEM_4, //[
VK_KEY_I,
VK_KEY_P,
VK_RETURN,
VK_KEY_L,
VK_KEY_J, //[38]
VK_OEM_3, //'
VK_KEY_K,
VK_OEM_1, //;
VK_OEM_5, //\    .
VK_OEM_COMMA,
VK_OEM_2, // /
VK_KEY_N,
VK_KEY_M,
VK_OEM_PERIOD, //.
VK_TAB,
VK_SPACE,
VK_OEM_8, // ~` [50]
VK_BACK,
0,
VK_ESCAPE,
0,
0, // [55] COMMAND
0, // [56] /// left shift
0, // [57] caps lock
0, // [58] left option
0, // [59] left control
0, // [60] // right shift
0, // [61] right option
0, // [62] right control
0, // [63] fn key
0, // [64]
0,
0,
0,
0, // [68]
0,
0,
0,
0, // [72]
0,
0,
0,
0, // [76]
0,
0,
0,
0, // [80]
0,
0,
0,
0, // [84]
0,
0,
0,
0, // [88]
0,
0,
0,
0, // [92]
0,
0,
0,
VK_F5, // F5 [96]
VK_F6, // F6 [97]
VK_F7, // F3 [98]
VK_F3, // F7 [99]
VK_F8, // F8 [100]
VK_F9, // F9 [101]
0,
VK_END, // [COPY - F11] 103
0,
VK_F11, // [SHIFT LOCK - F13] 105
0,
0,
0,
VK_F10, // F10
0,
VK_F12, //[BREAK - F12]
0, // [112]
0,
0,
0,
0, // pageup [116]
0,
VK_F4, // F4
VK_END, // <end>
VK_F2, // F2 [120]
0, //pagedown
VK_F1, // F1
VK_LEFT, // <left>
VK_RIGHT,  // <right> [124]
VK_DOWN,  // <down>
VK_UP,  // <up>
0,
VK_SHIFT, // <SHIFT> //[128]
0,
0,
0,
0, // [132]
0,
0,
0,
0, // [136]
0,
0,
0,
0, // [140]
0,
0,
0,
0, // [144]
0,
0,
0,
0, // [148]
0,
0,
0,
0, // [152]
0,
0,
0,
0, // [156]
0,
0,
0,
0, // [160]
0,
0,
0,
0, // [164]
0,
0,
0,
0, // [168]
0,
0,
0,
0, // [172]
0,
0,
0,
0, // [176]
0,
0,
0,
0, // [180]
0,
0,
0,
0, // [184]
0,
0,
0,
0, // [188]
0,
0,
0,
0, // [192]
0,
0,
0,
0, // [196]
0,
0,
0,
0, // [200]
VK_CONTROL, // 201 is CTRL
VK_CAPITAL, // 202 is ALT
0,
0, // [204]
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	0,
//	6 10 0 6 10 1 // NUMPAD0 [355]
//	6 11 0 6 11 1
//	7 12 0 7 12 1
//	6 12 0 6 12 1
//	7 10 0 7 10 1
//	7 11 0 7 11 1
//	1 10 0 1 10 1 // NUMPAD6
//	1 11 0 1 11 1
//	2 10 0 2 10 1
//	2 11 0 2 11 1
//	5 11 0 5 11 1
//	3 10 0 3 10 1
//	3 11 0 3 11 1 // VK_SUBTRACT
//	4 12 0 4 12 1
//	4 10 0 4 10 1
//	-3 -3 0 -3 -3 1
//	-3 -4 0 -3 -4 1 // <SHORTCUT>


};

int remapKeys(int k)
{
	if (k > sizeof(kmap))
		return k;
	
	return kmap[k];
}

// BeebKey structure - represents a BBC Micro keyboard matrix position
struct BeebKey {
	int row;              // BBC keyboard matrix row (0-7), or -1 if not available, -2 for Break
	int col;              // BBC keyboard matrix column (0-12)
	bool shift;           // Whether to press BBC Shift key along with this key
	bool respectModifiers; // Whether to respect Mac modifier state (Shift/Ctrl/Alt) for this key
	                       // true for function keys, arrows, etc. (allows Shift+F1, Shift+Arrow, etc.)
	                       // false for character keys (we control BBC Shift explicitly from here)
};

// Direct character-to-Beeb mapping for Logical keyboard mode
// Maps Unicode/ASCII characters directly to BBC Micro keyboard matrix positions
// Based on BBC keyboard matrix in lines 48-54 above
//
// This approach works with ANY Mac keyboard layout because it maps characters,
// not physical key positions. A French keyboard user typing '#' (however they do it)
// will get '#' on the Beeb, regardless of which physical key they pressed.
//
// Parameters:
//   charCode   - The Unicode scalar value of the character produced (from lParam in beeb_handlekeys)
//   macKeyCode - The macOS key code (from wParam) to distinguish keypad from main keyboard
//                and handle non-character keys (function keys, arrows, etc.)
//
// Returns BeebKey structure with:
//   row              - BBC keyboard matrix row (0-7), or -1 if character not available on BBC,
//                      or -2 for Break key, or -3 for special functions (Page Up/Down)
//   col              - BBC keyboard matrix column (0-12)
//   shift            - Whether to press BBC Shift along with this key (for character keys only)
//   respectModifiers - Whether to respect Mac modifier state for this key:
//                      false = character keys (we control BBC Shift explicitly based on character)
//                      true  = non-character keys (function keys, arrows - allow Shift+F1, etc.)
static BeebKey charToBeeb(int charCode, int macKeyCode)
{
	// Handle function keys and arrow keys by macOS key code (they don't produce character codes)
	// Note: Mac F3/F7 keys are swapped in hardware (Mac F3 is keycode 99, Mac F7 is keycode 98)
	// These are non-character keys, so respectModifiers=true (allow Shift+F1, Shift+Arrow, etc.)
	switch(macKeyCode)
	{
		case 122: return {7, 1, false, true};  // F1 → BBC f1
		case 120: return {7, 2, false, true};  // F2 → BBC f2
		case 99:  return {7, 3, false, true};  // F3 (Mac keycode 99) → BBC f3
		case 118: return {1, 4, false, true};  // F4 → BBC f4
		case 96:  return {7, 4, false, true};  // F5 → BBC f5
		case 97:  return {7, 5, false, true};  // F6 → BBC f6
		case 98:  return {1, 6, false, true};  // F7 (Mac keycode 98) → BBC f7
		case 100: return {7, 6, false, true};  // F8 → BBC f8
		case 101: return {7, 7, false, true};  // F9 → BBC f9
		case 109: return {2, 0, false, true};  // F10 → BBC f0
		case 103: return {6, 9, false, true};  // F11 → Copy
		case 111: return {-2, 0, false, true}; // F12 → Break
		case 105: return {5, 0, false, true};  // F13 → Shift Lock
		case 79:  return {5, 10, false, true}; // F18 → KP # (for extended Mac keyboards)
		case 80:  return {5, 12, false, true}; // F19 → KP , (for extended Mac keyboards)
		case 71:  return {4, 11, false, true}; // Clear (keypad) → KP Del
		case 123: return {1, 9, false, true};  // Left arrow
		case 124: return {7, 9, false, true};  // Right arrow
		case 125: return {2, 9, false, true};  // Down arrow
		case 126: return {3, 9, false, true};  // Up arrow
	}

	// Handle numeric keypad separately by checking macOS key code
	// Numeric keypad: macOS codes 82-92 (keypad digits and operators)
	if (macKeyCode >= 82 && macKeyCode <= 92)
	{
		// Numeric keypad keys map to BBC keypad positions
		switch(charCode)
		{
			case '0': return {6, 10, false, false};  // KP 0
			case '1': return {6, 11, false, false};  // KP 1
			case '2': return {7, 12, false, false};  // KP 2
			case '3': return {6, 12, false, false};  // KP 3
			case '4': return {7, 10, false, false};  // KP 4
			case '5': return {7, 11, false, false};  // KP 5
			case '6': return {1, 10, false, false};  // KP 6
			case '7': return {1, 11, false, false};  // KP 7
			case '8': return {2, 10, false, false};  // KP 8
			case '9': return {2, 11, false, false};  // KP 9
			case '*': return {5, 11, false, false};  // KP *
			case '+': return {3, 10, false, false};  // KP +
			case '-': return {3, 11, false, false};  // KP -
			case '.': return {4, 12, false, false};  // KP .
			case '/': return {4, 10, false, false};  // KP /
			default: return {-1, -1, false, false};
		}
	}

	// Main keyboard - direct character to Beeb position mapping
	switch(charCode)
	{
		// Letters - lowercase unshifted, uppercase shifted on Beeb
		case 'a': return {4, 1, false, false};
		case 'A': return {4, 1, true, false};
		case 'b': return {6, 4, false, false};
		case 'B': return {6, 4, true, false};
		case 'c': return {5, 2, false, false};
		case 'C': return {5, 2, true, false};
		case 'd': return {3, 2, false, false};
		case 'D': return {3, 2, true, false};
		case 'e': return {2, 2, false, false};
		case 'E': return {2, 2, true, false};
		case 'f': return {4, 3, false, false};
		case 'F': return {4, 3, true, false};
		case 'g': return {5, 3, false, false};
		case 'G': return {5, 3, true, false};
		case 'h': return {5, 4, false, false};
		case 'H': return {5, 4, true, false};
		case 'i': return {2, 5, false, false};
		case 'I': return {2, 5, true, false};
		case 'j': return {4, 5, false, false};
		case 'J': return {4, 5, true, false};
		case 'k': return {4, 6, false, false};
		case 'K': return {4, 6, true, false};
		case 'l': return {5, 6, false, false};
		case 'L': return {5, 6, true, false};
		case 'm': return {6, 5, false, false};
		case 'M': return {6, 5, true, false};
		case 'n': return {5, 5, false, false};
		case 'N': return {5, 5, true, false};
		case 'o': return {3, 6, false, false};
		case 'O': return {3, 6, true, false};
		case 'p': return {3, 7, false, false};
		case 'P': return {3, 7, true, false};
		case 'q': return {1, 0, false, false};
		case 'Q': return {1, 0, true, false};
		case 'r': return {3, 3, false, false};
		case 'R': return {3, 3, true, false};
		case 's': return {5, 1, false, false};
		case 'S': return {5, 1, true, false};
		case 't': return {2, 3, false, false};
		case 'T': return {2, 3, true, false};
		case 'u': return {3, 5, false, false};
		case 'U': return {3, 5, true, false};
		case 'v': return {6, 3, false, false};
		case 'V': return {6, 3, true, false};
		case 'w': return {2, 1, false, false};
		case 'W': return {2, 1, true, false};
		case 'x': return {4, 2, false, false};
		case 'X': return {4, 2, true, false};
		case 'y': return {4, 4, false, false};
		case 'Y': return {4, 4, true, false};
		case 'z': return {6, 1, false, false};
		case 'Z': return {6, 1, true, false};

		// Digits (unshifted on Beeb)
		case '0': return {2, 7, false, false};
		case '1': return {3, 0, false, false};
		case '2': return {3, 1, false, false};
		case '3': return {1, 1, false, false};
		case '4': return {1, 2, false, false};
		case '5': return {1, 3, false, false};
		case '6': return {3, 4, false, false};
		case '7': return {2, 4, false, false};
		case '8': return {1, 5, false, false};
		case '9': return {2, 6, false, false};

		// Beeb shifted number row: ! " # $ % & ' ( )
		case '!': return {3, 0, true, false};   // Shift+1 on Beeb
		case '"': return {3, 1, true, false};   // Shift+2 on Beeb
		case '#': return {1, 1, true, false};   // Shift+3 on Beeb
		case '$': return {1, 2, true, false};   // Shift+4 on Beeb
		case '%': return {1, 3, true, false};   // Shift+5 on Beeb
		case '&': return {3, 4, true, false};   // Shift+6 on Beeb
		case '\'': return {2, 4, true, false};  // Shift+7 on Beeb (apostrophe)
		case '(': return {1, 5, true, false};   // Shift+8 on Beeb
		case ')': return {2, 6, true, false};   // Shift+9 on Beeb (Shift+0 produces different char)

		// Row 1, col 7: -= (hyphen/equals)
		case '-': return {1, 7, false, false};
		case '=': return {1, 7, true, false};

		// Row 1, col 8: ^~ (caret/tilde)
		case '^': return {1, 8, false, false};
		case '~': return {1, 8, true, false};

		// Row 2, col 8: _£ (underscore/pound)
		case '_': return {2, 8, false, false};
		case 0xA3: return {2, 8, true, false};  // £ (Unicode U+00A3)

		// Row 3, col 8: [{ (brackets)
		case '[': return {3, 8, false, false};
		case '{': return {3, 8, true, false};

		// Row 4, col 7: @ (at sign)
		case '@': return {4, 7, false, false};

		// Row 4, col 8: :* (colon/asterisk)
		case ':': return {4, 8, false, false};
		case '*': return {4, 8, true, false};

		// Row 5, col 7: ;+ (semicolon/plus)
		case ';': return {5, 7, false, false};
		case '+': return {5, 7, true, false};

		// Row 5, col 8: ]} (brackets)
		case ']': return {5, 8, false, false};
		case '}': return {5, 8, true, false};

		// Row 6, col 6: ,< (comma/less-than)
		case ',': return {6, 6, false, false};
		case '<': return {6, 6, true, false};

		// Row 6, col 7: .> (period/greater-than)
		case '.': return {6, 7, false, false};
		case '>': return {6, 7, true, false};

		// Row 6, col 8: /? (slash/question)
		case '/': return {6, 8, false, false};
		case '?': return {6, 8, true, false};

		// Row 7, col 8: \| (backslash/pipe)
		case '\\': return {7, 8, false, false};
		case '|': return {7, 8, true, false};

		// Control characters
		case ' ':  return {6, 2, false, false};  // Space
		case '\t': return {6, 0, false, false};  // Tab
		case '\r': case '\n': return {4, 9, false, false};  // Return
		case 0x08: return {5, 9, false, false};  // Backspace
		case 0x7F: return {5, 9, false, false};  // Delete
		case 0x1B: return {7, 0, false, false};  // Escape

		// Shift+Tab produces charCode 25 (0x19) on macOS
		// Map it to Tab with BBC Shift pressed
		case 0x19: return {6, 0, true, false};  // Shift+Tab

		// Character not available on BBC keyboard - ignore
		default: return {-1, -1, false, false};
	}
}


/*
*/

// File-scoped variable to track last seen modifier state
// Moved from inside beeb_handlekeys to allow reset function access
static long last_wParam = 0;

// Signal that a character is not available on the BBC keyboard
// Logs to stderr and plays system alert sound to provide user feedback
static void signalUnsupportedKey()
{
	fprintf(stderr, "  Character not available on BBC keyboard - IGNORING\n");
	AudioServicesPlaySystemSound(kSystemSoundID_UserPreferredAlert);
}

// Send a complete Caps Lock key DOWN/UP cycle to the BBC emulator
// This triggers the BBC MOS to toggle its Caps Lock state
static void pressCapsLock()
{
	// Send DOWN - BBC MOS detects key press and toggles Caps Lock state
	int row, col;
	mainWin->TranslateKey(VK_CAPITAL, false, row, col);

	// Schedule UP for 50ms later (40ms Windows default + 25% reliability margin)
	// This delay ensures BBC MOS has time to scan keyboard and process the key press
	// The UP releases the key so the next toggle will be detected as a new press
	dispatch_after(dispatch_time(DISPATCH_TIME_NOW, 50 * NSEC_PER_MSEC),
					dispatch_get_main_queue(), ^{
		int up_row, up_col;
		mainWin->TranslateKey(VK_CAPITAL, true, up_row, up_col);
	});
}

// Sync BBC Caps Lock state to match Mac Caps Lock state
// Called on startup and when window gains focus
extern "C" void beeb_syncCapsLockState(int macCapsLockIsOn)
{
	// Read current BBC Caps Lock state from LEDs global
	bool macCapsOn = (macCapsLockIsOn != 0);
	bool bbcCapsLockIsOn = LEDs.CapsLock;

	// If states differ, send toggle to BBC to bring into sync
	if (macCapsOn != bbcCapsLockIsOn) {
		pressCapsLock();
	}
}

// Reset modifier tracking to current Mac state
// Called when window gains focus to prevent stale state issues
extern "C" void beeb_resetModifierTracking(long currentModifiers)
{
	last_wParam = currentModifiers;
}

// SWIFT calls this to
extern "C" void beeb_handlekeys(long message, long wParam, long lParam)
{
//	message :eventkind
//  wParam :keycode
//	lParam :charCode

	int row, col;
	int key;

	switch (message)
	{
		case kEventRawKeyDown:
			fprintf(stderr, "Key pressed: macKeyCode=%ld, charCode=%ld (0x%lx) '%c'\n",
			        wParam, lParam, lParam, lParam >= 32 && lParam < 127 ? (char)lParam : '?');
			// Reset shift state if it was set by Run Disc
			if (mainWin->m_ShiftBooted)
			{
				mainWin->m_ShiftBooted = false;
				BeebKeyUp(0, 0);
			}

			// In Logical mode, use direct character-to-BBC mapping for dynamic keyboard layout support
			if (mainWin->m_KeyboardMapping == KeyboardMappingType::Logical)
			{
				BeebKey beebKey = charToBeeb((int)lParam, (int)wParam);
				fprintf(stderr, "  charToBeeb returned: row=%d, col=%d, shift=%d, respectModifiers=%d\n",
				        beebKey.row, beebKey.col, beebKey.shift, beebKey.respectModifiers);

				if (beebKey.row < 0)
				{
					// Character not available on BBC keyboard - ignore and beep
					signalUnsupportedKey();
					break;
				}

				// Handle BBC Shift based on key type
				if (beebKey.respectModifiers)
				{
					// Non-character key (function keys, arrows, etc.)
					// Check Mac's physical Shift state and pass it through to BBC
					if (mainWin->m_ShiftPressed)
					{
						BeebKeyDown(0, 0);  // Press BBC Shift
					}
				}
				else
				{
					// Character key - we control BBC Shift explicitly based on the character
					// Ignore Mac's physical Shift state - we only care about the character produced
					if (beebKey.shift)
					{
						// This character needs BBC Shift - press it
						BeebKeyDown(0, 0);
					}
					else
					{
						// This character doesn't need BBC Shift - ensure it's not pressed
						// (In case Mac Shift is down but we don't want BBC Shift)
						BeebKeyUp(0, 0);
					}
				}

				// Press the key
				BeebKeyDown(beebKey.row, beebKey.col);
			}
			else
			{
				// Default/User mode - use VK codes as normal
				key = remapKeys((int)wParam);
				mainWin->TranslateKey(key, false, row, col);
			}
			break;
		case kEventRawKeyUp:
//          fprintf(stderr, "Key released: code = %d, '%c'\n", wParam, lParam);
			// In Logical mode, use direct character-to-BBC mapping for dynamic keyboard layout support
			if (mainWin->m_KeyboardMapping == KeyboardMappingType::Logical)
			{
				BeebKey beebKey = charToBeeb((int)lParam, (int)wParam);

				if (beebKey.row < 0)
				{
					// Check for special keys (Break, Page Up/Down)
					if (beebKey.row == -2)
					{
						mainWin->Break();
					}
					else if (beebKey.row == -3)
					{
						if (beebKey.col == -3) SoundTuning += 0.1; // Page Up
						if (beebKey.col == -4) SoundTuning -= 0.1; // Page Down
					}
					break;
				}

				// Release the key
				BeebKeyUp(beebKey.row, beebKey.col);

				// Release Beeb Shift if it was pressed
				if (beebKey.shift)
				{
					BeebKeyUp(0, 0);
				}
			}
			else
			{
				// Default/User mode - use VK codes as normal
				key = remapKeys((int)wParam);

				if (mainWin->TranslateKey(key, true, row, col) < 0)
					{

						if (row == -2)
						{
							mainWin->Break();
						}
						else if (row == -3)
						{
							if (col == -3) SoundTuning += 0.1; // Page Up
							if (col == -4) SoundTuning -= 0.1; // Page Down
						}
				}
			}
			break;
		case kEventRawKeyModifiersChanged:
		{
//            fprintf(stderr, "Key modifier : code = %016x\n", wParam);
			long diff_wParam = wParam ^ last_wParam; // XOR - to find what changed

			// bitpatterns
			// 0000 0010 0000 0000 0000 0010 - L SHIFT
			// 0000 0010 0000 0000 0000 0100 - R SHIFT
			// 0000 0100 0000 0000 0000 0001 - CTRL
			// 0000 1000 0000 0000 0010 0000 - L ALT
			// 0000 1000 0000 0000 0100 0000 - R ALT
			// 0000 0001 0000 0000 0000 0000 - CAPS
			// 1000 0000 0000 0000 0000 0000 - FN
			// 0001 0000 0000 0000 0000 1000 - L CMD
			// 0001 0000 0000 0000 0001 0000 - R CMD

#define SHIFTMASK 0x20000
#define ALTMASK 0x80000
#define CTRLMASK 0x40000
#define CMDMASK 0x100000
#define CAPSMASK 0x10000
#define FNMASK 0x800000

			// Shift key handling
			if ((diff_wParam & SHIFTMASK)!=0) // left and right shift key
			{
				bool shiftUp = (wParam & SHIFTMASK)==0;

				if (mainWin->m_KeyboardMapping == KeyboardMappingType::Logical)
				{
					// In Logical mode, just track the state - don't send BBC key events
					// The key down handler will decide whether to use this state based on respectModifiers
					mainWin->m_ShiftPressed = !shiftUp;
				}
				else
				{
					// In Default/User mode, send BBC Shift key events normally
					mainWin->TranslateKey(VK_SHIFT, shiftUp, row, col);
				}
			}

			// APPLE CTRL KEY
			if ((diff_wParam & CTRLMASK)!=0) // left and right ctrl key
			{
				// UP when mask is 0, DOWN if mask is 1
				mainWin->TranslateKey(VK_CONTROL, (wParam & CTRLMASK)==0, row, col);
			}

			// APPLE ALT/OPTION KEY
			if ((diff_wParam & ALTMASK)!=0) // left and right alt/option key
			{
				// UP when mask is 0, DOWN if mask is 1
				// Maps to Windows VK_MENU (the Alt key - confusing name,
				// but VK_MENU is indeed the Windows Alt key)
				mainWin->TranslateKey(VK_MENU, (wParam & ALTMASK)==0, row, col);
			}

			// APPLE CAPS LOCK KEY
			//
			// Implementation Notes (discovered through systematic research and testing):
			//
			// 1. macOS Caps Lock behavior:
			//    - Toggle key with persistent state (LED on physical key)
			//    - We receive modifier change events when state toggles (ON to OFF)
			//    - Events report the NEW state, not press/release
			//
			// 2. BBC Micro Caps Lock behavior:
			//    - Momentary key at keyboard matrix position row 4, column 0
			//    - BBC MOS scans keyboard matrix and toggles IC32 bit 6 on key PRESS
			//    - IC32 bit 6 controls Caps Lock LED (active-low: 0=ON, 1=OFF)
			//    - The MOS toggles Caps Lock state on key DOWN only, not on key UP
			//
			// 3. Discovered through testing:
			//    - Sending KEY DOWN triggers BBC MOS to toggle Caps Lock LED
			//    - Sending KEY UP has no effect (TranslateKey returns row=-1, col=-1)
			//
			// 4. State synchronization implications:
			//    - Mac and BBC Caps Lock states can get out of phase when Mac key is
			//      toggled outside of BeebEm, e.g. while BeebEm window is unfocused
			//      so we take special action on focus gain to resynchronise states
			//
			// 5. Known limitations:
			//    - INKEY(-65) cannot detect "key held". This is a limitation of how
			//      Caps Lock works in macOS.
			//    - Games needing held Caps Lock should use "Map A,S to Caps,Ctrl" option
			//    - Initial state may be out of sync (BBC boots with LED ON due to IC32State=0x00)
			//      so we take special action on startup to sync states
			//
			if ((diff_wParam & CAPSMASK)!=0)
			{
				pressCapsLock();
			}
			
			last_wParam = wParam;
		}
		break;
	}
}

extern "C" void beeb_handlemouse(long message, long wParam, long lParam)
{
	UINT msg = WM_LBUTTONUP;
	switch (message)
	{
		case kEventMouseUp:
//			fprintf(stderr, "mouseup : code = %ld\n", wParam);
			switch (wParam)
			{
				case MK_RBUTTON: msg = WM_RBUTTONUP; break;
				case MK_MBUTTON: msg = WM_MBUTTONUP; break;
				default: msg = WM_LBUTTONUP; break;
			}
			mainWin->AppProc(msg, wParam, lParam);
			break;
		case kEventMouseDown:
//			fprintf(stderr, "mousedown : code = %ld\n", wParam);
			switch (wParam)
			{
				case MK_RBUTTON: msg = WM_RBUTTONDOWN; break;
				case MK_MBUTTON: msg = WM_MBUTTONDOWN; break;
				default: msg = WM_LBUTTONDOWN; break;
			}
			mainWin->AppProc(msg, wParam, lParam);
			break;
		case kEventMouseMoved:
		case kEventMouseDragged:
			int x = mainWin->m_XWinSize*GET_X_LPARAM(lParam)/32768.0f;
			int y = mainWin->m_YWinSize*GET_Y_LPARAM(lParam)/32768.0f;
			
//			fprintf(stderr, "mousemove : %ld, x = %d, y = %d\n", message, x, y);

			bool useMovementDelta = true;
			if (useMovementDelta)
				mainWin->AppProc(WM_INPUT, wParam, MAKELPARAM(x,y));
			else
				mainWin->AppProc(WM_MOUSEMOVE, wParam, MAKELPARAM(x,y));
			break;
	}
}

static long joy1[4]; // 4 axes
static long buttons1; // button bitflags

extern "C" void beeb_handlejoystick(long message, long wParam, long lParam)
{
	// axis or buttons message (joystick1, joystick2)
	// axes:
	//  wParam is the axis number (axis on a PS3 controller)
	//  lParam is axis values
	// buttons:
	//  wParam is the button number
	//  lParam is value (0 up, 1 down)
	
	switch (message)
	{
		case aEventJoystick1Axis:
			// wParam is 0,1,2 or 3 for x and y axes (on joystick)
			if (wParam >= 0 && wParam < 4)
			{
				joy1[wParam] = 32768.0+lParam;
			}

			//	MM_JOY1MOVE
			//	lParam x,y from wXmax to wXmin..
			mainWin->AppProc(MM_JOY1MOVE, 0, MAKELPARAM(joy1[0],joy1[1]));
			break;
		case aEventJoystick1Button:
			//	 JOY_BUTTON1 , JOY_BUTTON2
			if (wParam == 0) break;
			
			int bitpos = 0;
			if (wParam == 1) bitpos = JOY_BUTTON1;
			if (wParam == 2) bitpos = JOY_BUTTON2;
			if (wParam == 3) bitpos = JOY_BUTTON3;
			if (wParam == 4) bitpos = JOY_BUTTON4;

			long oldpattern = buttons1;

			if (lParam==0)
			{
				buttons1 &= ~bitpos;
				if (oldpattern != buttons1)
					mainWin->AppProc(MM_JOY1BUTTONUP, buttons1, 0);
			}
			else
			{
				buttons1 |= bitpos;
				if (oldpattern != buttons1)
					mainWin->AppProc(MM_JOY1BUTTONDOWN, buttons1, 0);
			}
			break;
	}
	
}

