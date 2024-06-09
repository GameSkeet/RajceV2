// EACH STRING PASSED TO NATIVE IS FREE'D BY NATIVE!!!

using System;
using System.IO;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;

using RajceV2Cheat.Structs;

namespace RajceV2Cheat
{
    internal enum UIFonts : uint
    {
        Text,
        SectionText,
        Header,
    };

    internal static unsafe class FrontendPipe
    {
        private enum InitCode : int
        {
            OK = 0,

            InitFailed = 1,
            HookFailed = 2
        }
        private enum ElementType : uint
        {
            None,
            Button,
            Checkbox,
            SliderInt,
            SliderFloat,
            Colorpicker,
            Combo,
            Keybind
        };

        #region Delegates

        const CallingConvention CALL_CONV = CallingConvention.Cdecl;

        [UnmanagedFunctionPointer(CALL_CONV)]
        private delegate InitCode InitDelegate();
        [UnmanagedFunctionPointer(CALL_CONV)]
        private delegate void DestroyDelegate();

        #region Keybind management

        [UnmanagedFunctionPointer(CALL_CONV)]
        private delegate void RegisterKeybindDelegate(IntPtr name, IntPtr keybind);
        [UnmanagedFunctionPointer(CALL_CONV)]
        public delegate void UnregisterKeybindDelegate1(uint id);
        [UnmanagedFunctionPointer(CALL_CONV)]
        private delegate void UnregisterKeybindDelegate2(IntPtr name);

        #endregion
        #region Menu

        [UnmanagedFunctionPointer(CALL_CONV)]
        public delegate void SetMenuIconDelegate(IntPtr icon);
        [UnmanagedFunctionPointer(CALL_CONV)]
        public delegate void SetMenuKeybindDelegate(IntPtr keybind);
        [UnmanagedFunctionPointer(CALL_CONV)]
        private delegate void SetMenuKeycodeConverterDelegate(IntPtr func);

        #endregion
        #region UI builder

        [UnmanagedFunctionPointer(CALL_CONV)]
        private delegate uint BeginTabDelegate(IntPtr name);
        [UnmanagedFunctionPointer(CALL_CONV)]
        public delegate void EndTabDelegate();
        [UnmanagedFunctionPointer(CALL_CONV)]
        public delegate void RemoveTabDelegate(uint id);

        [UnmanagedFunctionPointer(CALL_CONV)]
        private delegate void BeginSectionDelegate(IntPtr name);
        [UnmanagedFunctionPointer(CALL_CONV)]
        public delegate void EndSectionDelegate();

        [UnmanagedFunctionPointer(CALL_CONV)]
        private delegate void BeginContentBoxDelegate(IntPtr name);
        [UnmanagedFunctionPointer(CALL_CONV)]
        public delegate void EndContentBoxDelegate();

        [UnmanagedFunctionPointer(CALL_CONV)]
        private delegate IntPtr AddElementDelegate(ElementType type, IntPtr text, void* target, IntPtr callback);
        [UnmanagedFunctionPointer(CALL_CONV)]
        private delegate void SetElementMinMaxDelegate(IntPtr elem, float min, float max);
        private delegate void SetComboValuesDelegate(IntPtr elem, IntPtr options, int num_options, bool multi);

        #endregion
        #region Keybind list

        [UnmanagedFunctionPointer(CALL_CONV)]
        public delegate bool* GetKeybindListVisiblePtrDelegate();

        #endregion

        [UnmanagedFunctionPointer(CALL_CONV)]
        public delegate void LoadFontDelegate(UIFonts type, IntPtr font, int buf_len);

        #endregion
        #region Callbacks

        [UnmanagedFunctionPointer(CALL_CONV)]
        public delegate void OnButtonClickCallback();
        [UnmanagedFunctionPointer(CALL_CONV)]
        public delegate void OnCheckboxCallback(bool val);
        [UnmanagedFunctionPointer(CALL_CONV)]
        public delegate void OnSliderChanged(float val);
        [UnmanagedFunctionPointer(CALL_CONV)]
        public delegate void OnComboChanged();
        [UnmanagedFunctionPointer(CALL_CONV)]
        public delegate IntPtr GetKeycodeNameCallback(int keycode);

        #endregion
        #region Functions

        #region Keybind managment

        public static UnregisterKeybindDelegate1 UnregisterKeybind => GetExport<UnregisterKeybindDelegate1>("UnregisterKeybind");

        #endregion
        #region Menu

        public static SetMenuIconDelegate SetMenuIcon => GetExport<SetMenuIconDelegate>("SetMenuIcon");
        public static SetMenuKeybindDelegate SetMenuKeybind => GetExport<SetMenuKeybindDelegate>("SetMenuKeybind");

        #endregion
        #region Keybind list

        public static GetKeybindListVisiblePtrDelegate GetKeybindListPtr => GetExport<GetKeybindListVisiblePtrDelegate>("GetKeybindListVisiblePtr");

        #endregion
        #region UI Builder

        public static EndTabDelegate EndTab => GetExport<EndTabDelegate>("EndTab");
        public static RemoveTabDelegate RemoveTab => GetExport<RemoveTabDelegate>("RemoveTab");

        public static EndSectionDelegate EndSection => GetExport<EndSectionDelegate>("EndSection");

        public static EndContentBoxDelegate EndContentBox => GetExport<EndContentBoxDelegate>("EndContentBox");

        private static AddElementDelegate AddElement => GetExport<AddElementDelegate>("AddElement");
        private static SetElementMinMaxDelegate SetElementMinMax => GetExport<SetElementMinMaxDelegate>("SetElementMinMax");

        #endregion
        #region Misc

        public static LoadFontDelegate LoadFont => GetExport<LoadFontDelegate>("LoadFont");

        #endregion

        #endregion

        private static readonly string _TargetPath = Path.Combine(RajceV2.UserData, "Frontend.dll");
        private static bool _Inited = false;
        private static IntPtr _Target = IntPtr.Zero;
        // Used for caching
        private static Dictionary<string, IntPtr> _Exports = new Dictionary<string, IntPtr>();

        public static int InitErrorCode { get; private set; }

        [DllImport("kernel32.dll")]
        extern private static IntPtr LoadLibraryA(string lpFileName);

        [DllImport("kernel32.dll")]
        extern private static IntPtr GetProcAddress(IntPtr hModule, string procName);
        [DllImport("kernel32.dll")]
        extern private static void FreeLibrary(IntPtr hModule);


        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        private static T GetExport<T>(string fnName) where T : Delegate
        {
            if (_Target == IntPtr.Zero)
                return null;

            // Check the cache if the export has already been resolved
            if (_Exports.TryGetValue(fnName, out IntPtr cexport))
                return (T)Marshal.GetDelegateForFunctionPointer(cexport, typeof(T));

            IntPtr export = GetProcAddress(_Target, fnName);
            _Exports[fnName] = export; // Add the export to cache

            return (T)Marshal.GetDelegateForFunctionPointer(export, typeof(T));
        }
        private static IntPtr GetFnPtr(Delegate d)
        {
            if (d == null)
                return IntPtr.Zero;

            return Marshal.GetFunctionPointerForDelegate(d);
        }

        public static bool InitTransporter()
        {
            if (_Inited)
                return true;
            
            // Delete the existing file as it might be outdated
            if (File.Exists(_TargetPath))
                File.Delete(_TargetPath);

            File.WriteAllBytes(_TargetPath, Utils.GetResource("RajceV2.dll"));

            // Load the frontend library
            if (_Target == IntPtr.Zero)
            {
                _Target = LoadLibraryA(_TargetPath);
                if (_Target == IntPtr.Zero)
                    return false;
            }

            InitDelegate initFunc = GetExport<InitDelegate>("Init");
            InitCode code = initFunc();
            InitErrorCode = (int)code;

            if (code != InitCode.OK)
                return false;

            _Inited = true;

            return true;
        }
        public static void DestroyTransporter()
        {
            if (!_Inited)
                return;

            DestroyDelegate destroy = GetExport<DestroyDelegate>("Destroy");
            destroy();

            FreeLibrary(_Target);

            _Exports.Clear();
            _Target = IntPtr.Zero;
            _Inited = false;
        }

        #region Keybind managment

        public static void RegisterKeybind(string name, IntPtr key)
        {
            RegisterKeybindDelegate regKey = GetExport<RegisterKeybindDelegate>("RegisterKeybind");

            regKey(Marshal.StringToHGlobalUni(name), key);
        }
        public static void UnregisterKeybindByName(string name)
        {
            UnregisterKeybindDelegate2 unregKey = GetExport<UnregisterKeybindDelegate2>("UnregisterKeybindByName");

            unregKey(Marshal.StringToHGlobalUni(name));
        }

        #endregion
        #region Menu

        public static void SetKeycodeConverter(GetKeycodeNameCallback callback)
        {
            SetMenuKeycodeConverterDelegate setConv = GetExport<SetMenuKeycodeConverterDelegate>("SetMenuKeybindConverter");
            setConv(GetFnPtr(callback));
        }

        #endregion
        #region UI builder

        public static uint BeginTab(string name)
        {
            BeginTabDelegate begin = GetExport<BeginTabDelegate>("BeginTab");

            return begin(Marshal.StringToHGlobalUni(name));
        }
        public static void BeginSection(string name)
        {
            BeginSectionDelegate begin = GetExport<BeginSectionDelegate>("BeginSection");

            begin(Marshal.StringToHGlobalUni(name));
        }
        public static void BeginContentBox(string name)
        {
            BeginContentBoxDelegate begin = GetExport<BeginContentBoxDelegate>("BeginContentBox");

            begin(Marshal.StringToHGlobalUni(name));
        }

        public static void AddButton(string text, OnButtonClickCallback onClick = null)
        {
            AddElement(ElementType.Button, Marshal.StringToHGlobalUni(text), (void*)0, GetFnPtr(onClick));
        }
        public static void AddCheckbox(string text, bool* target, OnCheckboxCallback onClick = null)
        {
            AddElement(ElementType.Checkbox, Marshal.StringToHGlobalUni(text), target, GetFnPtr(onClick));
        }
        public static void AddSliderInt(string text, int* target, int min, int max, OnSliderChanged onChanged = null)
        {
            IntPtr elem = AddElement(ElementType.SliderInt, Marshal.StringToHGlobalUni(text), target, GetFnPtr(onChanged));
            SetElementMinMax(elem, min, max);
        }
        public static void AddSliderFloat(string text, float* target, float min, float max, OnSliderChanged onChanged = null)
        {
            IntPtr elem = AddElement(ElementType.SliderFloat, Marshal.StringToHGlobalUni(text), target, GetFnPtr(onChanged));
            SetElementMinMax(elem, min, max);
        }
        // WARNING: set default selected value to -1, if using multi set all of the values to -1, also multi array should be the same length as the options array
        public static void AddComboBox(string text, int* selected, string[] options, bool isMulti = false, OnComboChanged onChanged = null)
        {
            SetComboValuesDelegate setValues = GetExport<SetComboValuesDelegate>("SetComboValues");

            // All of the marshaled strings are freed inside the setValues function
            IntPtr[] hgoptions = new IntPtr[options.Length];
            for (int i = 0; i < options.Length; i++)
                hgoptions[i] = Marshal.StringToHGlobalUni(options[i]);

            IntPtr elem = AddElement(ElementType.Combo, Marshal.StringToHGlobalUni(text), selected, GetFnPtr(onChanged));
            setValues(elem, Marshal.UnsafeAddrOfPinnedArrayElement(hgoptions, 0), hgoptions.Length, isMulti);
        }
        public static void AddKeybind(string text, IntPtr keybind)
        {
            AddElement(ElementType.Keybind, Marshal.StringToHGlobalUni(text), keybind.ToPointer(), IntPtr.Zero);
        }

        #endregion
    }
}
