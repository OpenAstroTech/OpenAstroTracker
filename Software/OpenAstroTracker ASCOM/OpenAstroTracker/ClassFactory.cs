using System;
using System.Runtime.InteropServices;
using System.Collections;

namespace ASCOM.OpenAstroTracker
{

    #region C# Definition of IClassFactory
    //
    // Provide a definition of theCOM IClassFactory interface.
    //
    [
      ComImport,												// This interface originated from COM.
      ComVisible(false),										// Must not be exposed to COM!!!
      InterfaceType(ComInterfaceType.InterfaceIsIUnknown),		// Indicate that this interface is not IDispatch-based.
      Guid("00000001-0000-0000-C000-000000000046")				// This GUID is the actual GUID of IClassFactory.
    ]
    public interface IClassFactory
    {
        void CreateInstance(IntPtr pUnkOuter, ref Guid riid, out IntPtr ppvObject);
        void LockServer(bool fLock);
    }
    #endregion

    //
    // Universal ClassFactory. Given a type as a parameter of the 
    // constructor, it implements IClassFactory for any interface
    // that the class implements. Magic!!!
    //
    public class ClassFactory : IClassFactory
    {

        #region Access to ole32.dll functions for class factories

        // Define two common GUID objects for public usage.
        public static Guid IID_IUnknown = new Guid("{00000000-0000-0000-C000-000000000046}");
        public static Guid IID_IDispatch = new Guid("{00020400-0000-0000-C000-000000000046}");

        [Flags]
        enum CLSCTX : uint
        {
            CLSCTX_INPROC_SERVER = 0x1,
            CLSCTX_INPROC_HANDLER = 0x2,
            CLSCTX_LOCAL_SERVER = 0x4,
            CLSCTX_INPROC_SERVER16 = 0x8,
            CLSCTX_REMOTE_SERVER = 0x10,
            CLSCTX_INPROC_HANDLER16 = 0x20,
            CLSCTX_RESERVED1 = 0x40,
            CLSCTX_RESERVED2 = 0x80,
            CLSCTX_RESERVED3 = 0x100,
            CLSCTX_RESERVED4 = 0x200,
            CLSCTX_NO_CODE_DOWNLOAD = 0x400,
            CLSCTX_RESERVED5 = 0x800,
            CLSCTX_NO_CUSTOM_MARSHAL = 0x1000,
            CLSCTX_ENABLE_CODE_DOWNLOAD = 0x2000,
            CLSCTX_NO_FAILURE_LOG = 0x4000,
            CLSCTX_DISABLE_AAA = 0x8000,
            CLSCTX_ENABLE_AAA = 0x10000,
            CLSCTX_FROM_DEFAULT_CONTEXT = 0x20000,
            CLSCTX_INPROC = CLSCTX_INPROC_SERVER | CLSCTX_INPROC_HANDLER,
            CLSCTX_SERVER = CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER | CLSCTX_REMOTE_SERVER,
            CLSCTX_ALL = CLSCTX_SERVER | CLSCTX_INPROC_HANDLER
        }

        [Flags]
        enum REGCLS : uint
        {
            REGCLS_SINGLEUSE = 0,
            REGCLS_MULTIPLEUSE = 1,
            REGCLS_MULTI_SEPARATE = 2,
            REGCLS_SUSPENDED = 4,
            REGCLS_SURROGATE = 8
        }
        //
        // CoRegisterClassObject() is used to register a Class Factory
        // into COM's internal table of Class Factories.
        //
        [DllImport("ole32.dll")]
        static extern int CoRegisterClassObject(
            [In] ref Guid rclsid,
            [MarshalAs(UnmanagedType.IUnknown)] object pUnk,
            uint dwClsContext,
            uint flags,
            out uint lpdwRegister);
        //
        // Called by a COM EXE Server that can register multiple class objects 
        // to inform COM about all registered classes, and permits activation 
        // requests for those class objects. 
        // This function causes OLE to inform the SCM about all the registered 
        // classes, and begins letting activation requests into the server process.
        //
        [DllImport("ole32.dll")]
        static extern int CoResumeClassObjects();
        //
        // Prevents any new activation requests from the SCM on all class objects
        // registered within the process. Even though a process may call this API, 
        // the process still must call CoRevokeClassObject for each CLSID it has 
        // registered, in the apartment it registered in.
        //
        [DllImport("ole32.dll")]
        static extern int CoSuspendClassObjects();
        //
        // CoRevokeClassObject() is used to unregister a Class Factory
        // from COM's internal table of Class Factories.
        //
        [DllImport("ole32.dll")]
        static extern int CoRevokeClassObject(uint dwRegister);
        #endregion

        #region Constructor and Private ClassFactory Data

        protected Type m_ClassType;
        protected Guid m_ClassId;
        protected ArrayList m_InterfaceTypes;
        protected uint m_ClassContext;
        protected uint m_Flags;
        protected UInt32 m_locked = 0;
        protected uint m_Cookie;
        protected string m_progid;

        public ClassFactory(Type type)
        {
            if (type == null)
                throw new ArgumentNullException("type");
            m_ClassType = type;

            //PWGS Get the ProgID from the MetaData
            m_progid = Marshal.GenerateProgIdForType(type);
            m_ClassId = Marshal.GenerateGuidForType(type);		// Should be nailed down by [Guid(...)]
            m_ClassContext = (uint)CLSCTX.CLSCTX_LOCAL_SERVER;	// Default
            m_Flags = (uint)REGCLS.REGCLS_MULTIPLEUSE |			// Default
                        (uint)REGCLS.REGCLS_SUSPENDED;
            m_InterfaceTypes = new ArrayList();
            foreach (Type T in type.GetInterfaces())			// Save all of the implemented interfaces
                m_InterfaceTypes.Add(T);
        }

        #endregion

        #region Common ClassFactory Methods
        public uint ClassContext
        {
            get { return m_ClassContext; }
            set { m_ClassContext = value; }
        }

        public Guid ClassId
        {
            get { return m_ClassId; }
            set { m_ClassId = value; }
        }

        public uint Flags
        {
            get { return m_Flags; }
            set { m_Flags = value; }
        }

        public bool RegisterClassObject()
        {
            // Register the class factory
            int i = CoRegisterClassObject
                (
                ref m_ClassId,
                this,
                m_ClassContext,
                m_Flags,
                out m_Cookie
                );
            return (i == 0);
        }

        public bool RevokeClassObject()
        {
            int i = CoRevokeClassObject(m_Cookie);
            return (i == 0);
        }

        public static bool ResumeClassObjects()
        {
            int i = CoResumeClassObjects();
            return (i == 0);
        }

        public static bool SuspendClassObjects()
        {
            int i = CoSuspendClassObjects();
            return (i == 0);
        }
        #endregion

        #region IClassFactory Implementations
        //
        // Implement creation of the type and interface.
        //
        void IClassFactory.CreateInstance(IntPtr pUnkOuter, ref Guid riid, out IntPtr ppvObject)
        {
            IntPtr nullPtr = new IntPtr(0);
            ppvObject = nullPtr;

            //
            // Handle specific requests for implemented interfaces
            //
            foreach (Type iType in m_InterfaceTypes)
            {
                if (riid == Marshal.GenerateGuidForType(iType))
                {
                    ppvObject = Marshal.GetComInterfaceForObject(Activator.CreateInstance(m_ClassType), iType);
                    return;
                }
            }
            //
            // Handle requests for IDispatch or IUnknown on the class
            //
            if (riid == IID_IDispatch)
            {
                ppvObject = Marshal.GetIDispatchForObject(Activator.CreateInstance(m_ClassType));
                return;
            }
            else if (riid == IID_IUnknown)
            {
                ppvObject = Marshal.GetIUnknownForObject(Activator.CreateInstance(m_ClassType));
            }
            else
            {
                //
                // Oops, some interface that the class doesn't implement
                //
                throw new COMException("No interface", unchecked((int)0x80004002));
            }
        }

        void IClassFactory.LockServer(bool bLock)
        {
            if (bLock)
                Server.CountLock();
            else
                Server.UncountLock();
            // Always attempt to see if we need to shutdown this server application.
            Server.ExitIf();
        }
        #endregion
    }
}
