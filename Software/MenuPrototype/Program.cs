using System;
using System.ComponentModel.Design;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace MenuPrototype
{
	partial class Program
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

			var dlgSlewToPolaris = new MultiStepActionRunnerModal("Slewing....", "SlewToPolaris", polarAlignFunction, dlgStoreSync);
			var paBtn = new Button("Polar Alignmnt", (eventArgs) => menu.activateDialog("SlewToPolaris"));
			calList.addMenuItem(paBtn);

			var driftAlignBtn = new Button("Drift Alignmnt", (eventArgs) => menu.activateDialog("DriftAlign"));
			var dlgDriftAlign = new MultiStepActionRunnerModal("Drift alignment", "DriftAlign", driftAlignPhaseFunction);
			calList.addMenuItem(driftAlignBtn);

			calList.addMenuItem(new Button("Speed Calibratn", new NumberInput("SPD", 1, "SpdFctr: @", null, speedIncr, NumberInput.BehaviorFlags.AcceleratingRepetition)));
			calList.addMenuItem(new Button("RA Step Adjust", new NumberInput("RA", 1, "RA Steps: @", null, speedIncr, NumberInput.BehaviorFlags.AcceleratingRepetition)));
			calList.addMenuItem(new Button("DEC Step Adjust", new NumberInput("DEC", 1, "DEC Steps: @", null, speedIncr, NumberInput.BehaviorFlags.AcceleratingRepetition)));
			calList.addMenuItem(new Button("Backlash Adjust", new NumberInput("DEC", 1, "DEC Steps: @", null, speedIncr, NumberInput.BehaviorFlags.AcceleratingRepetition)));
			calList.addMenuItem(new Button("Roll Offset", new PitchRollDisplay("ROLL")));
			calList.addMenuItem(new Button("Pitch Offset", new PitchRollDisplay("PITCH")));
			calMenu.addMenuItem(calList);

			//////  INFO  ///////
			var infoMenu = new MenuItem("INFO");
			var infoList = new ScrollList();
			infoList.addMenuItem(new TextInfoMulti(3, getRASteps));
			infoList.addMenuItem(new TextInfoMulti(3, getDECSteps));
			infoList.addMenuItem(new TextInfoMulti(2, getTRKSteps));
			//infoList.addMenuItem(new TextInfo( "Loc ", getLocation));
			//infoList.addMenuItem(new TextInfo( "Temp: ", getTemperature));
			//infoList.addMenuItem(new TextInfo( "MemAvail: ", getMemAvail));
			//infoList.addMenuItem(new TextInfo( "Up: ", getUpTime));
			infoList.addMenuItem(new TextInfo("Firmw.: ", () => "V1.9.00"));
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

		private static void polarAlignFunction(StepStateChangeArgs arg)
		{
			if (arg.State == StepStateChangeArgs.StepState.Running)
			{
				arg.Display = "Slewing...";
				//if (mount.nothingslewing)
				{
					arg.Result = StepStateChangeArgs.StepStateChange.Completed;
				}
			}
		}

			static ulong driftStart = 0;
		private static void driftAlignPhaseFunction(StepStateChangeArgs arg)
		{
			arg.Heading = "Drift Algnmnt...";
			switch (arg.State)
			{
				case StepStateChangeArgs.StepState.Starting:
					{
						driftStart = millis();
					}
					break;
				case StepStateChangeArgs.StepState.Running:
					{
						switch (arg.Step)
						{
							case 0:
								{
									arg.Display = "Pause 1.5s...";
									if (millis() - driftStart > 1500)
									{
										arg.Result = StepStateChangeArgs.StepStateChange.Proceed;
										driftStart = millis();
									}
								}
								break;
							case 1:
								{
									arg.Display = "Eastward pass...";
									if (millis() - driftStart > 10000)
									{
										arg.Result = StepStateChangeArgs.StepStateChange.Proceed;
										driftStart = millis();
									}
								}
								break;
							case 2:
								{
									arg.Display = "Pause 1.5s...";
									if (millis() - driftStart > 1500)
									{
										arg.Result = StepStateChangeArgs.StepStateChange.Proceed;
										driftStart = millis();
									}
								}
								break;
							case 3:
								{
									arg.Display = "Westward pass...";
									if (millis() - driftStart > 10000)
									{
										arg.Result = StepStateChangeArgs.StepStateChange.Proceed;
										driftStart = millis();
									}
								}
								break;
							case 4:
								{
									arg.Display = "Pause 1.5s...";
									if (millis() - driftStart > 1500)
									{
										arg.Result = StepStateChangeArgs.StepStateChange.Proceed;
										driftStart = millis();
									}
								}
								break;
							case 5:
								{
									arg.Display = "Reset mount...";
									arg.Result = StepStateChangeArgs.StepStateChange.Completed;
								}
								break;
						}
					}
					break;
			}
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
	}
}
