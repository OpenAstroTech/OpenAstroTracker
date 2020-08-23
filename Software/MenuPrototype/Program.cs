using System;
using System.Collections.Generic;
using System.ComponentModel.Design;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace MenuPrototype
{

	class Program
	{
		const int btnNONE = 0;
		const int btnLEFT = 1;
		const int btnRIGHT = 2;
		const int btnUP = 3;
		const int btnDOWN = 4;
		const int btnSELECT = 5;
		static DateTime _startTime = DateTime.UtcNow;

		[DllImport("user32.dll", EntryPoint = "GetKeyState")]
		internal static extern short GetKeyState(int keyCode);

		static public ulong millis()
		{
			TimeSpan dt = DateTime.UtcNow - _startTime;
			return (ulong)dt.TotalMilliseconds;
		}

		static int getKeyState()
		{
			int keyState = btnNONE;
			if (GetKeyState(0x26) < 0)
			{
				keyState = btnUP;
			}
			else if (GetKeyState(0x28) < 0)
			{
				keyState = btnDOWN;
			}
			else if (GetKeyState(0x25) < 0)
			{
				keyState = btnLEFT;
			}
			else if (GetKeyState(0x27) < 0)
			{
				keyState = btnRIGHT;
			}
			else if (GetKeyState(0x0D) < 0)
			{
				keyState = btnSELECT;
			}
			return keyState;
		}

		const int INCREMENT_NO_LIMITS = 0;
		const int INCREMENT_WRAP = 1;
		const int INCREMENT_CLAMP = 2;

		public class Incrementer
		{
			int _mode;
			int[] _maxVals;
			int[] _minVals;

			public Incrementer(int mode = INCREMENT_NO_LIMITS, int[] maxVals = null, int[] minVals = null)
			{
				_mode = mode;
				_maxVals = maxVals;
				_minVals = minVals;
			}

			protected void setMinVals(int[] minVals) { _minVals = minVals; }
			protected void setMaxVals(int[] maxVals) { _maxVals = maxVals; }

			public virtual void onChange(string tag, int[] numbers, int index, int val)
			{
				if (_mode == INCREMENT_CLAMP)
				{
					numbers[index] = adjustClamp(numbers[index], val, (_minVals != null) ? _minVals[index] : 0, _maxVals[index]);
				}
				else if (_mode == INCREMENT_WRAP)
				{
					numbers[index] = adjustWrap(numbers[index], val, (_minVals != null) ? _minVals[index] : 0, _maxVals[index]);
				}
				else if (_mode == INCREMENT_NO_LIMITS)
				{
					numbers[index] += val;
				}
			}

			public virtual string getDisplay(string tag, int index, int val, string formatString = null)
			{
				return val.ToString();
			}

			public virtual void getNumbers(string tag, int[] nums) { }

			public virtual void onSelect(EventArgs args) { }
		}

		public class SpeedIncrementer : Incrementer
		{
			int _speed = 0; // Come from mount
			int _ra = 0;
			int _dec = 0;
			int _back = 0;

			public override void onChange(string tag, int[] numbers, int index, int val)
			{
				base.onChange(tag, numbers, index, val);
				if (tag == "SPD")
				{
					_speed = numbers[0];
				}
				else if (tag == "RA")
				{
					_ra = numbers[0];
				}
				else if (tag == "DEC")
				{
					_dec = numbers[0];
				}
				else if (tag == "BACK")
				{
					_back = numbers[0];
				}
			}

			public override string getDisplay(string tag, int index, int val, string formatString = null)
			{
				if (tag == "SPD")
				{
					return string.Format("{0:0.0000}", 1.0 + 1.0 * val / 10000.0);
				}
				return val.ToString();
			}

			public override void getNumbers(string tag, int[] numbers)
			{
				if (tag == "SPD")
				{
					numbers[0] = _speed;
				}
				else if (tag == "RA")
				{
					numbers[0] = _ra;
				}
				else if (tag == "DEC")
				{
					numbers[0] = _dec;
				}
				else if (tag == "BACK")
				{
					numbers[0] = _back;
				}
			}

			public override void onSelect(EventArgs args)
			{
				Console.CursorTop = 4;
				if (args.source.getTag() == "SPD")
				{
					Console.WriteLine("Set speed factor to {0} ({1:0.0000})", _speed, 1.0 + 1.0 * _speed / 10000.0);
				}
				else if (args.source.getTag() == "RA")
				{
					Console.WriteLine("Set RA Steps to {0}", _ra);
				}
				else if (args.source.getTag() == "DEC")
				{
					Console.WriteLine("Set DEC Steps to {0}", _dec);
				}
				else if (args.source.getTag() == "BACK")
				{
					Console.WriteLine("Set Backlash asdjust to {0}", _back);
				}
			}
		}

		public class RaDecIncrementer : Incrementer
		{
			bool _isRA;
			public RaDecIncrementer(string which) : base(INCREMENT_WRAP, new[] { 90, 59, 59, 1 }, new[] { -90, 0, 0, 0 })
			{
				_isRA = which == "RA";
				if (_isRA)
				{
					setMinVals(new int[] { 0, 0, 0, 0 });
					setMaxVals(new int[] { 23, 59, 59, 1 });
				}
			}

			public override void onChange(string tag, int[] numbers, int index, int val)
			{
				if ((index == 0) && !_isRA)
				{
					numbers[0] = adjustClamp(numbers[0], val, -90, 90);
				}
				else
				{
					base.onChange(tag, numbers, index, val);
				}
			}
			public override string getDisplay(string tag, int index, int val, string formatString)
			{
				if (index == 3)
				{
					return val == 0 ? "Ta" : "Cu";
				}

				return val.ToString();
			}

			public override void getNumbers(string tag, int[] numbers)
			{
				if (numbers[3] == 0)
				{
					numbers[0] = 50; // mount get target ra/dec
				}
				else if (numbers[3] == 1)
				{
					numbers[0] = 80;// mount get current ra/dec
				}
			}
		}

		// Adjust the given number by the given adjustment, wrap around the limits.
		// Limits are inclusive, so they represent the lowest and highest valid number.
		static int adjustWrap(int current, int adjustBy, int minVal, int maxVal)
		{
			current += adjustBy;
			if (current > maxVal)
				current -= (maxVal + 1 - minVal);
			if (current < minVal)
				current += (maxVal + 1 - minVal);
			return current;
		}

		// Adjust the given number by the given adjustment, clamping to the limits.
		// Limits are inclusive, so they represent the lowest and highest valid number.
		static int adjustClamp(int current, int adjustBy, int minVal, int maxVal)
		{
			current += adjustBy;
			if (current > maxVal)
				current = maxVal;
			if (current < minVal)
				current = minVal;
			return current;
		}

		static void Main(string[] args)
		{
			var noLimitIncr = new Incrementer(INCREMENT_NO_LIMITS);
			var speedIncr = new SpeedIncrementer();
			var hourIncr = new Incrementer(INCREMENT_WRAP, new[] { 23, 59, 59 });
			var decIncr = new RaDecIncrementer("DEC");
			var raIncr = new RaDecIncrementer("RA");

			// Main top-level menu
			var menu = new MainMenu();

			//////  RA  ///////
			var raMenu = new MenuItem("RA");
			raMenu.addMenuItem(new NumberInput("TRA", 4, "^%02dh^%02dm^%02ds ^@", raConfirmed, raIncr));

			//////  DEC  ///////
			var decMenu = new MenuItem("DEC");
			decMenu.addMenuItem(new NumberInput("TDEC", 4, "^%02d*^%02d\"^%02d' ^@", decConfirmed, decIncr));

			//////  GO  ///////
			var goMenu = new MenuItem("GO");
			var goList = new ScrollList();
			goList.addMenuItem(new Button("Polaris", goPointChosen));
			goList.addMenuItem(new Button("Vega", goPointChosen));
			goList.addMenuItem(new Button("Orions Nebla", goPointChosen));
			goList.addMenuItem(new Button("M31 Andromeda", goPointChosen));
			goList.addMenuItem(new Button("Navi", goPointChosen));
			goList.addMenuItem(new Button("Antares", goPointChosen));
			goList.addMenuItem(new Button("Home", goPointChosen));
			goList.addMenuItem(new Button("Unpark", goPointChosen));
			goList.addMenuItem(new Button("Park", goPointChosen));
			goMenu.addMenuItem(goList);

			//////  HA  ///////
			var haMenu = new MenuItem("HA");
			haMenu.addMenuItem(new NumberInput("HA", new[] { 15, 32 }, "^%02dh^%02dm", haConfirmed, hourIncr));

			//////  CTRL  ///////
			var ctrlMenu = new MenuItem("CTRL");
			ctrlMenu.addMenuItem(new Button("Manual Control", startManualControl));
			var dlgManualControl = new ManualControlMenuItem(manualCompleted);

			var dlgSetHome = new MenuItem("Set home pos?", "ConfirmHome");
			dlgSetHome.addMenuItem(new OptionChooser(new string[] { "Yes", "No" }, 0, setHomePosition));

			//////  CAL  ///////
			var calMenu = new MenuItem("CAL");
			var calList = new ScrollList();

			var dlgStoreSync = new MenuItem("Adjust mount ", "StoreAndSync");
			dlgStoreSync.addMenuItem(new Button("Centered", storeAndSync));

			var dlgSlewToPolaris = new ActionRunnerModal("Slewing....", "SlewToPolaris", dlgStoreSync);
			var paBtn = new Button("Polar Alignmnt", (eventArgs) => menu.activateDialog("SlewToPolaris"));
			calList.addMenuItem(paBtn);

			var driftAlignBtn = new Button("Drift Alignmnt", (eventArgs) => menu.activateDialog("DriftAlign"));
			var dlgDriftAlign = new ActionRunnerModal("Drift alignment", "DriftAlign");
			calList.addMenuItem(driftAlignBtn);

			var speedBtn = new Button("Speed Calibratn", new NumberInput("SPD", 1, "SpdFctr: @", null, speedIncr, NumberInput.BehaviorFlags.AcceleratingRepetition));
			calList.addMenuItem(speedBtn);
			var raBtn = new Button("RA Step Adjust", new NumberInput("RA", 1, "RA Steps: @", null, speedIncr, NumberInput.BehaviorFlags.AcceleratingRepetition));
			calList.addMenuItem(raBtn);
			var decBtn = new Button("DEC Step Adjust", new NumberInput("DEC", 1, "DEC Steps: @", null, speedIncr, NumberInput.BehaviorFlags.AcceleratingRepetition));
			calList.addMenuItem(decBtn);
			var backBtn = new Button("Backlash Adjust", new NumberInput("DEC", 1, "DEC Steps: @", null, speedIncr, NumberInput.BehaviorFlags.AcceleratingRepetition));
			calList.addMenuItem(backBtn);
			var rollBtn = new Button("Roll Offset", new PitchRollDisplay("ROLL"));
			calList.addMenuItem(rollBtn);
			var pitchBtn = new Button("Pitch Offset", new PitchRollDisplay("PITCH"));
			calList.addMenuItem(pitchBtn);
			calMenu.addMenuItem(calList);

			//////  INFO  ///////
			var infoMenu = new MenuItem("INFO");
			var infoList = new ScrollList();
			infoList.addMenuItem(new TextInfoMulti( 3, getRASteps));
			infoList.addMenuItem(new TextInfoMulti( 3, getDECSteps));
			infoList.addMenuItem(new TextInfoMulti( 2, getTRKSteps));
			//infoList.addMenuItem(new TextInfo( "Loc ", getLocation));
			//infoList.addMenuItem(new TextInfo( "Temp: ", getTemperature));
			//infoList.addMenuItem(new TextInfo( "MemAvail: ", getMemAvail));
			//infoList.addMenuItem(new TextInfo( "Up: ", getUpTime));
			infoList.addMenuItem(new TextInfo( "Firmw.: ", () => "V1.9.00"));
			infoMenu.addMenuItem(infoList);

			// Add all the menus
			menu.addMenuItem(raMenu);
			menu.addMenuItem(decMenu);
			menu.addMenuItem(goMenu);
			menu.addMenuItem(haMenu);
			menu.addMenuItem(ctrlMenu);
			menu.addMenuItem(calMenu);
			menu.addMenuItem(infoMenu);

			// Add all the model dialogs.
			menu.addModalDialog(dlgManualControl);
			menu.addModalDialog(dlgSetHome);
			menu.addModalDialog(dlgSlewToPolaris);
			menu.addModalDialog(dlgStoreSync);
			menu.addModalDialog(dlgDriftAlign);

			// Startup Wizard
			var dlgStartIsHome = new MenuItem("Home Position?", "StartIsHome");
			dlgStartIsHome.addMenuItem(new OptionChooser(new string[] { "Yes", "No", "Cancl" }, 1, startupIsHomePosition));
			var dlgStartFindGPS = new MenuItem("Finding GPS...", "StartFindGPS");
			dlgStartFindGPS.addMenuItem(new FindGPSDisplay("FINDGPS"));
			var dlgStartHAGetGPS = new MenuItem("Set HA via?", "StartHAChooser");
			dlgStartHAGetGPS.addMenuItem(new OptionChooser(new string[] { "GPS Sync", "Manual Set" }, 0, startupHowToGetHA));
			var dlgStartHAGetManual = new MenuItem("Polaris HA?", "StartEnterHA");
			dlgStartHAGetManual.addMenuItem(new NumberInput("HA", new[] { 15, 32 }, "^%02dh^%02dm", startHAConfirmed, hourIncr, NumberInput.BehaviorFlags.ConstantRepetition));
			var dlgStartManualControl = new ManualControlMenuItem(startManualCompleted, "StartManualControl");

			menu.addModalDialog(dlgStartIsHome);
			menu.addModalDialog(dlgStartFindGPS);
			menu.addModalDialog(dlgStartHAGetGPS);
			menu.addModalDialog(dlgStartHAGetManual);
			menu.addModalDialog(dlgStartManualControl);

			// Set the first modal dialog on the menu system
			menu.activateDialog("StartIsHome");

			do
			{
				int keyState = getKeyState();

				// Console.Write("Key:{0} ", keyState);
				if (!menu.onPreviewKey(keyState))
				{
					if (menu.processKeys(keyState))
					{
						while (getKeyState() != btnNONE)
							;
					}
				}

				Console.CursorTop = 1;
				Console.CursorLeft = 0;
				menu.updateDisplay();
			}
			while (true);
		}

		private static void startManualCompleted(EventArgs args)
		{
			args.source.getMainMenu().activateDialog("StartIsHome");
		}

		public static void startupIsHomePosition(EventArgs args)
		{
			SelectEventArgs selectArgs = args as SelectEventArgs;
			switch (selectArgs.getSelected())
			{
				case "Yes":
					args.source.getMainMenu().activateDialog("StartFindGPS");
					break;
				case "No":
					args.source.getMainMenu().activateDialog("StartManualControl");
					break;
				case "Cancl":
					args.source.getMainMenu().closeDialog();
					break;
			}
		}

		public static void startupHowToGetHA(EventArgs args)
		{
			SelectEventArgs selectArgs = args as SelectEventArgs;
			switch (selectArgs.getSelected())
			{
				case "GPS Sync":
					args.source.getMainMenu().activateDialog("StartFindGPS");
					break;
				case "Manual Set":
					args.source.getMainMenu().activateDialog("StartEnterHA");
					break;
			}
		}

		public static void storeAndSync(EventArgs args)
		{
			Console.CursorTop = 4;
			Console.WriteLine("Set mount sync to Polaris.                 ");
			args.source.getMainMenu().closeDialog();
		}

		public static string getRASteps(int index)
		{
			switch (index)
			{
				case 0: return "RA Stpr: 2931";
				case 1: return "RTrg: 18h34m23s";
				case 2: return "RCur: 19h31m32s";
			}
			return "?";
		}

		public static string getDECSteps(int index)
		{
			switch (index)
			{
				case 0: return "DEC Stpr: 2931";
				case 1: return "DTrg: +78*14'52s\"";
				case 2: return "DCur: +89*51'12s\"";
			}
			return "?";
		}

		public static string getTRKSteps(int index)
		{
			var now = DateTime.UtcNow;
			int steps = now.Second + now.Minute * 60;
			switch (index)
			{
				case 0: return "TRK Stpr: " + steps.ToString();
				case 1: return "TRK Spd:1.678766";
			}
			return "?";

		}

		public static void manualCompleted(EventArgs args)
		{
			args.source.getMainMenu().activateDialog("ConfirmHome");
		}

		public static void startManualControl(EventArgs arg)
		{
			arg.source.getMainMenu().activateDialog("Control");
		}

		public static void answered(EventArgs args)
		{
			var arg = args as SelectEventArgs;
			Console.CursorTop = 3;
			Console.Write(arg.getSelected() + "      ");
		}

		public static void setHomePosition(EventArgs args)
		{
			var arg = args as SelectEventArgs;
			if (arg.getSelected() == "Yes")
			{
				Console.CursorTop = 3;
				Console.Write("Set mount home      ");
			}
			args.source.getMainMenu().closeDialog();
		}

		public static void goPointChosen(EventArgs args)
		{
			var arg = args as SelectEventArgs;
			Console.CursorTop = 4;
			Console.WriteLine("mount.slewTo({0})     ", arg.getSelected());
		}

		public static void raConfirmed(EventArgs args)
		{
			var num = args.source as NumberInput;
			Console.CursorTop = 4;
			Console.WriteLine("mount.targetRA() = {0}h {1}m {2}s", num.getValue(0), num.getValue(1), num.getValue(2));
		}

		public static void decConfirmed(EventArgs args)
		{
			var num = args.source as NumberInput;
			Console.CursorTop = 4;
			Console.WriteLine("mount.targetDEC() = {0}* {1}\" {2}'", num.getValue(0), num.getValue(1), num.getValue(2));
		}

		public static void haConfirmed(EventArgs args)
		{
			var num = args.source as NumberInput;
			Console.CursorTop = 4;
			Console.WriteLine("mount.setHA({0}, {1})", num.getValue(0), num.getValue(1));
		}

		public static void startHAConfirmed(EventArgs args)
		{
			var num = args.source as NumberInput;
			Console.CursorTop = 4;
			Console.WriteLine("mount.setHA({0}, {1})", num.getValue(0), num.getValue(1));
			args.source.getMainMenu().closeDialog();
		}

		////////////////////////////////////////////////////////////////////
		public class EventArgs
		{
			public EventArgs(MenuItem source) { this.source = source; }
			//public:
			public MenuItem source;
		}

		////////////////////////////////////////////////////////////////////
		public class SelectEventArgs : EventArgs
		{
			string _selected;
			public SelectEventArgs(MenuItem source) : base(source) { }
			public SelectEventArgs(MenuItem source, string selected) : base(source) { _selected = selected; }
			public string getSelected() { return _selected; }
		}

		////////////////////////////////////////////////////////////////////
		public class MainMenu
		{
			private MenuItem _activeDialog;
			private int _activeItem;
			private List<MenuItem> _topMenuList;
			private List<MenuItem> _dialogs;

			public MainMenu()
			{
				_topMenuList = new List<MenuItem>();
				_dialogs = new List<MenuItem>();
				_activeItem = 0;
				_activeDialog = null;
			}


			public void closeDialog()
			{
				_activeDialog = null;
			}

			public void activateDialog(string tag)
			{
				for (int i = 0; i < _dialogs.Count; i++)
				{
					if (_dialogs[i].getTag() == tag)
					{
						_activeDialog = _dialogs[i];
						break;
					}
				}
			}

			public void addMenuItem(MenuItem item)
			{
				_topMenuList.Add(item);
				item.setMainMenu(this);
			}

			public void addModalDialog(MenuItem dialog)
			{
				dialog.setMainMenu(this);
				_dialogs.Add(dialog);
			}

			public bool onPreviewKey(int keyState)
			{
				if (_activeDialog != null)
				{
					return _activeDialog.onPreviewKey(keyState);
				}
				return _topMenuList[_activeItem].onPreviewKey(keyState);
			}

			public bool processKeys(int key)
			{
				if (_activeDialog != null)
				{
					bool waitForRelease = _activeDialog.onKeypressed(key);
					return waitForRelease;
				}

				if (key == btnRIGHT)
				{
					_activeItem = (_activeItem + 1) % _topMenuList.Count;
					return true;
				}

				return _topMenuList[_activeItem].onKeypressed(key);
			}

			public void updateDisplay()
			{
				string menu = string.Empty;
				if (_activeDialog != null)
				{
					_activeDialog.onDisplay(true);
					return;
				}

				for (int i = 0; i < _topMenuList.Count; i++)
				{
					menu += string.Format("{0}{1}{2} ", _activeItem == i ? '>' : ' ', _topMenuList[i].getDisplayName(), _activeItem == i ? '<' : ' ');
				}
				Console.WriteLine(menu);

				_topMenuList[_activeItem].onDisplay();
			}

			public virtual void closeMenuItem(MenuItem closeMe)
			{
				if (_activeDialog != null)
				{
					_activeDialog.closeMenuItem(closeMe);
				}
				else
				{
					_topMenuList[_activeItem].closeMenuItem(closeMe);
				}
			}
		}

		////////////////////////////////////////////////////////////////////
		public class MenuItem
		{
			protected int _activeSubMenu;
			protected string _displayName;
			protected string _tag;
			protected List<MenuItem> _subMenuList;
			protected MainMenu _mainMenu;

			public MenuItem(string displayName, string tag = "")
			{
				_tag = tag;
				_displayName = displayName;
				_subMenuList = new List<MenuItem>();
				_activeSubMenu = 0;
			}

			public void addMenuItem(MenuItem item)
			{
				_subMenuList.Add(item);
			}

			public virtual void closeMenuItem(MenuItem closeMe)
			{
				if (_subMenuList.Any())
				{
					_subMenuList[_activeSubMenu].closeMenuItem(closeMe);
				}
			}

			public virtual MainMenu getMainMenu()
			{
				return _mainMenu;
			}

			public virtual void setMainMenu(MainMenu mainMenu)
			{
				_mainMenu = mainMenu;
				for (int i = 0; i < _subMenuList.Count; i++)
				{
					_subMenuList[i].setMainMenu(mainMenu);
				}
			}

			//public bool isActive() { return _active; }
			//public void setActive(bool state) { _active = state; }
			public string getTag() { return _tag; }
			public void setTag(string tag) { _tag = tag; }
			public string getDisplayName() { return _displayName; }

			public virtual void onDisplay(bool modal = false)
			{
				if (modal)
				{
					Console.WriteLine(_displayName);
				}
				if (_subMenuList.Count > 0)
				{
					_subMenuList[_activeSubMenu].onDisplay(modal);
				}
			}

			public virtual void onSelect() { }

			public virtual bool onPreviewKey(int keyState)
			{
				if (_subMenuList.Any())
				{
					return _subMenuList[_activeSubMenu].onPreviewKey(keyState);
				}
				return false;
			}

			public virtual bool onKeypressed(int key)
			{ // True if should wait for release
				if ((key == btnRIGHT) && (_subMenuList.Count > 0))
				{
					_activeSubMenu = (_activeSubMenu + 1) % _subMenuList.Count;
					return true;
				}
				if (_subMenuList.Any())
				{
					if (key == btnSELECT)
					{
						_subMenuList[_activeSubMenu].onSelect();
						return true;
					}
					return _subMenuList[_activeSubMenu].onKeypressed(key); ;
				}
				return false;
			}
		}

		////////////////////////////////////////////////////////////////////
		public class ScrollList : MenuItem
		{
			public ScrollList() : base(string.Empty)
			{
			}

			public override void onSelect()
			{
				if (_subMenuList.Count > 0)
				{
					_subMenuList[_activeSubMenu].onSelect();
				}
			}

			public override bool onKeypressed(int key)
			{
				if (key == btnDOWN)
				{
					_activeSubMenu = adjustWrap(_activeSubMenu, 1, 0, _subMenuList.Count - 1);
				}
				else if (key == btnUP)
				{
					_activeSubMenu = adjustWrap(_activeSubMenu, -1, 0, _subMenuList.Count - 1);
				}
				else
				{
					if (_subMenuList.Count > 0)
					{
						_subMenuList[_activeSubMenu].onKeypressed(key);
					}
				}
				return true;
			}
		}

		////////////////////////////////////////////////////////////////////
		public class TextInfo : MenuItem
		{
			String _prompt;
			Func<string> _displayFunc;

			public TextInfo(String prompt, Func<string> displayFunc) : base("")
			{
				_prompt = prompt;
				_displayFunc = displayFunc;
			}

			public override void onDisplay(bool modal = false)
			{
				Console.WriteLine(_prompt + _displayFunc() + "            ");
			}
		}

		////////////////////////////////////////////////////////////////////
		public class TextInfoMulti : MenuItem
		{
			String _prompt;
			Func<int, string> _displayFunc;
			int _currentSubItem;
			int _subItems;

			public TextInfoMulti(int subItems, Func<int, string> displayFunc) : base("")
			{
				_subItems = subItems;
				_currentSubItem = 0;
				_displayFunc = displayFunc;
			}

			public override bool onKeypressed(int key)
			{
				if (key == btnLEFT)
				{
					_currentSubItem = (_currentSubItem + 1) % _subItems;
					return true;
				}
				return base.onKeypressed(key);
			}
			public override void onDisplay(bool modal = false)
			{
				Console.WriteLine(_displayFunc(_currentSubItem) + "            ");
			}
		}

		////////////////////////////////////////////////////////////////////
		public class NumberInput : MenuItem
		{
			public enum BehaviorFlags { ConstantRepetition, AcceleratingRepetition} 
			String _mask;
			int _activeNumber;
			int[] _numbers;
			Action<EventArgs> _selectCallback;
			ulong _nextKeyOk;
			ulong _waitFor;
			BehaviorFlags _flags;
			bool _useIncrForDisplay;
			Incrementer _incr;

			public NumberInput(string tag, int numNumbers, String mask, Action<EventArgs> selectCallback, Incrementer incr, BehaviorFlags flags = BehaviorFlags.ConstantRepetition) : base("", tag)
			{
				_activeNumber = 0;
				_mask = mask;
				_numbers = new int[numNumbers];
				_selectCallback = selectCallback;
				_flags = flags;
				_incr = incr ?? new Incrementer();
				_useIncrForDisplay = true;
			}

			public NumberInput(string tag, int[] numbers, String mask, Action<EventArgs> selectCallback, Incrementer incr = null, BehaviorFlags flags = BehaviorFlags.ConstantRepetition) : base("", tag)
			{
				_activeNumber = 0;
				_mask = mask;
				_numbers = numbers;
				_selectCallback = selectCallback;
				_flags = flags;
				_incr = incr ?? new Incrementer();
				_useIncrForDisplay = false;
			}
			// ("RASteps : %d")  ("^%02dh^%02dm") ("^%02d*^%02d'^%02d\"") ("^+%02d*^%02d'^%02d\"")

			public override void onDisplay(bool modal = false)
			{
				int cursor = _activeNumber;
				int numberCursor = 0;
				string line = string.Empty;
				if (_useIncrForDisplay)
				{
					_incr.getNumbers(_tag, _numbers);
				}
				for (int i = 0; i < _mask.Length; i++)
				{
					if (_mask[i] == '^')
					{
						line += cursor == 0 ? '>' : ' ';
						cursor--;
					}
					else if (_mask[i] == '@')
					{
						line += _incr.getDisplay(_tag, numberCursor, _numbers[numberCursor]);
						numberCursor++; ;
					}
					else if (_mask[i] == '%')
					{
						int start = i;
						i++;
						while (char.IsDigit(_mask[i]))
						{
							i++;
						}

						line += _incr.getDisplay(_tag, numberCursor, _numbers[numberCursor], _mask.Substring(start, i - start));
						numberCursor++;
					}
					else
					{
						line += _mask[i];
					}
				}

				Console.WriteLine(line + "              ");
			}

			public override void onSelect()
			{
				if (_selectCallback != null)
				{
					_selectCallback(new SelectEventArgs(this));
				}
				else
				{
					_incr.onSelect(new SelectEventArgs(this));
				}
			}

			public int getValue(int index) { return _numbers[index]; }

			public override bool onPreviewKey(int keyState)
			{
				if (keyState == btnDOWN)
				{
					if (millis() > _nextKeyOk)
					{
						_incr.onChange(_tag, _numbers, _activeNumber, -1);
						_waitFor = (_flags == NumberInput.BehaviorFlags.AcceleratingRepetition) ? (ulong)Math.Max(10, 0.95 * _waitFor) : 200;
						_nextKeyOk = millis() + _waitFor;
					}
				}
				else if (keyState == btnUP)
				{
					if (millis() > _nextKeyOk)
					{
						_incr.onChange(_tag, _numbers, _activeNumber, 1);
						_waitFor = (_flags == NumberInput.BehaviorFlags.AcceleratingRepetition) ? (ulong)Math.Max(10, 0.95 * _waitFor) : 200;
						_nextKeyOk = millis() + _waitFor;
					}
				}
				else
				{
					_nextKeyOk = 0;
					_waitFor = 200;
					return false;
				}
				return true;
			}

			public override bool onKeypressed(int key)
			{
				if (!base.onKeypressed(key))
				{
					if (key == btnLEFT)
					{
						_activeNumber = (_activeNumber + 1) % _numbers.Length;
						return true;
					}
				}
				return false;
			}

		}


		/*
		ModalScreen - Takes over display
		ActionList - Select from list of items to invoke an action(used in GO)
		LabelList - Scroll through list of items(used in INFO)
		SubmenuList - Choose items(CTRL / CAL)
		ModalSubmenuList - Only for SubMenu, takes over 2nd line
		*/

		////////////////////////////////////////////////////////////////////
		class OptionChooser : MenuItem
		{ // 2 (Yes/No) or 3 (Yes/No/Cancl)
			private int _activeOption;
			string[] _choices;
			Action<EventArgs> _chosenFunc;
			public OptionChooser(string[] choices, int activeItemIndex, Action<EventArgs> chosenFunc) : base("")
			{
				_activeOption = activeItemIndex;
				_chosenFunc = chosenFunc;
				_choices = choices.ToArray();
			}

			public override void onDisplay(bool modal = false)
			{
				string line = string.Empty;
				for (int i = 0; i < _choices.Length; i++)
				{
					line += ((i == _activeOption) ? '>' : ' ') + _choices[i] + ((i == _activeOption) ? '<' : ' ') + ' ';
				}
				Console.WriteLine(line);
			}

			public override void onSelect()
			{
				_chosenFunc(new SelectEventArgs(this, _choices[_activeOption]));
			}

			public string getSelected()
			{
				return _choices[_activeOption];
			}

			public override bool onKeypressed(int key)
			{
				if (base.onKeypressed(key)) { return true; }
				if (key == btnLEFT)
				{
					_activeOption = (_activeOption + 1) % _choices.Length;
					return true;
				}
				return false;
			}
		}

		////////////////////////////////////////////////////////////////////
		public class Button : MenuItem
		{
			Action<EventArgs> _chosenFunc;
			MenuItem _subMenu;
			bool _subMenuActivated;
			public Button(string choice, MenuItem subMenu) : base(choice)
			{
				_chosenFunc = null;
				_subMenu = subMenu;
				_subMenuActivated = false;
			}

			public Button(string choice, Action<EventArgs> chosenFunc) : base(choice)
			{
				_chosenFunc = chosenFunc;
				_subMenu = null;
				_subMenuActivated = false;
			}

			public override void closeMenuItem(MenuItem closeMe)
			{
				if (_subMenuActivated) 
				{
					if (_subMenu == closeMe)
					{
						_subMenuActivated = false;
					}
					else
					{
						_subMenu.closeMenuItem(closeMe);
					}
				}
			}

			public override bool onKeypressed(int key)
			{
				if (_subMenuActivated)
				{
					return _subMenu.onKeypressed(key);
				}
				return base.onKeypressed(key);
			}

			public override bool onPreviewKey(int keyState)
			{
				if (_subMenuActivated)
				{
					return _subMenu.onPreviewKey(keyState);
				}
				return base.onPreviewKey(keyState);
			}

			public override void setMainMenu(MainMenu mainMenu)
			{
				base.setMainMenu(mainMenu);
				if (_subMenu != null)
				{
					_subMenu.setMainMenu(mainMenu);
				}
			}

			public override void onDisplay(bool modal = false)
			{
				if ((_subMenu != null) && _subMenuActivated)
				{
					_subMenu.onDisplay(modal);
				}
				else
				{
					string line = string.Empty;
					line = ">" + _displayName + "             ";
					Console.WriteLine(line);
				}
			}

			public override void onSelect()
			{
				if (_subMenu != null)
				{
					if (_subMenuActivated)
					{
						_subMenu.onSelect();
						_subMenuActivated = false;
					}
					else
					{
						_subMenuActivated = true;
					}
				}
				else
				{
					_chosenFunc(new SelectEventArgs(this, _displayName));
				}
			}
		}

		public class FindGPSDisplay : MenuItem
		{
			public FindGPSDisplay(string pitchOrRoll) : base("GPS", pitchOrRoll)
			{
			}

			public override void onDisplay(bool modal = false)
			{
				Console.WriteLine("* Found 0 sats              ");
			}

			//public override void onSelect()
			//{
			//	Console.CursorTop = 4;
			//	Console.WriteLine("Store offset for " + _tag);
			//}

			public override bool onKeypressed(int key)
			{
				if (key == btnLEFT)
				{
					getMainMenu().activateDialog("StartHAChooser");
				}
				return base.onKeypressed(key);
			}
		}

		public class PitchRollDisplay : MenuItem
		{
			public PitchRollDisplay(string pitchOrRoll) : base("Roll Offset", pitchOrRoll)
			{
			}

			public override void onDisplay(bool modal = false)
			{
				if (_tag == "ROLL")
				{
					Console.WriteLine("R ------0.4-<----            ");
				}
				else
				{
					Console.WriteLine("P --->--0.6------            ");
				}
			}

			public override void onSelect()
			{
				Console.CursorTop = 4;
				Console.WriteLine("Store offset for " + _tag);
			}

			public override bool onKeypressed(int key)
			{
				if (key == btnLEFT)
				{
					getMainMenu().closeMenuItem(this);
				}
				return base.onKeypressed(key);
			}

			public override bool onPreviewKey(int keyState)
			{
				// Ignore Up and Down buttons.
				if (keyState == btnUP) return true;
				if (keyState == btnDOWN) return true;
				if (keyState == btnRIGHT) return true;
			
				return base.onPreviewKey(keyState);
			}
		}

		/////////////////////////////////////////////////////////////
		public class ActionRunnerModal : MenuItem
		{
			protected MenuItem _followModal;
			protected Func<bool> _isComplete;

			public ActionRunnerModal(string prompt, string tag, MenuItem followModal = null) : base(prompt, tag)
			{
				_isComplete = null;
				_followModal = followModal;
			}

			public ActionRunnerModal(string prompt, string tag, Func<bool> isComplete, MenuItem followModal = null) : base(prompt, tag)
			{
				_isComplete = isComplete;
				_followModal = followModal;
			}

			public virtual bool isComplete() { return true; }

			public override void onDisplay(bool modal = false)
			{
				bool complete = _isComplete == null ? isComplete() : _isComplete();
				if (complete)
				{
					if (_followModal != null)
					{
						getMainMenu().activateDialog(_followModal.getTag());
					}
					else
					{
						getMainMenu().closeDialog();
					}
				}
			}
		}

		////////////////////////////////////////////////////////////////////
		public class ManualControlMenuItem : MenuItem
		{
			Action<EventArgs> _onClose;
			public ManualControlMenuItem(Action<EventArgs> onClose, string tag = "Control") : base("", tag)
			{
				_onClose = onClose;
			}

			public override void onDisplay(bool modal = false)
			{
				Console.CursorTop = 1;
				Console.CursorLeft = 0;
				Console.WriteLine("Use UDLR to home         ");
				Console.WriteLine("R031504 D+897322    ");
			}

			public override bool onKeypressed(int key)
			{
				if (key == btnSELECT)
				{
					_onClose(new SelectEventArgs(this));
					return true;
				}
				return false;
			}

		}

		public class DriftAlignRunner: ActionRunnerModal
		{
			int phase = 0;
			bool _completed = false;
			public DriftAlignRunner(string prompt, string tag, MenuItem followModal = null) : base(prompt, tag, followModal)
			{
			}

			public override bool isComplete() { return _completed; }

			public override void onDisplay(bool modal = false)
			{
				if (!string.IsNullOrEmpty(_displayName))
				{
					Console.WriteLine(_displayName + "            ");
				}

				base.onDisplay(modal);
			}
		}
	}
}
